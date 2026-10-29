#pragma once
#include <Runtime/RootSaveLoadDialogNativeSource.h>
#include <Runtime/RootSaveLoadDeletionNativeSource.h>
#include <Runtime/RootSaveLoadReturnTitleNativeSource.h>
#include <Runtime/SaveLoadDialogNativeConstructionRuntime.h>
#include <Runtime/RootSceneManagerNativeServicesRuntime.h>
#include <Runtime/RootScriptRecoveryRuntime.h>
#include <Runtime/SaveLoadSceneNative.h>

namespace KaGuYa::Start::Runtime
{
struct RootSaveLoadSceneServicesDependencies final
{
    RootRuntimeOwner& root;
    EnvironmentRootOwner& environment;
    NativeInputRuntime& input;
    NativeGraphicsRuntimeLibrary& graphics;
    IGuiControlResourceLoadServices& pictures;
    RootSaveLoadNativeSource& persistence;
    SaveLoadPageFactoryNative& pages;
    SaveLoadMessageBoxNativeServices& deletionConfirmation;
    GeneralConfirmDialogNativeServices& generalConfirmation;
    EnvironmentStartupRuntime& startup;
    TextWindowLayerNativeServices& text;
    IRuntimeStringManagerSource& strings=DefaultRuntimeStringManagerSource();
    IEnvironmentRootMemory& memory=DefaultEnvironmentRootMemory();
};

// 服务只借同根来源；不拥有184h/24h，也不在构造时捕获或创建原对象。
// 本集合与依赖须长于Manager及所有逃逸task/child的最终Ref释放。
class RootSaveLoadSceneServicesRuntime final
{
public:
    explicit RootSaveLoadSceneServicesRuntime(RootSaveLoadSceneServicesDependencies) noexcept;
    RootSaveLoadSceneServicesRuntime(const RootSaveLoadSceneServicesRuntime&)=delete;
    RootSaveLoadSceneServicesRuntime& operator=(const RootSaveLoadSceneServicesRuntime&)=delete;
    SaveLoadSceneNativeServices& Services() noexcept{return scene_;}
    SaveLoadDialogNativeRuntimeServices& DialogServices() noexcept{return runtime_;}
private:
    RootSaveLoadDialogNativeSource parent_;
    SaveLoadDialogNativeConstructionServices construction_;
    SaveLoadDialogNativeGraphicsSource drawing_;
    RootSceneGuiNativeScreenSource screen_;
    RootSaveLoadReturnTitleNativeSource returnTitle_;
    RootSaveLoadDeletionNativeSource deletion_;
    SaveLoadDialogNativeRuntimeServices runtime_;
    SaveLoadDialogNativeConstructionRuntime adapter_;
    RootScriptRecoveryRuntime recovery_;
    RootSaveLoadSceneNativeSource source_;
    SaveLoadSceneNativeServices scene_;
};
}
