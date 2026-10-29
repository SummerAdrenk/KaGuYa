#pragma once

#include <Runtime/GraffitiPageNative.h>

namespace KaGuYa::Start::Runtime
{
class RootRuntimeOwner;

// 只借稳定owner，不在构造/装配时Capture，不缓存或增持某代原正文。
// environment通常为EnvironmentStartupRuntime::RootSource()。
class RootGraffitiPageNativeSource final : public IGraffitiPageNativeSource
{
public:
    RootGraffitiPageNativeSource(RootRuntimeOwner& root, EnvironmentRootOwner& environment) noexcept;
    EnvironmentRootStorage* CaptureEnvironmentAt4180C0() override;
    DrawManagerNativeBody2Ch* CaptureDrawManagerAfter413BE0() override;
    GraffitiPageNativeParamsReceiver CaptureParamsRootAfter413BE0() override;
    SceneManagerNativeBody48h* CaptureSceneManagerAfter413BE0() override;
private:
    RootRuntimeOwner& root_;
    EnvironmentRootOwner& environment_;
};
}
