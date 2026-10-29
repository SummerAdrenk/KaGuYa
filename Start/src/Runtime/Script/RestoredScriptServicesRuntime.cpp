#include <Runtime/RestoredScriptServicesRuntime.h>

#include <Runtime/DrawManagerResourceRuntime.h>
#include <Runtime/GraphicsRuntimeApi.h>
#include <Runtime/NormalStartupScriptServices.h>
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/ScrVmCore.h>
#include <Runtime/ScrVmExecutableProfile.h>
#include <Runtime/ScrVmValueTable.h>
#include <Runtime/UnSaveDataRuntime.h>

#include <bit>
#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
RestoredScriptServicesRuntime::RestoredScriptServicesRuntime(RestoredScriptServicesDependencies dependencies)
    : dependencies_(std::move(dependencies))
{
    if (!dependencies_.generation)
        throw std::invalid_argument("restored script params generation slot is null");
    if (!dependencies_.script.UsesParamsGenerationSlot(dependencies_.generation))
        throw std::invalid_argument("restored script runtime uses a different params generation slot");
    // 构造只核对来源，不能借当前正文地址比较提前创建CParams。
    if (!dependencies_.values.UsesParamsGenerationSlot(dependencies_.generation))
        throw std::invalid_argument("restored script value table uses a different params generation slot");
}

std::int32_t RestoredScriptServicesRuntime::ReadRestoredScriptId() const
{
    // 4D4CF9：CSaveParams2+04。不是当前解析正文 ID，也不是 CParams+60。
    return dependencies_.script.EnvironmentScriptId();
}

ScrScriptSwitchResult RestoredScriptServicesRuntime::LoadScript(const std::int32_t scriptId)
{
    auto result = dependencies_.script.LoadScript(scriptId, dependencies_.resourceNames,
        dependencies_.catalog, dependencies_.transaction, dependencies_.files, dependencies_.key,
        dependencies_.codecs);
    // 4D3EE6 在失败返回之前显示；显示异常不伪装为成功或吞掉。
    if (!result.success) dependencies_.errors.Show(result.message.c_str());
    return result;
}

void RestoredScriptServicesRuntime::SynchronizeRootOwnersAtVslot18()
{
    dependencies_.rootOwners.SynchronizeAtVslot18();
}

const char* RestoredScriptServicesRuntime::ReadRegistrySlot1NameAt58() const
{
    return dependencies_.save.stringAt58.GetString();
}

const char* RestoredScriptServicesRuntime::ReadRegistrySlot1NameAt5C() const
{
    return dependencies_.save.stringAt5C.GetString();
}

void RestoredScriptServicesRuntime::SetGraphicsTextFirstName(const char* const nameCp932)
{
    dependencies_.graphics.SetTextFirstName(nameCp932);
}

void RestoredScriptServicesRuntime::SetGraphicsTextSecondName(const char* const nameCp932)
{
    dependencies_.graphics.SetTextSecondName(nameCp932);
}

void RestoredScriptServicesRuntime::WriteRegistrySlot2ByteAt0C(const std::uint8_t value)
{
    // 4D4D90 是 byte 写，不覆盖同一 DWORD 的高三字节。
    auto& field = dependencies_.unsave.DwordAt0C();
    field = (field & 0xFFFFFF00u) | value;
}

void RestoredScriptServicesRuntime::WriteRegistrySlot2DwordAt18(const std::uint32_t value)
{
    dependencies_.unsave.FlagsAt18() = value;
}

void RestoredScriptServicesRuntime::WriteRegistrySlot2DwordAt14(const std::uint32_t value)
{
    dependencies_.unsave.FlagsAt14() = value;
}

void RestoredScriptServicesRuntime::WriteRegistrySlot2DwordAt1C(const std::uint32_t value)
{
    dependencies_.unsave.FlagsAt1C() = value;
}

void RestoredScriptServicesRuntime::WriteRegistrySlot2DwordAt20(const std::uint32_t value)
{
    dependencies_.unsave.FlagsAt20() = value;
}

void RestoredScriptServicesRuntime::WriteRegistrySlot2ByteAt08(const std::uint8_t value)
{
    dependencies_.unsave.BytesAt08()[0] = value;
}

void RestoredScriptServicesRuntime::WriteRegistrySlot2ByteAt09(const std::uint8_t value)
{
    dependencies_.unsave.BytesAt08()[1] = value;
}

void RestoredScriptServicesRuntime::WriteRegistrySlot2ByteAt28(const std::uint8_t value)
{
    dependencies_.unsave.BytesAt24()[4] = value;
}

void RestoredScriptServicesRuntime::WriteRegistrySlot2ByteAt29(const std::uint8_t value)
{
    dependencies_.unsave.BytesAt24()[5] = value;
}

void RestoredScriptServicesRuntime::WriteScriptVariableReplay(const std::int32_t value)
{
    // 4D4E40..4D4E64：每次使用真实 profile 解析，再写同一当前代际的值表。
    dependencies_.values.WriteSigned(dependencies_.profile.ResolveNamedValue("replay"), value);
}

std::int32_t RestoredScriptServicesRuntime::ReadRestoredInstructionIndex() const
{
    // 4D4EA5：CurrentSaveSlot 的生产 API 转发到 generation 的 CSaveParams2+08。
    return dependencies_.script.CurrentSaveSlot();
}

const ScrContainer& RestoredScriptServicesRuntime::LoadedScript() const
{
    const auto* script = dependencies_.script.LoadedScript();
    if (!script) throw std::logic_error("restored script has no parsed SAVE table");
    return *script;
}

std::int32_t RestoredScriptServicesRuntime::ReadLoadedInstructionCount() const
{
    // 4D5EFD 写 +C0 的是 SAVE 表项数，不是扫描 opcode 得到的指令数量。
    return std::bit_cast<std::int32_t>(LoadedScript().SaveCount());
}

std::int32_t RestoredScriptServicesRuntime::ReadLoadedInstructionProgramCounter(
    const std::int32_t instructionIndex) const
{
    // 原 signed index<count 会放行负索引并越界读；现代边界显式报错，不回落首项/-1。
    if (instructionIndex < 0)
        throw std::out_of_range("restored script SAVE index is negative");
    return std::bit_cast<std::int32_t>(LoadedScript().SaveOffsetAt(static_cast<std::uint32_t>(instructionIndex)));
}

void RestoredScriptServicesRuntime::WriteEnvironmentProgramCounter(const std::int32_t programCounter)
{
    // 包括 -1/高位 offset，按原 DWORD 提交；实际取指合法性留给 VM 的解码边界。
    dependencies_.script.SetProgramCounter(std::bit_cast<std::uint32_t>(programCounter));
}

std::int32_t RestoredScriptServicesRuntime::DispatchCurrentOpcode()
{
    // 4D4F09 调 4D3FF0：执行到首个非零 route，不是只执行一条或代替外层处理 yield。
    return dependencies_.vm.ExecuteUntilYield(dependencies_.script);
}
}
