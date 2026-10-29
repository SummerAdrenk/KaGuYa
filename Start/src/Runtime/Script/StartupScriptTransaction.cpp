#include <Runtime/StartupScriptTransaction.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
void FinishStartup(ScrVmScriptCommandState& command,
    INormalStartupScriptTransactionServices& services)
{
    services.SynchronizeRootOwnersAtVslot18();
    // 4D53D0 先读 registry 名称，再分别进入 4D6CD0/4D6DB0 检查模块槽。
    // 模块不存在仍有两次 registry 读取，不能整个跳过或合并模块检查。
    const auto* firstName = services.ReadRegistrySlot1NameAt58();
    services.SetGraphicsTextFirstName(firstName);
    const auto* secondName = services.ReadRegistrySlot1NameAt5C();
    services.SetGraphicsTextSecondName(secondName);
    services.MirrorParamsDwordAt3CTo40();
    services.CallRegistrySlot4Nullsub3();
    command.WriteStateByteAt26(0u);
}
}
NormalStartupScriptTransaction::NormalStartupScriptTransaction(
    ScrVmScriptCommandState& scriptCommandState,
    INormalStartupScriptTransactionServices& services) noexcept
    : scriptCommandState_(&scriptCommandState), services_(&services)
{
}

ScrScriptSwitchResult NormalStartupScriptTransaction::Execute(
    const char* const scriptNameCp932)
{
    // 0x4D53E2..0x4D53F9：CParams 更换发生在模式字段与 registry 写入之前。
    services_->ReplaceParamsGeneration();
    scriptCommandState_->WriteFlagsAt04(0u);

    // 0x4D5400..0x4D54EE：顺序属于原函数的可见副作用，不能合并成原子配置写入。
    services_->WriteRegistrySlot2DwordAt04(1u);
    services_->WriteRegistrySlot2ByteAt0C(1u);
    services_->WriteRegistrySlot2DwordAt18(1u);
    services_->WriteRegistrySlot2DwordAt14(1u);
    services_->WriteRegistrySlot2DwordAt1C(1u);
    services_->WriteRegistrySlot2DwordAt20(1u);
    services_->WriteRegistrySlot2ByteAt08(1u);
    services_->WriteRegistrySlot2ByteAt09(1u);
    services_->WriteRegistrySlot2ByteAt28(1u);
    services_->WriteRegistrySlot2ByteAt29(0u);

    const std::int32_t scriptId = services_->ResolveScriptId(scriptNameCp932);
    ScrScriptSwitchResult result = services_->LoadScript(scriptId);
    if (!result.success)
        return result;

    services_->CopyRegistryPrologueFlag();
    FinishStartup(*scriptCommandState_, *services_);
    return result;
}

StartupScriptTransaction::StartupScriptTransaction(ScrVmScriptCommandState& command,
    IStartupScriptTransactionServices& services) noexcept
    : command_(&command), services_(&services)
{
}

ScrScriptSwitchResult StartupScriptTransaction::Execute(
    const char* const scriptNameCp932, const std::int32_t entry)
{
    if (entry == -1) return NormalStartupScriptTransaction(*command_, *services_).Execute(scriptNameCp932);

    services_->ReplaceParamsGeneration();
    command_->WriteFlagsAt04(2u);
    services_->WriteRegistrySlot2DwordAt04(2u);
    services_->WriteRegistrySlot2ByteAt0C(1u);
    services_->WriteRegistrySlot2DwordAt18(1u);
    services_->WriteRegistrySlot2DwordAt14(1u);
    services_->WriteRegistrySlot2DwordAt1C(1u);
    services_->WriteRegistrySlot2DwordAt20(1u);
    services_->WriteRegistrySlot2ByteAt29(0u);

    const auto selectedScript = services_->ResolveScriptId(scriptNameCp932);
    if (selectedScript < 0)
        return {.success = false, .error = ScrScriptSwitchError::MissingResourceName};
    services_->WriteReplayFile(selectedScript);
    services_->WriteReplayPosition(entry);
    // 原 4D3FA0 即使固定调度脚本未命中也调用 4D3C90(-1)，保留加载失败链。
    const auto dispatcher = services_->ResolveScriptId(Sample18ReplayStartupScriptCp932);
    auto result = services_->LoadScript(dispatcher);
    if (!result.success) return result;
    FinishStartup(*command_, *services_);
    return result;
}
}
