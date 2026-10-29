#pragma once
#include <Runtime/CgSceneNative.h>
#include <Runtime/AnimationFormatNativePictures.h>
#include <Runtime/RootModeSelectButtonNativeSource.h>
#include <Runtime/RootScriptRecoveryRuntime.h>
#include <Runtime/ModeButtonNativeGlobals.h>
#include <Runtime/CgSceneUiNativeGlobals.h>
#include <Runtime/AppreciationSceneResourceProfile.h>

namespace KaGuYa::Start::Runtime
{
class NativeInputRuntime;
class NativeGraphicsRuntimeLibrary;
struct RootCgSceneNativeDependencies final
{
    RootRuntimeOwner& root;
    EnvironmentStartupRuntime& environment;
    ParamsRuntimeRootOwner& params;
    NativeInputRuntime& input;
    NativeGraphicsRuntimeLibrary& graphics;
    IGuiControlResourceLoadServices& pictures;
    RootScriptRecoveryRuntime& recovery;
    IStartFileApi& files;
    const IEnvironmentKeyTableSource& key;
    ArchiveCodecServices codecs;
    IArchiveReaderMemoryApi& readers;
    IGuiControlResourceMessageApi& messages;
    IRuntimeStringManagerSource& strings=DefaultRuntimeStringManagerSource();
    const FileTransactionContext* transaction=nullptr;
    const RuntimeCString& confirmAt61A564=ProcessCgSceneNativeConfirmCue();
    const RuntimeCString& focusAt61A568=ProcessCgSceneNativeFocusCue();
    const RuntimeCString& exitAt61A56C=ProcessAppreciationSceneCancelCue();
};
// 仅承接原叶；三个消费者共享Root/Env/Input和同一reader/codec来源，不拥有任何场景正文。
class RootCgSceneNativeSource final:public ICgSceneNativeSource,
    public ICgButtonNativeSource,public ICgOperationWindowNativeSource
{
public:
    explicit RootCgSceneNativeSource(RootCgSceneNativeDependencies) noexcept;
    RootCgSceneNativeSource(const RootCgSceneNativeSource&)=delete;
    RootCgSceneNativeSource& operator=(const RootCgSceneNativeSource&)=delete;
    RootCgSceneNativeSource(RootCgSceneNativeSource&&)=delete;
    RootCgSceneNativeSource& operator=(RootCgSceneNativeSource&&)=delete;
    const ParamsRuntimeRoot* CaptureParamsAt418480(unsigned) override;
    EnvironmentRootStorage* CaptureEnvironmentAt4180C0(unsigned) override;
    InputRootNativeBody24h* CaptureInputAt418210(unsigned) override;
    EnvironmentRootStorage*& EnvironmentPublicationAt6198C8(unsigned) override;
    EnvironmentRootStorage* CaptureEnvironmentInlineAt6198C8(unsigned) override;
    void EnsureRootAt413BE0(unsigned) override;
    void* RootAt6198C4(unsigned) override;
    YtlReferenceSlotRuntime& RootDrawSlotAt18(void*) override;
    YtlReferenceSlotRuntime& RootSoundSlotAt24(void*) override;
    YtlReferenceSlotRuntime& RootScenesSlotAt30(void*) override;
    SceneManagerNativeBody48h* CaptureScenesAt413BE0(unsigned) override;
    void WriteRootByte(void*,unsigned,std::uint8_t) override;
    void ConfigureScenesAt4703D0(void*,int) override;
    void PushSceneAt4A9030(void*,const char*) override;
    void QueueSceneAt47F920(void*,const char*) override;
    void QueueTitleAfterConstructorAt47E180(void*) override;
    bool AccelerateAt4AA970(void*) override;
    bool LoadSurfaceAt480000(void*,YtlReferenceSlotRuntime&,const char*) override;
    YtlReferenceSlotRuntime* CaptureScreenAt4ACDD0(void*,YtlReferenceSlotRuntime*) override;
    SIZE ReadScreenSizeAfter4182F0(unsigned) override;
    void ClearDirtyAfter4182F0(unsigned) override;
    void InvalidateAfter4182F0(unsigned,const RECT*) override;
    void RenderAt449DD0(void*,TransitOperation&) override;
    void DrawPageAfter4182F0(GraphicsSurfaceNativeBody*,const RECT*) override;
    void InvalidatedRectAt4AC316(const RECT*) override;
    std::uint8_t LoadButtonAt480930(void*,CgUiButtonNativeBody6Ch*,const CgUiButtonRecordNative28h*,int) override;
    const char* CueNameAt(unsigned) override;
    void PlayMusicAt4BCCC0(void*,RuntimeCString&,const char*,int,int) override;
    void PlayCueAt4BCD70(void*,const char*,int,int,std::uint8_t) override;
    void OpenReaderAt483D90(ArchiveReaderReference&,const char*,unsigned) override;
    bool LoadSpriteAt480650(SpriteNativeBodyA0h&,const ArchiveReaderReference&,std::uint8_t,unsigned,unsigned) override;
    bool LoadSpriteMemberAt4B0840(SpriteNativeBodyA0h&,int,const ArchiveReaderReference&,std::uint8_t,unsigned) override;
private:
    RootCgSceneNativeDependencies dependencies_;
    RootArchiveResourceSource archives_;
    RootModeSelectButtonNativeSource records_;
    AnimationFormatNativePictureServices pictureServices_;
    AnimationFormatNativePictures pictures_;
};
}
