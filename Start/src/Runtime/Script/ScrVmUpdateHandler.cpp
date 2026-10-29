#include <Runtime/ScrVmUpdateHandler.h>

#include <Runtime/ScrVmControlHandlers.h>
#include <Runtime/ScriptUpdateCommandNative.h>

#include <bit>
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
            "SCR VM update operand is truncated");
    }
}

template <typename T>
T Read(const ScrInstructionView& instruction, const std::size_t offset)
{
    RequireOperand(instruction, offset + sizeof(T));
    T value{};
    std::memcpy(&value, instruction.body.data() + offset, sizeof(T));
    return value;
}
}

ScrVmUpdateCommandException::ScrVmUpdateCommandException(std::string detail)
    : std::runtime_error(detail), detail_(std::move(detail))
{
}

std::string_view ScrVmUpdateCommandException::Detail() const noexcept
{
    return detail_;
}

ScrUpdateInstructionHandler::ScrUpdateInstructionHandler(
    IScrVmUpdateValueSource& values,
    const IScrResourceNameSource& resourceNames,
    IScrVmUpdateRuntimeBoundary& updateRuntime,
    ScrVmScriptStorage& storage,
    IScrVmUpdateCommandErrorSink& errorSink) noexcept
    : values_(values),
      resourceNames_(resourceNames),
      updateRuntime_(updateRuntime),
      storage_(storage),
      errorSink_(errorSink)
{
}

ScrUpdateInstructionHandler::ScrUpdateInstructionHandler(IScrVmUpdateValueSource& values,
    const IScrResourceNameSource& resourceNames, IScrVmUpdateRuntimeBoundary& updateRuntime,
    ScrVmScriptStorage& storage, IScrVmUpdateCommandErrorSink& errorSink,
    IScriptFileCommandSource& pcSource, IScrVmCommandErrorSink& nativeErrors) noexcept
    : ScrUpdateInstructionHandler(values, resourceNames, updateRuntime, storage, errorSink)
{
    pcSource_ = &pcSource;
    nativeErrors_ = &nativeErrors;
}

std::int32_t ScrUpdateInstructionHandler::ExecuteRuntimeEntry(
    ScrScriptRuntimeState& runtime, IEnvironmentRootMemory& memory)
{
    if (!pcSource_ || !nativeErrors_)
        throw std::logic_error("UPDATE native registration requires explicit PC/error sources");
    return ExecuteScriptUpdateAt4D2800(runtime, *pcSource_, values_, resourceNames_, updateRuntime_,
        storage_, *nativeErrors_, memory);
}

std::int32_t ScrUpdateInstructionHandler::Execute(
    ScrScriptRuntimeState&,
    const ScrInstructionView& instruction)
{
    std::string failureDetail;
    try
    {
        // 4D294B/4D295B/4D298E：参数在各自消费点读取，不跨外部回调缓存后续字段。
        updateRuntime_.ApplyResultLayerList(Read<std::int32_t>(instruction, 0u));
        updateRuntime_.SelectLayerEffect(Read<std::int32_t>(instruction, 4u));
        storage_.EffectAt4AD140().Reset();
        const std::uint8_t flags = Read<std::uint8_t>(instruction, 8u);

        if ((flags & 0x01u) != 0u)
        {
            storage_.EffectAt4AD140().status = static_cast<std::uint16_t>(
                Read<std::uint32_t>(instruction, 9u));
        }
        else if ((flags & 0x06u) != 0u)
        {
            // 4D2A16/4D2A3B/4D2A68先发布，再在4D2A9A取值；失败保留该前缀。
            storage_.EffectAt4AD140().modeFlags = static_cast<std::uint8_t>(
                storage_.EffectAt4AD140().modeFlags | 0x01u | (flags & 0x06u));
            storage_.EffectAt4AD140().status = std::uint16_t{0x8001};
            std::int32_t resourceId = Read<std::int32_t>(instruction, 9u);
            if ((flags & 0x02u) != 0u)
                resourceId = values_.ReadUpdateValueAt4C73A0(resourceId);

            // 4D2AA2取Pattern，4D2AC8..4D2AEE捕获effect目标，随后4D2AF4才查询名称。
            const auto names = resourceNames_.CapturePatternNames();
            auto& effect = storage_.EffectAt4AD140();
            const char* const name = names.FindNameAt4AD1B0(resourceId);
            // 4D2B04..4D2B31：null走零长度清名，非null按首NUL写入，不提前拒绝缺名。
            effect.SetResourceName(name);
        }
        else if ((flags & 0x08u) != 0u)
        {
            storage_.EffectAt4AD140().modeFlags = static_cast<std::uint8_t>(
                storage_.EffectAt4AD140().modeFlags | 0x80u);

            // 4D2B70每轮读取目标数量；每个坐标取得后立即提交，后项失败不回滚前项。
            // 此处不按活动层数改尺寸，原数组由4C8FC7..4C90D8按配置数量建立。
            // 4D2B83使用signed JGE；control count的高位不能变成巨大的正数循环。
            for (std::int32_t index = 0; index < ScriptPairCount(storage_.EffectAt4AD140()); ++index)
            {
                ScrVmUpdatePair pair;
                if (!updateRuntime_.ReadActiveResultPair(index, pair)) throw 1; // 4D2F91。
                storage_.EffectAt4AD140().PairAt(index) = pair;
            }

            // 4D2C4C在基础坐标复制完成后才读额外条目数；逐条解析，不提前拒绝整批。
            const std::uint8_t count = Read<std::uint8_t>(instruction, 9u);
            for (std::size_t item = 0; item < count; ++item)
            {
                const std::size_t offset = 10u + item * 5u;
                const std::uint8_t pairIndex = Read<std::uint8_t>(instruction, offset);
                const ScrVmUpdatePair pair{Read<std::int16_t>(instruction, offset + 1u),
                    Read<std::int16_t>(instruction, offset + 3u)};
                if (static_cast<std::int32_t>(pairIndex) >= ScriptPairCount(storage_.EffectAt4AD140()))
                    throw 1; // 4D2D67/4D2D69也是signed JGE。

                storage_.EffectAt4AD140().PairAt(pairIndex) = pair;
            }
            storage_.EffectAt4AD140().status = std::uint16_t{12};
        }
        else if (flags != 0u)
        {
            throw 1;
        }

        // bit 0x10 与主模式复用 record+0x0D 的首 byte，并非追加 operand。
        storage_.EffectAt4AD140().auxiliaryByte = (flags & 0x10u) != 0u
            ? Read<std::uint8_t>(instruction, 9u)
            : std::uint8_t{0};
        return 3;
    }
    // PE 5F1B80/5F1B90：const char*（含空指针）后接catch-all；没有GetValue局部catch。
    catch (const char* detail)
    {
        failureDetail = detail ? detail : "";
    }
    catch (...) { failureDetail = SyntaxError; }
    // 4D2F15位于原try之外；错误格式化/显示失败不可被同一catch再次报告。
    errorSink_.ReportUpdateFailure(failureDetail);
    return 2;
}
}
