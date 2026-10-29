#include <Runtime/PreSceneEngineUpdateRuntime.h>

namespace KaGuYa::Start::Runtime
{
PreSceneEngineUpdateServicesRuntime::PreSceneEngineUpdateServicesRuntime(
    IMesWinButtonsUpdateBoundary& mesWinButtons,
    SoundMappingRestoreRuntime& soundMappingRestore) noexcept
    : mesWinButtons_(&mesWinButtons), soundMappingRestore_(&soundMappingRestore)
{
}

void PreSceneEngineUpdateServicesRuntime::InvokeMesWinButtons434FB0(
    const std::int32_t argument)
{
    mesWinButtons_->Invoke434FB0(argument);
}

void PreSceneEngineUpdateServicesRuntime::RestoreSoundMappings()
{
    // gate 已由 PreSceneEngineUpdateRuntime 按 CSaveData+0x38 bit 0x20 判定。
    soundMappingRestore_->Restore(true);
}

PreSceneEngineUpdateRuntime::PreSceneEngineUpdateRuntime(
    IPreSceneEngineUpdateState& state,
    IPreSceneEngineUpdateServices& services) noexcept
    : state_(&state), services_(&services)
{
}

void PreSceneEngineUpdateRuntime::Update()
{
    // 46D448..46D49B：只有配置描述项 2 的 +0x14 低两位等于 3 才继续。
    if ((state_->ConfigDescriptor2Field14() & 3u) != 3u)
        return;

    // 46D4A1..46D53F：原函数按此顺序短路检查四个触发源。
    if (!state_->InputDescriptor0Byte13() &&
        !state_->InputDescriptor0Byte14() &&
        !state_->InputDescriptor2Action(5u) &&
        !state_->InputDescriptor2Action(0u))
    {
        return;
    }

    // 46D545..46D611：这些写入没有事务或回滚；下游抛出时必须保留此前副作用。
    state_->MaskConfigDescriptor2Field14(1u);
    services_->InvokeMesWinButtons434FB0(0);
    state_->SetInputDescriptor0Byte14(0u);
    state_->SetInputDescriptor0Byte13(0u);
    state_->SetInputDescriptor0Field28(1u);

    // 46D618..46D628：配置描述项 1 的 +0x38 bit 0x20 门控后半段同步循环。
    if (state_->ConfigDescriptor1Field38Bit20())
        services_->RestoreSoundMappings();
}
}
