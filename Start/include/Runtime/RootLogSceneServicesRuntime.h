#pragma once
#include <Runtime/RootLogDialogNativeSource.h>
#include <Runtime/RootLogSceneNativeSource.h>
#include <Runtime/RootSceneManagerNativeServicesRuntime.h>

namespace KaGuYa::Start::Runtime
{
struct RootLogSceneServicesDependencies final
{
    RootRuntimeOwner& root;
    EnvironmentRootOwner& environment;
    ParamsRuntimeRootOwner& params;
    NativeInputRuntime& input;
    NativeGraphicsRuntimeLibrary& graphics;
    IGuiControlResourceLoadServices& pictures;
    IGuiControlGraphicsSource& guiGraphics;
    IStartFileApi& files;
    const IMessageEnvironmentPathSource& messagePaths;
    const RuntimeCString& font=ProcessLogDialogNativeFontFace();
    IRuntimeStringManagerSource& strings=DefaultRuntimeStringManagerSource();
    IEnvironmentRootMemory& memory=DefaultEnvironmentRootMemory();
};
// 不拥有原20h/DC/行缓存/历史；集合及外部依赖须长于所有scene/GUI逃逸Ref。
class RootLogSceneServicesRuntime final
{
public:
    explicit RootLogSceneServicesRuntime(RootLogSceneServicesDependencies) noexcept;
    RootLogSceneServicesRuntime(const RootLogSceneServicesRuntime&)=delete;
    RootLogSceneServicesRuntime& operator=(const RootLogSceneServicesRuntime&)=delete;
    LogSceneNativeServices& Services() noexcept{return scene_;}
    LogDialogNativeServices& DialogServices() noexcept{return gui_;}
private:
    RootLogDialogNativeSource source_;
    RootSceneGuiNativeScreenSource screen_;
    GuiModalInputSource input_;
    LogDialogNativeTextApi text_;
    LogDialogNativeServices gui_;
    LogDialogNativeConstruction construction_;
    RootLogSceneNativeSource sceneSource_;
    LogSceneNativeServices scene_;
};
}
