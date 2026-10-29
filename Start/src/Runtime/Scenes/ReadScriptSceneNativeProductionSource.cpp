#include <Runtime/ReadScriptSceneNativeProductionSource.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/RootScriptRecoveryRuntime.h>
#include <Runtime/ReadScriptFrameAdvanceNative.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/ScriptArithmeticCommandNative.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
ReadScriptSceneNativeProductionSource::ReadScriptSceneNativeProductionSource(
    ReadScriptSceneNativeProductionDependencies dependencies) noexcept:dependencies_(dependencies){}
void ReadScriptSceneNativeProductionSource::EnsureCommandRootAt413BE0(std::uint32_t)
{dependencies_.root.EnsureCreatedAt413BE0();}
void* ReadScriptSceneNativeProductionSource::CommandRootAt6198C4(std::uint32_t)
{return dependencies_.root.ReadPublishedAt6198C4();}
SceneManagerNativeBody48h* ReadScriptSceneNativeProductionSource::CommandManagerAt4186D0(void* root,std::uint32_t)
{return ResolveEnvironmentObjectReference<SceneManagerNativeBody48h>(static_cast<RootRuntimePayload*>(root)->SceneManagerSlotAt30());}
IScrVmYieldBoundary& ReadScriptSceneNativeProductionSource::VmYield(){return dependencies_.yield;}
std::int32_t ReadScriptSceneNativeProductionSource::ExecuteCurrentScriptAt4D3FF0()
{
    dependencies_.root.EnsureCreatedAt413BE0();
    auto& selected=*dependencies_.root.Published();
    auto* script=selected.ScriptAt47CD00();
    auto scope=dependencies_.execution.BindSelectedReceiver(selected,*script);
    return dependencies_.execution.DispatchAt4D3FF0(*script);
}
ReadScriptEffectFactoryNativeServices& ReadScriptSceneNativeProductionSource::EffectFactory(){return dependencies_.effects;}
IReadScriptEffectMaskNativeServices& ReadScriptSceneNativeProductionSource::EffectMasks(){return dependencies_.masks;}
IMoveEffectNativeConstructionSource& ReadScriptSceneNativeProductionSource::MoveConstruction(){return dependencies_.moves;}
IReadScriptFlashShakeNativeSource& ReadScriptSceneNativeProductionSource::FlashShakeSource(){return dependencies_.flash;}
IReadScriptWaitStateSource& ReadScriptSceneNativeProductionSource::WaitState(){return dependencies_.waits;}
IReadScriptSoundWaitSource& ReadScriptSceneNativeProductionSource::SoundWait(){return dependencies_.sound;}
IReadScriptInteractionStateSource& ReadScriptSceneNativeProductionSource::InteractionState(){return dependencies_.interaction;}
IReadScriptInteractionNativeHost& ReadScriptSceneNativeProductionSource::InteractionHost(){return dependencies_.host;}
ReadScriptReturnTitleNativeServices& ReadScriptSceneNativeProductionSource::ReturnTitleNativeServices()
{return dependencies_.returnTitle.ReturnTitleNativeServices();}
bool ReadScriptSceneNativeProductionSource::ReadCommandReplayAt4C73A0(void* capturedRoot)
{
    auto* env=dependencies_.environment.CaptureAt4180C0();
    static_cast<void>(ResolveEnvironmentArchiveAt417990(*env,0)); // 498020不消费ECX，捕获/417990仍必须发生。
    const auto index=ResolveScriptNamedValueNativeAt498020("replay",dependencies_.commandNames,dependencies_.commandStrings);
    auto& params=static_cast<RootRuntimePayload*>(capturedRoot)->ParamsAt47BDB0();
    struct Errors final:IScrVmCommandErrorSink
    {
        EnvironmentRootOwner& environment;decltype(&::MessageBoxA) message;
        Errors(EnvironmentRootOwner& e,decltype(message) m):environment(e),message(m){}
        EnvironmentRootStorage& CaptureAt4180C0()override{return *environment.CaptureAt4180C0();}
        int ShowMessageBoxA(HWND window,const char* text,const char* caption,UINT type)override{return message(window,text,caption,type);}
    } errors(dependencies_.environment,dependencies_.messageBox);
    return ReadScriptValueAt4C73A0(params,index,errors)!=0;
}
IReadScriptToolServices& ReadScriptSceneNativeProductionSource::Tools(){return dependencies_.tools;}
IReadScriptTextServices& ReadScriptSceneNativeProductionSource::Text(){return dependencies_.text;}
IReadScriptMenuServices& ReadScriptSceneNativeProductionSource::Menu(){return dependencies_.menu;}
IReadScriptMenuServices& ReadScriptSceneNativeProductionSource::CommandMenu()
{
    if (!dependencies_.commandMenu) throw std::logic_error("Native command-menu source is not bound");
    return *dependencies_.commandMenu;
}
ReadScriptSceneNativeScrollServices ReadScriptSceneNativeProductionSource::ScrollServices(){return dependencies_.scroll;}
IDrawLayersViewSource& ReadScriptSceneNativeProductionSource::DrawLayers(){return dependencies_.layers;}
IGraphicsScreenCalls& ReadScriptSceneNativeProductionSource::Screen(){return dependencies_.screen;}
PatternLayerNativeRenderServices& ReadScriptSceneNativeProductionSource::Patterns(){return dependencies_.patterns;}
bool ReadScriptSceneNativeProductionSource::SuppressFrameAdvanceAt4C3350()
{return SuppressReadScriptDrawAt4B5A3A(dependencies_.environment,dependencies_.input);}
ReadScriptSceneNativeErrorTarget ReadScriptSceneNativeProductionSource::CaptureErrorTargetAt4180C0()
{
    const auto* env=dependencies_.environment.CaptureAt4180C0();
    return {reinterpret_cast<HWND>(env->windowAt2C),env->titleAt10.GetString()};
}
int ReadScriptSceneNativeProductionSource::MessageBoxErrorA(HWND window,const char* text,const char* caption,unsigned type)
{return dependencies_.messageBox(window,text,caption,type);}
void ReadScriptSceneNativeProductionSource::CleanupCurrentEnvironmentAt4AC6E0()
{dependencies_.errors.CleanupCurrentEnvironment();}
}
