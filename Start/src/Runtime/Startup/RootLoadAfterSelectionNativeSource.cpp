#include <Runtime/RootLoadAfterSelectionNativeSource.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/GuiControlResourceLoadRuntime.h>
#include <Runtime/GraphicsSurfaceCaptureRuntime.h>
#include <Runtime/GraphicsRuntimeApi.h>
#include <Runtime/DrawTransitionView.h>
#include <Runtime/SceneManagerNative.h>
#include <Runtime/SceneManagerView.h>
#include <Runtime/ReadScriptWaitStateSource.h>
#include <Runtime/TextWindowSoundSource.h>
#include <Runtime/InformationDisplayView.h>
#include <Runtime/RootScriptRecoveryRuntime.h>
#include <Runtime/ScrVmScriptCommandState.h>
#include <Runtime/RestoredScriptNative.h>
#include <Runtime/RootRestoredScriptNativeServices.h>
#include <Runtime/RootGaugeRuntime.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
RootRuntimePayload& CaptureRoot(RootRuntimeOwner& root)
{root.EnsureCreatedAt413BE0();return *root.Published();}
SceneManagerNativeBody48h* Scenes(RootRuntimePayload& root)
{return ResolveEnvironmentObjectReference<SceneManagerNativeBody48h>(root.SceneManagerSlotAt30());}
SceneControllerNativeBody28h* Controller(SceneManagerNativeBody48h* scenes)
{return ResolveEnvironmentObjectReference<SceneControllerNativeBody28h>(scenes->controllerAt10);}
template<class T>T ReadAt(const void* body,std::uint32_t offset)
{return *reinterpret_cast<const volatile T*>(reinterpret_cast<std::uintptr_t>(body)+offset);}
// 仅将合法性记录绑定本次选定payload；业务叶仍逐点取根，不能跟着新发布根改记录。
class PublishedRestore final:public IRestoredScriptNativeServices
{
public:
    PublishedRestore(IRestoredScriptNativeServices& source,ScriptNativePublication& publication) noexcept
        :source_(source),publication_(publication){}
    RestoredScriptNativeRootReceiver CaptureAfterEnsureAt413BE0() override{return source_.CaptureAfterEnsureAt413BE0();}
    EnvironmentRootStorage* CaptureEnvironmentAt4180C0() override{return source_.CaptureEnvironmentAt4180C0();}
    std::uint8_t LoadScriptAt4D3C90(ScriptNativeBody& body,int id) override{return source_.LoadScriptAt4D3C90(body,id);}
    void ResetDrawOwnersAt47E940(DrawLayersView view) override{source_.ResetDrawOwnersAt47E940(view);}
    void SetGraphicsTextFirstNameAt4D6CD0(const char* name) override{source_.SetGraphicsTextFirstNameAt4D6CD0(name);}
    void SetGraphicsTextSecondNameAt4D6DB0(const char* name) override{source_.SetGraphicsTextSecondNameAt4D6DB0(name);}
    int ResolveNamedValueAt498020(IEnvironmentArchiveRuntime* receiver,const char* name) override
    {return source_.ResolveNamedValueAt498020(receiver,name);}
    void WriteValueAt4C8220(ScriptParamsNativeBody& body,int index,int value) override{source_.WriteValueAt4C8220(body,index,value);}
    int DispatchAt4D3FF0(ScriptNativeBody& body) override{return source_.DispatchAt4D3FF0(body);}
    void OnPublishCommandFields(ScriptNativeBody&,unsigned mask) noexcept override{publication_.commandMask|=mask;}
private:
    IRestoredScriptNativeServices& source_;
    ScriptNativePublication& publication_;
};
}
RootLoadAfterSelectionNativeSource::RootLoadAfterSelectionNativeSource(
    RootLoadAfterSelectionNativeDependencies dependencies) noexcept:dependencies_(dependencies){}
void* RootLoadAfterSelectionNativeSource::CaptureArchive(std::uint32_t slot)
{return ResolveEnvironmentArchiveAt417990(*dependencies_.environment.CaptureAt4180C0(),slot);}
bool RootLoadAfterSelectionNativeSource::LoadBlackAt480000(YtlReferenceSlotRuntime& output,const char* name)
{
    auto* draw=CaptureRoot(dependencies_.root).PostWindow().DrawManagerAt18();
    return LoadGuiControlSurfaceReferenceAt480000(output,draw,name,dependencies_.pictures,
        dependencies_.graphics,dependencies_.strings);
}
std::uint32_t RootLoadAfterSelectionNativeSource::ReadModeAt04(){return ReadAt<std::uint32_t>(CaptureArchive(2),4);}
void RootLoadAfterSelectionNativeSource::WriteModeAt04(std::uint32_t value)
{*reinterpret_cast<volatile std::uint32_t*>(reinterpret_cast<std::uintptr_t>(CaptureArchive(2))+4u)=value;}
void RootLoadAfterSelectionNativeSource::WriteRootByteAt218(std::uint8_t value)
{CaptureRoot(dependencies_.root).WriteByteAt218(value);}
void RootLoadAfterSelectionNativeSource::WriteRootByteAt219(std::uint8_t value)
{CaptureRoot(dependencies_.root).WriteByteAt219(value);}
std::int32_t RootLoadAfterSelectionNativeSource::ReadStepAt497900()
{
    const auto index=ReadAt<std::uint32_t>(CaptureArchive(1),0x70);
    static_cast<void>(CaptureArchive(0));
    // 源PE6018A0和497903..790C：unsigned index<4，否则取0，不是自定范围拒绝。
    constexpr std::int32_t steps[]{4,8,16,32};return steps[index<4?index:0];
}
bool RootLoadAfterSelectionNativeSource::ReadGateAt6F(){return ReadAt<std::uint8_t>(CaptureArchive(1),0x6F)!=0;}
bool RootLoadAfterSelectionNativeSource::AccelerateAt4AA970(){return dependencies_.wait.SkipEffectAt4AA970();}
void RootLoadAfterSelectionNativeSource::RenderAt4C39E0(TransitOperation& operation)
{static_cast<void>(CaptureRoot(dependencies_.root).PostWindow().DrawTransitionAt18(dependencies_.graphics).RenderTransitionAt4C39E0(operation));}
void RootLoadAfterSelectionNativeSource::CommitAt4B02E0(){dependencies_.graphics.InvalidatedRectIfLoaded(nullptr);}
void RootLoadAfterSelectionNativeSource::WriteRootSubobjectDwordAt10(std::uint32_t value)
{
    // 根+3C尚为既有InformationDisplay宿主；只写其同一字段，不存场景私有副本。
    CaptureRoot(dependencies_.root).PostWindow().BorrowInformationDisplayAt3C().SuppressedAt10()=value;
}
void RootLoadAfterSelectionNativeSource::StopMusicAt4383B0()
{const auto sound=dependencies_.sounds.CaptureRootAfter413BE0().Resolve();sound.slots.StopAt4383B0(sound.slots.MusicSlotAt10());}
void RootLoadAfterSelectionNativeSource::ClearVoicesAt4384D0()
{dependencies_.sounds.CaptureRootAfter413BE0().Resolve().voice.StopAndClearAllVoiceSlots();}
void RootLoadAfterSelectionNativeSource::StopMappedEffectsAt413270()
{
    // 4B6536固定ESI根；Env2+30捕获后才从这个原root解manager，不能再取新root。
    const auto root=dependencies_.sounds.CaptureRootAfter413BE0();
    auto* mapping=dependencies_.sounds.CaptureMappingsAt30();root.Resolve().effects.StopEffectsAt413270(mapping);
}
std::int32_t RootLoadAfterSelectionNativeSource::RestoreScriptAt4D4CC0()
{
    auto& root=CaptureRoot(dependencies_.root);auto* script=root.ScriptAt47CD00();
    auto scope=dependencies_.execution.BindSelectedReceiver(root,*script);
    PublishedRestore services(dependencies_.restored,root.ScriptPublication());
    return RestoreScriptNativeAt4D4CC0(*script,services);
}
void RootLoadAfterSelectionNativeSource::WriteSceneMarkerAt38(std::int32_t value)
{Scenes(CaptureRoot(dependencies_.root))->unwrittenAt38=value;}
void RootLoadAfterSelectionNativeSource::RestorePatternsAt4C14E0()
{CaptureRoot(dependencies_.root).PostWindow().DrawLayersAt18().WindowZero().RestoreSavedPatternsAt4C14E0(dependencies_.graphics,&dependencies_.patterns);}
void RootLoadAfterSelectionNativeSource::RebuildTextAt4C1650(std::uint32_t value)
{static_cast<void>(CaptureRoot(dependencies_.root).PostWindow().DrawLayersAt18().TextAtSlotThree().RebuildAt4C1650(value,&dependencies_.text,nullptr));}
void RootLoadAfterSelectionNativeSource::RestorePrimaryGauge()
{RootProgramGaugeRequests(dependencies_.root).RestoreVisiblePrimaryGauge();}
void RootLoadAfterSelectionNativeSource::PropagatePatternsAt4C4250()
{CaptureRoot(dependencies_.root).PostWindow().DrawLayersAt18().WindowZero().PropagateDirtyRectsAt4C4250();}
void RootLoadAfterSelectionNativeSource::DrawRegisteredAt4AC490()
{CaptureRoot(dependencies_.root).PostWindow().DrawLayersAt18().RegisteredLayers().RenderDirtyRects(dependencies_.graphics);}
void RootLoadAfterSelectionNativeSource::CaptureScreenAt4ACDD0(YtlReferenceSlotRuntime& output)
{
    // 4B66A7的4175A0接收者捕获仍发生，4ACDD0自身不消费该ECX。
    static_cast<void>(CaptureRoot(dependencies_.root).PostWindow().DrawManagerAt18());
    CaptureGraphicsScreenAt4ACDD0(output,dependencies_.graphics,dependencies_.graphics,dependencies_.graphics);
}
void RootLoadAfterSelectionNativeSource::ClearScreenAt4AA890(){dependencies_.graphics.ClearScreenIfLoaded(nullptr);}
void RootLoadAfterSelectionNativeSource::ClearCommandAt437680()
{SetRuntimeCStringAt437680(Scenes(CaptureRoot(dependencies_.root))->commandAt1C,"",0);}
void RootLoadAfterSelectionNativeSource::ClearScenesAt4C1460()
{ClearSceneControllerNativeAt4C14A5(*Controller(Scenes(CaptureRoot(dependencies_.root))));}
void RootLoadAfterSelectionNativeSource::ClearScriptFlagsAt4B6766()
{
    auto& root=CaptureRoot(dependencies_.root);auto* script=root.ScriptAt47CD00();
    script->commandAt04.flags=0;script->commandAt04.reload[0]=0;
    // 仅合法性元数据，业务仍是同一CCh字段；不写高三byte，也不复制script状态。
    root.ScriptPublication().commandMask|=ScrVmScriptCommandState::FlagsAt04Mask|ScrVmScriptCommandState::ReloadByteAt08Mask;
}
void RootLoadAfterSelectionNativeSource::PushActiveAt4A9030(const char* name)
{
    auto* controller=Controller(Scenes(CaptureRoot(dependencies_.root)));
    auto* factory=ResolveEnvironmentObjectReference<SceneFactoryNativeBody4h>(controller->factoryAt1C);
    auto* task=factory->vptrAt00->createAt00(factory,name);
    PushSceneTaskNativeAt40B050(controller->activeAt10,task,RequireSceneManagerNativeServices().controller);
}
void RootLoadAfterSelectionNativeSource::ResetAndQueueTitleAt47E180()
{
    auto* receiver=Scenes(CaptureRoot(dependencies_.root));
    dependencies_.recovery.ResetAndQueueTitleAt47E180(SceneManagerView(receiver,RequireSceneManagerNativeServices()));
}
void RootLoadAfterSelectionNativeSource::QueueAt47F920(const char* name)
{QueueSceneControllerNativeAt47F966(*Controller(Scenes(CaptureRoot(dependencies_.root))),name,RequireSceneManagerNativeServices().controller);}
}
