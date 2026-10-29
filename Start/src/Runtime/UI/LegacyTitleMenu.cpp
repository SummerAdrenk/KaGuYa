#include <Runtime/LegacyTitleMenu.h>
#include <Runtime/CgUiButtonSurfaceAppendNative.h>
#include <bit>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Child = CgUiButtonNativeBody6Ch;
template<class T> T Read(const T& value) noexcept
{ return *static_cast<const volatile T*>(&value); }
LONG Add(LONG a, LONG b) noexcept
{ return std::bit_cast<LONG>(std::uint32_t(a) + std::uint32_t(b)); }
LONG Sub(LONG a, LONG b) noexcept
{ return std::bit_cast<LONG>(std::uint32_t(a) - std::uint32_t(b)); }
RECT Bounds(Child& button)
{
    RECT result{0, 0, Read(button.extentAt38.cx), Read(button.extentAt38.cy)};
    const auto& position = Read(button.useNaturalAt2C) ? button.naturalAt30 : button.positionAt24;
    ::OffsetRect(&result, Read(position.x), Read(position.y));
    return result;
}
class ButtonControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit ButtonControl(Child* body) noexcept : EnvironmentObjectReferenceControl(body)
    { strideAt0C = sizeof(Child); countAt10 = 1; }
    void DestroyPayloadAtVslot00() override
    { DeleteCgUiButtonNativeAt4669E0(static_cast<Child*>(payloadAt14), 1); }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    { RetireEnvironmentObjectControl(this, flags); }
};
// 49DF60/49CE90：正常调用先撤本阶段资格；失败时只展开后续成员，不重试失败叶。
template<class F> struct Cleanup final
{
    F action;
    bool armed = true;
    ~Cleanup() noexcept { if (armed) action(); }
    void Run() { armed = false; action(); }
};
}

LegacyTitleMenu::LegacyTitleMenu(const LegacyTitleMenuData& data, LegacyTitleMenuServices& services)
    : data_(data), services_(services)
{
    // 49CAF2在menu catch之前；不把base/dirty分配异常伪装成资源未ready。
    ModeButtonNativeServices mode{services.buttons, services.memory};
    ConstructModeButtonNativeAt4A2E60(state_, mode);
    GraphicsSurfaceNativeScope graphics(services.surfaces);
    try
    {
        if (!services.host.LoadBackground(background_, services.host.BackgroundName())) throw 1;
        const auto* overlayName = services.host.OverlayName();
        if (*overlayName && !services.host.LoadOverlay(overlay_, overlayName, 1)) throw 1;
        for (const auto& record : data.buttons)
        {
            // 49CBF8/49CC2F：先append再load，失败清表包含本次部分加载的按钮。
            auto* button = AppendButton();
            if (!LoadCgUiButtonRecordNativeAt480930(button, &record, 0, services.archives,
                services.transaction, services.files, services.key, services.surfaces,
                services.errors, services.codecs, services.buttons.strings))
            {
                ClearModeButtonChildrenNativeAt4A8490(state_);
                throw 1;
            }
        }
        for (const auto& entry : data.navigation) AppendNavigation(entry);
        state_.neighborsAt20 = data.useNeighborTable ? data.neighbors.data() : nullptr;
        state_.releaseStateAt10 = 0;
        SelectModeButtonNativeAt4C2200(state_, -1);
        ready_ = true;
    }
    catch (...)
    {
        // 49CC8C只吞资源阶段异常。抛出而非返回0的loader不走49CC42清表。
        // 已取得的background/overlay和未清children均留给外层析构，不能在这里全回滚。
    }
}

LegacyTitleMenu::~LegacyTitleMenu() { ReleaseResources(); }

void LegacyTitleMenu::ReleaseResources()
{
    if (released_) return;
    ready_ = false;
    // 一次退休资格。外传的异常不能使owner随后析构重新减持已失败的Ref。
    released_ = true;
    GraphicsSurfaceNativeScope graphics(services_.surfaces);
    Cleanup base{[&] { ReleaseBase(); }};
    Cleanup background{[&] { ReleaseYtlReference(background_); }};
    ReleaseYtlReference(overlay_);
    background.Run();
    base.Run();
}

void LegacyTitleMenu::ReleaseBase()
{
    Cleanup storage{[&] { DestroyModeButtonChildrenNativeAt4A3A00(state_.childrenAt14); }};
    Cleanup dirty{[&] { ReleaseYtlReference(state_.dirtyAt24); }};
    Cleanup navigation{[&] { DestroyModeButtonNavigationNativeAt4A3AB0(state_.navigationAt30); }};
    ClearModeButtonChildrenNativeAt4A8490(state_);
    navigation.Run();
    dirty.Run();
    storage.Run();
}

Child* LegacyTitleMenu::AppendButton()
{
    auto* button = new Child;
    try { ConstructCgUiButtonNativeAt459530(*button); }
    catch (...) { delete button; throw; }
    // 使用共享正文而非复制原78h。控制块分配失败保留原“正文已构造”的资格边界。
    YtlReferenceSlotRuntime local{reinterpret_cast<std::uintptr_t>(button), 0, nullptr};
    local.controlAt08 = new ButtonControl(button);
    try { AppendCgUiButtonSurfaceReferenceAt499920(state_.childrenAt14, local); }
    catch (...) { ReleaseYtlReference(local); throw; }
    ReleaseYtlReference(local);
    return ResolveModeButtonChildNativeAt4ACF20(state_, ChildCount() - 1);
}

void LegacyTitleMenu::AppendNavigation(const LegacyTitleMenuNavigation& entry)
{
    auto* view = new ModeButtonNavigationNativeBody34h{};
    view->itemAt00 = entry.item;
    // 49DB00/4B4AD4：共享导航读取顺序为2、3、0、1；只构造视图，不复制旧对象。
    constexpr int fieldByDirection[]{2, 3, 0, 1};
    for (int direction = 0; direction < 4; ++direction)
    {
        auto range = entry.candidatesByDirection[direction];
        auto* first = const_cast<std::int32_t*>(range.data());
        auto* last = range.empty() ? first : first + range.size();
        view->candidatesAt04[fieldByDirection[direction]] = {first, last, last};
    }
    YtlReferenceSlotRuntime local;
    ReplaceEnvironmentObjectReference(local, view, sizeof(*view));
    try { AppendCgUiButtonSurfaceReferenceAt499920(state_.navigationAt30, local); }
    catch (...) { ReleaseYtlReference(local); throw; }
    // Ref只拥有view；候选是data的只读借用，view的普通delete不能释放这些借用数组。
    ReleaseYtlReference(local);
}

std::int32_t LegacyTitleMenu::ChildCount() const noexcept
{ return released_ ? 0 : GuiControlSurfaceReferenceCount(state_.childrenAt14); }

const Child* LegacyTitleMenu::Button(std::int32_t index) const noexcept
{
    if (index < 0 || index >= ChildCount()) return nullptr;
    return ResolveEnvironmentObjectReference<CgUiButtonNativeBody6Ch>(state_.childrenAt14.begin[index]);
}

void LegacyTitleMenu::RequireReady() const
{
    // 49C8D0外层先检查ready。独立调用显式拒绝，不能让未装好菜单假成功。
    if (!ready_) throw std::logic_error("Legacy title menu is not ready");
}

std::int32_t LegacyTitleMenu::ReadInput()
{
    auto mouse = [&]() {
        auto* root = services_.buttons.input.CaptureAt418210();
        return ResolveEnvironmentObjectReference<MouseManagerNativeBody3Ch>(root->mouseAt00);
    };
    auto keyboard = [&]() {
        auto* root = services_.buttons.input.CaptureAt418210();
        return ResolveEnvironmentObjectReference<KeyboardManagerNativeBody438h>(root->keyboardAt18);
    };
    auto pressed = [&](int action) { return ReadKeyboardPressedAt47F4E0(*keyboard(), action) != 0; };
    auto held = [&](int action) { return ReadKeyboardHeldAt4198D0(*keyboard(), action) != 0; };
    // 49D1D9/49D1F3：短路，两个独立mouse捕获；不能帧首快照或优先处理键盘。
    bool reset = Read(mouse()->movedAt33) != 0;
    if (!reset)
    {
        const auto* current = mouse();
        reset = !Read(current->previousAt30[0]) && Read(current->pressedAt10[0]) && !Read(current->suppressionAt28);
    }
    if (reset) { SelectModeButtonNativeAt4C2200(state_, -1); return -1; }
    std::int32_t result = -2;
    if (CurrentModeButtonNativeAt4ACFA0(state_) >= 0)
    {
        if (pressed(5))
        {
            state_.confirmLatchAt0C = 1;
            PressModeButtonChildNativeAt4C1980(state_, CurrentModeButtonNativeAt4ACFA0(state_));
        }
        else if (state_.confirmLatchAt0C && !held(5))
        {
            result = CurrentModeButtonNativeAt4ACFA0(state_);
            SelectModeButtonNativeAt4C2200(state_, -1);
            CommitModeButtonChildNativeAt4B10F0(state_, result);
        }
        if (state_.confirmLatchAt0C) return -3;
    }
    if (state_.directionAt08 == -1)
    {
        for (int action = 1; action <= 4; ++action)
            if (pressed(action)) { state_.directionAt08 = action - 1; return result; }
        if (state_.selectedAt04 == -1 && result == -2) return -1;
    }
    else if (std::uint32_t(state_.directionAt08) <= 3u && !held(data_.directionActions[state_.directionAt08]))
    {
        NavigateModeButtonNativeAt4B4920(state_);
        // 49D403：无条件-4。现代4A9D90的“current变动才-4”不能替代本分支。
        return -4;
    }
    return result;
}

void LegacyTitleMenu::Cue(LegacyTitleMenuCue cue)
{
    const auto volume = services_.host.ReadCueVolume();
    const auto* name = services_.host.CueName(cue);
    services_.host.PlayCue(name, cue == LegacyTitleMenuCue::Click ? 0 : 1, volume, 1);
}

std::int32_t LegacyTitleMenu::PollSelection()
{
    RequireReady();
    const auto count = ChildCount(); // 49DC43先缓存count，再调用input；不调用releaseAt0C。
    auto result = ReadInput();
    if (result == -1)
        for (std::int32_t index = 0; index < count; ++index)
        {
            auto* button = ResolveModeButtonChildNativeAt4ACF20(state_, index);
            if (!button) continue;
            const auto before = Read(button->stateAt04);
            if (ProcessCgUiButtonNativeAt4A98E0(*button, 0, services_.buttons) >= 2) result = index;
            const auto after = Read(button->stateAt04);
            if (before != after)
            {
                InvalidateModeButtonNativeAt4B4570(state_, Bounds(*button));
                if (after == 1 && result != index) Cue(LegacyTitleMenuCue::Select);
            }
            // 49DC6D..49DD5D：不提前break，多项激活最后一项胜出。
        }
    if (overlay_.baseAt00 && services_.host.UpdateOverlay(overlay_) == 0)
        InvalidateModeButtonNativeAt4B4570(state_, services_.host.OverlayBounds(overlay_));
    if (result >= 0) Cue(LegacyTitleMenuCue::Click);
    else if (result == -4) Cue(LegacyTitleMenuCue::Select);
    return result;
}

void LegacyTitleMenu::Compose(GraphicsSurfaceNativeBody& destination, const RECT* clip)
{
    RequireReady();
    auto* background = ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(background_);
    if (destination.handleAt04 && background->handleAt04)
        static_cast<void>(services_.buttons.graphics.SurfaceBlitRaw(destination.handleAt04, background->handleAt04,
            0, 0, nullptr, nullptr, clip, 0));
    if (overlay_.baseAt00) services_.host.BlitOverlay(overlay_, destination, clip);
    const auto count = ChildCount();
    for (int index = 0; index < count; ++index)
        if (auto* button = ResolveModeButtonChildNativeAt4ACF20(state_, index))
            DrawCgUiButtonNativeAt4B5290(*button, &destination, clip, services_.buttons);
}

void LegacyTitleMenu::DrawButton(CgUiButtonNativeBody6Ch& button, const RECT* clip)
{
    // 4BFF40：共享绘制入口只有Blit；屏幕通道直接调用既有SurfaceDrawRaw，绝不伪造destination。
    const auto state = Read(button.stateAt04);
    if (state < 0) return;
    if (state > 6) throw std::out_of_range("Legacy title button state outside seven slots");
    const auto slot = Read(button.slotsAt40[state]);
    if (slot < 0) return;
    auto* surface = ResolveCgUiButtonSurfaceNativeAt4ADED0(button, slot);
    if (!surface) return;
    const auto dx = Sub(Read(surface->originXAt10), Read(button.naturalAt30.x));
    const auto dy = Sub(Read(surface->originYAt14), Read(button.naturalAt30.y));
    const auto& position = Read(button.useNaturalAt2C) ? button.naturalAt30 : button.positionAt24;
    const auto x = Add(Read(position.x), dx), y = Add(Read(position.y), dy);
    if (Read(surface->handleAt04))
        static_cast<void>(services_.buttons.graphics.SurfaceDrawRaw(Read(surface->handleAt04), x, y, nullptr, nullptr, clip, 0));
}

void LegacyTitleMenu::FlushDirty()
{
    RequireReady();
    auto dirty = [&]() { return ResolveGuiDirtyRectReference(state_.dirtyAt24); };
    auto* collector = dirty();
    collector->vptrAt00->coalesceAt04(collector);
    collector = dirty();
    const auto count = collector->vptrAt00->countAt18(collector);
    if (!count) return; // 49E7D0：没有rect时不取rect指针，也不clear。
    collector = dirty();
    const auto* rects = collector->vptrAt00->rectsAt1C(collector);
    for (std::uint32_t index = 0; index < count; ++index)
    {
        const auto* clip = rects + index;
        auto* background = ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(background_);
        if (background->handleAt04)
            static_cast<void>(services_.buttons.graphics.SurfaceDrawRaw(background->handleAt04, 0, 0, nullptr, nullptr, clip, 0));
        if (overlay_.baseAt00) services_.host.DrawOverlay(overlay_, clip);
        const auto children = ChildCount();
        for (int child = 0; child < children; ++child)
            if (auto* button = ResolveModeButtonChildNativeAt4ACF20(state_, child)) DrawButton(*button, clip);
        services_.screen.InvalidatedRectIfLoaded(clip);
    }
    // 49E8CE：完整循环后才clear，画图或commit抛出保留dirty，普通false不门控。
    collector = dirty();
    collector->vptrAt00->clearAt00(collector);
}
}
