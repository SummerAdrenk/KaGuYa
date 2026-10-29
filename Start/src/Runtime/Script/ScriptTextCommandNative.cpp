#include <Runtime/ScriptTextCommandNative.h>
#include <Runtime/ScrVmTextHandler.h>
#include <Runtime/ScriptSoundGroupSource.h>
#include <Runtime/ScriptParamsHistoryNative.h>
#include <Runtime/GameBuildProfile.h>
#include "ScriptFileCommandSupport.h"

#include <bit>

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace FileCommandDetail;
const ParamsGameSystem& CaptureGameSystem(IScriptSoundGroupSource& source)
{
    auto* const root = source.CaptureAt418480();
    if (!root || !root->GameSystem())
        throw std::logic_error("TEXT null params/GameSystem (modern safety boundary)");
    return *root->GameSystem();
}
const ScriptControlCommandNative* CaptureBody(const Command& command) noexcept
{
    return ResolveEnvironmentObjectReference<ScriptControlCommandNative>(command.reference);
}
std::int32_t CapturedOperand(const Command& command, const ScriptControlCommandNative* body, std::uint32_t field)
{
    // 4D2359/4D23E9之前已解命令Ref；两次GameSystem取源后才读该正文的source。
    if (!body || !body->source)
        throw std::logic_error("TEXT null command/source (modern safety boundary)");
    const auto pointer = reinterpret_cast<std::uintptr_t>(body->source);
    const auto offset = static_cast<std::uint32_t>(pointer - command.base);
    if (command.knownExtent && std::uint64_t{offset} + field + 4u > command.extent)
        throw std::out_of_range("TEXT operand outside allocation (modern safety boundary)");
    std::int32_t result;
    std::memcpy(&result, reinterpret_cast<const void*>(pointer + field), sizeof(result));
    return result;
}
std::int32_t TailOperand(const Command& command, IScriptSoundGroupSource& source, std::uint32_t base)
{
    const auto* const body = CaptureBody(command);
    const auto& first = CaptureGameSystem(source);
    const auto& second = CaptureGameSystem(source);
    return CapturedOperand(command, body, base + 4u * (second.soundSlotCounts[1] + first.soundSlotCounts[3]));
}
}

std::int32_t ExecuteScriptTextAt4D1E50(ScrScriptRuntimeState& runtime,
    ScrVmScriptCommandState& state, ScrMusicInstructionHandler& music,
    IScrVmTextRuntimeBoundary& text, const ScriptTextCommandNativeContext& context,
    IEnvironmentRootMemory& memory, IRuntimeStringManagerSource& strings)
{
    constexpr char prefix[] = "CmdText\x8F\x88\x97\x9D\x82\xAA\x8E\xB8\x94\x73"
        "\x82\xB5\x82\xDC\x82\xB5\x82\xBD\x81\x49 : ";
    static_assert(sizeof(prefix) == 31);
    const auto result = ExecuteWithReport(prefix, context.errors, strings, [&] {
        // Baku 4CF5D0直接进入TEXT记录构造，没有新版restart_point写出前缀。
        // 仅关闭未提供的脚本变量出口；下方正文、重载、存档索引和历史仍完整执行。
        if (CompiledGameProfile().restartPointEnabled)
        {
            const auto oldRoot = context.pc.CaptureAfterEnsureAt413BE0();
            const auto oldPc = oldRoot.paramsAt47BDB0(oldRoot.object).programCounter;
            const auto* const names = context.pc.CaptureSlot0At4180C0();
            const auto writeRoot = context.pc.CaptureAfterEnsureAt413BE0();
            const auto index = context.pc.ResolveAt498020(names, "restart_point");
            WriteScriptValueAt4C8220(writeRoot.paramsAt47BDB0(writeRoot.object), index,
                std::bit_cast<std::int32_t>(oldPc), context.errors);
        }

        auto command = CreateCommand(runtime.NativeParserAtA0(), context.pc, memory, ScriptControlCommandKind::Text);
        CommandUnwind cleanup{&command.reference};
        const auto format = Operand<std::int32_t>(command, 4);
        const auto formatRoot = context.pc.CaptureAfterEnsureAt413BE0();
        formatRoot.paramsAt47BDB0(formatRoot.object).messageFormatIndexAt60 = format;
        if ((state.ReadFlagsAt04() & 2u) || state.ReadReloadByteAt08())
        {
            text.ApplyResultLayerList(Operand<std::int32_t>(command, 8));
            music.ApplyNativeResourceId(Operand<std::int32_t>(command, 12));
            for (std::uint32_t i = 0; std::bit_cast<std::int32_t>(i) <
                std::bit_cast<std::int32_t>(CaptureGameSystem(context.groups).soundSlotCounts[3]); ++i)
            {
                ApplyScriptFirstSoundAt4D6640(Operand<std::int32_t>(command, 16u + 4u * i), i,
                    context.names, context.soundLeaves, context.soundGate);
            }
            for (std::uint32_t i = 0; std::bit_cast<std::int32_t>(i) <
                std::bit_cast<std::int32_t>(CaptureGameSystem(context.groups).soundSlotCounts[1]); ++i)
            {
                const auto& first = CaptureGameSystem(context.groups);
                const auto* const body = CaptureBody(command);
                const auto id = CapturedOperand(command, body, 16u + 4u * (first.soundSlotCounts[3] + i));
                ApplyScriptSecondSoundAt4D62A0(id, i, context.names, context.groups,
                    context.soundLeaves, context.soundGate);
            }
            state.WriteReloadByteAt08(0);
        }
        text.SelectEffect(TailOperand(command, context.groups, 16));
        if ((state.ReadFlagsAt04() & 1u) == 0)
        {
            const auto saveIndex = TailOperand(command, context.groups, 20);
            const auto saveRoot = context.pc.CaptureAfterEnsureAt413BE0();
            saveRoot.paramsAt47BDB0(saveRoot.object).SaveParams2At30().instructionIndexAtSaveParams08 = saveIndex;
            const auto historyRoot = context.pc.CaptureAfterEnsureAt413BE0();
            auto& params = historyRoot.paramsAt47BDB0(historyRoot.object);
            const auto capturedFormat = static_cast<std::uint32_t>(params.messageFormatIndexAt60);
            AppendScriptParamsHistoryAt4C81F0(params.historyAt3C, capturedFormat, memory);
        }
        // 4D2496/4C9A30都在Destroy后重读slot.control，且释放抛出必须terminate。
        UnwindScriptControlCommand(command.reference);
    });
    return result == 0 ? 5 : result;
}
}
