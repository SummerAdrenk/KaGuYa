#pragma once
#include <Runtime/SoundManagerNativeVolume.h>

namespace KaGuYa::Start::Runtime
{
void AppendSoundLowerAt45D435(SoundManagerNativeBody50h&, const YtlReferenceSlotRuntime&,
    IEnvironmentRootMemory& = DefaultEnvironmentRootMemory());
void ClearSoundLowerAt45D368(SoundManagerNativeBody50h&) noexcept;
void DestroySoundLowerAt462240(SoundManagerNativeBody50h&,
    IEnvironmentRootMemory& = DefaultEnvironmentRootMemory()) noexcept;
}
