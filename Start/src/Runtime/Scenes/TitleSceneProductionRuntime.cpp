#include <Runtime/TitleSceneProductionRuntime.h>

namespace KaGuYa::Start::Runtime
{
TitleSceneProductionRuntime::TitleSceneProductionRuntime(TitleSceneProductionDependencies dependencies)
{
    dependencies.publicDialogs.ClearAt46DB90();
    dialog_ = std::make_unique<TitleDialogProductionRuntime>(dependencies.dialog);
    auto& state = dependencies.construction;
    state.WriteSlot2DwordAt04(0);
    state.WriteSlot2ByteAt24(1);
    state.WriteSlot2ByteAt25(1);
    state.WriteSlot2ByteAt26(1);
    state.WriteSlot2ByteAt27(0);
    state.WriteSlot2ByteAt08(0);
    state.WriteSlot2ByteAt09(1);
    state.WriteSlot2ByteAt0C(0);
    state.WriteSlot2DwordAt18(0);
    state.WriteSlot2DwordAt14(1);
    state.WriteSlot2DwordAt1C(1);
    state.WriteSlot2ByteAt29(0);
    state.WriteSlot2ByteAt28(0);
    // 4A2DA7 才置 stage=0；TitleDialog 的 phase/result 仍等首次 Update 初始化。
    scene_ = std::make_unique<TitleSceneRuntime>(*dialog_, dependencies.scene, dependencies.menu);
    dependencies.menu.SetRootByteAt218(0);
    dependencies.menu.SetRootByteAt219(1);
    state.ClearReplayAt4A2DD6();
}

TitleSceneConstructionServicesSample18::TitleSceneConstructionServicesSample18(
    ISceneConstructionServices& upstream, TitleSceneProductionDependencies dependencies) noexcept
    : SceneConstructionServicesForwarderRuntime(upstream), dependencies_(dependencies)
{
}

std::shared_ptr<ISceneTask> TitleSceneConstructionServicesSample18::CreateTitle()
{
    auto owner = std::make_shared<TitleSceneProductionRuntime>(dependencies_);
    return std::shared_ptr<ISceneTask>(owner, &owner->Scene());
}
}
