#include <Runtime/ProgramRestartRuntime.h>
#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/MappedSoundSlotRuntime.h>
#include <Runtime/ParamsRuntimeRoot.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/ScrVmMusicHandler.h>
#include <Runtime/ScrVmUpdateHandler.h>
#include <bit>
#include <cstring>

namespace KaGuYa::Start::Runtime
{
namespace
{
// 4D4FBF/4C8500的text_cmd是4-byte指针包装，不是8-byte的MessageTextCommand。
struct TextRecord final { const std::byte* source; };
static_assert(sizeof(TextRecord) == 4);
struct CommandLocal final
{
    YtlReferenceSlotRuntime slot;
    ~CommandLocal() noexcept { ReleaseYtlReference(slot); }
};

// 读取时才检查边界，不预扫整个记录而吞掉原已提交的+60/图层/音乐前缀。
std::int32_t ReadField(const TextRecord& record, std::span<const std::byte> code,
    std::uint64_t offset)
{
    const auto base = reinterpret_cast<std::uintptr_t>(code.data());
    const auto source = reinterpret_cast<std::uintptr_t>(record.source);
    if (!source || source < base || source - base > code.size() ||
        offset > code.size() - (source - base) ||
        sizeof(std::int32_t) > code.size() - (source - base) - offset)
        throw std::out_of_range("4D4F20 text record field outside loaded code (modern safety boundary)");
    std::int32_t value;
    std::memcpy(&value, record.source + static_cast<std::size_t>(offset), sizeof(value));
    return value;
}
}

ProgramRestartRuntime::ProgramRestartRuntime(RootRuntimeOwner& root, ParamsRuntimeRootOwner& params,
    ScrScriptRuntimeState& script, IScrVmUpdateRuntimeBoundary& layers,
    ScrMusicInstructionHandler& music, MappedSoundSlotRuntime& sound,
    IEnvironmentRootMemory& commandMemory) noexcept
    : root_(root), params_(params), script_(script), layers_(layers), music_(music),
      sound_(sound), commandMemory_(commandMemory) {}

std::int32_t ProgramRestartRuntime::ReadGroupCount(std::size_t field)
{
    const auto* game = params_.GetOrCreateAt418480().GameSystem().get();
    if (!game) throw std::logic_error("4D4F20 null GameSystem (modern safety boundary)");
    return std::bit_cast<std::int32_t>(game->soundSlotCounts[field]);
}

void ProgramRestartRuntime::Restore(ScrVmProgramRestartPoint restartPoint)
{
    // 先申请包装，再捕获当前CScript+A4。原指针不保活脚本正文，调用方须遵守同一寿命。
    void* storage = commandMemory_.AllocateBody(sizeof(TextRecord));
    auto* body = storage ? ::new (storage) TextRecord : nullptr;
    std::span<const std::byte> code;
    if (body)
    {
        // 4D5BA0接收this=CScript+A0；其+04仍为整个文件，restart_point是文件绝对PC。
        if (const auto* loaded = script_.LoadedScript()) code = loaded->Bytes();
        const auto address = reinterpret_cast<std::uintptr_t>(code.data());
        body->source = address ? reinterpret_cast<const std::byte*>(address +
            std::bit_cast<std::uint32_t>(restartPoint.value)) : nullptr;
    }
    YtlReferenceSlotRuntime slot{};
    if (body)
    {
        slot.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
        using Control = EnvironmentObjectControl<TextRecord>;
        static_assert(sizeof(Control) == 0x18);
        // 4C8524先发布正文，控制块申请失败时原调用尚无EH清理资格，不能自动回收正文。
        storage = commandMemory_.AllocateBody(sizeof(Control));
        auto* control = storage ? ::new (storage) Control(body, Control::UninitializedFieldsTag{}) : nullptr;
        slot.controlAt08 = control;
        if (!control) throw std::bad_alloc{}; // 原空control解引用的显式安全边界。
        control->referenceCountAt04 = 1;
        control->owningAt08 = 1;
        control->strideAt0C = 4;
        control->countAt10 = 1;
    }
    CommandLocal local{slot}; // 4D5028才获得原var_24的展开清理资格。
    if (!body) throw std::logic_error("4D504A null text_cmd (modern safety boundary)");
    const auto format = ReadField(*body, code, 4);
    root_.EnsureCreatedAt413BE0();
    root_.RequirePublished().ParamsAt47BDB0().messageFormatIndexAt60 = format;
    layers_.ApplyResultLayerList(ReadField(*body, code, 8));
    music_.ApplyResourceId(ReadField(*body, code, 12));
    // 4D50E0及4D51E0每轮重取GameSystem；两个循环均为有符号比较，负数不执行。
    for (std::int32_t index = 0; index < ReadGroupCount(3); ++index)
        sound_.ApplyFirst(ReadField(*body, code, 16ull + 4ull * index), static_cast<std::size_t>(index));
    for (std::int32_t index = 0; index < ReadGroupCount(1); ++index)
    {
        // 4D529C..4D5378的第二次捕获只供当前参数下标，不沿用上一次循环条件的正文。
        const auto firstCount = ReadGroupCount(3);
        const auto offset = 16ll + 4ll * (static_cast<std::int64_t>(firstCount) + index);
        sound_.ApplySecond(ReadField(*body, code, static_cast<std::uint64_t>(offset)),
            static_cast<std::size_t>(index));
    }
}
}
