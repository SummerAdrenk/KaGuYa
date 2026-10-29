#pragma once
#include <Runtime/MusicSceneNative.h>
#include <Runtime/ModeButtonNativeGlobals.h>

namespace KaGuYa::Start::Runtime
{
class RootRuntimeOwner;
class NativeGraphicsRuntimeLibrary;
struct RootMusicSceneNativeDependencies final
{
    RootRuntimeOwner& root;
    EnvironmentRootOwner& environment;
    InputRootOwner& input;
    NativeGraphicsRuntimeLibrary& graphics;
    IGuiControlResourceLoadServices& pictures;
    const RuntimeCString& exitNameAt61A56C=ProcessModeButtonNativeExitCue();
    IRuntimeStringManagerSource& strings=DefaultRuntimeStringManagerSource();
};
// Scene/CSoundBtn共用同一根与声音服务；getter只借固定receiver，不额外Ensure或增持。
class RootMusicSceneNativeSource final:public IMusicSceneNativeSource,public IMusicButtonNativeSource
{
public:
    explicit RootMusicSceneNativeSource(RootMusicSceneNativeDependencies d) noexcept:dependencies_(d){}
    EnvironmentRootStorage* CaptureEnvironmentAt4180C0(unsigned) override;
    InputRootNativeBody24h* CaptureInputAt418210(unsigned) override;
    void EnsureRootAt413BE0(unsigned) override;
    void* RootAt6198C4(unsigned) override;
    YtlReferenceSlotRuntime& RootDrawSlotAt18(void*) override;
    YtlReferenceSlotRuntime& RootSoundSlotAt24(void*) override;
    YtlReferenceSlotRuntime& RootScenesSlotAt30(void*) override;
    void WriteRootByte(void*,unsigned,std::uint8_t) override;
    bool LoadSurfaceAt480000(void*,YtlReferenceSlotRuntime&,const char*) override;
    YtlReferenceSlotRuntime* CaptureScreenAt4ACDD0(void*,YtlReferenceSlotRuntime*) override;
    void ConfigureScenesAt4703D0(void*,int) override;
    void ClearScreenAfter4182F0(unsigned,const RECT*) override;
    void ClearDirtyAfter4182F0(unsigned) override;
    void InvalidateAfter4182F0(unsigned,const RECT*) override;
    void StopSoundAt4383B0(void*,unsigned) override;
    bool AccelerateAt4AA970(void*) override;
    void RenderAt449DD0(void*,TransitOperation&) override;
    void PushSceneAt4A9030(void*,const char*) override;
    void QueueSceneAt47F920(void*,const char*) override;
    void SynchronizeButtonAt4ABB40(void*) override;
    void* ButtonItemAt4ACF20(void*,int) override;
    const YtlReferenceSlotRuntime* PublicGuiAt47B7B0(void*) override;
    const char* ExitNameAt61A56C() override;
    void PlayMusicAt4BCCC0(void*,RuntimeCString&,const char*,int,int) override;
    void PlayCueAt4BCD70(void*,const char*,int,int,std::uint8_t) override;
    void DrawSurfaceAt441980(GraphicsSurfaceNativeBody*,int,int,const RECT*) override;
    void InvalidateRectAfter4182F0(const RECT*) override;
private:RootMusicSceneNativeDependencies dependencies_;
};
}
