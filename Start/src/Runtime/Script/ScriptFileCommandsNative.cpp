#include <Runtime/ScriptFileCommandsNative.h>
#include "ScriptFileCommandSupport.h"

#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace FileCommandDetail;
struct StackUnwind
{
    YtlReferenceSlotRuntime* reference;
    ~StackUnwind() noexcept { if (reference) ReleaseScriptStackReferenceAt4C9330(*reference); }
};
void SwitchOrThrow(ScrScriptRuntimeState& runtime, std::int32_t target,
    IScrVmScriptSwitchBoundary& scriptSwitch, IScrVmCommandErrorSink& errors)
{
    // 4D3C90仍是既有显式装载适配边界；失败先报告LoadScript，再进入命令catch。
    const auto result = scriptSwitch.Switch(runtime, target);
    if (!result.success) { if(!result.failureAlreadyReported)Show(errors, result.message.c_str());throw 1; }
}
ScriptStackFrameNative& ResolveFrame(const YtlReferenceSlotRuntime& reference)
{
    auto* body = ResolveEnvironmentObjectReference<ScriptStackFrameNative>(reference);
    if (!body) throw std::logic_error("File return null resolved frame (modern safety boundary)");
    return *body;
}
}

std::int32_t ExecuteScriptFileCallAt4CD240(ScrScriptRuntimeState& runtime,
    IScriptFileCommandSource& source, IScrVmScriptSwitchBoundary& scriptSwitch,
    IScrVmCommandErrorSink& errors, IEnvironmentRootMemory& memory, IRuntimeStringManagerSource& strings)
{
    constexpr char prefix[] = "CmdCall\x8F\x88\x97\x9D\x82\xAA\x8E\xB8\x94\x73"
        "\x82\xB5\x82\xDC\x82\xB5\x82\xBD\x81\x49 : "; // PE5B5F98。
    static_assert(sizeof(prefix) == 31);
    return ExecuteWithReport(prefix, errors, strings, [&] {
        auto command = CreateCommand(runtime.NativeParserAtA0(), source, memory);
        CommandUnwind cleanup{&command.reference};
        std::int32_t target;
        if (Operand<std::uint8_t>(command, 4) & 2u)
        {
            const auto index = Operand<std::int32_t>(command, 5);
            const auto root = source.CaptureAfterEnsureAt413BE0();
            target = ReadScriptValueAt4C73A0(root.paramsAt47BDB0(root.object), index, errors);
        }
        else if (Operand<std::uint8_t>(command, 4) & 4u)
            target = Operand<std::int32_t>(command, 5);
        else throw 1;

        const auto root = source.CaptureAfterEnsureAt413BE0();
        const auto returnPc = runtime.StagedProgramCounter(); // 4CD3D4：在Params getter之前。
        auto& params = root.paramsAt47BDB0(root.object);
        void* allocation = memory.AllocateBody(0x0C);
        // 4CD3F4：分配之后从此前捕获的Params取当前scriptId，不重取root。
        auto* body = allocation ? ::new (allocation) ScriptStackFrameNative(
            params.SaveParams2At30().scriptFileIdAtSaveParams04, returnPc) : nullptr;
        auto& frames = params.FileCallStackAt18();
        YtlReferenceSlotRuntime reference{};
        CreateScriptStackReference(reference, body, memory);
        StackUnwind temporary{&reference};
        frames.Append(reference, memory);
        auto* const captured = reference.controlAt08;
        temporary.reference = nullptr;
        ReleaseCapturedScriptStackControl(captured); // state5，正常释放不重读Ref。
        SwitchOrThrow(runtime, target, scriptSwitch, errors);
        const auto next = source.CaptureAfterEnsureAt413BE0();
        runtime.SetStagedProgramCounter(next.paramsAt47BDB0(next.object).programCounter);
    });
}

std::int32_t ExecuteScriptFileReturnAt4D08A0(ScrScriptRuntimeState& runtime,
    IScriptFileCommandSource& source, IScrVmScriptSwitchBoundary& scriptSwitch,
    IScrVmCommandErrorSink& errors, IRuntimeStringManagerSource& strings)
{
    constexpr char prefix[] = "CmdRtn\x8F\x88\x97\x9D\x82\xAA\x8E\xB8\x94\x73"
        "\x82\xB5\x82\xDC\x82\xB5\x82\xBD\x81\x49 : "; // PE5B5FB8。
    static_assert(sizeof(prefix) == 30);
    return ExecuteWithReport(prefix, errors, strings, [&] {
        const auto root = source.CaptureAfterEnsureAt413BE0();
        YtlReferenceSlotRuntime frame{};
        root.paramsAt47BDB0(root.object).FileCallStackAt18().PopReferenceAt4D4B40(frame);
        StackUnwind cleanup{&frame};
        if (!frame.baseAt00) throw 1;
        const auto scriptId = ResolveFrame(frame).scriptIdAt04;
        SwitchOrThrow(runtime, scriptId, scriptSwitch, errors);
        // 4D0966：LoadScript之后重新解析保活Ref，既不缓存正文，也不缓存返回PC。
        runtime.SetStagedProgramCounter(ResolveFrame(frame).returnPcAt08);
    });
}

std::int32_t ExecuteScriptFileJumpAt4CDD90(ScrScriptRuntimeState& runtime,
    IScriptFileCommandSource& source, IScrVmScriptSwitchBoundary& scriptSwitch,
    IScrVmCommandErrorSink& errors, IEnvironmentRootMemory& memory, IRuntimeStringManagerSource& strings)
{
    constexpr char prefix[] = "CmdExec\x8F\x88\x97\x9D\x82\xAA\x8E\xB8\x94\x73"
        "\x82\xB5\x82\xDC\x82\xB5\x82\xBD\x81\x49 : "; // PE5B5F5C。
    static_assert(sizeof(prefix) == 31);
    return ExecuteWithReport(prefix, errors, strings, [&] {
        auto command = CreateCommand(runtime.NativeParserAtA0(), source, memory);
        CommandUnwind cleanup{&command.reference};
        const auto flags = Operand<std::uint8_t>(command, 4); // 4CDE85：整个入口只缓存这一次。
        std::int32_t target;
        if (flags & 2u)
        {
            const auto index = Operand<std::int32_t>(command, 5);
            const auto root = source.CaptureAfterEnsureAt413BE0();
            target = ReadScriptValueAt4C73A0(root.paramsAt47BDB0(root.object), index, errors);
        }
        else if (flags & 4u) target = Operand<std::int32_t>(command, 5);
        else throw 1;

        const auto localRoot = source.CaptureAfterEnsureAt413BE0();
        localRoot.paramsAt47BDB0(localRoot.object).LocalCallStackAt24().clear();
        if (flags & 0x80u)
        {
            const auto fileRoot = source.CaptureAfterEnsureAt413BE0();
            fileRoot.paramsAt47BDB0(fileRoot.object).FileCallStackAt18().clear();
        }
        auto targetPc = target;
        if (flags & 0x10u)
        {
            if (flags & 0x20u) targetPc = Operand<std::int32_t>(command, 9);
            else if (flags & 0x40u)
            {
                const auto index = Operand<std::int32_t>(command, 9);
                const auto root = source.CaptureAfterEnsureAt413BE0();
                targetPc = ReadScriptValueAt4C73A0(root.paramsAt47BDB0(root.object), index, errors);
            }
        }
        SwitchOrThrow(runtime, target, scriptSwitch, errors);
        if ((flags & 0x10u) && targetPc >= 0)
        {
            const auto writeRoot = source.CaptureAfterEnsureAt413BE0();
            writeRoot.paramsAt47BDB0(writeRoot.object).programCounter = static_cast<std::uint32_t>(targetPc);
        }
        const auto readRoot = source.CaptureAfterEnsureAt413BE0();
        runtime.SetStagedProgramCounter(readRoot.paramsAt47BDB0(readRoot.object).programCounter);
    });
}
}
