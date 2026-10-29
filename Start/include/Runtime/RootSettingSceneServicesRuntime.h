#pragma once
#include <Runtime/RootSettingDialogNativeSource.h>
#include <Runtime/RootSettingPageNativeSource.h>
#include <Runtime/RootSettingPage04NativeSource.h>
#include <Runtime/RootSettingDialogNativeApplyRuntime.h>
#include <Runtime/RootSettingDialogNativeConfirmRuntime.h>
#include <Runtime/SettingDialogNativeBusiness.h>
#include <Runtime/SettingDialogNativePages.h>
#include <Runtime/RootSettingSceneNativeSource.h>
#include <Runtime/RootSceneManagerNativeServicesRuntime.h>
#include <functional>
#include <optional>

namespace KaGuYa::Start::Runtime
{
struct RootSettingSceneServicesDependencies final
{
    RootRuntimeOwner& root;
    EnvironmentRootOwner& environment;
    ParamsRuntimeRootOwner& params;
    NativeInputRuntime& input;
    NativeGraphicsRuntimeLibrary& graphics;
    NativeSoundRuntimeLibrary& sound;
    RootDrawServicesRuntime& draws;
    IGuiControlResourceLoadServices& pictures;
    GeneralConfirmDialogNativeServices& general;
    NameRegistrationDialogNativeServices& names;
    IModalDialogRunnerServices& modal;
    // nullopt显式选择调用期绑定，不是固定域失败后的兜底。
    std::optional<std::reference_wrapper<TextWindowLayerNativeBodyE8h>> textBody=std::nullopt;
    const char* const& textFontFace;
    IScrVmCommandErrorSink& errors;
    ISettingPage01PreviewBlockNativeGlobals& previewGlobals=ProcessSettingDialogPreviewGlobals();
    RuntimeCString& nameFontFace=ProcessSettingDialogNativeGlobal(SettingDialogGlobalId::NameFace);
    RuntimeCString& systemFontFace=ProcessSettingDialogNativeGlobal(SettingDialogGlobalId::SystemFace);
    IEnvironmentRootMemory& memory=DefaultEnvironmentRootMemory();
    IRuntimeStringManagerSource& strings=DefaultRuntimeStringManagerSource();
    IRuntimeWideStringNativeApi& wide=DefaultRuntimeWideStringNativeApi();
    IStartFileApi& files=DefaultNativeSerialize18Files();
    decltype(&::MessageBoxA) messageBox=&::MessageBoxA;
    decltype(&::PostMessageA) post=&::PostMessageA;
};
// 所有原正文/全局字体由caller拥有。本集合不Capture、不建根、不复制业务状态。
// Manager/Task及所有逃逸GUI Ref必须先于本集合退役。
class RootSettingSceneServicesRuntime final
{
public:
    explicit RootSettingSceneServicesRuntime(RootSettingSceneServicesDependencies) noexcept;
    RootSettingSceneServicesRuntime(const RootSettingSceneServicesRuntime&)=delete;
    RootSettingSceneServicesRuntime& operator=(const RootSettingSceneServicesRuntime&)=delete;
    SettingSceneNativeServices& Services() noexcept{return scene_;}
    SettingDialogNativeRuntimeServices& DialogServices() noexcept{return dialog_;}
private:
    RootSettingDialogNativeSource parentSource_;
    RootSettingPageNativeSource pagesSource_;
    RootSettingPage04NativeSource textSource_;
    RootSceneGuiNativeScreenSource screen_;
    GuiModalInputSource input_;
    RootArchiveResourceSource resources_;
    SoundManagerNativeOpen open_;
    SettingPage01PreviewBlockNativeText previewText_;
    SettingPage04NativeTextApi text_;
    SettingDialogNativePages pages_;
    RootSettingDialogNativeApplyRuntime apply_;
    RootSettingDialogNativeConfirmRuntime confirm_;
    SettingDialogNativeBusiness business_;
    SettingPage02NativeServices pagesServices_;
    SettingPage01PreviewBlockNativeServices previewServices_;
    SettingPage03NativeServices soundServices_;
    SettingPage04NativeServices textServices_;
    SettingDialogNativeServices parent_;
    SettingDialogNativeRuntimeServices dialog_;
    SettingDialogNativeConstruction construction_;
    RootSettingSceneNativeSource sceneSource_;
    SettingSceneNativeServices scene_;
};
}
