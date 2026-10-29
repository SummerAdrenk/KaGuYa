#include <Runtime/ProgramSelectionSystemDialogObjectGraphRuntime.h>

#include <algorithm>
#include <stdexcept>
#include <string>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
constexpr char kBase[] =
    "\x91\x49\x91\xF0\x89\xE6\x96\xCA\x5F\x89\xBA\x92\x6E";
constexpr char kCommand[] = "\x83\x52\x83\x7D\x83\x93\x83\x68";
constexpr char kOn[] = "\x83\x49\x83\x93";
constexpr char kOff[] = "\x83\x49\x83\x74";
constexpr char kTarget[] = "\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67";
constexpr char kClick[] = "\x83\x4E\x83\x8A\x83\x62\x83\x4E";
constexpr char kClick2[] = "\x83\x4E\x83\x8A\x83\x62\x83\x4E\x32";
constexpr char kRange[] = "\x94\xCD\x88\xCD";
constexpr char kPlay[] = "\x83\x76\x83\x8C\x83\x43";
constexpr char kPlayOff[] =
    "\x83\x76\x83\x8C\x83\x43\x83\x49\x83\x74";
constexpr char kQuickLoad[] =
    "\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x8D\x81\x5B\x83\x68";
constexpr char kQuickSave[] =
    "\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x5A\x81\x5B\x83\x75";
constexpr char kLoad[] = "\x83\x8D\x81\x5B\x83\x68";
constexpr char kSave[] = "\x83\x5A\x81\x5B\x83\x75";
constexpr char kAuto[] = "\x83\x49\x81\x5B\x83\x67";
constexpr char kSkip[] = "\x83\x58\x83\x4C\x83\x62\x83\x76";
constexpr char kVoice[] = "\x83\x7B\x83\x43\x83\x58";
constexpr char kOpacity[] = "\x93\xA7\x96\xBE\x93\x78";
constexpr char kLikability[] = "\x8D\x44\x8A\xB4\x93\x78";
constexpr char kTitle[] = "\x83\x5E\x83\x43\x83\x67\x83\x8B";

[[nodiscard]] std::string Resource(
    const char* const state,
    const char* const command)
{
    return std::string("vrm:\\") + kCommand + state + command + ".alp";
}

[[nodiscard]] CgOperationControlResourceDescriptor SingleGroup(
    const char* const command,
    const char* const clickedState = kClick)
{
    const std::string on = Resource(kOn, command);
    const std::string target = Resource(kTarget, command);
    return CgOperationControlResourceDescriptor{
        {on,
         Resource(kOff, command),
         {},
         target,
         target,
         Resource(clickedState, command),
         Resource(kRange, command)},
        std::nullopt};
}

[[nodiscard]] ProgramSelectionSystemControlDescriptor StaticDescriptor(
    const std::int32_t eventValue)
{
    const std::string name = std::string("vrm:\\") + kBase + ".alp";
    return ProgramSelectionSystemControlDescriptor{
        {CgOperationControlKind::Static,
         eventValue,
         {{name, name, {}, name, name, {}, {}}, std::nullopt},
         7,
         -1},
        std::nullopt};
}

[[nodiscard]] ProgramSelectionSystemControlDescriptor ButtonDescriptor(
    const std::int32_t eventValue,
    const char* const command,
    const char* const clickedState = kClick)
{
    return ProgramSelectionSystemControlDescriptor{
        {CgOperationControlKind::Button,
         eventValue,
         SingleGroup(command, clickedState),
         6,
         -1},
        std::nullopt};
}

[[nodiscard]] ProgramSelectionSystemControlDescriptor CheckBoxDescriptor(
    const std::int32_t eventValue,
    const char* const command,
    const bool autoCommand)
{
    const std::string play = Resource(kPlay, command);
    const std::string click = Resource(
        autoCommand ? kClick2 : kClick, command);
    CgOperationControlResourceDescriptor checked{
        {play,
         Resource(autoCommand ? kOff : kPlayOff, command),
         {},
         play,
         play,
         click,
         Resource(kRange, command)},
        std::nullopt};
    return ProgramSelectionSystemControlDescriptor{
        {CgOperationControlKind::CheckBox,
         eventValue,
         SingleGroup(command),
         6,
         autoCommand ? 26 : -1},
        std::move(checked)};
}

[[nodiscard]] std::array<ProgramSelectionSystemControlDescriptor, 11>
BuildDescriptors(const bool mode01)
{
    // 四组的边界分别为 1/4/2/4；这里保持原插入顺序而不按事件值排序。
    const std::array<std::int32_t, 11> ids = mode01 ?
        std::array<std::int32_t, 11>{
            23, 28, 27, 33, 32, 29, 30, 24, 26, 31, 34} :
        std::array<std::int32_t, 11>{
            48, 53, 52, 58, 57, 54, 55, 49, 51, 56, 59};
    return {
        StaticDescriptor(ids[0]),
        ButtonDescriptor(ids[1], kQuickLoad),
        ButtonDescriptor(ids[2], kQuickSave),
        ButtonDescriptor(ids[3], kLoad),
        ButtonDescriptor(ids[4], kSave),
        CheckBoxDescriptor(ids[5], kAuto, true),
        CheckBoxDescriptor(ids[6], kSkip, false),
        ButtonDescriptor(ids[7], kVoice),
        ButtonDescriptor(ids[8], kOpacity, kTarget),
        ButtonDescriptor(ids[9], kLikability),
        ButtonDescriptor(ids[10], kTitle)};
}
}

const std::array<ProgramSelectionSystemControlDescriptor, 11>&
ProgramSelectionSystemDescriptorsSample18(
    const ProgramSelectionDialogMode mode)
{
    static const auto mode00 = BuildDescriptors(false);
    static const auto mode01 = BuildDescriptors(true);
    if (mode == ProgramSelectionDialogMode::Mode00)
        return mode00;
    if (mode == ProgramSelectionDialogMode::Mode01)
        return mode01;
    throw std::invalid_argument("ProgramSelection system dialog mode is invalid");
}

ProgramSelectionSystemDialogObjectGraphRuntime::
ProgramSelectionSystemDialogObjectGraphRuntime(
    ICgOperationSurfaceFactory& surfaceFactory,
    ICgOperationButtonInputServices& buttonInputServices,
    IProgramSelectionSystemDialogServices& services,
    const ProgramSelectionDialogMode mode)
    : mode_(mode), services_(&services)
{
    const auto& descriptors = ProgramSelectionSystemDescriptorsSample18(mode);
    children_.reserve(descriptors.size());
    for (const auto& descriptor : descriptors)
    {
        auto resources =
            std::make_shared<CgOperationControlResourceGroupRuntime>(
                surfaceFactory, descriptor.control.resources);
        std::shared_ptr<CgOperationControlResourceGroupRuntime> checked;
        if (descriptor.checkedResources)
        {
            checked = std::make_shared<CgOperationControlResourceGroupRuntime>(
                surfaceFactory, *descriptor.checkedResources);
        }

        auto child = std::make_shared<CgOperationChildControlRuntime>(
            descriptor.control.kind,
            descriptor.control.eventValueAt04,
            descriptor.control.childIdAt0C,
            descriptor.control.initialResourceSlotAt24,
            std::move(resources),
            std::move(checked));
        if (descriptor.control.kind != CgOperationControlKind::Static)
            child->BindButtonInputServicesAt4C(buttonInputServices);

        RECT combined{};
        const RECT rect = child->ReadRectAtVslot3C();
        ::UnionRect(&combined, &combinedRect_, &rect);
        combinedRect_ = combined;
        children_.push_back(std::move(child));
    }

    clickedEventsAt5C_ = CgOperationClickedEventManagerRuntime::
        CreateForProgramSelectionSystemAt4DF420(
        children_, mode == ProgramSelectionDialogMode::Mode01);
}

void ProgramSelectionSystemDialogObjectGraphRuntime::OpenAtVslot44()
{
    // 50C300/50C5F0：清事件、启用 Dialog、重置全部 child 后再同步四个控件。
    clickedEventsAt5C_->ClearAllAt532200();
    dialogStateAt38_ = 1;
    for (const auto& child : children_)
        child->InvokeAtVslot08();

    const bool mode01 = mode_ == ProgramSelectionDialogMode::Mode01;
    ChildByEventValue(mode01 ? 24 : 49)->WriteAtVslot44(0);
    ChildByEventValue(mode01 ? 26 : 51)->WriteAtVslot44(0);

    const auto autoChild = std::dynamic_pointer_cast<
        CgOperationChildControlRuntime>(
            ChildByEventValue(mode01 ? 29 : 54));
    const auto skipChild = std::dynamic_pointer_cast<
        CgOperationChildControlRuntime>(
            ChildByEventValue(mode01 ? 30 : 55));
    if (!autoChild || !skipChild)
        throw std::logic_error("ProgramSelection System checkbox type is invalid");

    if ((services_->ReadScriptSlot2DwordAt18() & 2u) != 0u)
        autoChild->WriteCheckBoxStateAt5037D0(1);
    skipChild->WriteCheckBoxStateAt5037D0(static_cast<std::int32_t>(
        (services_->ReadScriptSlot2DwordAt14() >> 1u) & 1u));
    skipChild->WriteAtVslot44(0);
}

std::int32_t ProgramSelectionSystemDialogObjectGraphRuntime::UpdateAtVslot48()
{
    // 52D090/52D270：公共更新先行；命中事件的 child 在发布场景名之前重置。
    services_->UpdateInheritedAtVslot54();
    const std::int32_t event = clickedEventsAt5C_->PollFirstAt50FDB0(1);
    const bool mode01 = mode_ == ProgramSelectionDialogMode::Mode01;

    struct SceneCommand final
    {
        std::int32_t mode00Event;
        std::int32_t mode01Event;
        std::string_view sceneName;
    };
    static constexpr std::array<SceneCommand, 6> commands{{
        {53, 28, "\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x8D\x81\x5B\x83\x68"},
        {52, 27, "\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x5A\x81\x5B\x83\x75"},
        {58, 33, "\x83\x8D\x81\x5B\x83\x68\x89\xE6\x96\xCA"},
        {57, 32, "\x83\x5A\x81\x5B\x83\x75\x89\xE6\x96\xCA"},
        {59, 34, "\x83\x5E\x83\x43\x83\x67\x83\x8B\x82\xD6"},
        {56, 31, "\x8D\x44\x8A\xB4\x93\x78\x89\xE6\x96\xCA"},
    }};
    for (const auto& command : commands)
    {
        const std::int32_t expected =
            mode01 ? command.mode01Event : command.mode00Event;
        if (event == expected)
        {
            ChildByEventValue(event)->InvokeAtVslot08();
            services_->WriteCurrentSceneNameCp932(command.sceneName);
            return 1;
        }
    }

    if (event == (mode01 ? 29 : 54))
    {
        // 原函数先切换 53DDE0 返回对象 +0x5C，再异或脚本槽 +0x18 的 bit 1。
        services_->ToggleStateAt53DDE0Offset5C();
        services_->XorScriptSlot2DwordAt18With2();
    }
    return 1;
}

std::int32_t ProgramSelectionSystemDialogObjectGraphRuntime::
CloseAtVslot4C() const noexcept
{
    // 508CC0/508CD0。
    return 1;
}

ProgramSelectionDialogMode
ProgramSelectionSystemDialogObjectGraphRuntime::Mode() const noexcept
{
    return mode_;
}

const std::vector<CgOperationChildControlRef>&
ProgramSelectionSystemDialogObjectGraphRuntime::
ChildrenInInsertionOrder() const noexcept
{
    return children_;
}

const std::shared_ptr<CgOperationClickedEventManagerRuntime>&
ProgramSelectionSystemDialogObjectGraphRuntime::ClickedEventsAt5C() const noexcept
{
    return clickedEventsAt5C_;
}

const RECT&
ProgramSelectionSystemDialogObjectGraphRuntime::CombinedRect() const noexcept
{
    return combinedRect_;
}

std::int32_t ProgramSelectionSystemDialogObjectGraphRuntime::
DialogStateAt38() const noexcept
{
    return dialogStateAt38_;
}

CgOperationChildControlRef
ProgramSelectionSystemDialogObjectGraphRuntime::ChildByEventValue(
    const std::int32_t eventValue) const
{
    const auto it = std::ranges::find_if(
        children_, [eventValue](const auto& child) {
            return child && child->EventValueAt04() == eventValue;
        });
    if (it == children_.end())
        throw std::out_of_range("ProgramSelection System child is not owned");
    return *it;
}
}
