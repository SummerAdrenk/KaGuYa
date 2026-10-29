#include <Runtime/LoadAfterSelectionScene.h>
#include <Runtime/GameSceneRoutes.h>

#include <algorithm>

namespace KaGuYa::Start::Runtime
{
namespace
{
constexpr char kBlackTransitionResource[] = "bg_black.bgd";
constexpr char kTitleSceneCp932[] = "\x83\x5E\x83\x43\x83\x67\x83\x8B";
constexpr char kScriptSceneCp932[] =
    "\x83\x58\x83\x4E\x83\x8A\x83\x76\x83\x67\x8F\x88\x97\x9D";
constexpr char kMapSceneCp932[] =
    "\x83\x7D\x83\x62\x83\x76\x8F\x88\x97\x9D";
}

void LoadAfterTransitionOperation::Reset() noexcept
{
    // 0x4C57F0 的十二个槽；不能保留上一段 57/58 转场的 source 或进度。
    valueAt00 = 0;
    sourceAt04.reset();
    valueAt08 = 0;
    valueAt0C = 0;
    valueAt10 = 0;
    valueAt14 = 0;
    valueAt18 = 0;
    selectorAt1C = -1;
    valueAt20 = 0;
    progressAt24 = 0;
    maximumAt28 = 255;
    valueAt2C = 0;
}

LoadAfterSelectionConstructionException::LoadAfterSelectionConstructionException(
    const LoadAfterSelectionConstructionError error,
    const char* const message)
    : std::runtime_error(message), error_(error)
{
}

LoadAfterSelectionConstructionError
LoadAfterSelectionConstructionException::Error() const noexcept
{
    return error_;
}

LoadAfterSelectionScene::LoadAfterSelectionScene(
    ScrVmScriptCommandState& scriptCommandState,
    RestoredScriptTransaction& restoredScript,
    ILoadAfterSelectionServices& services)
    : scriptCommandState_(&scriptCommandState),
      restoredScript_(&restoredScript),
      services_(&services)
{
    // 原构造函数在资源成功前没有建立可运行状态。C++ 实现用显式构造失败暴露
    // 该原版缺陷，不生成替代黑图，也不返回一个带伪造默认状态的对象。
    blackTransitionSource_ =
        services_->LoadTransitionSource(kBlackTransitionResource);
    if (!blackTransitionSource_)
    {
        throw LoadAfterSelectionConstructionException(
            LoadAfterSelectionConstructionError::BlackTransitionResourceLoadFailed,
            "bg_black.bgd load failed while constructing CLoadAfter");
    }

    savedRegistrySlot2DwordAt04_ = services_->ReadRegistrySlot2DwordAt04();
    services_->WriteRegistrySlot2DwordAt04(0u);
    state_ = 0;
    services_->WriteRootByteAt218(1u);
    services_->WriteRootByteAt219(1u);
}

void LoadAfterSelectionScene::Run(const std::int32_t argument)
{
    static_cast<void>(argument);
    Update();
    Draw();
}

std::int32_t LoadAfterSelectionScene::State() const noexcept
{
    return state_;
}

const LoadAfterTransitionOperation& LoadAfterSelectionScene::Transition() const noexcept
{
    return transition_;
}

std::int32_t LoadAfterSelectionScene::ResolveTransitionStep(
    const std::uint32_t speedIndex) noexcept
{
    constexpr std::int32_t steps[] = {4, 8, 16, 32};
    return steps[speedIndex < 4u ? speedIndex : 0u];
}

void LoadAfterSelectionScene::InitializeTransition(
    const std::int32_t selector,
    const LoadAfterTransitionSourceRef& source)
{
    transition_.Reset();
    transition_.selectorAt1C = selector;
    transition_.sourceAt04 = source;
    transitionStep_ = ResolveTransitionStep(
        services_->ReadTransitionSpeedIndexAt70());
    ++state_;
}

void LoadAfterSelectionScene::AdvanceTransition()
{
    // 0x4B6418：gate 为零或 slot 5 输入成立时，先 +256 并截到 maximum。
    if (!services_->ReadTransitionAccelerationGateAt6F() ||
        services_->PollTransitionAccelerationAtSlot5())
    {
        transition_.progressAt24 += 256;
        if (transition_.progressAt24 >= transition_.maximumAt28)
            transition_.progressAt24 = transition_.maximumAt28;
    }

    // 原函数在绘制完成帧之前先推进 state；本帧仍必须执行 renderer 与提交。
    if (transition_.selectorAt1C == -1 ||
        transition_.progressAt24 >= transition_.maximumAt28)
    {
        ++state_;
    }

    services_->RenderTransition(transition_);

    transition_.progressAt24 += transitionStep_;
    if (transition_.progressAt24 >= transition_.maximumAt28)
        transition_.progressAt24 = transition_.maximumAt28;

    services_->CommitTransitionFrame(0);
}

void LoadAfterSelectionScene::RestoreNormalRoute(const std::int32_t route)
{
    const bool skipGroup3 = route == 3;
    switch (route)
    {
    case 3:
        services_->WriteSceneMarkerAt38(3);
        break;
    case 4:
        services_->WriteSceneMarkerAt38(1);
        break;
    case 5:
        services_->WriteSceneMarkerAt38(0);
        break;
    case 14:
        services_->WriteSceneMarkerAt38(2);
        break;
    default:
        return;
    }

    services_->SynchronizeRestoredGroup0BeforeGroup3();
    if (!skipGroup3)
        services_->RestoreEnvironmentGroup3(1);
    services_->SynchronizeRestoredGroup0AfterGroup3();
    services_->SynchronizeRestoredRootOwners();

    // 屏幕备份失败时不制造替代 Surface。空引用会原样进入 selector 58，保留
    // 原版在后续 renderer 解引用处暴露错误的行为。
    screenBackupSource_ = services_->CaptureScreenBackup();
    services_->ApplyPostRestoreScreenState(0);
    services_->ClearCurrentSceneName();
    InitializeTransition(58, screenBackupSource_);
}

void LoadAfterSelectionScene::RestoreImmediateRoute(
    const char* const secondSceneNameCp932)
{
    services_->WriteRegistrySlot2DwordAt04(1u);
    services_->ClearScenes();
    scriptCommandState_->WriteFlagsAt04(0u);
    // 0x4B6766/0x4B6806 写 dword +4，但 0x4B6769/0x4B6809 只写 byte +8。
    // 保留 +9..+0B，不能用整型赋零把相邻的原状态一并覆盖。
    scriptCommandState_->WriteReloadByteAt08(0u);

    // route 9/13 使用立即构造并追加 active；第二项位于尾部，因此先执行。
    // 第二次构造抛出时，第一项必须继续留在 active。
    services_->PushActiveScene(kScriptSceneCp932);
    services_->PushActiveScene(secondSceneNameCp932);
}

void LoadAfterSelectionScene::RestoreDefaultRoute()
{
    services_->ResetScriptForDefaultRoute();
    services_->ResetOwnerGroup3ForDefaultRoute();
    services_->ClearScenes();
    // 4B6853 -> 47E285 的 5AE938 是标题；正常 state 4 才排脚本处理。
    services_->QueueScene(kTitleSceneCp932);
}

void LoadAfterSelectionScene::Update()
{
    switch (state_)
    {
    case 0:
        InitializeTransition(57, blackTransitionSource_);
        return;

    case 1:
    case 3:
        AdvanceTransition();
        return;

    case 2:
    {
        // 0x4B64D3：声音清理和当前名称清理均发生在 LoadScript 前，失败不回滚。
        services_->WriteRootServiceDwordAt10(1u);
        services_->StopSoundManagerEntryNamedAt10();
        services_->ClearSoundManagerEntries();
        services_->StopRootSoundEntryNamedByRegistrySlot2At30();

        const std::int32_t route = restoredScript_->Execute();
        switch (route)
        {
        case 3:
        case 4:
        case 5:
        case 14:
            RestoreNormalRoute(route);
            return;

        case 9:
            RestoreImmediateRoute(kMapSceneCp932);
            return;

        case 13:
            RestoreImmediateRoute(GameFixedActionSceneName(ScrVmFixedAction::Code13).data());
            return;

        default:
            RestoreDefaultRoute();
            return;
        }
    }

    case 4:
        services_->WriteRegistrySlot2DwordAt04(1u);
        // QueueScene 必须先弹出当前 active，再向 pending 写入脚本场景。
        services_->QueueScene(kScriptSceneCp932);
        return;

    default:
        return;
    }
}

void LoadAfterSelectionScene::Draw() noexcept
{
    // 原 CLoadAfter::Draw 是严格空函数。
}

LoadAfterSelectionSceneConstructionServicesRuntime::
LoadAfterSelectionSceneConstructionServicesRuntime(
    ISceneConstructionServices& upstream,
    ScrVmScriptCommandState& scriptCommandState,
    RestoredScriptTransaction& restoredScript,
    ILoadAfterSelectionServices& services) noexcept
    : SceneConstructionServicesForwarderRuntime(upstream),
      scriptCommandState_(&scriptCommandState),
      restoredScript_(&restoredScript),
      services_(&services)
{
}

std::shared_ptr<ISceneTask>
LoadAfterSelectionSceneConstructionServicesRuntime::
CreateLoadAfterSelection()
{
    return std::make_shared<LoadAfterSelectionScene>(
        *scriptCommandState_, *restoredScript_, *services_);
}
}
