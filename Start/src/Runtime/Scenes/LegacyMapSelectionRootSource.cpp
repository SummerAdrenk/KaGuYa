#include <Runtime/LegacyMapSelectionRootSource.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/EnvironmentStateArchives.h>
#include <Runtime/InputRootNative.h>
#include <Runtime/InputNativeObjects.h>
#include <Runtime/ReadScriptInputNative.h>
#include <Runtime/GraphicsSurfaceCaptureRuntime.h>
#include <Runtime/NativeGraphicsRuntimeLibrary.h>
#include <Runtime/SoundManagerNativeRuntime.h>
#include <Runtime/SoundParamsEnvironmentArchive.h>
#include <Runtime/SoundParameterNative.h>
#include <Runtime/DrawManagerNative.h>
#include <Runtime/DateWindowNative.h>
#include <Runtime/LoveHolicGaugesNative.h>
#include <Runtime/GameBuildProfile.h>
#include <bit>
#include <cstring>
#include <stdexcept>
#include <string>

namespace KaGuYa::Start::Runtime
{
const char* LegacyMapSelectionActionName(LegacyMapSelectionAction action)
{
    using A=LegacyMapSelectionAction;
    switch(action)
    {
    case A::Save:return "\x83\x5A\x81\x5B\x83\x75\x89\xE6\x96\xCA";
    case A::Load:return "\x83\x8D\x81\x5B\x83\x68\x89\xE6\x96\xCA";
    case A::QuickSave:return "\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x5A\x81\x5B\x83\x75";
    case A::QuickLoad:return "\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x8D\x81\x5B\x83\x68";
    case A::Settings:return "\x90\xDD\x92\xE8\x89\xE6\x96\xCA";
    case A::Title:return "\x83\x5E\x83\x43\x83\x67\x83\x8B\x82\xD6";
    case A::Likability:return "\x8D\x44\x8A\xB4\x93\x78\x89\xE6\x96\xCA";
    }
    throw std::invalid_argument("Map selection: unknown system action");
}
RootRuntimePayload& LegacyMapSelectionRootSource::Root()
{d_.root.EnsureCreatedAt413BE0();return *d_.root.ReadPublishedAt6198C4();}
SceneManagerView LegacyMapSelectionRootSource::Scenes()
{auto v=Root().PostWindow().BorrowSceneManagerAt30();if(!v.IsNative()||!v.NativeBody())throw std::logic_error("Map selection requires native SceneManager");return {v.NativeBody(),v.NativeServices()};}
ScriptParamsNativeBody& LegacyMapSelectionRootSource::Params(){return Root().ParamsAt47BDB0();}
ScriptNativeBody& LegacyMapSelectionRootSource::Script(){return *Root().ScriptAt47CD00();}
UnSaveDataRuntime& LegacyMapSelectionRootSource::UnSave()
{return *static_cast<UnSaveDataRuntime*>(ResolveEnvironmentArchiveAt417990(*d_.environment.CaptureAt4180C0(),2));}
SaveDataEnvironmentArchive& LegacyMapSelectionRootSource::SaveData()
{return *static_cast<SaveDataEnvironmentArchive*>(ResolveEnvironmentArchiveAt417990(*d_.environment.CaptureAt4180C0(),1));}
SIZE LegacyMapSelectionRootSource::ScreenSize()
{auto* e=d_.environment.CaptureAt4180C0();return {static_cast<LONG>(e->widthAt18),static_cast<LONG>(e->heightAt1C)};}
bool LegacyMapSelectionRootSource::CharacterDisplay(unsigned index)
{
    auto* p=ResolveEnvironmentObjectReference<EnvironmentSoundParamsArchive>(SaveData().soundParamsAt1C);
    return *BorrowSoundCharacterDisplayAt4787E0(*p,int(index))!=0; // 06:455670返回角色记录+5。
}
std::uint16_t LegacyMapSelectionRootSource::GlobalWord(unsigned index)
{return static_cast<GlobalParamsArchive*>(ResolveEnvironmentArchiveAt417990(*d_.environment.CaptureAt4180C0(),7))->wordsAt0C.at(index);}
void LegacyMapSelectionRootSource::SetCommand(const char* name){Scenes().WritePendingCommandAt1C(name);}
void LegacyMapSelectionRootSource::ClearCommand(){SetCommand("");}
bool LegacyMapSelectionRootSource::Requested(LegacyMapSelectionAction action)
{
    if(Scenes().ReadPendingCommandAt1C()==LegacyMapSelectionActionName(action))return true;
    using A=LegacyMapSelectionAction;
    switch(action)
    {
    case A::Save:return ReadScriptSaveKeyAt47F830(d_.environment,d_.input);
    case A::Load:return ReadScriptLoadKeyAt47F650(d_.environment,d_.input);
    case A::QuickSave:return ReadScriptQuickSaveKeyAt47F780(d_.environment,d_.input,d_.root,d_.errors,d_.names,d_.strings);
    case A::QuickLoad:return ReadScriptQuickLoadKeyAt47F6D0(d_.environment,d_.input,d_.root,d_.errors,d_.names,d_.strings);
    default:return false;
    }
}
void LegacyMapSelectionRootSource::CaptureTransition()
{
    GraphicsSurfaceNativeScope scope(d_.graphics);GraphicsSurfaceReference image(d_.graphics);
    CaptureGraphicsScreenAt4ACDD0(image.Slot(),d_.graphics,d_.graphics,d_.graphics);
    auto& root=Root();root.BindGraphicsSurfaceApi(d_.graphics);auto owned=image.Detach();AssignGraphicsSurfaceReferenceAt4C1C60(root.TransitionFrameSlotAt1F0(),owned);
}
void LegacyMapSelectionRootSource::CapturePreview()
{
    GraphicsSurfaceNativeScope scope(d_.graphics);GraphicsSurfaceReference image(d_.graphics);
    CaptureGraphicsScreenAt4ACDD0(image.Slot(),d_.graphics,d_.graphics,d_.graphics);
    auto preview=Root().CreateSavePreviewService(d_.graphics,d_.graphics,d_.graphics,d_.root.Files(),d_.preview);preview.SetCaptureAt48CFF0(image.Slot());
}
void LegacyMapSelectionRootSource::PushSystemScene(LegacyMapSelectionAction a)
{if(a==LegacyMapSelectionAction::Save||a==LegacyMapSelectionAction::Load)d_.interaction.MarkRootSaveLoadAt1AF();Scenes().PushActiveScene(LegacyMapSelectionActionName(a));}
void LegacyMapSelectionRootSource::QuickSave()
{
    // 04:44CF30/05-12的QS Begin按开关确认；15:470840与共享Entries自行确认，唯一owner留在共享层。
    if(SaveData().bytesAt77[2])CaptureTransition();
    d_.interaction.QuickSaveAt485FC0(0); // 原十款Execute(0)：复用Scene已提交的预览，不再次截图。
}
bool LegacyMapSelectionRootSource::QuickLoad()
{
    // 04:44D0C0、05:45EBB0至15:4671F0；关闭确认时不消耗/替换转场帧。
    if(SaveData().bytesAt77[3])CaptureTransition();
    return d_.interaction.QuickLoadAt485CE0();
}
void LegacyMapSelectionRootSource::QueueLoadAfter()
{auto scenes=Scenes();scenes.ClearScenes();scenes.QueueScene("\x83\x8D\x81\x5B\x83\x68\x91\x49\x91\xF0\x8C\xE3\x8F\x88\x97\x9D");}
void LegacyMapSelectionRootSource::PostTitle()
{auto* e=d_.environment.CaptureAt4180C0();static_cast<void>(d_.postMessage(reinterpret_cast<HWND>(e->windowAt2C),WM_COMMAND,0x8015,0));}
int LegacyMapSelectionRootSource::Confirm(int mode,unsigned flags)
{
    if(!ResolveEnvironmentObjectReference<void>(Root().InformationDisplaySlotAt3C()))
    {
        const char* text;
        if(mode==3)text="\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x5A\x81\x5B\x83\x75\x82\xB5\x82\xDC\x82\xB7\x82\xA9\x81\x48";
        else if(mode==4)text="\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x8D\x81\x5B\x83\x68\x82\xB5\x82\xDC\x82\xB7\x82\xA9\x81\x48";
        else if(mode==1)text="\x83\x5E\x83\x43\x83\x67\x83\x8B\x82\xC9\x96\xDF\x82\xE8\x82\xDC\x82\xB7\x81\x42\x82\xE6\x82\xEB\x82\xB5\x82\xA2\x82\xC5\x82\xB7\x82\xA9\x81\x48";
        else throw std::invalid_argument("Map selection: confirmation mode");
        auto* e=d_.environment.CaptureAt4180C0();const int result=d_.messageBox(reinterpret_cast<HWND>(e->windowAt2C),text,e->titleAt10.GetString(),flags);
        if(!result)return 0;return result==IDYES||result==IDOK?6:7;
    }
    CaptureTransition();GeneralConfirmDialogNativeSession confirm(d_.confirmation,mode);confirm.ConsumeRootCaptureAt477C60();return confirm.RunAt5071D0();
}
void LegacyMapSelectionRootSource::PrepareLayers(){d_.interaction.ClearModalResultAt10();}
void LegacyMapSelectionRootSource::FinishScene()
{
    auto& r=Root();auto* draw=ResolveDrawManagerReferenceAt4175A0(r.DrawManagerSlotAt18());
    DrawLayersView(draw,d_.memory).WithNativeCallServices(r.PostWindow().BorrowNativeDrawCallServices()).ResetOwnedLayersAt47E940(d_.graphics);
    Scenes().PopThenActivatePendingAt4C1760();
}
bool LegacyMapSelectionRootSource::AccelerateTransition()
{return ReadScriptSuppressInputAt4C3430(d_.environment,d_.input)||ReadScriptAccelerateInputAt4AA970(d_.input);}
bool LegacyMapSelectionRootSource::EscapePressed()
{return ReadKeyboardPressedAt47F4E0(*ResolveEnvironmentObjectReference<KeyboardManagerNativeBody438h>(d_.input.CaptureAt418210()->keyboardAt18),0);}
bool LegacyMapSelectionRootSource::RightPressed()
{const auto* m=ResolveEnvironmentObjectReference<MouseManagerNativeBody3Ch>(d_.input.CaptureAt418210()->mouseAt00);return !m->previousAt30[1]&&m->pressedAt10[1]&&!m->suppressionAt28;}
void LegacyMapSelectionRootSource::ResetKeyboard()
{
    // 06:4F3A50->40A630晚取raw键盘的+08复位；只清raw，不伪改双缓冲绑定。
    const auto* keyboard=ResolveEnvironmentObjectReference<KeyboardManagerNativeBody438h>(d_.input.CaptureAt418210()->keyboardAt18);
    auto* raw=ResolveEnvironmentObjectReference<KeyboardRawNativeBody104h>(keyboard->rawAt04);
    using Reset=void(__thiscall*)(void*);Reset reset;
    std::memcpy(&reset,reinterpret_cast<const std::byte*>(raw->vptrAt00)+8,sizeof(reset));reset(raw);
}
TransitRuntime& LegacyMapSelectionRootSource::Transition()
{return *ResolveDrawTransit(*ResolveDrawManagerReferenceAt4175A0(Root().DrawManagerSlotAt18()));}
void LegacyMapSelectionRootSource::Present(){d_.graphics.InvalidatedRectIfLoaded(nullptr);}
void LegacyMapSelectionRootSource::PlayBgmAndStopEffects(const char* name)
{
    auto* params=ResolveEnvironmentObjectReference<EnvironmentSoundParamsArchive>(SaveData().soundParamsAt1C);
    auto& post=Root().PostWindow();auto* receiver=post.SoundManagerAt24();auto& sound=post.SoundServices();auto& u=UnSave();u.StringAt2C()=name;
    static_cast<void>(sound.PlayBgmCapturedAt42BFE0(receiver,name,std::bit_cast<int>(*params->MusicVolumeAt14()),1));
    static_cast<void>(sound.ClearVoicesCapturedAt4384D0(receiver));sound.StopEffectsCapturedAt413270(receiver,&u.mappingsAt30_);
}
void LegacyMapSelectionRootSource::PlayCue(const LegacyMapSelectionCue& cue)
{
    auto* params=ResolveEnvironmentObjectReference<EnvironmentSoundParamsArchive>(SaveData().soundParamsAt1C);auto& post=Root().PostWindow();
    static_cast<void>(post.SoundServices().PlayCueAt4BCD70(post.SoundManagerAt24(),cue.name,cue.group,std::bit_cast<int>(*params->NormalEffectVolumeAt24()),1));
}
void LegacyMapSelectionRootSource::LoadAnimation(SpriteRuntime& sprite,const char* name)
{
    const auto logical=std::string("vrm:\\")+name;
    ArchiveReaderFactoryRuntime reader(d_.archives,d_.transaction,d_.files,d_.readers,d_.codecs);
    if(!reader.LoadBorrowedAt483D90(logical.c_str(),d_.key)||!sprite.LoadArchiveReaderMemberAt4B0840(0,reader.Reader(),1,d_.codecs))throw 1;
    sprite.SetLogicalName(logical);sprite.SetActive(true);
}
void LegacyMapSelectionRootSource::CreateSnapshot(GraphicsSurfaceReference& out)
{
    out.ConstructAt40C2E0();const auto size=ScreenSize();
    if(!out->CreateAt415010(d_.graphics,size.cx,size.cy,1,1))throw 1;
    d_.graphics.SurfaceClearRaw(out->handleAt04,out->clearColorAt20,nullptr);
}
void LegacyMapSelectionRootSource::CaptureDate(GraphicsSurfaceReference& out)
{
    if(!CompiledGameProfile().dateWindow)throw std::logic_error("Map selection: Date layer not bound");
    auto* draw=ResolveDrawManagerReferenceAt4175A0(Root().DrawManagerSlotAt18());
    auto& date=*reinterpret_cast<DateWindowNativeBody*>(DrawLayerAt418C10(*draw,5));
    auto& service=*date.services;const auto& p=service.profile;auto& state=service.state;
    GraphicsSurfaceNativeScope scope(d_.graphics);CreateSnapshot(out);
    // 05:4FD610、07:4EE120、08:4F74C0快照读实时日期，忽略date_win；不改常驻层缓存。
    const bool love=p.strategy==DateWindowStrategy::LoveHolicDays;
    const int month=love?0:state.ReadDateValue("month"),day=love?0:state.ReadDateValue("date");
    const int weekday=love?0:state.ReadDateValue("day"),days=love?state.ReadDateValue("days"):0,time=state.ReadDateValue("time");
    if(love?(unsigned(days)>=60||unsigned(time)>=2):(unsigned(month)-1u>=12||unsigned(day)-1u>=31||unsigned(weekday)>=7||unsigned(time)>=p.times.size()))return;
    const auto paint=[&](const char* name,const POINT* at=nullptr)
    {
        if(!name)return;
        GraphicsSurfaceReference image(d_.graphics);image.ConstructAt40C2E0();
        if(!service.resources.LoadIntoAt4800A0(service.resources.CaptureLoadReceiverAt413BE0(),*image,name))throw 1;
        const auto point=at?*at:POINT{image->originXAt10,image->originYAt14};
        static_cast<void>(d_.graphics.SurfaceBlitRaw(out->handleAt04,image->handleAt04,point.x,point.y,nullptr,nullptr,nullptr,0));
    };
    paint(p.background);
    if(love)paint(p.wholeDays[days]);
    else
    {
        const bool digits=p.strategy==DateWindowStrategy::EroZemiDigits;
        paint(p.months[month-(digits?1:0)]);
        if(!digits)paint(p.wholeDays[day]);
        else if(day>=10){paint(p.digits[day%10],&p.dayPositions[1]);paint(p.digits[day/10],&p.dayPositions[2]);}
        else paint(p.digits[day],&p.dayPositions[0]);
        paint(p.weekdays[weekday]);
    }
    paint(p.times[time]);
}
void LegacyMapSelectionRootSource::CaptureMasoGauge(GraphicsSurfaceReference& out)
{
    if(!CompiledGameProfile().loveHolicGauges)throw std::logic_error("Map selection: Maso layer not bound");
    auto* draw=ResolveDrawManagerReferenceAt4175A0(Root().DrawManagerSlotAt18());
    auto& gauge=*reinterpret_cast<LoveHolicGaugeNativeBody*>(DrawLayerAt418C10(*draw,StartupMasoGaugeLayerIndex()));
    auto& service=*gauge.services;const auto& p=service.profile.maso;
    GraphicsSurfaceNativeScope scope(d_.graphics);CreateSnapshot(out);
    const unsigned value=unsigned(service.state.ReadGaugeValue(p.valueName));if(value>5)return;
    // 08:4F7600只画0..5，不使用常驻Update的非法值清零策略；全屏画布保留绝对坐标。
    GraphicsSurfaceReference bg(d_.graphics),off(d_.graphics),on(d_.graphics);
    const auto load=[&](GraphicsSurfaceReference& image,const char* name)
    {image.ConstructAt40C2E0();if(!service.resources.LoadIntoAt4800A0(service.resources.CaptureLoadReceiverAt413BE0(),*image,name))throw 1;};
    load(bg,p.background);load(off,p.off);load(on,p.on);
    static_cast<void>(d_.graphics.SurfaceBlitRaw(out->handleAt04,bg->handleAt04,bg->originXAt10,bg->originYAt14,nullptr,nullptr,nullptr,0));
    for(unsigned i=0;i<5;++i)
    {
        auto point=p.positions[i];const bool lit=i<value;if(lit){--point.x;point.y-=21;}
        static_cast<void>(d_.graphics.SurfaceBlitRaw(out->handleAt04,(lit?on:off)->handleAt04,point.x,point.y,nullptr,nullptr,nullptr,0));
    }
}
}
