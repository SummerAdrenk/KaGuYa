#include <Runtime/RestoredScriptTransaction.h>

namespace KaGuYa::Start::Runtime
{
RestoredScriptTransaction::RestoredScriptTransaction(
    ScrVmScriptCommandState& scriptCommandState,
    IRestoredScriptTransactionServices& services) noexcept
    : scriptCommandState_(&scriptCommandState), services_(&services)
{
}

std::int32_t RestoredScriptTransaction::Execute()
{
    const ScrScriptSwitchResult loadResult =
        services_->LoadScript(services_->ReadRestoredScriptId());
    if (!loadResult.success)
        return 2;

    // 0x4D4D0E..0x4D4D7F：owner 同步和人名写入先于 registry 恢复。
    // 4D4D54/4D4D71 先读人名；两个包装器随后各自检查模块，不能合并门控。
    services_->SynchronizeRootOwnersAtVslot18();
    const char* const firstName = services_->ReadRegistrySlot1NameAt58();
    services_->SetGraphicsTextFirstName(firstName);
    const char* const secondName = services_->ReadRegistrySlot1NameAt5C();
    services_->SetGraphicsTextSecondName(secondName);

    // 0x4D4D80..0x4D4E2F：顺序属于可见副作用，任何后续失败均不回滚。
    services_->WriteRegistrySlot2ByteAt0C(1u);
    services_->WriteRegistrySlot2DwordAt18(1u);
    services_->WriteRegistrySlot2DwordAt14(1u);
    services_->WriteRegistrySlot2DwordAt1C(1u);
    services_->WriteRegistrySlot2DwordAt20(1u);
    services_->WriteRegistrySlot2ByteAt08(1u);
    services_->WriteRegistrySlot2ByteAt09(1u);
    services_->WriteRegistrySlot2ByteAt28(1u);
    services_->WriteRegistrySlot2ByteAt29(0u);

    services_->WriteScriptVariableReplay(0);
    scriptCommandState_->WriteStateByteAt26(1u);

    // 原函数只判断 index < count。负索引属于损坏状态，由生产数据边界显式报错，
    // 这里不添加会改变原控制流的非负兜底判断。
    const std::int32_t instructionIndex =
        services_->ReadRestoredInstructionIndex();
    std::int32_t programCounter = -1;
    if (instructionIndex < services_->ReadLoadedInstructionCount())
    {
        programCounter = services_->ReadLoadedInstructionProgramCounter(
            instructionIndex);
    }

    services_->WriteEnvironmentProgramCounter(programCounter);
    scriptCommandState_->WriteFlagsAt04(3u);

    // 不使用 scope guard：dispatcher 抛出时，原版会让 +0x26 保持 1。
    const std::int32_t route = services_->DispatchCurrentOpcode();
    scriptCommandState_->WriteStateByteAt26(0u);
    return route;
}
}
