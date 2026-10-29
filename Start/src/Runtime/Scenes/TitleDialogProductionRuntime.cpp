#include <Runtime/TitleDialogProductionRuntime.h>

namespace KaGuYa::Start::Runtime
{
TitleDialogProductionRuntime::TitleDialogProductionRuntime(TitleDialogProductionDependencies dependencies)
    : dependencies_(dependencies), input_(*this), render_(dependencies.graphics)
{
    try
    {
        inputState_ = std::make_unique<NullGuiInputStateRuntime>();
        dirty_ = CreateCgDisplayButtonGroupDirtyRectsAt4A2E60();
        children_ = std::make_unique<TitleDialogChildObjectGraphRuntime>();
        children_->PopulateChildrenAt4F4530(dependencies.surfaces, input_);
        auto black = dependencies.surfaces.LoadStatic("bg_black.bgd");
        if (!black) throw 1;
        black_ = std::make_shared<GraphicsTransitionSourceRuntime>(std::move(black));
        auto canvas = dependencies.surfaces.CreateBlank(dependencies.screenWidth, dependencies.screenHeight, false, true);
        if (!canvas) throw 1;
        canvas_ = std::make_shared<GraphicsTransitionSourceRuntime>(std::move(canvas));
        // 4F4ACA -> 536EA0：先分配新对象，然后释放 Null 并安装 GameInputState。
        auto gameInput = std::make_unique<GameGuiInputStateRuntime>(dependencies.mouse, dependencies.keyboard);
        inputState_.reset();
        inputState_ = std::move(gameInput);
        navigation_ = CreateTitleDialogNavigationSample18At6083E0();
        notifications_ = std::make_unique<TitleDialogVisualNotificationsRuntime>(
            static_cast<ITitleDialogVisualServices&>(*this), *children_->ClickedEventsAtC8());
        state_ = std::make_unique<TitleDialogRuntime>(static_cast<ITitleDialogServices&>(*this),
            black_, canvas_, RECT{0, 0, dependencies.screenWidth, dependencies.screenHeight});
    }
    catch (...)
    {
        // EH 5FA31C 的状态 3→2→1→0：事件、画布、黑图、GuiDialog 基类。
        ReleaseAt4FE690();
        throw;
    }
}

TitleDialogProductionRuntime::~TitleDialogProductionRuntime() { ReleaseAt4FE690(); }

void TitleDialogProductionRuntime::ReleaseAt4FE690() noexcept
{
    // C++ 状态核心只借用 Dialog 行为，但持有转场来源；先撤掉这些适配层引用。
    state_.reset();
    notifications_.reset();
    if (children_) children_->ReleaseClickedEventsAtC8();
    canvas_.reset();
    black_.reset();
    dirty_.reset();
    for (auto& group : navigation_) group.reset();
    navigation_.clear();
    inputState_.reset();
    if (children_) children_->ReleaseChildrenAt0C();
    children_.reset();
}

void TitleDialogProductionRuntime::InvokeVslot44() { state_->InvokeVslot44(); }
bool TitleDialogProductionRuntime::Run() { return state_->Run(); }
std::uint32_t TitleDialogProductionRuntime::SelectionResult() const { return state_->SelectionResult(); }
void TitleDialogProductionRuntime::RedrawAllAt530E60() { state_->RedrawAllAt530E60(); }
void TitleDialogProductionRuntime::ClearClickedEventsAt532200() { children_->ClickedEventsAtC8()->ClearAllAt532200(); }
void TitleDialogProductionRuntime::WriteDialogEnabledAt538660(std::int32_t v) { render_.WriteEnabledAt38(v); }
void TitleDialogProductionRuntime::ResetInputAt510840() { input_.ResetAt510840(); }
void TitleDialogProductionRuntime::ResetChildrenAt522E90()
{
    for (const auto& child : children_->ChildrenInInsertionOrder()) child->InvokeAtVslot08();
}
void TitleDialogProductionRuntime::UpdateGuiDialogAt518C30()
{
    input_.UpdateAt518C30(render_.ModeAt34(), render_.EnabledAt38(),
        children_->ChildrenInInsertionOrder(), navigation_, *notifications_);
}
std::int32_t TitleDialogProductionRuntime::PollClickedEventAt50FDB0(std::int32_t v)
{
    return children_->ClickedEventsAtC8()->PollFirstAt50FDB0(v);
}
ICgDisplayButtonGroupDirtyRects& TitleDialogProductionRuntime::OwnDirtyRectsAtVslot00() { return *dirty_; }
void TitleDialogProductionRuntime::DrawChildrenAt5230B0(const RECT& clip)
{
    render_.DrawChildrenAt5230B0(children_->ChildrenInInsertionOrder(), clip);
}
void TitleDialogProductionRuntime::CompositeChildrenAt5231C0(
    const LoadAfterTransitionSourceRef& target, const RECT* clip)
{
    const auto surface = std::dynamic_pointer_cast<GraphicsTransitionSourceRuntime>(target);
    if (!surface) throw std::invalid_argument("Title composition target is not a Graphics surface");
    render_.CompositeChildrenAt5231C0(children_->ChildrenInInsertionOrder(),
        GraphicsSurfaceDestination::FromNative(surface->Surface().NativeBody()), clip);
}
void TitleDialogProductionRuntime::SetActiveControlAtVslot30(ICgOperationDialogEventTarget& child) { input_.SetActiveControlAtVslot30(child); }
bool TitleDialogProductionRuntime::ReadInputAtVslot6C(std::int32_t v) { return inputState_->ReadHeldAtVslot00(v); }
bool TitleDialogProductionRuntime::ReadInputAtVslot70(std::int32_t v) { return inputState_->ReadPressedAtVslot04(v); }
POINT TitleDialogProductionRuntime::ReadPointerAtVslot74() { return inputState_->ReadPointerAtVslot08(); }
bool TitleDialogProductionRuntime::ReadPressAtVslot80() { return inputState_->ReadMousePressAtVslot14(); }
bool TitleDialogProductionRuntime::ReadReleaseAtVslot84() { return inputState_->ReadMouseReleaseAtVslot18(); }
std::uint32_t TitleDialogProductionRuntime::ReadSurfacePixelAtGetSurfacePixel(GraphicsSurfaceHandle h, std::int32_t x, std::int32_t y)
{
    return dependencies_.pixels.GetSurfacePixel(h, x, y);
}
bool TitleDialogProductionRuntime::ReadPointerMovedAtVslot7C() { return inputState_->ReadPointerMovedAtVslot10(); }
void TitleDialogProductionRuntime::SubmitChildRectAt50FC80(const RECT& rect) { (void)dirty_->AddAtVslot08(rect); }
const char* TitleDialogProductionRuntime::ReadHoverSoundNameAt61A65C() { return dependencies_.sound.ReadHoverSoundNameAt61A65C(); }
std::int32_t TitleDialogProductionRuntime::ReadEffectVolumeAtVslot24() { return dependencies_.sound.ReadEffectVolumeAtVslot24(); }
void TitleDialogProductionRuntime::PlayEffectAt4BCD70(const char* n, std::int32_t m, std::int32_t v, std::int32_t l) { dependencies_.sound.PlayEffectAt4BCD70(n, m, v, l); }
void TitleDialogProductionRuntime::InvalidatedRect(const RECT& r) { dependencies_.external.InvalidatedRect(r); }
bool TitleDialogProductionRuntime::ReadInputAt4C3430() { return dependencies_.external.ReadInputAt4C3430(); }
bool TitleDialogProductionRuntime::ReadInputAt4AA970() { return dependencies_.external.ReadInputAt4AA970(); }
void TitleDialogProductionRuntime::RenderTransitionAt449DD0(LoadAfterTransitionOperation& op) { dependencies_.external.RenderTransitionAt449DD0(op); }
void TitleDialogProductionRuntime::CommitTransitionFrameAt4B02E0(std::int32_t v) { dependencies_.external.CommitTransitionFrameAt4B02E0(v); }
void TitleDialogProductionRuntime::WriteRegistrySlot2DwordAt04(std::uint32_t v) { dependencies_.external.WriteRegistrySlot2DwordAt04(v); }
std::int32_t TitleDialogProductionRuntime::ReadRegistrySlot1BgmVolumeAtVslot14() { return dependencies_.external.ReadRegistrySlot1BgmVolumeAtVslot14(); }
const char* TitleDialogProductionRuntime::ReadTitleBgmNameAt61A654() { return dependencies_.external.ReadTitleBgmNameAt61A654(); }
void TitleDialogProductionRuntime::WriteRegistrySlot2BgmNameAt2C(const char* n) { dependencies_.external.WriteRegistrySlot2BgmNameAt2C(n); }
void TitleDialogProductionRuntime::PlayBgmAt42BFE0(const char* n, std::int32_t v, std::int32_t l) { dependencies_.external.PlayBgmAt42BFE0(n, v, l); }
void TitleDialogProductionRuntime::ClearSoundManagerAt4384D0() { dependencies_.external.ClearSoundManagerAt4384D0(); }
void TitleDialogProductionRuntime::StopSoundNamedByRegistrySlot2At30() { dependencies_.external.StopSoundNamedByRegistrySlot2At30(); }
bool TitleDialogProductionRuntime::ReadInputAt47F6D0() { return dependencies_.external.ReadInputAt47F6D0(); }
bool TitleDialogProductionRuntime::ReadSceneManagerAt485CE0() { return dependencies_.external.ReadSceneManagerAt485CE0(); }
void TitleDialogProductionRuntime::PlayConfirmSoundAt4BCD70() { dependencies_.external.PlayConfirmSoundAt4BCD70(); }
std::uint32_t TitleDialogProductionRuntime::ReadObjectAt50A620DwordAt1C() { return dependencies_.external.ReadObjectAt50A620DwordAt1C(); }
std::uint8_t TitleDialogProductionRuntime::ReadRegistrySlot1ByteAt54() { return dependencies_.external.ReadRegistrySlot1ByteAt54(); }
std::shared_ptr<IModalDialogRuntimeTargetReference> TitleDialogProductionRuntime::CreateConfirmationAt4E4340() { return dependencies_.external.CreateConfirmationAt4E4340(); }
void TitleDialogProductionRuntime::PrepareModalAt4AD230() { dependencies_.external.PrepareModalAt4AD230(); }
std::int32_t TitleDialogProductionRuntime::RunModalAt5071D0(IModalDialogRuntimeTargetReference& target) { return dependencies_.external.RunModalAt5071D0(target); }
void TitleDialogProductionRuntime::WriteCurrentSceneName(const char* n) { dependencies_.external.WriteCurrentSceneName(n); }
void TitleDialogProductionRuntime::PostCloseMessage() { dependencies_.external.PostCloseMessage(); }
}
