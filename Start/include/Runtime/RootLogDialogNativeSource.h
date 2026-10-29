#pragma once
#include <Runtime/LogDialogNative.h>

namespace KaGuYa::Start::Runtime
{
class RootRuntimeOwner;
class NativeGraphicsRuntimeLibrary;
class IMessageEnvironmentPathSource;
struct RootLogDialogNativeDependencies final
{
    RootRuntimeOwner& root;
    EnvironmentRootOwner& environment;
    ParamsRuntimeRootOwner& params;
    InputRootOwner& input;
    NativeGraphicsRuntimeLibrary& graphics;
    IGuiControlResourceLoadServices& pictures;
    IStartFileApi& files;
    const IMessageEnvironmentPathSource& messagePaths;
    const RuntimeCString& font=ProcessLogDialogNativeFontFace();
    IRuntimeStringManagerSource& strings=DefaultRuntimeStringManagerSource();
};
// 只借唯一根的原槽/正文；Source方法不代做整个Log算法，不额外增持receiver。
class RootLogDialogNativeSource final:public ILogDialogNativeSource
{
public:
    explicit RootLogDialogNativeSource(RootLogDialogNativeDependencies d) noexcept:dependencies_(d){}
    EnvironmentRootStorage* CaptureEnvironmentAt4180C0(std::uint32_t) override;
    ParamsRuntimeRoot* CaptureParamsAt418480(std::uint32_t) override;
    InputRootNativeBody24h* CaptureInputAt418210(std::uint32_t) override;
    void EnsureRootAt413BE0(std::uint32_t) override;
    void* RootAt6198C4(std::uint32_t) override;
    YtlReferenceSlotRuntime& RootDrawSlotAt18(void*) override;
    YtlReferenceSlotRuntime& RootSoundSlotAt24(void*) override;
    YtlReferenceSlotRuntime& RootFileManagerSlotAt00(void*) override;
    YtlReferenceSlotRuntime& FileManagerMessageSlotAt10(void*) override;
    ScriptParamsNativeBody* ParamsAt47BDB0(void*) override;
    const char* FontFaceAt61A684() override;
    YtlReferenceSlotRuntime* CaptureBackgroundAt4ACDD0(void*,YtlReferenceSlotRuntime*) override;
    bool LoadSurfaceAt480000(void*,YtlReferenceSlotRuntime&,const char*) override;
    std::int32_t ReadFormatAt417C40(void*,std::int32_t,MessageFormatSelectionRawTarget,std::uint32_t,bool) override;
    bool PlayVoicesAt42C4F0(SoundManagerNativeBody50h*,const YtlReferenceSlotRuntime*) override;
    bool AnyVoiceAt4196C0(SoundManagerNativeBody50h*) override;
    void AdjustEffectsAt4C2410(SoundManagerNativeBody50h*) override;
    void RefreshSoundAt433F60(SoundManagerNativeBody50h*) override;
private:RootLogDialogNativeDependencies dependencies_;
};
}
