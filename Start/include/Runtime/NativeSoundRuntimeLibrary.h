#pragma once
#include <Runtime/SoundRuntimeApi.h>
#include <Runtime/SoundRootNativeInitialize.h>

namespace KaGuYa::Start::Runtime
{
// 借用原生Sound singleton与调用服务；析构不卸载，不存HMODULE或诊断状态。
class NativeSoundRuntimeLibrary final : public ISoundRuntimeLibrary
{
public:
    NativeSoundRuntimeLibrary() noexcept;
    NativeSoundRuntimeLibrary(SoundRootOwner&, IEnvironmentRootMemory&, IRuntimeStringManagerSource&,
        IGraphicsModuleNativeWin32Api&, ISoundInitializationPathsNativeSource&) noexcept;
    bool Initialize(HWND) override;
    // 显式现场路径源，复用同一原生根；非进程宿主不借另一个Environment singleton。
    bool Initialize(HWND, ISoundInitializationPathsNativeSource&);
    void Shutdown() override;
    void ActivateAppRestoreIfLoaded(WPARAM) override;
    void SetGlobalFocusAt45D28C(std::uint8_t) override;
    GUID ReadDefaultDeviceIdAt45D57A() override;
    long CalcValueToVolume(std::int32_t maximum, std::int32_t value) const override;
    SoundRuntimeHandle AddSound(std::uint8_t) override;
    void RemoveSound(SoundRuntimeHandle) override;
    bool OpenSound(SoundRuntimeHandle, const wchar_t*, const wchar_t*, std::int32_t offset, std::int32_t size) override;
    void SetSoundVolume(SoundRuntimeHandle, std::int32_t maximum, std::int32_t value) override;
    void SetSoundLoop(SoundRuntimeHandle, std::uint8_t) override;
    bool IsSlotPlaying(SoundRuntimeHandle) override;
    void PlaySlot(SoundRuntimeHandle) override;
    void StopSlot(SoundRuntimeHandle) override;
    SoundModuleNativeApi& Calls() noexcept { return api_; }
    // 正式manager服务借同一singleton及调用依赖，不另建模块owner。
    ISoundModuleNativeSource& ModuleSource() noexcept { return root_; }
    IEnvironmentRootMemory& MemorySource() noexcept { return memory_; }
    IRuntimeStringManagerSource& StringSource() noexcept { return strings_; }
    IGraphicsModuleNativeWin32Api& Win32Source() noexcept { return win32_; }
private:
    SoundRootOwner& root_;
    IEnvironmentRootMemory& memory_;
    IRuntimeStringManagerSource& strings_;
    IGraphicsModuleNativeWin32Api& win32_;
    ISoundInitializationPathsNativeSource& paths_;
    mutable SoundModuleNativeApi api_;
};
}
