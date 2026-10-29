#pragma once

#include <Runtime/ScrVmSceneHandler.h>

#include <Windows.h>

namespace KaGuYa::Start::Runtime
{
class ParamsRuntimeRootOwner;
class EnvironmentStartupRuntime;

// 两个稳定的进程 owner 须长于本适配器；只借用，不缓存 child、归档槽或窗口字段。
// Name 与 SecretSC 的本次借用期间须保持所属对象存活，不提供跨并发换代保活。
class EnvironmentSceneRuntime final : public IScrVmSceneSource,
    public IScrVmSceneCommandErrorSink
{
public:
    EnvironmentSceneRuntime(ParamsRuntimeRootOwner& params,
        EnvironmentStartupRuntime& environment, decltype(&::MessageBoxA) messageBox) noexcept;

    [[nodiscard]] std::size_t CurrentSceneLabelCount() override;
    [[nodiscard]] const char* CurrentSceneLabelName(std::size_t index) override;
    [[nodiscard]] const ParamsGameSystem& CurrentGameSystem() override;
    [[nodiscard]] SecretScArchive* CurrentSecretSc() override;
    void ReportSceneLabelIndexOutOfRange(std::uint16_t index) override;

private:
    ParamsRuntimeRootOwner& params_;
    EnvironmentStartupRuntime& environment_;
    decltype(&::MessageBoxA) messageBox_;
};
}
