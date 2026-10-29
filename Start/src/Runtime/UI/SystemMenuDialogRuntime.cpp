#include <Runtime/SystemMenuDialogRuntime.h>
#include <Runtime/SystemMenuSample18Data.h>

#include <utility>

namespace KaGuYa::Start::Runtime
{
struct SystemMenuDialogRuntime::State final : GuiModalDialogState
{
    State(GuiModalDialogDependencies dependencies, ISystemMenuDialogServices& systemServices)
        : GuiModalDialogState(dependencies), services(systemServices)
    {
        // 4F4215/4F421F：+80=1、+84=0；+68/+6C 仍由 Begin/首次消费发布。
        for (const auto& descriptor : SystemMenuChildDescriptorsSample18At606978())
        {
            auto resources = std::make_shared<CgOperationControlResourceGroupRuntime>(deps.resources, descriptor.resources);
            auto child = std::make_shared<CgOperationChildControlRuntime>(descriptor.kind,
                descriptor.eventValueAt04, descriptor.childIdAt0C,
                descriptor.initialResourceSlotAt24, std::move(resources));
            if (descriptor.kind == CgOperationControlKind::Static)
            {
                children.push_back(std::move(child));
                continue;
            }
            auto event = std::make_shared<CgOperationClickedEventRuntime>(descriptor.eventValueAt04);
            child->BindButtonInputServicesAt4C(*this);
            child->BindClickedEventAt3C(event);
            child->WriteClickedEventEnabledAt48(1);
            // 4F4382..4F4424：先保存控件，再保存事件；基类析构负责半构造清理。
            children.push_back(std::move(child));
            events.push_back(std::move(event));
        }
        // 4F44CF..4F44FF：与确认框不同，全部控件完成后才换掉 NullInput。
        InstallGameInput();
    }

    CgOperationChildControlRef Find(std::int32_t event) const
    {
        for (const auto& child : children)
            if (child->EventValueAt04() == event) return child;
        return {};
    }

    bool CanRemainOpenAt5104A0()
    {
        // +20 是 mode+34，不是 enabled+38。失败才清 +84，异常不能吞掉。
        if (renderer.ModeAt34() != 0 && suppressReopenAt84 == 0 && !services.IsGameWindowIconic())
        {
            const auto background = Find(0);
            if (background)
            {
                const auto pointer = ReadPointerAtVslot74();
                deps.modal.SynchronizeRuntimeAt413BE0();
                const auto rootRect = services.ReadRootLogicalRectangleAt478E50();
                if (pointer.x >= rootRect.left && pointer.x < rootRect.right)
                {
                    const RECT childRect = background->ReadRectAtVslot3C();
                    // 原分支不检查 child.right/top；bottom 是闭区间，不可改成 PtInRect。
                    if (pointer.y >= rootRect.top && pointer.y <= childRect.bottom && pointer.x >= childRect.left)
                        return true;
                }
            }
        }
        suppressReopenAt84 = 0;
        return false;
    }

    void ProcessModalInputAtVslot54() override
    {
        if (!CanRemainOpenAt5104A0()) { result = 0; return; }
        ProcessInput();
        for (const auto& event : events)
        {
            if (event->ClickedFlagAt04() != 0) { result = event->EventValueAt08(); return; }
        }
    }
    std::int32_t PollModalResultAtVslot90() override
    {
        // 525470 的判定是 != -1，不是 >0；鼠标移出产生的 0 同样结束。
        return result.value() != -1 ? 1 : 0;
    }

    ISystemMenuDialogServices& services;
    const std::int32_t valueAt80 = 1;
    std::int32_t suppressReopenAt84 = 0;
};

SystemMenuDialogRuntime::SystemMenuDialogRuntime(GuiModalDialogDependencies dependencies,
    ISystemMenuDialogServices& services) : state_(std::make_unique<State>(dependencies, services)) {}
SystemMenuDialogRuntime::~SystemMenuDialogRuntime() noexcept = default;

bool SystemMenuDialogRuntime::CanRemainOpenAt5104A0() { return state_->CanRemainOpenAt5104A0(); }
void SystemMenuDialogRuntime::BeginAtVslot44()
{
    auto& s = *state_;
    s.BeginBaseAt50CF50();
    s.deps.modal.SynchronizeRuntimeAt413BE0();
    const auto mode = s.services.ReadCurrentWindowModeAt04();
    if (const auto child = s.Find(1)) child->WriteAtVslot44(mode != 0 ? 1 : 0);
    if (const auto child = s.Find(2)) child->WriteAtVslot44(mode == 0 ? 1 : 0);
    if (const auto child = s.Find(3))
    {
        s.deps.modal.SynchronizeRuntimeAt413BE0();
        child->WriteAtVslot44(s.services.ReadSettingsEnabledAtRoot219() != 0 ? 1 : 0);
    }
    s.result = -1;
}
bool SystemMenuDialogRuntime::PollAtVslot48() { return state_->PollAt52DBC0(); }
bool SystemMenuDialogRuntime::EndAtVslot4C()
{
    auto& s = *state_;
    s.EndBaseAt508E80();
    switch (s.result.value())
    {
    case 1: s.services.PostGameWindowMessage(WM_COMMAND, 0x800C, 0); break;
    case 2: s.services.PostGameWindowMessage(WM_COMMAND, 0x800B, 0); break;
    case 3:
        s.deps.modal.SynchronizeRuntimeAt413BE0();
        s.services.WriteSceneManagerPendingCommandAt1C(SystemMenuSettingsSceneNameSample18At5B4E68());
        s.suppressReopenAt84 = 1;
        break;
    case 4: s.services.PostGameWindowMessage(WM_CLOSE, 0, 0); break;
    default: break;
    }
    // 509550 所有正常分支返回 0；不能沿用确认框 End 的 true。
    return false;
}
std::int32_t SystemMenuDialogRuntime::ResultAtVslot50() const { return state_->result.value(); }
IModalDialogRuntimeTarget* SystemMenuDialogRuntime::ResolveAt4A5330() { return this; }
void SystemMenuDialogRuntime::DrawAt5230B0(const RECT& clip) { state_->DrawAt5230B0(clip); }
void SystemMenuDialogRuntime::CompositeAt5231C0(GraphicsSurfaceDestination destination, const RECT* clip)
{ state_->CompositeAt5231C0(destination, clip); }
void SystemMenuDialogRuntime::SubmitControlRectAt50FC80(const RECT& rect)
{ state_->dirty.AddAtVslot08(rect); }
void SystemMenuDialogRuntime::FlushDirtyRectsAt5267E0() { state_->FlushDirtyRectsAt5267E0(); }
void SystemMenuDialogRuntime::AssignCapturedFrameAt48CA30(GraphicsSurfaceReference frame)
{
    GraphicsSurfaceNativeScope scope(state_->deps.surfaces);
    auto argument = frame.Detach();
    AssignGraphicsSurfaceReferenceAt48CA30(state_->captured.Slot(), argument);
}
std::optional<std::int32_t> SystemMenuDialogRuntime::PhaseAt6C() const noexcept { return state_->phase; }
std::int32_t SystemMenuDialogRuntime::SuppressReopenAt84() const noexcept { return state_->suppressReopenAt84; }
std::span<const CgOperationChildControlRef> SystemMenuDialogRuntime::ChildrenInInsertionOrder() const noexcept
{ return state_->children; }
std::span<const CgOperationClickedEventRef> SystemMenuDialogRuntime::ClickedEventsInInsertionOrder() const noexcept
{ return state_->events; }
const GraphicsSurfaceReference& SystemMenuDialogRuntime::CapturedFrameAt5C() const noexcept
{ return state_->captured; }
}
