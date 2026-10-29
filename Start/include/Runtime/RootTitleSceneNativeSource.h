#pragma once
#include <Runtime/TitleSceneNative.h>
#include <Runtime/ScriptStartupNative.h>
#include <Runtime/ScriptNamedValueNative.h>

namespace KaGuYa::Start::Runtime
{
class RootRuntimeOwner;
class InputRootOwner;
class IScrVmCommandErrorSink;
class IGraphicsRuntimeCalls;
class IRestoredScriptNativeExecution;
class IScrResourceLookupSource;
struct RootTitleSceneNativeDependencies final
{
    RootRuntimeOwner& root;
    EnvironmentRootOwner& environment;
    InputRootOwner& input;
    IGraphicsRuntimeCalls& graphics;
    IRestoredScriptNativeExecution& execution;
    const IScrResourceLookupSource& resources;
    IScrVmCommandErrorSink& errors;
    ScriptNamedValueNativeTables names;
    IRuntimeStringManagerSource& strings;
};
class RootTitleSceneNativeSource final:public ITitleSceneNativeSource
{
public:
    explicit RootTitleSceneNativeSource(RootTitleSceneNativeDependencies d) noexcept:dependencies_(d){}
    TitleSceneNativeManagerReceiver CaptureScenesAfter413BE0() override;
    EnvironmentRootStorage* CaptureEnvironmentAt4180C0() override;
    void ClearInformationDwordAfter413BE0() override;
    bool EvaluateLoadPredicateAfter413BE0() override;
    void WriteRootByteAfter413BE0(std::uint32_t,std::uint8_t) override;
    void ClearReplayAt4A2DD6() override;
    std::uint8_t LoadStartupScriptAfter413BE0(const char*,std::int32_t) override;
private:
    RootTitleSceneNativeDependencies dependencies_;
};
}
