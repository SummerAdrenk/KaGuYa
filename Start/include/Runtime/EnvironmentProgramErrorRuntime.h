#pragma once

#include <Runtime/ScrVmProgramHandler.h>
#include <Windows.h>

namespace KaGuYa::Start::Runtime
{
class EnvironmentStartupRuntime;

// 4D070F..4D0755的报告叶；不拥有Environment，不捕获命令执行时的窗口/标题快照。
class EnvironmentProgramErrorRuntime final : public IScrVmProgramCommandErrorSink
{
public:
    EnvironmentProgramErrorRuntime(EnvironmentStartupRuntime& environment,
        decltype(&::MessageBoxA) messageBox) noexcept;
    void ReportProgramFailure(std::string_view detail) override;

private:
    EnvironmentStartupRuntime& environment_;
    decltype(&::MessageBoxA) messageBox_;
};
}
