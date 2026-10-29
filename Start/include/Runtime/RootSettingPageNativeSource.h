#pragma once
#include <Runtime/SettingPage02Native.h>
#include <Runtime/SettingPage03Native.h>

namespace KaGuYa::Start::Runtime
{
class RootRuntimeOwner;
class EnvironmentRootOwner;

// 50D9D9/52E5F4/52E6D0：只借现有根及环境，不提前捕获或复制配置正文。
class RootSettingPageNativeSource final : public ISettingPage02NativeSource, public ISettingPage03NativeSource
{
public:
    RootSettingPageNativeSource(RootRuntimeOwner& root, EnvironmentRootOwner& environment,
        decltype(&::PostMessageA) postMessage = &::PostMessageA) noexcept;
    EnvironmentRootStorage* CaptureEnvironmentAt4180C0() override;
    EnvironmentRootStorage* CaptureEnvironmentInlineAt6198C8() override;
    void EnsureRootAt413BE0() override;
    void* RootAt6198C4() override;
    YtlReferenceSlotRuntime& RootWindowModeSlotAt0C(void* fixedRoot) override;
    YtlReferenceSlotRuntime& RootSoundSlotAt24(void* fixedRoot) override;
    BOOL PostMessageA(HWND, UINT, WPARAM, LPARAM) override;
private:
    RootRuntimeOwner& root_;
    EnvironmentRootOwner& environment_;
    decltype(&::PostMessageA) postMessage_;
};
}
