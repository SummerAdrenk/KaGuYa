#pragma once
#include <Runtime/LikabilitySceneNative.h>
#include <Runtime/RootSceneManagerNativeServicesRuntime.h>
#include <Runtime/LikabilityProfileRuntime.h>
#include <optional>

namespace KaGuYa::Start::Runtime
{
struct RootLikabilitySceneNativeDependencies final
{
    RootRuntimeOwner& root;
    EnvironmentRootOwner& environment;
    NativeInputRuntime& input;
    NativeGraphicsRuntimeLibrary& graphics;
    IGuiControlResourceLoadServices& loader;
    IScrVmCommandErrorSink& errors;
    IRuntimeStringManagerSource& strings=DefaultRuntimeStringManagerSource();
    IEnvironmentRootMemory& memory=DefaultEnvironmentRootMemory();
    ScriptNamedValueNativeTables names=GameScriptNamedValueTables();
    std::optional<LikabilityAnimationServices> animation = std::nullopt;
};
// 20h场景与带现代扩展的GUI共用同一Root/Environment/Input/Graphics，不复制mode或CParams。
class RootLikabilitySceneNativeSource final:public ILikabilitySceneNativeSource,public ILikabilityDialogNativeSource,public ILikabilityProfileHost
{
public:
    explicit RootLikabilitySceneNativeSource(RootLikabilitySceneNativeDependencies d)noexcept:dependencies_(d){}
    RootLikabilitySceneNativeSource(const RootLikabilitySceneNativeSource&)=delete;
    RootLikabilitySceneNativeSource& operator=(const RootLikabilitySceneNativeSource&)=delete;
    EnvironmentRootStorage* CaptureEnvironmentAt4180C0(unsigned)override;
    EnvironmentRootStorage*& EnvironmentPublicationAt6198C8(unsigned)override;
    InputRootNativeBody24h* CaptureInputAt418210(unsigned)override;
    void EnsureRootAt413BE0(unsigned)override;
    void* RootAt6198C4(unsigned)override;
    YtlReferenceSlotRuntime& RootDrawSlotAt18(void*)override;
    YtlReferenceSlotRuntime& RootScenesSlotAt30(void*)override;
    ScriptParamsNativeBody* ParamsAt47BDB0(void*,unsigned)override;
    YtlReferenceSlotRuntime* CaptureBackgroundAt4ACDD0(void*,YtlReferenceSlotRuntime*,unsigned)override;
    void DrawBackgroundAt52624D(int,const RECT*)override;
    void AdvanceScenesAt4C1760(void*)override;
    GuiDialogNativeBody5Ch* ConstructDialogAt4E0E30(void*)override;
    std::int32_t ReadLikabilityPage()override;
    void StoreLikabilityPage(std::int32_t)override;
    IGraphicsSurfaceRawCopyApi& LikabilityCopyApi()override;
    LikabilityAnimationServices& LikabilityAnimation()override;
private:
    RootLikabilitySceneNativeDependencies dependencies_;
};
class RootLikabilitySceneServicesRuntime final
{
public:
    explicit RootLikabilitySceneServicesRuntime(RootLikabilitySceneNativeDependencies)noexcept;
    RootLikabilitySceneServicesRuntime(const RootLikabilitySceneServicesRuntime&)=delete;
    RootLikabilitySceneServicesRuntime& operator=(const RootLikabilitySceneServicesRuntime&)=delete;
    LikabilitySceneNativeServices& Scene()noexcept{return scene_;}
    LikabilityDialogNativeServices& Dialog()noexcept{return dialog_;}
    RootLikabilitySceneNativeSource& Source()noexcept{return source_;}
private:
    RootLikabilitySceneNativeSource source_;
    GuiModalInputSource input_;
    RootSceneGuiNativeScreenSource screen_;
    LikabilityDialogNativeServices dialog_;
    LikabilitySceneNativeServices scene_;
};
}
