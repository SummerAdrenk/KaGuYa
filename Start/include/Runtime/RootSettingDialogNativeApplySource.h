#pragma once
#include <Runtime/SettingDialogNativeApply.h>

namespace KaGuYa::Start::Runtime
{
class RootRuntimeOwner;
class NativeGraphicsRuntimeLibrary;
class RootSettingDialogNativeApplySource final : public ISettingDialogNativeApplySource
{
public:
    RootSettingDialogNativeApplySource(RootRuntimeOwner&,EnvironmentRootOwner&,ParamsRuntimeRootOwner&,
        NativeGraphicsRuntimeLibrary&,decltype(&::MessageBoxA) = &::MessageBoxA) noexcept;
    EnvironmentRootStorage* CaptureEnvironmentAt4180C0(std::uint32_t) override;
    EnvironmentRootStorage* CaptureEnvironmentInlineAt6198C8(std::uint32_t) override;
    ParamsRuntimeRoot* CaptureParamsInlineAt6198C0(std::uint32_t) override;
    ParamsRuntimeRoot* CaptureParamsAt418480(std::uint32_t) override;
    void EnsureRootAt413BE0(std::uint32_t) override;
    void* RootAt6198C4(std::uint32_t) override;
    YtlReferenceSlotRuntime& RootSoundSlotAt24(void*) override;
    YtlReferenceSlotRuntime& RootDrawSlotAt18(void*) override;
    YtlReferenceSlotRuntime& RootFileSlotAt00(void*) override;
    GraphicsRootNativeBody1Ch* CaptureGraphicsAt4182F0(std::uint32_t) override;
    int MessageBoxA(HWND,const char*,const char*,UINT) override;
private:
    RootRuntimeOwner& root_;
    EnvironmentRootOwner& environment_;
    ParamsRuntimeRootOwner& params_;
    NativeGraphicsRuntimeLibrary& graphics_;
    decltype(&::MessageBoxA) messageBox_;
};
}
