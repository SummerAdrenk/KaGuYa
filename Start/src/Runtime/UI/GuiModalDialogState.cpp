#include <Runtime/GuiModalDialogState.h>
#include <Runtime/CgOperationSurfaceResourceRuntime.h>
#include <bit>
#include <map>
#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
std::int32_t AddDword(const std::int32_t a, const std::int32_t b) noexcept
{
    return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(a) + static_cast<std::uint32_t>(b));
}

template<class T> void ReleaseInOrder(std::vector<std::shared_ptr<T>>& refs) noexcept
{
    for (auto& ref : refs) ref.reset();
    std::vector<std::shared_ptr<T>>{}.swap(refs);
}
}

GuiModalDialogState::GuiModalDialogState(GuiModalDialogDependencies dependencies)
    : deps(dependencies), raw(deps.drawing), renderer(raw), runner(deps.modal),
      inputSource(deps.input), dirty(dirtySlot), captured(deps.surfaces)
{
    struct ConstructionCleanup final
    {
        GuiModalDialogState& owner;
        bool completed = false;
        ~ConstructionCleanup() noexcept { if (!completed) owner.Release(); }
    } cleanup{*this};
    // 4E0BE0没有catch：必须在展开期间释放，而非先进入catch再清理。
    ConstructNullGuiInputReferenceAt4E0BE0(inputSlot);
    CreateGuiDirtyRectReferenceAt4E0D42(dirtySlot);
    cleanup.completed = true;
}
GuiModalDialogState::~GuiModalDialogState() noexcept { Release(); }

void GuiModalDialogState::Release() noexcept
{
    // 4FCAC0 -> 4F7AC0；半构造失败同样不调用模态 End，也不清 root gate。
    ReleaseInOrder(events);
    captured.Reset();
    // 4F7AC0：+50 dirty Ref -> +24导航 -> +18输入Ref -> +0C children。
    ReleaseGuiDirtyRectReferenceAt460500(dirtySlot);
    DestroyGuiNavigationReferencesAt4F6E70(navigation);
    ReleaseGuiInputReferenceAt4F6620(inputSlot);
    ReleaseInOrder(children);
}
void GuiModalDialogState::AppendStatic(std::int32_t event, const char* name)
{
    CgOperationControlResourceDescriptor descriptor;
    descriptor.resourceNamesAt04 = {name, {}, {}, name, name, {}, {}};
    auto resources = std::make_shared<CgOperationControlResourceGroupRuntime>(deps.resources, descriptor);
    children.push_back(std::make_shared<CgOperationChildControlRuntime>(
        CgOperationControlKind::Static, event, -1, 7, std::move(resources)));
}
GraphicsSurfaceNativeBody& GuiModalDialogState::Captured() const
{
    if (!captured)
        throw std::logic_error("52DBC0/5267E0 dereferenced a null captured frame");
    return *captured;
}
void GuiModalDialogState::AddCapturedRect()
{
    const auto& frame = Captured();
    const RECT rect{frame.originXAt10, frame.originYAt14,
        AddDword(frame.originXAt10, frame.Width()), AddDword(frame.originYAt14, frame.Height())};
    dirty.AddAtVslot08(rect);
}
void GuiModalDialogState::SetActive(ICgOperationChildControl* child)
{
    if (active && active == child) return;
    if (active) active->LoseFocusAtVslot24();
    if (child) child->GainFocusAtVslot20();
    active = child;
}
void GuiModalDialogState::SetActiveControlAtVslot30(ICgOperationDialogEventTarget& control)
{
    for (const auto& child : children)
    {
        if (child.get() == &control) { SetActive(child.get()); return; }
    }
    throw std::logic_error("GuiModalDialog active control is not an owned child");
}
void GuiModalDialogState::InstallGameInput()
{ AdoptGuiInputAt536EA0(inputSlot, NewGameGuiInputAt4EAAED()); }
GuiInputNativeBody& GuiModalDialogState::InputBody() const
{
    auto* body = ResolveEnvironmentObjectReference<GuiInputNativeBody>(inputSlot);
    if (!body) throw std::logic_error("GuiDialog dereferenced a null input body");
    return *body;
}
bool GuiModalDialogState::ReadInputAtVslot6C(std::int32_t action)
{
    GuiInputNativeScope scope(inputSource);
    auto& body = InputBody();
    return body.vptrAt00->readHeldAt00(&body, action) != 0;
}
bool GuiModalDialogState::ReadInputAtVslot70(std::int32_t action)
{
    GuiInputNativeScope scope(inputSource);
    auto& body = InputBody();
    return body.vptrAt00->readPressedAt04(&body, action) != 0;
}
POINT GuiModalDialogState::ReadPointerAtVslot74()
{
    GuiInputNativeScope scope(inputSource);
    auto& body = InputBody();
    POINT output;
    body.vptrAt00->readPointerAt08(&body, &output);
    return output;
}
bool GuiModalDialogState::ReadPointerMoved()
{
    GuiInputNativeScope scope(inputSource);
    auto& body = InputBody();
    return body.vptrAt00->readPointerMovedAt10(&body) != 0;
}
bool GuiModalDialogState::ReadPressAtVslot80()
{
    GuiInputNativeScope scope(inputSource);
    auto& body = InputBody();
    return body.vptrAt00->readMousePressAt14(&body) != 0;
}
bool GuiModalDialogState::ReadReleaseAtVslot84()
{
    GuiInputNativeScope scope(inputSource);
    auto& body = InputBody();
    return body.vptrAt00->readMouseReleaseAt18(&body) != 0;
}
std::uint32_t GuiModalDialogState::ReadSurfacePixelAtGetSurfacePixel(GraphicsSurfaceHandle h, std::int32_t x, std::int32_t y)
{
    return deps.pixels.GetSurfacePixel(h, x, y);
}
void GuiModalDialogState::ResetInput()
{
    actionFive = 0;
    SetActive(nullptr);
    inputLatch = 0;
}
void GuiModalDialogState::ResetChildren()
{
    for (const auto& child : children) child->InvokeAtVslot08();
}
bool GuiModalDialogState::TryCandidate(std::int32_t event, bool reset)
{
    CgOperationChildControlRef candidate;
    for (const auto& child : children)
        if (child->EventValueAt04() == event) { candidate = child; break; }
    if (!candidate) throw 1;
    const auto visual = candidate->VisualStateAtVslot38();
    if (visual == 1 || visual == 2) return false;
    if (reset) ResetChildren();
    SetActive(candidate.get());
    inputLatch = 1;
    return true;
}
std::int32_t GuiModalDialogState::ProcessActionFive()
{
    if (active->InputModeAt08() != 0) return 0;
    const auto previous = actionFive.value();
    if (previous != 0)
    {
        if (!ReadInputAtVslot6C(5))
        {
            // 510C22..510C61 强引用保活跨过触发和 latch 清零，不检查 Button+48。
            const auto& button = static_cast<CgOperationChildControlRuntime&>(*active);
            const auto event = button.ClickedEventAt3C();
            if (event) event->WriteClickedFlagAt04(1);
            actionFive = 0;
        }
        return previous;
    }
    if (!ReadInputAtVslot70(5)) return 0;
    actionFive = 1;
    active->WriteAtVslot70(1);
    return 1;
}
std::int32_t GuiModalDialogState::ProcessUpperInput()
{
    if (ReadPointerMoved() || ReadPressAtVslot80())
    {
        ResetInput();
        return 0;
    }
    // +40=511610 固定返回0。
    if (active && ProcessActionFive() != 0) return 1;
    const auto previousResult = inputLatch != 0 ? 2 : 0;
    for (std::int32_t groupIndex = 0; groupIndex < GuiNavigationReferenceCount(navigation); ++groupIndex)
    {
        if (!ReadInputAtVslot70(RequireGuiNavigation(navigation.begin[groupIndex]).inputAt04)) continue;
        if (active)
        {
            const auto initial = active->EventValueAt04();
            auto next = initial;
            for (;;)
            {
                auto& group = RequireGuiNavigation(navigation.begin[groupIndex]);
                next = group.vptrAt00->nextAt00(&group, next);
                if (next < 0 || next == initial) break;
                if (TryCandidate(next, true)) return 3;
            }
        }
        else
        {
            // 51E880：每次候选回调后重新解析组和pair范围，不保留候选镜像。
            for (std::int32_t index = 0;; ++index)
            {
                auto& group = RequireGuiNavigation(navigation.begin[groupIndex]);
                if (index >= GuiNavigationPairCount(group)) break;
                const auto next = group.pairsAt08.begin[index].sourceEventValue;
                if (TryCandidate(next, false)) return 3;
            }
        }
        // 51E8B0/51EA1F/51EA28 都回 51E84A，不继续扫描其他按下组。
        return previousResult;
    }
    return previousResult;
}
void GuiModalDialogState::ProcessInput()
{
    if (renderer.EnabledAt38() == 0 || renderer.ModeAt34() == 0) return;
    std::map<std::int32_t, std::int32_t> before;
    for (const auto& child : children)
    {
        const auto visual = child->VisualStateAtVslot38();
        before.insert({child->EventValueAt04(), visual});
    }
    if (ProcessUpperInput() == 0)
        for (const auto& child : children)
            if (child->ProcessAtVslot14() == 0) static_cast<void>(child->ProcessAtVslot18());
    for (const auto& child : children)
    {
        const auto previous = before[child->EventValueAt04()];
        if (previous != child->VisualStateAtVslot38() ||
            (child->InputModeAt08() == 5 && child->ReadModeFiveChangedAtVslot6C() != 0))
        {
            dirty.AddAtVslot08(child->ReadRectAtVslot3C());
            // +88=518A80 只释放按值 Ref，发生在提交矩形之后。
            const auto argument = child;
        }
    }
}

void GuiModalDialogState::BeginBaseAt50CF50()
{
    if (!captured)
    {
        auto frame = deps.rootSource.CaptureScreenAt413BE0();
        GraphicsSurfaceNativeScope scope(deps.surfaces);
        // 50CFC4 -> 465870：释放后清旧槽，再复制来源；临时Ref在继续Begin前析构。
        CopyAssignGraphicsSurfaceReferenceAt465870(captured.Slot(), frame.Slot());
    }
    renderer.WriteEnabledAt38(1);
    for (const auto& event : events) event->ClearAtVslot04();
    phase = 0;
    ResetInput();
    ResetChildren();
    active = nullptr;
    // +8C=5086B0 是原 nullsub。
    deps.rootSource.ModalGateAt413BE0() = 1;
}
bool GuiModalDialogState::PollAt52DBC0()
{
    switch (phase.value())
    {
    case 0:
        AddCapturedRect();
        phase = 3;
        FlushDirtyRectsAt5267E0();
        return true;
    case 3:
        ProcessModalInputAtVslot54();
        if (PollModalResultAtVslot90() != 0) phase = 4;
        FlushDirtyRectsAt5267E0();
        return true;
    case 4:
        AddCapturedRect();
        renderer.WriteEnabledAt38(0);
        // +94=51C100 是原 nullsub。
        phase = -1;
        FlushDirtyRectsAt5267E0();
        return true;
    default:
        return false;
    }
}
void GuiModalDialogState::EndBaseAt508E80()
{
    captured.Reset();
    deps.rootSource.ModalGateAt413BE0() = 0;
}
void GuiModalDialogState::FlushDirtyRectsAt5267E0()
{
    dirty.PrepareAtVslot04();
    const auto count = std::bit_cast<std::int32_t>(dirty.CountAtVslot18());
    if (count == 0) return;
    const auto rects = dirty.RectsAtVslot1C();
    for (std::int32_t i = 0; i < count; ++i)
    {
        const auto handle = Captured().Handle();
        if (handle != 0)
            static_cast<void>(deps.drawing.SurfaceDrawRaw(handle, 0, 0, nullptr, nullptr, &rects[i], 0));
        DrawAt5230B0(rects[i]);
        deps.screen.InvalidatedRectIfLoaded(&rects[i]);
    }
    dirty.ClearAtVslot00();
}
void GuiModalDialogState::DrawAt5230B0(const RECT& clip)
{
    renderer.DrawChildrenAt5230B0(children, clip);
}
void GuiModalDialogState::CompositeAt5231C0(GraphicsSurfaceDestination destination, const RECT* clip)
{
    renderer.CompositeChildrenAt5231C0(children, destination, clip);
}
}
