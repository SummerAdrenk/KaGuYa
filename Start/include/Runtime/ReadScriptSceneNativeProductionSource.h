#pragma once
#include <Runtime/ReadScriptSceneNative.h>
#include <Runtime/RootRestoredScriptNativeServices.h>

namespace KaGuYa::Start::Runtime
{
class RootRuntimeOwner;
class RootScriptRecoveryRuntime;
class InputRootOwner;
struct ReadScriptSceneNativeProductionDependencies final
{
    RootRuntimeOwner& root;
    EnvironmentRootOwner& environment;
    InputRootOwner& input;
    IRestoredScriptNativeExecution& execution;
    IScrVmYieldBoundary& yield;
    ReadScriptEffectFactoryNativeServices& effects;
    IReadScriptEffectMaskNativeServices& masks;
    IMoveEffectNativeConstructionSource& moves;
    IReadScriptFlashShakeNativeSource& flash;
    IReadScriptWaitStateSource& waits;
    IReadScriptSoundWaitSource& sound;
    IReadScriptInteractionStateSource& interaction;
    IReadScriptInteractionNativeHost& host;
    IReadScriptReturnTitleNativeServicesSource& returnTitle;
    IReadScriptToolServices& tools;
    IReadScriptTextServices& text;
    IReadScriptMenuServices& menu;
    ReadScriptSceneNativeScrollServices scroll;
    IDrawLayersViewSource& layers;
    IGraphicsScreenCalls& screen;
    PatternLayerNativeRenderServices& patterns;
    RootScriptRecoveryRuntime& errors;
    decltype(&::MessageBoxA) messageBox=&::MessageBoxA;
    ScriptNamedValueNativeTables commandNames=GameScriptNamedValueTables();
    IRuntimeStringManagerSource& commandStrings=DefaultRuntimeStringManagerSource();
    IReadScriptMenuServices* commandMenu=nullptr;
};
// 外部服务集合，无BCh标量/Ref/packet/CString镜像，getter不提前取根。
class ReadScriptSceneNativeProductionSource final:public IReadScriptSceneNativeSource
{
public:
    explicit ReadScriptSceneNativeProductionSource(ReadScriptSceneNativeProductionDependencies) noexcept;
    void EnsureCommandRootAt413BE0(std::uint32_t)override;
    void* CommandRootAt6198C4(std::uint32_t)override;
    SceneManagerNativeBody48h* CommandManagerAt4186D0(void*,std::uint32_t)override;
    IScrVmYieldBoundary& VmYield() override;
    std::int32_t ExecuteCurrentScriptAt4D3FF0() override;
    ReadScriptEffectFactoryNativeServices& EffectFactory() override;
    IReadScriptEffectMaskNativeServices& EffectMasks() override;
    IMoveEffectNativeConstructionSource& MoveConstruction() override;
    IReadScriptFlashShakeNativeSource& FlashShakeSource() override;
    IReadScriptWaitStateSource& WaitState() override;
    IReadScriptSoundWaitSource& SoundWait() override;
    IReadScriptInteractionStateSource& InteractionState() override;
    IReadScriptInteractionNativeHost& InteractionHost() override;
    ReadScriptReturnTitleNativeServices& ReturnTitleNativeServices()override;
    bool ReadCommandReplayAt4C73A0(void*)override;
    IReadScriptToolServices& Tools() override;
    IReadScriptTextServices& Text() override;
    IReadScriptMenuServices& Menu() override;
    IReadScriptMenuServices& CommandMenu() override;
    ReadScriptSceneNativeScrollServices ScrollServices() override;
    IDrawLayersViewSource& DrawLayers() override;
    IGraphicsScreenCalls& Screen() override;
    PatternLayerNativeRenderServices& Patterns() override;
    bool SuppressFrameAdvanceAt4C3350() override;
    ReadScriptSceneNativeErrorTarget CaptureErrorTargetAt4180C0() override;
    int MessageBoxErrorA(HWND,const char*,const char*,unsigned) override;
    void CleanupCurrentEnvironmentAt4AC6E0() override;
private:
    ReadScriptSceneNativeProductionDependencies dependencies_;
};
}
