#pragma once
#include <Runtime/ReplaySceneNative.h>
#include <Runtime/RootCgSceneNativeSource.h>
#include <Runtime/RootReplayScriptNativeServices.h>

namespace KaGuYa::Start::Runtime
{
struct PatternLayerNativeRenderServices;
struct RootReplaySceneNativeDependencies final
{
    RootCgSceneNativeDependencies common;
    PatternLayerNativeRenderServices& patterns;
    IRestoredScriptNativeExecution& execution;
    IScrVmCommandErrorSink& errors;
    ScriptNamedValueNativeTables names;
    const IScrResourceLookupSource& resourceNames;
    decltype(&::MessageBoxA) messageBox=&::MessageBoxA;
};
// 只装配既有原叶；不持有Scene80、ScriptCCh或Root/Params业务副本。
class RootReplaySceneNativeSource final:public IReplaySceneNativeSource,public IReplayButtonNativeSource
{
public:
    explicit RootReplaySceneNativeSource(RootReplaySceneNativeDependencies)noexcept;
    RootReplaySceneNativeSource(const RootReplaySceneNativeSource&)=delete;
    RootReplaySceneNativeSource& operator=(const RootReplaySceneNativeSource&)=delete;
    RootReplaySceneNativeSource(RootReplaySceneNativeSource&&)=delete;
    RootReplaySceneNativeSource& operator=(RootReplaySceneNativeSource&&)=delete;
    const ParamsRuntimeRoot* CaptureParamsAt418480(unsigned)override;
    EnvironmentRootStorage* CaptureEnvironmentAt4180C0(unsigned)override;
    InputRootNativeBody24h* CaptureInputAt418210(unsigned)override;
    EnvironmentRootStorage*& EnvironmentPublicationAt6198C8(unsigned)override;
    void EnsureRootAt413BE0(unsigned)override;
    void* RootAt6198C4(unsigned)override;
    YtlReferenceSlotRuntime& RootDrawSlotAt18(void*)override;
    YtlReferenceSlotRuntime& RootSoundSlotAt24(void*)override;
    YtlReferenceSlotRuntime& RootScenesSlotAt30(void*)override;
    YtlReferenceSlotRuntime& RootInformationSlotAt3C(void*)override;
    ReplayScriptNativeReceiver ScriptAt47CD00(void*)override;
    void WriteRootByte(void*,unsigned,std::uint8_t)override;
    void ConfigureScenesAt4703D0(void*,int)override;
    void PushSceneAt4A9030(void*,const char*)override;
    void QueueSceneAt47F920(void*,const char*)override;
    void QueueTitleAfterConstructorAt47E180(void*)override;
    bool AccelerateAt4AA970(void*)override;
    bool LoadSurfaceAt480000(void*,YtlReferenceSlotRuntime&,const char*)override;
    YtlReferenceSlotRuntime* CaptureScreenAt4ACDD0(void*,YtlReferenceSlotRuntime*)override;
    SIZE ReadScreenSizeAfter4182F0(unsigned)override;
    void ClearDirtyAfter4182F0(unsigned)override;
    void InvalidateAfter4182F0(unsigned,const RECT*)override;
    void RenderAt449DD0(void*,TransitOperation&)override;
    void DrawPageAfter4182F0(GraphicsSurfaceNativeBody*,const RECT*)override;
    void InvalidatedRectAt4AC316(const RECT*)override;
    std::uint8_t LoadButtonAt480930(void*,CgUiButtonNativeBody6Ch*,const CgUiButtonRecordNative28h*,int)override;
    const char* CueNameAt(unsigned)override;
    void PlayMusicAt4BCCC0(void*,RuntimeCString&,const char*,int,int)override;
    void PlayCueAt4BCD70(void*,const char*,int,int,std::uint8_t)override;
    void OpenReaderAt483D90(ArchiveReaderReference&,const char*,unsigned)override;
    bool LoadSpriteAt480650(SpriteNativeBodyA0h&,const ArchiveReaderReference&,std::uint8_t,unsigned,unsigned)override;
    bool LoadSpriteMemberAt4B0840(SpriteNativeBodyA0h&,int,const ArchiveReaderReference&,std::uint8_t,unsigned)override;
    const char* PatternNameAt4AD1B0(const ParamsPatternNative*,int)override;
    void StopMusicAt4383B0(void*,unsigned)override;
    void ClearVoicesAt4384D0(void*)override;
    void StopMappedEffectsAt413270(void*,EnvironmentVectorStorage<RuntimeCString>&)override;
    void RestorePatternsAt4C4330(void*)override;
    void PropagatePatternsAt4C4250(void*)override;
    void DrawRegisteredAt4AC490(void*)override;
    void ClearScreenAfter4182F0(unsigned,const RECT*)override;
    void ShowReplayError(EnvironmentRootStorage*,const char*)override;
private:
    RootReplaySceneNativeDependencies dependencies_;
    RootCgSceneNativeSource common_;
};
}
