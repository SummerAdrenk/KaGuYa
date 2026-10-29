#include <Runtime/LegacyItemAcquisitionSource.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/NativeGraphicsRuntimeLibrary.h>
#include <Runtime/ScriptArithmeticCommandNative.h>
#include <Runtime/ScriptNativeBody.h>
#include <Runtime/ReadScriptInputNative.h>
#include <Runtime/InputRootNative.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/DrawManagerNative.h>
#include <Runtime/LayerListNative.h>
#include <Runtime/LoveHolicGaugesNative.h>
#include <Runtime/DateWindowNative.h>
#include <Runtime/GameBuildProfile.h>
#include <Runtime/SoundParamsEnvironmentArchive.h>
#include <Runtime/GraphicsSurfaceCaptureRuntime.h>
#include <Runtime/GraphicsSurfaceReference.h>
#include <memory>
#include <string_view>
#include <cstring>
namespace KaGuYa::Start::Runtime
{
namespace
{
struct Image final{YtlReferenceSlotRuntime slot{};~Image() noexcept{ReleaseYtlReference(slot);}};
LayerItemNativeBody14h& Frame(RootRuntimePayload& root,bool maso)
{
    if(!CompiledGameProfile().dateWindow||!CompiledGameProfile().loveHolicGauges)
        throw std::logic_error("Item snapshot requires Love Holic date and Maso profiles");
    auto* draw=ResolveDrawManagerReferenceAt4175A0(root.DrawManagerSlotAt18());
    return *DrawLayerAt418C10(*draw,maso?StartupMasoGaugeLayerIndex():5u);
}
void SnapshotDate(DateWindowNativeBody& existing,GraphicsSurfaceNativeBody& full,ILegacyItemAcquisitionSource& source,
    IEnvironmentRootMemory& memory,IGraphicsScreenCalls& graphics)
{
    // 原0x4F7380忽略date_win。只读适配器复用共享日期渲染器，不改常驻Params/Date状态。
    struct State final:IDateWindowStateSource
    {
        ILegacyItemAcquisitionSource& source;
        explicit State(ILegacyItemAcquisitionSource& s):source(s){}
        int ReadDateValue(const char* n) override{return std::string_view(n)=="date_win"?0:source.Read(n);}
    } state(source);
    auto& original=*existing.services;
    DateWindowNativeServices services{original.resources,state,original.surfaces,original.drawing,original.strings,original.profile};
    LayerListNativeScope scope(memory,&graphics);
    auto* list=CreateLayerListNativeAt440E30(memory);
    struct ListOwner final{LayerListNativeBody18h* list;~ListOwner(){DeleteLayerListNativeAt441400(*list,1);}} listOwner{list};
    DateWindowNativeBody body;
    ConstructDateWindowNative(body,list,services);
    struct DateOwner final{DateWindowNativeBody& body;~DateOwner(){DeleteDateWindowNative(body,0);}} dateOwner{body};
    UpdateDateWindowNative(body);CompositeDateWindowNative(body,&full,nullptr);
}
}
RootRuntimePayload& LegacyItemAcquisitionRootSource::Root()
{d_.root.EnsureCreatedAt413BE0();return *d_.root.ReadPublishedAt6198C4();}
SceneManagerView LegacyItemAcquisitionRootSource::Scenes()
{
    auto manager=Root().PostWindow().BorrowSceneManagerAt30();
    if(!manager.IsNative()||!manager.NativeBody())throw std::logic_error("Item acquisition requires native SceneManager");
    return {manager.NativeBody(),manager.NativeServices()};
}
int LegacyItemAcquisitionRootSource::Read(const char* name)
{auto& root=Root();const int index=ResolveScriptNamedValueNativeAt498020(name,d_.names,d_.strings);return ReadScriptValueAt4C73A0(root.ParamsAt47BDB0(),index,d_.errors);}
void LegacyItemAcquisitionRootSource::Write(const char* name,int value)
{auto& root=Root();const int index=ResolveScriptNamedValueNativeAt498020(name,d_.names,d_.strings);WriteScriptValueAt4C8220(root.ParamsAt47BDB0(),index,value,d_.errors);}
UnSaveDataRuntime& LegacyItemAcquisitionRootSource::UnSave()
{return *static_cast<UnSaveDataRuntime*>(ResolveEnvironmentArchiveAt417990(*d_.environment.CaptureAt4180C0(),2));}
SaveDataEnvironmentArchive& LegacyItemAcquisitionRootSource::SaveData()
{return *static_cast<SaveDataEnvironmentArchive*>(ResolveEnvironmentArchiveAt417990(*d_.environment.CaptureAt4180C0(),1));}
SIZE LegacyItemAcquisitionRootSource::CanvasSize()
{auto* env=d_.environment.CaptureAt4180C0();return {static_cast<LONG>(env->widthAt18),static_cast<LONG>(env->heightAt1C)};}
RECT LegacyItemAcquisitionRootSource::SnapshotBounds(bool maso)
{
    auto& frame=Frame(Root(),maso);RECT result;
    reinterpret_cast<const LayerItemNativeFrameVtable*>(frame.vptrAt00)->boundsAt0C(&frame,&result);return result;
}
void LegacyItemAcquisitionRootSource::SnapshotLayer(bool maso,GraphicsSurfaceNativeBody& target)
{
    GraphicsSurfaceNativeScope scope(d_.graphics);LayerListNativeScope layers(d_.memory,&d_.graphics);
    const auto& profile=RequireLegacyItemAcquisitionProfile(d_.profile);
    if(maso&&static_cast<unsigned>(Read(profile.balance))>5u)return; // 原0x4F76A4为ja，负数与超5均不画。
    if(!maso&&(static_cast<unsigned>(Read("days"))>=60u||static_cast<unsigned>(Read("time"))>1u))return;
    const auto size=CanvasSize();Image full;AdoptGraphicsSurfaceAt407A80(full.slot,NewGraphicsSurfaceAt40C2E0(d_.graphics,d_.strings));
    auto* image=ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(full.slot);
    if(!image->CreateAt415010(d_.graphics,size.cx,size.cy,1,1))throw 1;
    auto& frame=Frame(Root(),maso);
    if(maso)
    {auto& body=reinterpret_cast<LoveHolicGaugeNativeBody&>(frame);UpdateLoveHolicGaugeNative(body);CompositeLoveHolicGaugeNative(body,image,nullptr);}
    else SnapshotDate(reinterpret_cast<DateWindowNativeBody&>(frame),*image,*this,d_.memory,d_.graphics);
    // 共享组件按全屏自然坐标画；有界裁剪到局部画布，不能把绝对坐标直接画到小Surface。
    const RECT region{target.originXAt10,target.originYAt14,target.originXAt10+target.widthAt08,target.originYAt14+target.heightAt0C};
    if(!d_.graphics.GuiCalls().CompositionAt4182F0().BlitSurface(target.handleAt04,image->handleAt04,0,0,nullptr,&region,nullptr,0))throw 1;
}
void LegacyItemAcquisitionRootSource::StartAudio(const char* name)
{
    // 原0x509A24/0x534CC0：先记录BGM12，播放loop1，再清voice和持久化effect列表。
    const auto volume=*SaveData().BorrowSoundParamsAt1C().MusicVolumeAt14();
    SetRuntimeCStringAt437680(UnSave().StringAt2C(),name,name?static_cast<int>(std::strlen(name)):0);
    auto* sound=ResolveEnvironmentObjectReference<SoundManagerNativeBody50h>(Root().SoundManagerSlotAt24());
    static_cast<void>(d_.root.NativeSoundServices().PlayBgmCapturedAt42BFE0(sound,name,static_cast<int>(volume),1));
    sound=ResolveEnvironmentObjectReference<SoundManagerNativeBody50h>(Root().SoundManagerSlotAt24());
    static_cast<void>(d_.root.NativeSoundServices().ClearVoicesCapturedAt4384D0(sound));
    sound=ResolveEnvironmentObjectReference<SoundManagerNativeBody50h>(Root().SoundManagerSlotAt24());
    d_.root.NativeSoundServices().StopEffectsCapturedAt413270(sound,&UnSave().mappingsAt30_);
}
void LegacyItemAcquisitionRootSource::SelectionCue(const char* name)
{
    const auto volume=*SaveData().BorrowSoundParamsAt1C().NormalEffectVolumeAt24();
    auto* sound=ResolveEnvironmentObjectReference<SoundManagerNativeBody50h>(Root().SoundManagerSlotAt24());
    static_cast<void>(d_.root.NativeSoundServices().PlayCueAt4BCD70(sound,name,0,static_cast<int>(volume),1));
}
int LegacyItemAcquisitionRootSource::Confirm(int mode,const YtlReferenceSlotRuntime& image)
{
    GeneralConfirmDialogNativeSession dialog(d_.confirmation,mode);
    auto argument=image;if(argument.controlAt08)++argument.controlAt08->referenceCountAt04;
    AssignGeneralConfirmDialogNativeCaptureAt48CA30(&dialog.Body(),argument);
    return dialog.RunAt5071D0();
}
int LegacyItemAcquisitionRootSource::TransitionStep()
{constexpr int steps[]{4,8,16,32};const auto speed=SaveData().valueAt70;return steps[speed<4?speed:0];}
bool LegacyItemAcquisitionRootSource::AccelerateTransition()
{return ReadScriptSuppressInputAt4C3430(d_.environment,d_.input)||ReadScriptAccelerateInputAt4AA970(d_.input);}
void LegacyItemAcquisitionRootSource::RenderTransition(const TransitOperation& operation)
{
    auto* draw=ResolveDrawManagerReferenceAt4175A0(Root().DrawManagerSlotAt18());
    static_cast<void>(RenderDrawTransitionAt4C39E0(*draw,operation,d_.graphics));d_.graphics.InvalidatedRectIfLoaded(nullptr);
}
void LegacyItemAcquisitionRootSource::ResetScriptState()
{Root().ParamsAt47BDB0().messageFormatIndexAt60=-1;auto* script=Root().ScriptAt47CD00();script->commandAt04.flags=0;script->commandAt04.reload[0]=0;}
void LegacyItemAcquisitionRootSource::ClearModalResult(){d_.interaction.ClearModalResultAt10();}
void LegacyItemAcquisitionRootSource::Command(const char* text){Scenes().WritePendingCommandAt1C(text);}
bool LegacyItemAcquisitionRootSource::Requested(LegacyItemAcquisitionAction action)
{
    if(Scenes().ReadPendingCommandAt1C()==LegacyItemAcquisitionActionName(action))return true;
    switch(action)
    {
    case LegacyItemAcquisitionAction::Save:return ReadScriptSaveKeyAt47F830(d_.environment,d_.input);
    case LegacyItemAcquisitionAction::Load:return ReadScriptLoadKeyAt47F650(d_.environment,d_.input);
    case LegacyItemAcquisitionAction::QuickSave:return ReadScriptQuickSaveKeyAt47F780(d_.environment,d_.input,d_.root,d_.errors,d_.names,d_.strings);
    case LegacyItemAcquisitionAction::QuickLoad:return ReadScriptQuickLoadKeyAt47F6D0(d_.environment,d_.input,d_.root,d_.errors,d_.names,d_.strings);
    // 原0x4961D0的后三项只有命令比较，没有Title/Likability独立键盘predicate。
    case LegacyItemAcquisitionAction::Likability:case LegacyItemAcquisitionAction::Title:case LegacyItemAcquisitionAction::Settings:return false;
    }
    throw std::invalid_argument("Unknown item acquisition system request");
}
void LegacyItemAcquisitionRootSource::CapturePreview()
{
    GraphicsSurfaceNativeScope scope(d_.graphics);Image image;
    CaptureGraphicsScreenAt4ACDD0(image.slot,d_.graphics,d_.graphics,d_.graphics);
    auto preview=Root().CreateSavePreviewService(d_.graphics,d_.graphics,d_.graphics,d_.root.Files(),d_.preview);preview.SetCaptureAt48CFF0(image.slot);
}
void LegacyItemAcquisitionRootSource::CaptureTransition()
{
    GraphicsSurfaceNativeScope scope(d_.graphics);Image image;CaptureGraphicsScreenAt4ACDD0(image.slot,d_.graphics,d_.graphics,d_.graphics);
    auto& root=Root();root.BindGraphicsSurfaceApi(d_.graphics);auto argument=image.slot;image.slot={};
    AssignGraphicsSurfaceReferenceAt4C1C60(root.TransitionFrameSlotAt1F0(),argument);
}
void LegacyItemAcquisitionRootSource::Push(LegacyItemAcquisitionAction a)
{if(a==LegacyItemAcquisitionAction::Save||a==LegacyItemAcquisitionAction::Load)d_.interaction.MarkRootSaveLoadAt1AF();Scenes().PushActiveScene(LegacyItemAcquisitionActionName(a));}
void LegacyItemAcquisitionRootSource::QuickSave(){CaptureTransition();d_.interaction.QuickSaveAt485FC0(0);}
bool LegacyItemAcquisitionRootSource::QuickLoad(){CaptureTransition();return d_.interaction.QuickLoadAt485CE0();}
bool LegacyItemAcquisitionRootSource::ConfirmTitle()
{
    if(!SaveData().bytesAt77[6])return true;
    if(!ResolveEnvironmentObjectReference<void>(Root().InformationDisplaySlotAt3C()))
    {
        // 原0x460780/0x53AC78：无Information走原Win32确认，不伪接受。
        auto* env=d_.environment.CaptureAt4180C0();
        constexpr char text[]="\x83\x5E\x83\x43\x83\x67\x83\x8B\x82\xC9\x96\xDF\x82\xE8\x82\xDC\x82\xB7\x81\x42\x82\xE6\x82\xEB\x82\xB5\x82\xA2\x82\xC5\x82\xB7\x82\xA9\x81\x48";
        return d_.messageBox(reinterpret_cast<HWND>(env->windowAt2C),text,env->titleAt10.GetString(),0x21)==IDOK;
    }
    CaptureTransition();GeneralConfirmDialogNativeSession confirm(d_.confirmation,1);confirm.ConsumeRootCaptureAt477C60();return confirm.RunAt5071D0()==6;
}
void LegacyItemAcquisitionRootSource::QueueLoadAfter()
{auto scenes=Scenes();scenes.ClearScenes();scenes.QueueScene("\x83\x8D\x81\x5B\x83\x68\x91\x49\x91\xF0\x8C\xE3\x8F\x88\x97\x9D");}
void LegacyItemAcquisitionRootSource::PostTitle()
{auto* env=d_.environment.CaptureAt4180C0();static_cast<void>(d_.postMessage(reinterpret_cast<HWND>(env->windowAt2C),WM_COMMAND,0x8015,0));}
void LegacyItemAcquisitionRootSource::Finish()
{
    auto& root=Root();auto* draw=ResolveDrawManagerReferenceAt4175A0(root.DrawManagerSlotAt18());
    DrawLayersView(draw,d_.memory).WithNativeCallServices(root.PostWindow().BorrowNativeDrawCallServices()).ResetOwnedLayersAt47E940(d_.graphics);
    Scenes().PopThenActivatePendingAt4C1760();
}
}
