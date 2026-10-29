#pragma once

#include <Runtime/StartupParamsState.h>
#include <Runtime/ScriptNamedValueNative.h>
#include <Windows.h>

namespace KaGuYa::Start::Runtime
{
class EnvironmentStartupRuntime;
class RootRuntimeOwner;
class RootRuntimePayload;

class RootStartupParamsState final : public IStartupParamsState
{
public:
    RootStartupParamsState(EnvironmentStartupRuntime& environment, RootRuntimeOwner& root,
        ScriptNamedValueNativeTables tables, decltype(&::MessageBoxA) messageBox,
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource()) noexcept;
    void ReplaceParamsGeneration() override;
    void CopyRegistryPrologueFlag() override;
    void WriteReplayFile(std::int32_t value) override;
    void WriteReplayPosition(std::int32_t value) override;
    void ClearHistoryAt4D5730() override;
private:
    void WriteNamedValue(RootRuntimePayload& capturedRoot, const char* name, std::int32_t value);
    EnvironmentStartupRuntime& environment_;
    RootRuntimeOwner& root_;
    ScriptNamedValueNativeTables tables_;
    decltype(&::MessageBoxA) messageBox_;
    IRuntimeStringManagerSource& strings_;
};
}
