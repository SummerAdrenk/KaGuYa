#include <Runtime/GeneralConfirmationDialogRuntime.h>
#include <Runtime/CgOperationSurfaceResourceRuntime.h>
#include <Runtime/GuiInputStateRuntime.h>
#include <Runtime/OpacityDialogObjectGraphRuntime.h>
#include <Runtime/ReadScriptEffectSurfaceRuntime.h>

#include <array>
#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
constexpr const char* kFrame = "\x8A\x6D\x94\x46\x98\x67.vrm";
constexpr std::array<const char*, 5> kMessages{{
    "\x8A\x6D\x94\x46\x83\x81\x83\x62\x83\x5A_\x8F\x49\x97\xB9.vrm",
    "\x8A\x6D\x94\x46\x83\x81\x83\x62\x83\x5A_\x83\x5E\x83\x43\x83\x67\x83\x8B.vrm",
    "\x8A\x6D\x94\x46\x83\x81\x83\x62\x83\x5A_\x8F\x89\x8A\xFA\x90\xDD\x92\xE8.vrm",
    "\x8A\x6D\x94\x46\x83\x81\x83\x62\x83\x5A_\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x5A\x81\x5B\x83\x75.vrm",
    "\x8A\x6D\x94\x46\x83\x81\x83\x62\x83\x5A_\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x8D\x81\x5B\x83\x68.vrm",
}};
constexpr std::array<std::array<const char*, 3>, 2> kButtons{{
    {{"\x8A\x6D\x94\x46\x91\x49\x91\xF0_\x82\xCD\x82\xA2.vrm",
      "\x8A\x6D\x94\x46\x91\x49\x91\xF0_\x82\xCD\x82\xA2\x83\x4A\x81\x5B\x83\x5C\x83\x8B.vrm",
      "\x8A\x6D\x94\x46\x91\x49\x91\xF0_\x82\xCD\x82\xA2\x83\x4E\x83\x8A\x83\x62\x83\x4E.vrm"}},
    {{"\x8A\x6D\x94\x46\x91\x49\x91\xF0_\x82\xA2\x82\xA2\x82\xA6.vrm",
      "\x8A\x6D\x94\x46\x91\x49\x91\xF0_\x82\xA2\x82\xA2\x82\xA6\x83\x4A\x81\x5B\x83\x5C\x83\x8B.vrm",
      "\x8A\x6D\x94\x46\x91\x49\x91\xF0_\x82\xA2\x82\xA2\x82\xA6\x83\x4E\x83\x8A\x83\x62\x83\x4E.vrm"}},
}};

}

struct GeneralConfirmationDialogRuntime::State final : GuiModalDialogState
{
    explicit State(GeneralConfirmationDialogDependencies dependencies, GeneralConfirmationMode mode)
        : GuiModalDialogState(dependencies)
    {
        // 5A18A0仅展开一次基类；这里不能catch后Release，再让基类析构重复清理。
        const auto index = static_cast<std::uint32_t>(mode);
        if (index >= kMessages.size())
            throw std::out_of_range("4E3330 mode is outside the five rows at 605CD8");
        // 4E0BE0 共用基类已创建 NullInput 和 dirty；4E3330 在子控件前替换输入。
        InstallGameInput();
        AppendStatic(0, kFrame);
        AppendStatic(1, kMessages[index]);
        for (std::int32_t i = 0; i != 2; ++i)
        {
            const auto& names = kButtons[static_cast<std::size_t>(i)];
            CgOperationControlResourceDescriptor descriptor;
            descriptor.resourceNamesAt04 = {names[0], {}, {}, names[1], names[1], names[2], {}};
            auto resources = std::make_shared<CgOperationControlResourceGroupRuntime>(deps.resources, descriptor);
            auto child = std::make_shared<CgOperationChildControlRuntime>(
                CgOperationControlKind::Button, i + 2, -1, 7, std::move(resources));
            auto event = std::make_shared<CgOperationClickedEventRuntime>(i + 2);
            child->BindButtonInputServicesAt4C(*this);
            child->BindClickedEventAt3C(event);
            child->WriteClickedEventEnabledAt48(1);
            // 4E3727..4E37B6：child 入列先于 event，失败由基类展开同序清理。
            children.push_back(std::move(child));
            events.push_back(std::move(event));
        }
        static constexpr GuiDialogNavigationMapping forward[]{{2, 3}, {3, 2}, {-1, 0}};
        static constexpr GuiDialogNavigationMapping backward[]{{3, 2}, {2, 3}, {-1, 0}};
        CreateAndAppendGuiNavigation(navigation, 3, forward);
        CreateAndAppendGuiNavigation(navigation, 4, backward);
        modeAt80 = mode; // 4E38E4：全部子对象完成后才写 mode。
    }

    void ProcessModalInputAtVslot54() override { ProcessInput(); }
    std::int32_t PollModalResultAtVslot90() override
    {
        for (const auto& event : events)
        {
            if (event->ClickedFlagAt04() == 0) continue;
            switch (event->EventValueAt08())
            {
            case 2: result = 6; return 1;
            case 3: result = 7; return 1;
            case 4: result = 1; return 1;
            default: return 0;
            }
        }
        return 0;
    }
    GeneralConfirmationMode modeAt80;
};

GeneralConfirmationDialogRuntime::GeneralConfirmationDialogRuntime(
    GeneralConfirmationDialogDependencies dependencies, GeneralConfirmationMode mode)
    : state_(std::make_unique<State>(dependencies, mode)) {}

GeneralConfirmationDialogRuntime::~GeneralConfirmationDialogRuntime() noexcept = default;

void GeneralConfirmationDialogRuntime::CaptureDisplayAt48CA30()
{
    // 485DA6/486086：先Ensure再取根，本次getter内始终使用选定的槽。
    auto& source = state_->deps.rootSource.CaptureFrameSlotAt413BE0();
    // 477C60 先增持，再清根引用；48CA30 在此之后才释放对话框旧帧。
    GraphicsSurfaceNativeScope scope(state_->deps.surfaces);
    YtlReferenceSlotRuntime argument;
    ConsumeGraphicsSurfaceReferenceAt477C60(argument, source);
    AssignGraphicsSurfaceReferenceAt48CA30(state_->captured.Slot(), argument);
}

void GeneralConfirmationDialogRuntime::AssignCapturedFrameAt48CA30(GraphicsSurfaceReference frame)
{
    GraphicsSurfaceNativeScope scope(state_->deps.surfaces);
    auto argument = frame.Detach();
    AssignGraphicsSurfaceReferenceAt48CA30(state_->captured.Slot(), argument);
}

std::int32_t GeneralConfirmationDialogRuntime::RunModalAt5071D0()
{
    return state_->runner.RunAt5071D0(*this);
}

void GeneralConfirmationDialogRuntime::BeginAtVslot44()
{ state_->BeginBaseAt50CF50(); }

bool GeneralConfirmationDialogRuntime::PollAtVslot48()
{ return state_->PollAt52DBC0(); }

bool GeneralConfirmationDialogRuntime::EndAtVslot4C()
{ state_->EndBaseAt508E80(); return true; }

std::int32_t GeneralConfirmationDialogRuntime::ResultAtVslot50() const { return state_->result.value(); }
IModalDialogRuntimeTarget* GeneralConfirmationDialogRuntime::ResolveAt4A5330() { return this; }
void GeneralConfirmationDialogRuntime::DrawAt5230B0(const RECT& clip)
{ state_->DrawAt5230B0(clip); }
void GeneralConfirmationDialogRuntime::CompositeAt5231C0(GraphicsSurfaceDestination destination, const RECT* clip)
{ state_->CompositeAt5231C0(destination, clip); }
void GeneralConfirmationDialogRuntime::SubmitControlRectAt50FC80(const RECT& rect)
{
    state_->dirty.AddAtVslot08(rect);
}
void GeneralConfirmationDialogRuntime::FlushDirtyRectsAt5267E0()
{ state_->FlushDirtyRectsAt5267E0(); }

std::int32_t GeneralConfirmationDialogRuntime::PollClickedResultAt524E60()
{ return state_->PollModalResultAtVslot90(); }
GeneralConfirmationMode GeneralConfirmationDialogRuntime::ModeAt80() const noexcept { return state_->modeAt80; }
std::optional<std::int32_t> GeneralConfirmationDialogRuntime::PhaseAt6C() const noexcept { return state_->phase; }
std::span<const CgOperationChildControlRef> GeneralConfirmationDialogRuntime::ChildrenInInsertionOrder() const noexcept
{
    return state_->children;
}
std::span<const CgOperationClickedEventRef> GeneralConfirmationDialogRuntime::ClickedEventsInInsertionOrder() const noexcept
{
    return state_->events;
}
ICgOperationChildControl* GeneralConfirmationDialogRuntime::ActiveControlAt4C() const noexcept { return state_->active; }
const GraphicsSurfaceReference& GeneralConfirmationDialogRuntime::CapturedFrameAt5C() const noexcept
{
    return state_->captured;
}
}
