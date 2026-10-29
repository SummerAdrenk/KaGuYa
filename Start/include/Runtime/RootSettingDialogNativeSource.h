#pragma once
#include <Runtime/SettingDialogNative.h>

namespace KaGuYa::Start::Runtime
{
class RootRuntimeOwner;
class NativeGraphicsRuntimeLibrary;
class InputRootOwner;
class SoundManagerNativeRuntime;
// 50F3E0..50F609及5272BF的现场来源；只借同一Root，不创建另一份GUI或声音owner。
class RootSettingDialogNativeSource final : public ISettingDialogNativeSource
{
public:
    RootSettingDialogNativeSource(RootRuntimeOwner&,EnvironmentRootOwner&,InputRootOwner&,
        NativeGraphicsRuntimeLibrary&,SoundManagerNativeRuntime&,decltype(&::PostMessageA) = &::PostMessageA) noexcept;
    void EnsureRootAt413BE0() override;
    void* RootAt6198C4() override;
    std::uint8_t RootByteAt218(void* fixedRoot) override;
    YtlReferenceSlotRuntime& RootDrawSlotAt18(void*) override;
    YtlReferenceSlotRuntime& RootSoundSlotAt24(void*) override;
    std::int32_t& RootPageAt214(void*) override;
    EnvironmentRootStorage* SystemAt4180C0() override;
    InputRootNativeBody24h* InputAt418210() override;
    YtlReferenceSlotRuntime* CaptureBackgroundAt4ACDD0(void*,YtlReferenceSlotRuntime*) override;
    void StopSoundAt4384D0(void*) override;
    void StopSoundMappingsAt413270(void*,EnvironmentVectorStorage<RuntimeCString>*) override;
    void PrepareSoundAt433F60(void*) override;
    void DrawBackgroundAt5272BF(std::int32_t,const RECT*) override;
    BOOL PostMessageA(HWND,UINT,WPARAM,LPARAM) override;
private:
    RootRuntimeOwner& root_;
    EnvironmentRootOwner& environment_;
    InputRootOwner& input_;
    NativeGraphicsRuntimeLibrary& graphics_;
    SoundManagerNativeRuntime& sounds_;
    decltype(&::PostMessageA) postMessage_;
};
}
