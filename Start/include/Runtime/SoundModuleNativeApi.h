#pragma once
#include <Runtime/GraphicsModuleNative.h>

namespace KaGuYa::Start::Runtime
{
// Sound singleton的当前module Ref12；不增持、不复制状态，也不默认返回空模块。
class ISoundModuleNativeSource
{
public:
    virtual ~ISoundModuleNativeSource() = default;
    virtual const YtlReferenceSlotRuntime* CaptureSoundModuleReference() = 0;
};

class SoundModuleNativeApi final
{
public:
    SoundModuleNativeApi(ISoundModuleNativeSource&, IRuntimeStringManagerSource&,
        IGraphicsModuleNativeWin32Api&) noexcept;
    bool IsPlaying(std::int32_t capturedHandle);
    void ActivateAppRestoreAt42103E(WPARAM active);
    long CalcValueToVolumeAt4A0E3A(std::int32_t maximum, std::int32_t value);
    void SetVolume(std::int32_t capturedHandle, std::int32_t maximum, std::int32_t value);
    void StopAt4385F1(std::int32_t capturedHandle);
    void PlayHandleAt438090(std::int32_t capturedHandle);
    void SetLoopAt42BEBF(std::int32_t capturedHandle, std::int32_t loop);
    void SetGlobalFocusAt45D1F4(std::int32_t enabled = 1);
    std::int32_t AddAt412AB0(std::int32_t mode, std::int32_t reserved);
    const GUID* DefaultDeviceAt45D4E2();
    std::uint8_t ResetDeviceAt432C99(HWND capturedWindow);
    const YtlReferenceSlotRuntime* CaptureAt42BD45() { return modules_.CaptureSoundModuleReference(); }
    bool OpenCapturedAt42BD8F(const YtlReferenceSlotRuntime&, std::int32_t handle,
        const wchar_t* logical, const wchar_t* physical, std::int32_t offset, std::int32_t length);
    // 42C810固定槽地址；42C888在Sound捕获后才读这个槽的handle。
    void PlaySlotAt42C87F(const std::int32_t* capturedSlot);
private:
    FARPROC ResolveExport(const YtlReferenceSlotRuntime&, std::uintptr_t base, const char*, std::int32_t);
    ISoundModuleNativeSource& modules_;
    IRuntimeStringManagerSource& strings_;
    IGraphicsModuleNativeWin32Api& win32_;
};
}
