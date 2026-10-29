#pragma once

#include <Runtime/ScriptDispatchNative.h>

namespace KaGuYa::Start::Runtime
{
class EnvironmentStartupRuntime;

// 4CDD0C与4CDD1D之间由调用者读当前TEXT、caption、window，不把它们提前快照。
class IScrVmCommandErrorSink
{
public:
    virtual ~IScrVmCommandErrorSink() = default;
    virtual EnvironmentRootStorage& CaptureAt4180C0() = 0;
    virtual int ShowMessageBoxA(HWND window, const char* text, const char* caption, UINT type) = 0;
};

class EnvironmentInvalidOpcodeRuntime final : public IScrVmCommandErrorSink
{
public:
    EnvironmentInvalidOpcodeRuntime(EnvironmentStartupRuntime& environment,
        decltype(&::MessageBoxA) messageBox) noexcept;
    EnvironmentRootStorage& CaptureAt4180C0() override;
    int ShowMessageBoxA(HWND window, const char* text, const char* caption, UINT type) override;
private:
    EnvironmentStartupRuntime& environment_;
    decltype(&::MessageBoxA) messageBox_;
};

// 4C98E0只用于异常展开，Delete重读slot；正常4CDD46仍使用GetNil之后捕获的EDI。
void UnwindScriptInvalidCommandAt4C98E0(YtlReferenceSlotRuntime& slot) noexcept;
[[nodiscard]] std::int32_t ExecuteScriptInvalidAt4CDC00(ScrContainer& parser,
    IScriptExecutionRootSource& root, IScrVmCommandErrorSink& errors,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory(),
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
}
