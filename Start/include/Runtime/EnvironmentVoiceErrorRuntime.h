#pragma once

#include <Runtime/ScrVmVoiceHandler.h>
#include <Windows.h>

namespace KaGuYa::Start::Runtime
{
class EnvironmentStartupRuntime;

// 4D3406..4D3436的报告叶；Environment须长于本对象，不缓存标题或窗口。
class EnvironmentVoiceErrorRuntime final : public IScrVmVoiceCommandErrorSink
{
public:
    EnvironmentVoiceErrorRuntime(EnvironmentStartupRuntime& environment,
        decltype(&::MessageBoxA) messageBox) noexcept;
    void ReportVoiceFailure(std::string_view detail) override;

private:
    EnvironmentStartupRuntime& environment_;
    decltype(&::MessageBoxA) messageBox_;
};
}
