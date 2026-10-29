#include <Runtime/ScrVmProgramHandler.h>
#include <Runtime/ScrVmMusicHandler.h>
#include <Runtime/ScrVmUpdateHandler.h>
#include <Runtime/ScriptSoundGroupSource.h>
#include "ScriptFileCommandSupport.h"

#include <bit>

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace FileCommandDetail;
struct ProgramUnwind
{
    YtlReferenceSlotRuntime* slot;
    ~ProgramUnwind() noexcept { if (slot) UnwindScriptControlCommand(*slot); }
    void Finish(YtlReferenceControlRuntime* captured) noexcept
    {
        // 4D0666等正常出口保留EDI；4C9790展开才重读slot.control。
        slot = nullptr;
        ReleaseCapturedScriptStackControl(captured);
    }
};
template<class T> T Read(const Command& command, std::uint32_t field)
{
    // 4CFC51..4CFCA6内联解引用已知bias=0的body；不是4CA1F0。
    const auto* body = reinterpret_cast<const ScriptControlCommandNative*>(command.reference.baseAt00);
    if (!body || !body->source) throw std::logic_error("PROGRAM null command/source (modern safety boundary)");
    const auto pointer = reinterpret_cast<std::uintptr_t>(body->source);
    const auto offset = static_cast<std::uint32_t>(pointer - command.base);
    if (command.knownExtent && std::uint64_t{offset} + field + sizeof(T) > command.extent)
        throw std::out_of_range("PROGRAM operand outside allocation (modern safety boundary)");
    T value;
    std::memcpy(&value, reinterpret_cast<const void*>(pointer + field), sizeof(value));
    return value;
}
const ScriptControlCommandNative* ResolveCaptured(std::uintptr_t base,
    YtlReferenceControlRuntime* control, std::uint32_t bias)
{
    if (!base) return nullptr;
    const auto stride = control ? static_cast<EnvironmentObjectReferenceControl*>(control)->strideAt0C : 0u;
    return reinterpret_cast<const ScriptControlCommandNative*>(base + bias * stride);
}
std::int32_t RestartField(const Command& command, const ScriptControlCommandNative* body, std::uint32_t field)
{
    if (!body || !body->source) throw std::logic_error("PROGRAM restart null body/source (modern safety boundary)");
    const auto pointer = reinterpret_cast<std::uintptr_t>(body->source);
    const auto offset = static_cast<std::uint32_t>(pointer + field - command.base);
    if (command.knownExtent && std::uint64_t{offset} + 4 > command.extent)
        throw std::out_of_range("PROGRAM restart outside allocation (modern safety boundary)");
    std::int32_t value;
    std::memcpy(&value, reinterpret_cast<const void*>(pointer + field), sizeof(value));
    return value;
}
const ParamsGameSystem& RestartGroups(IScriptSoundGroupSource& source)
{
    const auto* root = source.CaptureAt418480();
    if (!root || !root->GameSystem()) throw std::logic_error("PROGRAM restart null GameSystem (modern safety boundary)");
    return *root->GameSystem();
}
}

void RestoreScriptProgramAt4D4F20(ScrScriptRuntimeState& runtime,
    IScrVmProgramRuntimeBoundary&, const ScriptProgramCommandNativeContext& c,
    IEnvironmentRootMemory& memory)
{
    // 4D4F4D..4D4FBA先slot0，后root，才解析restart_point；不能复用当前PC。
    const auto* names = c.pc.CaptureSlot0At4180C0();
    const auto root = c.pc.CaptureAfterEnsureAt413BE0();
    const auto index = c.pc.ResolveAt498020(names, "restart_point");
    const auto restart = ReadScriptValueAt4C73A0(root.paramsAt47BDB0(root.object), index, c.errors);
    void* allocation = memory.AllocateBody(4);
    auto* body = allocation ? ::new (allocation) ScriptControlCommandNative : nullptr;
    Command command;
    if (body)
    {
        // 4D4FD4在Allocate4返回后捕获+A4/A8/AC；DataAt保留32-bit bias/stride回绕。
        auto& bytes = runtime.NativeParserAtA0().bytesAt04;
        body->source = reinterpret_cast<const std::byte*>(bytes.DataAt(std::bit_cast<std::uint32_t>(restart)));
        command.base = bytes.RawSlot().baseAt00;
        if (const auto* control = static_cast<const EnvironmentObjectReferenceControl*>(bytes.RawSlot().controlAt08))
        { command.knownExtent = true; command.extent = std::uint64_t{control->countAt10} * control->strideAt0C; }
    }
    CreateScriptControlCommandReference(command.reference, body, ScriptControlCommandKind::Text, memory);
    const auto base = command.reference.baseAt00;
    auto* const control = command.reference.controlAt08;
    ProgramUnwind unwind{&command.reference};
    const auto format = RestartField(command, ResolveCaptured(base, control, command.reference.biasAt04), 4);
    const auto destination = c.pc.CaptureAfterEnsureAt413BE0();
    destination.paramsAt47BDB0(destination.object).messageFormatIndexAt60 = format;
    // 4D5076..4D50B4：图层前缓存bias，音乐前只重读旧control的stride。
    const auto layerBias = command.reference.biasAt04;
    c.layers.ApplyResultLayerList(RestartField(command, ResolveCaptured(base, control, layerBias), 8));
    c.music.ApplyNativeResourceId(RestartField(command, ResolveCaptured(base, control, layerBias), 12));
    for (std::uint32_t i = 0; std::bit_cast<std::int32_t>(i) <
        std::bit_cast<std::int32_t>(RestartGroups(c.groups).soundSlotCounts[3]); ++i)
    {
        const auto* current = ResolveEnvironmentObjectReference<ScriptControlCommandNative>(command.reference);
        ApplyScriptFirstSoundAt4D6640(RestartField(command, current, 16u + 4u * i), i, c.names, c.soundLeaves, c.soundGate);
    }
    for (std::uint32_t i = 0; std::bit_cast<std::int32_t>(i) <
        std::bit_cast<std::int32_t>(RestartGroups(c.groups).soundSlotCounts[1]); ++i)
    {
        const auto& groups = RestartGroups(c.groups);
        const auto* current = ResolveEnvironmentObjectReference<ScriptControlCommandNative>(command.reference);
        const auto first = groups.soundSlotCounts[3];
        ApplyScriptSecondSoundAt4D62A0(RestartField(command, current, 16u + 4u * (first + i)), i,
            c.names, c.groups, c.soundLeaves, c.soundGate);
    }
    // 4D538D在开始释放时捕获当前control，不是工厂返回时的control。
    unwind.Finish(command.reference.controlAt08);
}

std::int32_t ExecuteScriptProgramAt4CFB30(ScrScriptRuntimeState& runtime,
    ScrProgramInstructionHandler& handler, const ScriptProgramCommandNativeContext& c,
    IEnvironmentRootMemory& memory, IRuntimeStringManagerSource& strings)
{
    constexpr char prefix[] = "CmdProgram\x8F\x88\x97\x9D\x82\xAA\x8E\xB8\x94\x73"
        "\x82\xB5\x82\xDC\x82\xB5\x82\xBD\x81\x49 : ";
    TextSlot detail;
    InitializeEmptyRuntimeCString(detail.value, strings);
    TextUnwind detailUnwind{&detail.value};
    const char* const initialChars = detail.value.GetString();
    bool success = false;
    try
    {
        auto command = CreateCommand(runtime.NativeParserAtA0(), c.pc, memory, ScriptControlCommandKind::Program);
        auto* const captured = command.reference.controlAt08;
        ProgramUnwind unwind{&command.reference};
        std::int32_t index;
        if (Read<std::uint8_t>(command, 4) & 2u)
        {
            const auto operand = Read<std::uint16_t>(command, 5);
            const auto root = c.pc.CaptureAfterEnsureAt413BE0();
            index = ReadScriptValueAt4C73A0(root.paramsAt47BDB0(root.object), operand, c.errors);
        }
        else
        {
            if (!(Read<std::uint8_t>(command, 4) & 1u)) throw 1;
            index = Read<std::uint16_t>(command, 5);
        }
        if (static_cast<std::uint32_t>(index) > 23u) throw 1;
        const auto result = handler.ExecuteResolvedCommand(static_cast<ScrVmProgramCommand>(index), runtime, &c, &memory);
        unwind.Finish(captured);
        if (result)
        {
            // 返回8..16的支路在内层try内销毁当前CString；Free异常进入同一报告链。
            DestroyRuntimeCStringAt40CD40(&detail.value);
            detailUnwind.value = nullptr;
            return result;
        }
        success = true;
    }
    catch (const char* text)
    {
        SetRuntimeCStringAt437680(detail.value, text, text ? static_cast<int>(std::strlen(text)) : 0);
    }
    catch (...) { SetRuntimeCStringAt437680(detail.value, "syntax error", 12); }
    if (success)
    {
        // 4D0689先撤EH，再释放最初ESI记录的header；Free异常不能转成命令失败。
        detailUnwind.value = nullptr;
        ReleaseRuntimeCStringHeader(reinterpret_cast<ATL::CStringData*>(const_cast<char*>(initialChars)) - 1);
        return 0;
    }
    TextSlot report;
    ConstructReportAt4CA280(report.value, prefix, detail.value, strings);
    TextUnwind reportUnwind{&report.value};
    const char* const reportChars = report.value.GetString();
    Show(c.errors, reportChars);
    DestroyRuntimeCStringAt40CD40(&report.value);
    DestroyRuntimeCStringAt40CD40(&detail.value);
    reportUnwind.value = nullptr;
    detailUnwind.value = nullptr;
    return 2;
}
}
