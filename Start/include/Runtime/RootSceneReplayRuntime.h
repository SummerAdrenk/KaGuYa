#pragma once

#include <Runtime/ScrVmSceneHandler.h>
#include <Runtime/ScriptNamedValueNative.h>
#include <Windows.h>

namespace KaGuYa::Start::Runtime
{
class EnvironmentStartupRuntime;
class RootRuntimeOwner;

// 4D0CCA..4D0D40：借稳定进程owner；不绑定某一代root或CParams。
class RootSceneReplayRuntime final : public IScrVmSceneReplaySource
{
public:
    RootSceneReplayRuntime(EnvironmentStartupRuntime& environment, RootRuntimeOwner& root,
        ScriptNamedValueNativeTables tables, decltype(&::MessageBoxA) messageBox,
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource()) noexcept;
    [[nodiscard]] std::int32_t ReadReplayValue() override;

private:
    EnvironmentStartupRuntime& environment_;
    RootRuntimeOwner& root_;
    ScriptNamedValueNativeTables tables_;
    decltype(&::MessageBoxA) messageBox_;
    IRuntimeStringManagerSource& strings_;
};
}
