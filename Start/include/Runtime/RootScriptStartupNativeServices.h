#pragma once
#include <Runtime/ScriptStartupNative.h>
#include <Runtime/RootRestoredScriptNativeServices.h>
#include <Runtime/ScrResourceNameSource.h>

namespace KaGuYa::Start::Runtime
{
// publication只借caller选定CScript的记录；不是另一个根或脚本业务状态。
class RootScriptStartupNativeServices final:public IScriptStartupNativeServices
{
public:
    RootScriptStartupNativeServices(RootRestoredScriptNativeDependencies dependencies,
        const IScrResourceLookupSource& resources,ScriptNativePublication& publication) noexcept
        :common_(dependencies),resources_(resources),publication_(publication){}
    RestoredScriptNativeRootReceiver CaptureAfterEnsureAt413BE0() override;
    void ReplaceCapturedParamsAt481720(RestoredScriptNativeRootReceiver) override;
    EnvironmentRootStorage* CaptureEnvironmentAt4180C0() override;
    std::int32_t FindScriptIdAt47A020(const char*) override;
    std::uint8_t LoadScriptAt4D3C90(ScriptNativeBody&,std::int32_t) override;
    void ResetDrawOwnersAt47E940(DrawLayersView) override;
    void SetGraphicsTextFirstNameAt4D6CD0(const char*) override;
    void SetGraphicsTextSecondNameAt4D6DB0(const char*) override;
    std::int32_t ResolveNamedValueAt498020(IEnvironmentArchiveRuntime*,const char*) override;
    void WriteValueAt4C8220(ScriptParamsNativeBody&,std::int32_t,std::int32_t) override;
    std::int32_t DispatchAt4D3FF0(ScriptNativeBody&) override;
    void OnPublishCommandFields(ScriptNativeBody&,unsigned) noexcept override;
private:
    RootRestoredScriptNativeServices common_;
    const IScrResourceLookupSource& resources_;
    ScriptNativePublication& publication_;
};
}
