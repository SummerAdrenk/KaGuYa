#include <Runtime/ScriptUpdateCommandNative.h>
#include "ScriptFileCommandSupport.h"

#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace FileCommandDetail;
class ResultControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit ResultControl(ScriptUpdateResultNative* body) noexcept
        : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    // 5B5C44：4D74A0直接删除0Ch正文（包括null），4CA830退化后按flags删除18h。
    void DestroyPayloadAtVslot00() override { ::operator delete(payloadAt14, std::size_t{12}); }
    void DeleteControlAtVslot04(std::uint32_t flags) override { RetireEnvironmentObjectControl(this, flags); }
};
static_assert(sizeof(ResultControl) == 24);
struct ReferenceUnwind
{
    YtlReferenceSlotRuntime* slot;
    ~ReferenceUnwind() noexcept { if (slot) UnwindScriptControlCommand(*slot); }
    void Finish(YtlReferenceControlRuntime* captured) noexcept
    {
        slot = nullptr;
        YtlReferenceSlotRuntime normal{0, 0, captured};
        ReleaseScriptDispatchCommandAt4D40C6(normal);
    }
};
ScriptUpdateResultNative* CreateResult(YtlReferenceSlotRuntime& slot, IEnvironmentRootMemory& memory)
{
    void* allocation = memory.AllocateBody(12);
    auto* body = allocation ? ::new (allocation) ScriptUpdateResultNative : nullptr;
    if (body) { body->first = 0; body->second = 0; }
    // 4D2CD1：正文为null也分配control；control失败不回收孤立正文。
    slot.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    slot.biasAt04 = 0;
    allocation = memory.AllocateBody(24);
    auto* control = allocation ? ::new (allocation) ResultControl(body) : nullptr;
    if (!control) throw std::bad_alloc{}; // 原null control写入的显式安全拒绝。
    control->referenceCountAt04 = 1;
    control->owningAt08 = 1;
    control->strideAt0C = 12;
    control->countAt10 = 1;
    slot.controlAt08 = control;
    return body;
}
}

std::int32_t ExecuteScriptUpdateAt4D2800(ScrScriptRuntimeState& runtime,
    IScriptFileCommandSource& pcSource, IScrVmUpdateValueSource& values,
    const IScrResourceNameSource& names, IScrVmUpdateRuntimeBoundary& layers,
    ScrVmScriptStorage& storage, IScrVmCommandErrorSink& errors, IEnvironmentRootMemory& memory,
    IRuntimeStringManagerSource& strings)
{
    constexpr char prefix[] = "CmdUpdate\x8F\x88\x97\x9D\x82\xAA\x8E\xB8\x94\x73"
        "\x82\xB5\x82\xDC\x82\xB5\x82\xBD\x81\x49 : "; // PE5B5E04，32字节。
    static_assert(sizeof(prefix) == 33);
    const auto status = ExecuteWithReport(prefix, errors, strings, [&] {
        auto command = CreateCommand(runtime.NativeParserAtA0(), pcSource, memory, ScriptControlCommandKind::Update);
        auto* const commandControl = command.reference.controlAt08;
        ReferenceUnwind commandUnwind{&command.reference};
        layers.ApplyResultLayerList(Operand<std::int32_t>(command, 4));
        layers.SelectLayerEffect(Operand<std::int32_t>(command, 8));
        storage.EffectAt4AD140().Reset();
        const auto flags = Operand<std::uint8_t>(command, 12);
        if (flags & 1u)
        {
            const auto value = Operand<std::uint32_t>(command, 13);
            storage.EffectAt4AD140().status = static_cast<std::uint16_t>(value);
        }
        else if (flags & 6u)
        {
            storage.EffectAt4AD140().modeFlags |= 1u; // 4D2A16、4D2A3B分开重新解effect。
            storage.EffectAt4AD140().modeFlags |= flags & 6u;
            storage.EffectAt4AD140().status = 0x8001;
            auto id = Operand<std::int32_t>(command, 13);
            if (flags & 2u) id = values.ReadUpdateValueAt4C73A0(Operand<std::int32_t>(command, 13));
            const auto table = names.CapturePatternNames();
            auto& target = storage.EffectAt4AD140(); // 先捕获effect，之后才调用名称查询。
            const auto* name = table.FindNameAt4AD1B0(id);
            target.SetResourceName(name);
        }
        else if (flags & 8u)
        {
            storage.EffectAt4AD140().modeFlags |= 0x80u;
            for (std::int32_t i = 0; i < ScriptPairCount(storage.EffectAt4AD140()); ++i)
            {
                ScrVmUpdatePair pair;
                if (!layers.ReadActiveResultPair(i, pair)) throw 1;
                storage.EffectAt4AD140().PairAt(i) = pair;
            }
            const auto count = Operand<std::uint8_t>(command, 13);
            for (std::uint32_t i = 0; i < count; ++i)
            {
                YtlReferenceSlotRuntime result{};
                ReferenceUnwind resultUnwind{&result}; // state4先于逐项count重读和两次分配。
                if (i >= Operand<std::uint8_t>(command, 13)) throw 1;
                auto* body = CreateResult(result, memory);
                if (!body) throw std::logic_error("UPDATE null Result (modern safety boundary)");
                const auto offset = 14u + 5u * i;
                body->index = Operand<std::uint8_t>(command, offset);
                body->first = Operand<std::int16_t>(command, offset + 1);
                body->second = Operand<std::int16_t>(command, offset + 3);
                if (body->index >= ScriptPairCount(storage.EffectAt4AD140())) throw 1;
                auto& destination = storage.EffectAt4AD140().PairAt(body->index);
                destination.first = body->first;
                destination.second = body->second;
                resultUnwind.Finish(result.controlAt08); // state5：正常释放固定control。
            }
            storage.EffectAt4AD140().status = 12;
        }
        else if (flags) throw 1;
        const auto auxiliary = flags & 0x10u ? Operand<std::uint8_t>(command, 13) : std::uint8_t{0};
        storage.EffectAt4AD140().auxiliaryByte = auxiliary;
        commandUnwind.Finish(commandControl); // state6；初始CString释放仍在内部catch之外。
    });
    return status == 0 ? 3 : status;
}
}
