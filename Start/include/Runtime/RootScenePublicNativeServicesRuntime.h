#pragma once
#include <Runtime/RootModeSelectButtonNativeSource.h>
#include <Runtime/RootOptionCharSelectNativeSource.h>
#include <Runtime/NativeInputRuntime.h>
#include <Runtime/GuiModalNativeAdapters.h>
#include <Runtime/NativeGraphicsRuntimeLibrary.h>

namespace KaGuYa::Start::Runtime
{
struct RootScenePublicNativeDependencies final
{
    RootRuntimeOwner& root;
    EnvironmentStartupRuntime& environment;
    NativeInputRuntime& input;
    NativeGraphicsRuntimeLibrary& graphics;
    IStartFileApi& files;
    const IEnvironmentKeyTableSource& key;
    ArchiveCodecServices codecs;
    IGuiControlResourceMessageApi& messages;
    IRuntimeStringManagerSource& strings=DefaultRuntimeStringManagerSource();
    IEnvironmentRootMemory& memory=DefaultEnvironmentRootMemory();
    const FileTransactionContext* transaction=nullptr;
};
class RootCgUiButtonInputNativeSource final:public ICgUiButtonInputNativeSource
{
public:
    explicit RootCgUiButtonInputNativeSource(NativeInputRuntime& input)noexcept:input_(input){}
    InputRootNativeBody24h* CaptureAt418210()override{return input_.CaptureAt418210();}
private:
    NativeInputRuntime& input_;
};
class RootSceneGuiNativeScreenSource final:public IGuiDialogNativeScreenSource
{
public:
    explicit RootSceneGuiNativeScreenSource(NativeGraphicsRuntimeLibrary& graphics)noexcept:graphics_(graphics){}
    void InvalidateRectAt52603A(const RECT* rectangle)override{graphics_.InvalidatedRectIfLoaded(rectangle);}
private:
    NativeGraphicsRuntimeLibrary& graphics_;
};

// 公共Mode/Option的唯一服务owner；无factory依赖，可先于CG/Replay和完整Manager冷装配。
// 不创建原10h/8Ch正文；全部逃逸公共控件和借用它的Manager/场景服务须先退役。
class RootScenePublicNativeServicesRuntime final
{
public:
    explicit RootScenePublicNativeServicesRuntime(RootScenePublicNativeDependencies)noexcept;
    RootScenePublicNativeServicesRuntime(const RootScenePublicNativeServicesRuntime&)=delete;
    RootScenePublicNativeServicesRuntime& operator=(const RootScenePublicNativeServicesRuntime&)=delete;
    ModeSelectButtonNativeServices& Mode()noexcept{return modeServices_;}
    OptionCharSelectDialogNativeDependencies& Dialog()noexcept{return characterServices_;}
private:
    RootCgUiButtonInputNativeSource input_;
    GuiModalInputSource guiInput_;
    RootSceneGuiNativeScreenSource screen_;
    GuiControlResourceLoadRuntime pictures_;
    RootModeSelectButtonNativeSource modes_;
    RootOptionCharSelectNativeSource characters_;
    CgUiButtonNativeServices buttons_;
    ModeSelectButtonNativeServices modeServices_;
    OptionCharSelectDialogNativeDependencies characterServices_;
};
}
