#include <Runtime/MappedSoundSlotRuntime.h>

#include <limits>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
MappedSoundSlotRuntime::MappedSoundSlotRuntime(
    const IScrResourceNameSource& resourceNames,
    IScrVmSoundRuntimeBoundary& soundRuntime) noexcept
    : resourceNames_(&resourceNames), soundRuntime_(&soundRuntime)
{
}

std::size_t MappedSoundSlotRuntime::FirstCount() const
{
    return soundRuntime_->FirstSlotGroupCountAt50();
}

std::size_t MappedSoundSlotRuntime::SecondCount() const
{
    return soundRuntime_->SecondSlotGroupCountAt48();
}

void MappedSoundSlotRuntime::ApplyFirst(
    const std::int32_t resourceId,
    const std::size_t groupIndex)
{
    if (groupIndex >= FirstCount())
        throw ScrVmSoundCommandException({});
    Apply(ScrVmSoundSlotGroup::FirstAt50, resourceId, groupIndex, groupIndex);
}

void MappedSoundSlotRuntime::ApplySecond(
    const std::int32_t resourceId,
    const std::size_t groupIndex)
{
    const std::size_t firstCount = FirstCount();
    if (groupIndex >= SecondCount())
        throw ScrVmSoundCommandException({});
    if (groupIndex > (std::numeric_limits<std::size_t>::max)() - firstCount)
        throw std::overflow_error("mapped sound port overflow");
    Apply(
        ScrVmSoundSlotGroup::SecondAt48,
        resourceId,
        groupIndex,
        firstCount + groupIndex);
}

void MappedSoundSlotRuntime::Apply(
    const ScrVmSoundSlotGroup group,
    const std::int32_t resourceId,
    const std::size_t groupIndex,
    const std::size_t absolutePort)
{
    if (resourceId < 0)
    {
        soundRuntime_->ClearMappedResourceNameAtPort(absolutePort);
        soundRuntime_->StopAndClearMappedSlotAtPort(absolutePort);
        return;
    }

    const auto receiver = resourceNames_->CapturePatternNames();
    soundRuntime_->StoreMappedPatternNameAtPort(absolutePort, receiver, resourceId);
    if (soundRuntime_->ShouldSkipOpening(group, groupIndex))
        return;

    // 两个专用组都忽略打开失败；公共第三组不经过本执行器。
    static_cast<void>(
        soundRuntime_->OpenMappedSlotAtPort(group, groupIndex, absolutePort));
}
}
