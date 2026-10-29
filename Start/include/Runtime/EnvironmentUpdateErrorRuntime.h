#pragma once

#include <Runtime/ScrVmUpdateHandler.h>
#include <Windows.h>

namespace KaGuYa::Start::Runtime
{
class EnvironmentStartupRuntime;

// 4D2F15..4D2F45 的报告叶；Environment 须长于本对象，不缓存标题或窗口。
class EnvironmentUpdateErrorRuntime final : public IScrVmUpdateCommandErrorSink
{
public:
    EnvironmentUpdateErrorRuntime(EnvironmentStartupRuntime& environment,
        decltype(&::MessageBoxA) messageBox) noexcept;
    void ReportUpdateFailure(std::string_view detail) override;

private:
    EnvironmentStartupRuntime& environment_;
    decltype(&::MessageBoxA) messageBox_;
};
}
