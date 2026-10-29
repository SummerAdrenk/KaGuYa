#include <Runtime/ScriptFollowCommandNative.h>
#include "ScriptFileCommandSupport.h"

#include <bit>

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace FileCommandDetail;
void WriteReturn(IScriptFileCommandSource& source, IScrVmCommandErrorSink& errors,
    const char* name, std::int32_t value, ScrScriptRuntimeState* stagedSource = nullptr)
{
    const auto root = source.CaptureAfterEnsureAt413BE0();
    const auto* table = source.CaptureSlot0At4180C0();
    const auto index = source.ResolveAt498020(table, name);
    auto& params = root.paramsAt47BDB0(root.object);
    // 4CE603..4CE60B：follow_point的+B0读取发生在Params getter之后。
    if (stagedSource) value = std::bit_cast<std::int32_t>(stagedSource->StagedProgramCounter());
    WriteScriptValueAt4C8220(params, index, value, errors);
}
}

std::int32_t ExecuteScriptFollowAt4CE3E0(ScrScriptRuntimeState& runtime,
    IScriptFileCommandSource& source, IScrVmScriptSwitchBoundary& scriptSwitch,
    IScrVmCommandErrorSink& errors, IEnvironmentRootMemory& memory, IRuntimeStringManagerSource& strings)
{
    constexpr char prefix[] = "CmdFollow\x8F\x88\x97\x9D\x82\xAA\x8E\xB8\x94\x73"
        "\x82\xB5\x82\xDC\x82\xB5\x82\xBD\x81\x49 : ";
    static_assert(sizeof(prefix) == 33);
    return ExecuteWithReport(prefix, errors, strings, [&] {
        auto command = CreateCommand(runtime.NativeParserAtA0(), source, memory);
        CommandUnwind cleanup{&command.reference};
        std::int32_t target;
        if (Operand<std::uint8_t>(command, 4) & 2u)
        {
            const auto raw = Operand<std::int32_t>(command, 5);
            const auto root = source.CaptureAfterEnsureAt413BE0();
            target = ReadScriptValueAt4C73A0(root.paramsAt47BDB0(root.object), raw, errors);
        }
        else if (Operand<std::uint8_t>(command, 4) & 4u)
            target = Operand<std::int32_t>(command, 5);
        else throw 1;

        const auto returnRoot = source.CaptureAfterEnsureAt413BE0();
        const auto returnId = returnRoot.paramsAt47BDB0(returnRoot.object).SaveParams2At30().scriptFileIdAtSaveParams04;
        WriteReturn(source, errors, "follow_return", returnId);
        WriteReturn(source, errors, "follow_point", 0, &runtime);
        WriteReturn(source, errors, "game_end", 3);
        const auto result = scriptSwitch.Switch(runtime, target);
        if (!result.success) { if(!result.failureAlreadyReported)Show(errors, result.message.c_str());throw 1; }
        const auto nextRoot = source.CaptureAfterEnsureAt413BE0();
        runtime.SetStagedProgramCounter(nextRoot.paramsAt47BDB0(nextRoot.object).programCounter);
    });
}
}
