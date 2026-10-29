#include <Runtime/ScrVmSceneHandler.h>

#include <Runtime/ParamsRuntimeProfile.h>
#include <Runtime/ScrVmControlHandlers.h>
#include <Runtime/ScriptControlCommandNative.h>

#include <bit>
#include <mbstring.h>

namespace KaGuYa::Start::Runtime
{
ScrSceneInstructionHandler::ScrSceneInstructionHandler(
    IScrVmSceneReplaySource& replay,
    IScrVmSceneSource& source,
    IScrVmSceneCommandErrorSink& errorSink,
    IScriptControlCommandSource* pcSource)
    : replay_(replay),
      source_(source),
      errorSink_(errorSink),
      pcSource_(pcSource)
{
    // 4C8E9A 仅注册 SCENE 入口；装配不查 Params 或 Environment 归档槽。
}

std::int32_t ScrSceneInstructionHandler::ExecuteRuntimeEntry(
    ScrScriptRuntimeState& runtime, IEnvironmentRootMemory& memory)
{
    // 独立VM明确绑定自身PC；生产root由装配方提供，不能提前解码而跳过replay门控。
    BoundScriptControlCommandSource bound(runtime);
    return ExecuteScriptSceneAt4D0CA0(runtime.NativeParserAtA0(), pcSource_ ? *pcSource_ : bound,
        replay_, source_, errorSink_, memory);
}

std::int32_t ScrSceneInstructionHandler::Execute(
    ScrScriptRuntimeState&,
    const ScrInstructionView& instruction)
{
    // 0x4D0D21..0x4D0D40：replay 在分配命令 operand 视图之前读取并短路。
    if (replay_.ReadReplayValue() != 0)
        return 6;

    if (instruction.body.size() < sizeof(std::uint16_t))
    {
        throw ScrVmOperandException(
            ScrVmOperandError::OperandTruncated,
            instruction.opcode,
            "SCR VM scene label index is truncated");
    }

    const std::uint16_t sceneLabelIndex = static_cast<std::uint16_t>(
        std::to_integer<std::uint16_t>(instruction.body[0]) |
        (std::to_integer<std::uint16_t>(instruction.body[1]) << 8u));
    return ApplyScriptSceneLabelAt4D0E39(sceneLabelIndex, source_, errorSink_);
}

std::int32_t ApplyScriptSceneLabelAt4D0E39(const std::uint16_t sceneLabelIndex,
    IScrVmSceneSource& source, IScrVmSceneCommandErrorSink& errorSink)
{
    // 4D0E69..4D0E7B是有符号除法/JGE；WORD索引零扩展，但count不能按unsigned比较。
    const auto count = std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(source.CurrentSceneLabelCount()));
    if (sceneLabelIndex >= count)
    {
        errorSink.ReportSceneLabelIndexOutOfRange(sceneLabelIndex);
        return 2;
    }

    // 4D0E7D 再取标签；4D0ED8 随即取得本次 SecretSC，即使循环数量为零。
    const char* const labelName = source.CurrentSceneLabelName(sceneLabelIndex);
    SecretScArchive* const secretSc = source.CurrentSecretSc();

    std::int32_t index = 0;
    while (index < std::bit_cast<std::int32_t>(source.CurrentGameSystem().sceneNames.size()))
    {
        // 4D6164..4D623D：本轮再次取 GameSystem，随后才检查标签 null。
        const ParamsGameSystem& gameSystem = source.CurrentGameSystem();
        if (labelName == nullptr)
        {
            // 4D6289 -> 413880(E_FAIL)，不是自定义runtime_error；空串仍合法。
            throw ATL::CAtlException(E_FAIL);
        }
        // 现代候选越界保护，不冒充原 null 标签异常。
        const char* const sceneName = gameSystem.sceneNames.at(index).GetString();
        if (_mbscmp(
                reinterpret_cast<const unsigned char*>(sceneName),
                reinterpret_cast<const unsigned char*>(labelName)) != 0)
        {
            ++index;
            continue;
        }

        // 0x4D625D..0x4D6273 只设置首个匹配 bit，不清除其他位。
        // 4D6268 首次解引用 receiver；现代在此拒绝 null，零数量/无匹配不拒绝。
        if (!secretSc)
            throw std::logic_error("SCR VM matched scene has a null SecretSC receiver (modern safety boundary)");
        // 原写入不检查边界；现代仅在实际写入点明确拒绝越界，不扩容或补默认值。
        if (static_cast<std::uint32_t>(index) >= secretSc->bitsAt0C.countAt0C)
            throw std::out_of_range("SCR VM scene name has no SecretSC flag");
        if (!secretSc->bitsAt0C.beginAt00)
            throw std::logic_error("SCR VM matched scene has null bit storage (modern safety boundary)");
        // 4D6268直接按+0C中的DWORD地址置位，不经过任何SecretSC虚槽。
        secretSc->bitsAt0C.beginAt00[static_cast<std::uint32_t>(index) >> 5] |=
            1u << (static_cast<std::uint32_t>(index) & 31u);
        break;
    }
    return 0;
}
}
