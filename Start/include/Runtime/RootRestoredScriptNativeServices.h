#pragma once
#include <Runtime/RestoredScriptNative.h>
#include <Runtime/ScriptNamedValueNative.h>
#include <Runtime/ScriptNativeExecutionSelection.h>

namespace KaGuYa::Start::Runtime
{
class RootRuntimeOwner;
class RootRuntimePayload;
class IGraphicsRuntimeCalls;
class IScrVmCommandErrorSink;
// 两个CScript业务叶的明确依赖；不允许换成另一个独立Script或假成功。
class IRestoredScriptNativeExecution
{
public:
    virtual ~IRestoredScriptNativeExecution()=default;
    // caller已完成原取根/Script getter；不得在此重新推定最新Root的publication/槽。
    virtual ScriptNativeExecutionSelectionScope BindSelectedReceiver(RootRuntimePayload&,ScriptNativeBody&)=0;
    virtual std::uint8_t LoadAt4D3C90(ScriptNativeBody&,std::int32_t)=0;
    virtual std::int32_t DispatchAt4D3FF0(ScriptNativeBody&)=0;
};
struct RootRestoredScriptNativeDependencies final
{
    RootRuntimeOwner& root;
    EnvironmentRootOwner& environment;
    IGraphicsRuntimeCalls& graphics;
    IRestoredScriptNativeExecution& execution;
    IScrVmCommandErrorSink& errors;
    ScriptNamedValueNativeTables names;
    IRuntimeStringManagerSource& strings;
};
class RootRestoredScriptNativeServices final:public IRestoredScriptNativeServices
{
public:
    explicit RootRestoredScriptNativeServices(RootRestoredScriptNativeDependencies) noexcept;
    RestoredScriptNativeRootReceiver CaptureAfterEnsureAt413BE0() override;
    EnvironmentRootStorage* CaptureEnvironmentAt4180C0() override;
    std::uint8_t LoadScriptAt4D3C90(ScriptNativeBody&,std::int32_t) override;
    void ResetDrawOwnersAt47E940(DrawLayersView) override;
    void SetGraphicsTextFirstNameAt4D6CD0(const char*) override;
    void SetGraphicsTextSecondNameAt4D6DB0(const char*) override;
    std::int32_t ResolveNamedValueAt498020(IEnvironmentArchiveRuntime*,const char*) override;
    void WriteValueAt4C8220(ScriptParamsNativeBody&,std::int32_t,std::int32_t) override;
    std::int32_t DispatchAt4D3FF0(ScriptNativeBody&) override;
private:
    RootRestoredScriptNativeDependencies dependencies_;
};
}
