#include <Runtime/SaveLoadSystemRuntime.h>

#include <algorithm>
#include <array>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
[[nodiscard]] std::string Cp932(const char* const value)
{
    return std::string(value);
}

[[nodiscard]] CgOperationChildDescriptor ButtonDescriptor(
    const std::int32_t eventValue,
    const char* const normal,
    const char* const disabled,
    const char* const target,
    const char* const clicked,
    const std::int32_t childId)
{
    const std::string normalName = Cp932(normal);
    const std::string targetName = Cp932(target);
    return CgOperationChildDescriptor{
        CgOperationControlKind::Button,
        eventValue,
        {{{normalName,
           disabled == nullptr ? std::string{} : Cp932(disabled),
           {}, targetName, targetName, Cp932(clicked), {}}},
         std::nullopt},
        0,
        childId};
}

[[nodiscard]] const std::array<CgOperationChildDescriptor, 4>&
LoadDescriptorsAt603DB0()
{
    static const std::array<CgOperationChildDescriptor, 4> descriptors{{
        ButtonDescriptor(9,
            "\x83\x8D\x81\x5B\x83\x68\x89\xE6\x96\xCA\x82\xD6\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D",
            nullptr,
            "\x83\x8D\x81\x5B\x83\x68\x89\xE6\x96\xCA\x82\xD6\x83\x5E\x83\x51\x2E\x76\x72\x6D",
            "\x83\x8D\x81\x5B\x83\x68\x89\xE6\x96\xCA\x82\xD6\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D",
            19),
        ButtonDescriptor(10,
            "\x83\x5E\x83\x43\x83\x67\x83\x8B\x82\xD6\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D",
            nullptr,
            "\x83\x5E\x83\x43\x83\x67\x83\x8B\x82\xD6\x83\x5E\x83\x51\x2E\x76\x72\x6D",
            "\x83\x5E\x83\x43\x83\x67\x83\x8B\x82\xD6\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D",
            17),
        ButtonDescriptor(11,
            "\x83\x4C\x83\x83\x83\x93\x83\x5A\x83\x8B\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D",
            nullptr,
            "\x83\x4C\x83\x83\x83\x93\x83\x5A\x83\x8B\x83\x5E\x83\x51\x2E\x76\x72\x6D",
            "\x83\x4C\x83\x83\x83\x93\x83\x5A\x83\x8B\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D",
            -1),
        ButtonDescriptor(12,
            "\x91\x53\x8F\xC1\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D",
            "\x91\x53\x8F\xC1\x83\x49\x83\x74\x2E\x76\x72\x6D",
            "\x91\x53\x8F\xC1\x83\x5E\x83\x51\x2E\x76\x72\x6D",
            "\x91\x53\x8F\xC1\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D",
            -1),
    }};
    return descriptors;
}

[[nodiscard]] const std::array<CgOperationChildDescriptor, 7>&
SaveDescriptorsAt603E60()
{
    static const std::array<CgOperationChildDescriptor, 7> descriptors{{
        ButtonDescriptor(13,
            "\x83\x5A\x81\x5B\x83\x75\x89\xE6\x96\xCA\x82\xD6\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D",
            "\x83\x5A\x81\x5B\x83\x75\x89\xE6\x96\xCA\x82\xD6\x83\x49\x83\x74\x2E\x76\x72\x6D",
            "\x83\x5A\x81\x5B\x83\x75\x89\xE6\x96\xCA\x82\xD6\x83\x5E\x83\x51\x2E\x76\x72\x6D",
            "\x83\x5A\x81\x5B\x83\x75\x89\xE6\x96\xCA\x82\xD6\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D",
            18),
        ButtonDescriptor(10,
            "\x4C\x83\x5E\x83\x43\x83\x67\x83\x8B\x82\xD6\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D",
            "\x4C\x83\x5E\x83\x43\x83\x67\x83\x8B\x82\xD6\x83\x49\x83\x74\x2E\x76\x72\x6D",
            "\x4C\x83\x5E\x83\x43\x83\x67\x83\x8B\x82\xD6\x83\x5E\x83\x51\x2E\x76\x72\x6D",
            "\x4C\x83\x5E\x83\x43\x83\x67\x83\x8B\x82\xD6\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D",
            17),
        ButtonDescriptor(11,
            "\x4C\x83\x4C\x83\x83\x83\x93\x83\x5A\x83\x8B\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D",
            nullptr,
            "\x4C\x83\x4C\x83\x83\x83\x93\x83\x5A\x83\x8B\x83\x5E\x83\x51\x2E\x76\x72\x6D",
            "\x4C\x83\x4C\x83\x83\x83\x93\x83\x5A\x83\x8B\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D",
            -1),
        ButtonDescriptor(12,
            "\x4C\x91\x53\x8F\xC1\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D",
            "\x4C\x91\x53\x8F\xC1\x83\x49\x83\x74\x2E\x76\x72\x6D",
            "\x4C\x91\x53\x8F\xC1\x83\x5E\x83\x51\x2E\x76\x72\x6D",
            "\x4C\x91\x53\x8F\xC1\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D",
            -1),
        ButtonDescriptor(14,
            "\x92\xCA\x8F\xED\x83\x5A\x81\x5B\x83\x75\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D",
            "\x92\xCA\x8F\xED\x83\x5A\x81\x5B\x83\x75\x83\x49\x83\x74\x2E\x76\x72\x6D",
            "\x92\xCA\x8F\xED\x83\x5A\x81\x5B\x83\x75\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D",
            "\x92\xCA\x8F\xED\x83\x5A\x81\x5B\x83\x75\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D",
            -1),
        ButtonDescriptor(15,
            "\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x5A\x81\x5B\x83\x75\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D",
            "\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x5A\x81\x5B\x83\x75\x83\x49\x83\x74\x2E\x76\x72\x6D",
            "\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x5A\x81\x5B\x83\x75\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D",
            "\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x5A\x81\x5B\x83\x75\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D",
            -1),
        ButtonDescriptor(16,
            "\x83\x49\x81\x5B\x83\x67\x83\x5A\x81\x5B\x83\x75\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D",
            "\x83\x49\x81\x5B\x83\x67\x83\x5A\x81\x5B\x83\x75\x83\x49\x83\x74\x2E\x76\x72\x6D",
            "\x83\x49\x81\x5B\x83\x67\x83\x5A\x81\x5B\x83\x75\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D",
            "\x83\x49\x81\x5B\x83\x67\x83\x5A\x81\x5B\x83\x75\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D",
            -1),
    }};
    return descriptors;
}

class SaveLoadSystemEventTargetRuntime final
    : public ISaveLoadDialogEventTarget
{
public:
    explicit SaveLoadSystemEventTargetRuntime(
        CgOperationChildControlRef control)
        : control_(std::move(control))
    {
        if (!control_)
            throw std::invalid_argument("SaveLoadSystem event target is null");
    }

    void InvokeAtVslot08() override { control_->InvokeAtVslot08(); }
    [[nodiscard]] RECT ReadRectAtVslot3C() const override
    {
        return control_->ReadRectAtVslot3C();
    }

private:
    CgOperationChildControlRef control_;
};
}

std::span<const CgOperationChildDescriptor>
SaveLoadSystemChildDescriptorsSample18At603DB0(
    const bool saveMode) noexcept
{
    if (saveMode)
        return SaveDescriptorsAt603E60();
    return LoadDescriptorsAt603DB0();
}

SaveLoadSystemObjectGraphRuntime::SaveLoadSystemObjectGraphRuntime(
    ICgOperationSurfaceFactory& surfaceFactory,
    ICgOperationButtonInputServices& buttonInputServices,
    ISaveLoadSystemInheritedServices& inheritedServices,
    const bool saveMode)
    : inheritedServices_(&inheritedServices)
{
    const auto descriptors =
        SaveLoadSystemChildDescriptorsSample18At603DB0(saveMode);
    children_.reserve(descriptors.size());
    eventTargets_.reserve(descriptors.size());
    for (const auto& descriptor : descriptors)
    {
        auto resources =
            std::make_shared<CgOperationControlResourceGroupRuntime>(
                surfaceFactory, descriptor.resources);
        auto child = std::make_shared<CgOperationChildControlRuntime>(
            descriptor.kind,
            descriptor.eventValueAt04,
            descriptor.childIdAt0C,
            descriptor.initialResourceSlotAt24,
            std::move(resources));
        child->BindButtonInputServicesAt4C(buttonInputServices);

        const RECT rect = child->ReadRectAtVslot3C();
        RECT combined{};
        ::UnionRect(&combined, &combinedRectAt60_, &rect);
        combinedRectAt60_ = combined;
        children_.push_back(child);
        eventTargets_.push_back(
            std::make_shared<SaveLoadSystemEventTargetRuntime>(
                std::move(child)));
    }

    clickedEventManagerAt70_ =
        CgOperationClickedEventManagerRuntime::
            CreateForSaveLoadSystemAt4F0C80(children_);
}

std::int32_t SaveLoadSystemObjectGraphRuntime::StateAtVslot20() const
{
    return stateAt34_;
}

void SaveLoadSystemObjectGraphRuntime::WriteEnabledAtVslot24(
    const std::int32_t value)
{
    gateAt38_ = value;
}

std::int32_t SaveLoadSystemObjectGraphRuntime::GateAtVslot28() const
{
    return gateAt38_;
}

void SaveLoadSystemObjectGraphRuntime::OpenAtVslot44()
{
    // 0x50EC20：先清 ClickedEvent，再执行公共 +0x68；随后每次读取
    // root+0x218 前都调用 0x413BE0，依次更新事件 13 与事件 10 控件。
    clickedEventManagerAt70_->ClearAllAt532200();
    inheritedServices_->ResetInputAtVslot68();

    for (const std::int32_t event : {13, 10})
    {
        const auto child = std::find_if(children_.begin(), children_.end(),
            [event](const CgOperationChildControlRef& value) {
                return value && value->EventValueAt04() == event;
            });
        if (child == children_.end())
            continue;
        (*child)->WriteAtVslot44(
            inheritedServices_->
                ReadRootByteAt218AfterSynchronizeAt413BE0() ? 1 : 0);
    }
}

void SaveLoadSystemObjectGraphRuntime::UpdateAtVslot48()
{
    eventAt5C_ = -1;
    eventAt5CInitialized_ = true;
    if (inheritedServices_->ReadCancelRequestAt52FDA0())
    {
        eventAt5C_ = 11;
        return;
    }

    inheritedServices_->UpdateGuiDialogAtVslot54();
    eventAt5C_ = clickedEventManagerAt70_->PollFirstAt50FDB0(1);
}

void SaveLoadSystemObjectGraphRuntime::EndAtVslot4C()
{
    // SaveLoadSystem vtable +0x4C = 0x5094C0，只有固定成功返回，无副作用。
}

std::int32_t SaveLoadSystemObjectGraphRuntime::EventAtVslot50() const
{
    if (!eventAt5CInitialized_)
        throw std::logic_error("SaveLoadSystem +0x5C has not been initialized");
    return eventAt5C_;
}

SaveLoadDialogEventTargetRef
SaveLoadSystemObjectGraphRuntime::ResolveEventTargetAtVslot14(
    const std::int32_t event)
{
    for (std::size_t index = 0; index < children_.size(); ++index)
    {
        if (children_[index]->EventValueAt04() == event)
            return eventTargets_[index];
    }
    return {};
}

void SaveLoadSystemObjectGraphRuntime::ApplyPageModeAt535160(
    const std::int32_t mode)
{
    constexpr std::array<std::int32_t, 3> events{14, 15, 16};
    for (std::size_t index = 0; index < events.size(); ++index)
    {
        const auto child = std::find_if(children_.begin(), children_.end(),
            [event = events[index]](const CgOperationChildControlRef& value) {
                return value->EventValueAt04() == event;
            });
        if (child != children_.end())
            (*child)->WriteAtVslot44(mode != static_cast<std::int32_t>(index + 1u));
    }
}

void SaveLoadSystemObjectGraphRuntime::WriteStateAtVslot20(
    const std::int32_t value) noexcept
{
    stateAt34_ = value;
}

void SaveLoadSystemObjectGraphRuntime::ClearChildrenAtVslot08() noexcept
{
    children_.clear();
    eventTargets_.clear();
}

const std::vector<CgOperationChildControlRef>&
SaveLoadSystemObjectGraphRuntime::ChildrenInInsertionOrder() const noexcept
{
    return children_;
}

const std::shared_ptr<CgOperationClickedEventManagerRuntime>&
SaveLoadSystemObjectGraphRuntime::ClickedEventManagerAt70() const noexcept
{
    return clickedEventManagerAt70_;
}

const RECT&
SaveLoadSystemObjectGraphRuntime::CombinedRectAt60() const noexcept
{
    return combinedRectAt60_;
}
}
