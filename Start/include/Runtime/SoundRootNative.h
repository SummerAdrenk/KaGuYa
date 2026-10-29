#pragma once
#include <Runtime/SoundModuleNativeApi.h>

namespace KaGuYa::Start::Runtime
{
// 6198BC只有Ref12；不同于CSoundManager50h和Environment60h。
class SoundRootOwner final : public ISoundModuleNativeSource
{
public:
    SoundRootOwner(IEnvironmentRootLock&, IEnvironmentRootMemory& = DefaultEnvironmentRootMemory(),
        IGraphicsModuleNativeWin32Api& = DefaultGraphicsModuleNativeWin32Api()) noexcept;
    ~SoundRootOwner() noexcept;
    SoundRootOwner(const SoundRootOwner&) = delete;
    SoundRootOwner& operator=(const SoundRootOwner&) = delete;
    YtlReferenceSlotRuntime* CaptureAt4183D0();
    const YtlReferenceSlotRuntime* CaptureSoundModuleReference() override { return CaptureAt4183D0(); }
    void ReleaseAt5AC690() noexcept;
    YtlReferenceSlotRuntime* Published() const noexcept { return published_; }
    YtlReferenceSlotRuntime*& PublicationSlot() noexcept { return published_; }
private:
    void LeaveNoexcept() noexcept;
    IEnvironmentRootLock& lock_;
    IEnvironmentRootMemory& memory_;
    IGraphicsModuleNativeWin32Api& win32_;
    YtlReferenceSlotRuntime* published_ = nullptr;
};
}
