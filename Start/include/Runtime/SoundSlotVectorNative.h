#pragma once
#include <Runtime/SoundManagerNativeVolume.h>

namespace KaGuYa::Start::Runtime
{
void DestroySoundSlotRangeAt48FC60(SoundManagerNativeSlot8h* begin, SoundManagerNativeSlot8h* end);
void ResizeSoundSlotsAt4574C0(SoundManagerNativeBody50h&, std::uint32_t count,
    IRuntimeStringManagerSource& = DefaultRuntimeStringManagerSource(),
    IEnvironmentRootMemory& = DefaultEnvironmentRootMemory());
void DestroySoundSlotsAt4627C0(SoundManagerNativeBody50h&,
    IEnvironmentRootMemory& = DefaultEnvironmentRootMemory()) noexcept;
}
