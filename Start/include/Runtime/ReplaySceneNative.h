#pragma once
#include <Runtime/ReplaySceneNativeBody.h>
#include <Runtime/ReplaySceneNativeScope.h>
#include <Runtime/ReplaySelectionNative.h>
#include <Runtime/CgSceneNative.h>
#include <Runtime/ReplayScriptNativeServices.h>

namespace KaGuYa::Start::Runtime
{
// Root/Env/图形/声音公共叶沿既有原生合同；新增叶的receiver均来自本次固定根。
class IReplaySceneNativeSource:public ICgSceneNativeSource,public IReplaySelectionParamsNativeSource
{
public:
    virtual const ParamsRuntimeRoot* CaptureParamsAt418480(std::uint32_t site) override=0;
    virtual YtlReferenceSlotRuntime& RootInformationSlotAt3C(void*)=0;
    virtual ReplayScriptNativeReceiver ScriptAt47CD00(void* capturedRoot)=0;
    virtual const char* PatternNameAt4AD1B0(const ParamsPatternNative*,std::int32_t)=0;
    virtual void StopMusicAt4383B0(void* sound,std::uint32_t slot)=0;
    virtual void ClearVoicesAt4384D0(void* sound)=0;
    virtual void StopMappedEffectsAt413270(void* sound,EnvironmentVectorStorage<RuntimeCString>&)=0;
    virtual void RestorePatternsAt4C4330(void* capturedDraw)=0;
    virtual void PropagatePatternsAt4C4250(void* capturedDraw)=0;
    virtual void DrawRegisteredAt4AC490(void* capturedDraw)=0;
    virtual void ClearScreenAfter4182F0(std::uint32_t site,const RECT*)=0;
    virtual void ShowReplayError(EnvironmentRootStorage*,const char* text)=0;
};
struct ReplaySceneNativeServices final
{
    IReplaySceneNativeSource& source;
    ReplayButtonNativeServices& buttons;
    IReplayScriptNativeExecution& script;
    IEnvironmentRootMemory& memory;
    IGraphicsSurfaceApi& surfaces;
};
ReplaySceneNativeServices& RequireReplaySceneNativeServices();
const ReplaySceneNativeVtable4& ReplaySceneNativeMethods() noexcept;
ReplaySceneNativeBody80h* ConstructReplaySceneNativeAt49EC90(ReplaySceneNativeBody80h&,ReplaySceneNativeServices&);
void DestroyReplaySceneNativeAt4A6C60(ReplaySceneNativeBody80h&) noexcept;
ReplaySceneNativeBody80h* DeleteReplaySceneNativeAt4A6C60(ReplaySceneNativeBody80h*,std::uint32_t) noexcept;
void UpdateReplaySceneNativeAt4B7CE0(ReplaySceneNativeBody80h&,ReplaySceneNativeServices&);
void DrawReplaySceneNativeAt4B5E20(ReplaySceneNativeBody80h&) noexcept;
}
