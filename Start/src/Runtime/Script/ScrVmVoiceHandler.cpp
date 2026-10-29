#include <Runtime/ScrVmVoiceHandler.h>
#include <Runtime/ScriptAudioCommandsNative.h>

#include <Runtime/ScrVmControlHandlers.h>

#include <cstring>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
constexpr std::string_view SyntaxError = "syntax error";

void RequireOperand(
    const ScrInstructionView& instruction,
    const std::size_t required)
{
    if (instruction.body.size() < required)
    {
        throw ScrVmOperandException(
            ScrVmOperandError::OperandTruncated,
            instruction.opcode,
            "SCR VM voice operand is truncated");
    }
}

std::int32_t ReadIndex(
    const ScrInstructionView& instruction,
    const std::size_t offset)
{
    RequireOperand(instruction, offset + sizeof(std::int32_t));
    std::int32_t value = 0;
    std::memcpy(&value, instruction.body.data() + offset, sizeof(value));
    return value;
}
}

ScrVmVoiceNameTable::ScrVmVoiceNameTable(std::vector<std::string> names)
    : names_(std::move(names))
{
}

const std::string& ScrVmVoiceNameTable::NameAt(const std::int32_t index) const noexcept
{
    static const std::string Empty;
    if (index < 0 || static_cast<std::size_t>(index) >= names_.size())
        return Empty;
    return names_[static_cast<std::size_t>(index)];
}

std::size_t ScrVmVoiceNameTable::Size() const noexcept
{
    return names_.size();
}

const char* ScrVmVoiceNameTable::FindVoiceNameAt4D3286(const std::int32_t index) const noexcept
{
    if (index < 0 || static_cast<std::size_t>(index) >= names_.size()) return nullptr;
    return names_[static_cast<std::size_t>(index)].c_str();
}

ScrVmVoiceCommandException::ScrVmVoiceCommandException(std::string detail)
    : std::runtime_error(detail), detail_(std::move(detail))
{
}

std::string_view ScrVmVoiceCommandException::Detail() const noexcept
{
    return detail_;
}

ScrVoiceInstructionHandler::ScrVoiceInstructionHandler(
    const IScrVmVoiceNameSource& voiceNames,
    IScrVmVoiceRuntimeBoundary& voiceRuntime,
    IScrVmVoiceCommandErrorSink& errorSink,
    std::pmr::memory_resource& temporaryMemory) noexcept
    : voiceNames_(voiceNames), voiceRuntime_(&voiceRuntime), errorSink_(&errorSink), temporaryMemory_(&temporaryMemory)
{
}

ScrVoiceInstructionHandler::ScrVoiceInstructionHandler(const IScrVmVoiceNameSource& names,
    IScriptFileCommandSource& pc, IScrVmCommandErrorSink& nativeErrors, IScriptVoiceSource& source) noexcept
    : voiceNames_(names), pcSource_(&pc), nativeErrors_(&nativeErrors), voiceSource_(&source) {}
std::int32_t ScrVoiceInstructionHandler::ExecuteRuntimeEntry(ScrScriptRuntimeState& runtime,
    IEnvironmentRootMemory& memory)
{
    if (!pcSource_ || !nativeErrors_ || !voiceSource_)
        throw std::logic_error("VOICE native entry requires PC/error/manager sources");
    return ExecuteScriptVoiceAt4D2FF0(runtime, *pcSource_, voiceNames_, *voiceSource_, *nativeErrors_, memory);
}
std::int32_t ScrVoiceInstructionHandler::Execute(
    ScrScriptRuntimeState&,
    const ScrInstructionView& instruction)
{
    if (!voiceRuntime_ || !errorSink_ || !temporaryMemory_)
        throw std::logic_error("VOICE offline entry requires an explicit offline manager/error/memory binding");
    auto& voice = *voiceRuntime_;
    std::string failureDetail;
    try
    {
        RequireOperand(instruction, 1u);
        const std::uint8_t count = std::to_integer<std::uint8_t>(instruction.body[0]);
        if (count == 0u)
        {
            // 0x4D311B..0x4D3164 直接进入逐槽 SoundStop + 清名称链。
            voice.StopAndClearAllVoiceSlots();
            return 0;
        }

        // 4D317F先建立指定数量的字符串槽；CString/Ref分配协议仍待替代现代容器。
        std::pmr::vector<std::string> resourceNames(count, temporaryMemory_);
        for (std::size_t index = 0; index < count; ++index)
        {
            // 4D31C3逐项读ID，4D31D1..3284重新取得Pattern；不预校验剩余整批输入。
            const char* const name = voiceNames_.FindVoiceNameAt4D3286(
                ReadIndex(instruction, 1u + index * sizeof(std::int32_t)));
            // 4D330C..332E：null按零长度写入，非null只复制首NUL之前的正文。
            resourceNames[index].assign(name ? name : "");
        }

        // 42C520..42C5B1：存在播放槽、准备状态和容量门槛均在停止旧槽之前。
        if (!voice.HasPlaybackSlots() || !voice.IsBatchObjectReadyAtA4() ||
            static_cast<std::int32_t>(count) > voice.VoiceBatchCapacityAt42C599())
        {
            return 0;
        }

        // 42C5DE先分配DWORD状态数组，42C5E5才停止旧槽；失败不得先停语音。
        std::pmr::vector<std::uint32_t> states(resourceNames.size(), 0u, temporaryMemory_);
        voice.StopAndClearAllVoiceSlots();
        for (std::size_t index = 0; index < resourceNames.size(); ++index)
        {
            const std::string& resourceName = resourceNames[index];
            if (!voice.PrepareSlotAtA8(index, resourceName))
            {
                states[index] = 1u;
                continue;
            }

            std::int32_t resolvedValue =
                voice.ReadSlotValueAtAC(index, resourceName);
            if (resolvedValue < 0)
            {
                try
                {
                    // 42C6EA..42C6FA的局部捕获只覆盖fallback，不吞AC/Open/Play异常。
                    resolvedValue = voice.ReadFallbackValueAt6C(index, resourceName);
                }
                catch (...) { continue; }
            }

            // 42C6FD..42C728：资源值和全局 fallback 均为负数时保持状态 0，
            // 不进入 42BC90，也不影响同批次其他语音的第二遍提交。
            if (resolvedValue < 0)
                continue;

            if (voice.OpenSlot(index, resourceName, resolvedValue))
                states[index] = 3u;
        }

        // 0x42C7EB..0x42C9B4：全部打开尝试结束后，第二遍才 Play 并保存名称。
        for (std::size_t index = 0; index < states.size(); ++index)
        {
            if (states[index] != 3u)
                continue;
            voice.PlayVoiceSlot(index);
            voice.StoreVoiceSlotResourceName(index, resourceNames[index]);
        }
        return 0;
    }
    // PE 5F1E7C/5F1E8C：const char*后接catch-all，不按现代异常类提取Detail。
    catch (const char* detail) { failureDetail = detail ? detail : ""; }
    catch (...) { failureDetail = SyntaxError; }
    // 4D3406..4D3436位于原try之外；连接/显示异常必须外传，不重复报告。
    errorSink_->ReportVoiceFailure(failureDetail);
    return 2;
}
}
