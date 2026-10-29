#include <Runtime/LegacyCharacterSelectionRootSource.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/InputRootNative.h>
#include <Runtime/ReadScriptInputNative.h>
#include <Runtime/ReadScriptFrameAdvanceNative.h>
#include <Runtime/GraphicsSurfaceCaptureRuntime.h>
#include <Runtime/NativeGraphicsRuntimeLibrary.h>
#include <Runtime/ArchiveReaderFactoryRuntime.h>
#include <Runtime/SoundManagerNativeRuntime.h>
#include <Runtime/SoundParamsEnvironmentArchive.h>
#include <Runtime/DrawManagerNative.h>
#include <Runtime/LayerListNative.h>
#include <Runtime/LoveHolicGaugesNative.h>
#include <Runtime/GameBuildProfile.h>
#include <bit>
#include <vector>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
const char* LegacyCharacterSelectionActionName(LegacyCharacterSelectionAction action)
{
    using A=LegacyCharacterSelectionAction;
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
    throw std::invalid_argument("Character selection: unknown system action");
}
RootRuntimePayload& LegacyCharacterSelectionRootSource::Root()
{d_.root.EnsureCreatedAt413BE0();return *d_.root.ReadPublishedAt6198C4();}
SceneManagerView LegacyCharacterSelectionRootSource::Scenes()
{auto view=Root().PostWindow().BorrowSceneManagerAt30();if(!view.IsNative()||!view.NativeBody())throw std::logic_error("Character selection requires native SceneManager");return {view.NativeBody(),view.NativeServices()};}
ScriptParamsNativeBody& LegacyCharacterSelectionRootSource::Params(){return Root().ParamsAt47BDB0();}
ScriptNativeBody& LegacyCharacterSelectionRootSource::Script(){return *Root().ScriptAt47CD00();}
UnSaveDataRuntime& LegacyCharacterSelectionRootSource::UnSave()
{return *static_cast<UnSaveDataRuntime*>(ResolveEnvironmentArchiveAt417990(*d_.environment.CaptureAt4180C0(),2));}
SaveDataEnvironmentArchive& LegacyCharacterSelectionRootSource::SaveData()
{return *static_cast<SaveDataEnvironmentArchive*>(ResolveEnvironmentArchiveAt417990(*d_.environment.CaptureAt4180C0(),1));}
SIZE LegacyCharacterSelectionRootSource::ScreenSize()
{auto* e=d_.environment.CaptureAt4180C0();return {static_cast<LONG>(e->widthAt18),static_cast<LONG>(e->heightAt1C)};}
void LegacyCharacterSelectionRootSource::SetCommand(const char* name){Scenes().WritePendingCommandAt1C(name);}
void LegacyCharacterSelectionRootSource::ClearCommand(){SetCommand("");}
bool LegacyCharacterSelectionRootSource::Requested(LegacyCharacterSelectionAction action)
{
    if(Scenes().ReadPendingCommandAt1C()==LegacyCharacterSelectionActionName(action))return true;
    using A=LegacyCharacterSelectionAction;
    switch(action)
    {
    case A::Save:return ReadScriptSaveKeyAt47F830(d_.environment,d_.input);
    case A::Load:return ReadScriptLoadKeyAt47F650(d_.environment,d_.input);
    case A::QuickSave:return ReadScriptQuickSaveKeyAt47F780(d_.environment,d_.input,d_.root,d_.errors,d_.names,d_.strings);
    case A::QuickLoad:return ReadScriptQuickLoadKeyAt47F6D0(d_.environment,d_.input,d_.root,d_.errors,d_.names,d_.strings);
    default:return false;
    }
}
void LegacyCharacterSelectionRootSource::CaptureTransition()
{
    GraphicsSurfaceNativeScope scope(d_.graphics);GraphicsSurfaceReference image(d_.graphics);
    CaptureGraphicsScreenAt4ACDD0(image.Slot(),d_.graphics,d_.graphics,d_.graphics);
    auto& root=Root();root.BindGraphicsSurfaceApi(d_.graphics);auto owned=image.Detach();
    AssignGraphicsSurfaceReferenceAt4C1C60(root.TransitionFrameSlotAt1F0(),owned);
}
void LegacyCharacterSelectionRootSource::CapturePreview()
{
    GraphicsSurfaceNativeScope scope(d_.graphics);GraphicsSurfaceReference image(d_.graphics);
    CaptureGraphicsScreenAt4ACDD0(image.Slot(),d_.graphics,d_.graphics,d_.graphics);
    auto preview=Root().CreateSavePreviewService(d_.graphics,d_.graphics,d_.graphics,d_.root.Files(),d_.preview);
    preview.SetCaptureAt48CFF0(image.Slot());
}
void LegacyCharacterSelectionRootSource::PushSystemScene(LegacyCharacterSelectionAction a)
{if(a==LegacyCharacterSelectionAction::Save||a==LegacyCharacterSelectionAction::Load)d_.interaction.MarkRootSaveLoadAt1AF();Scenes().PushActiveScene(LegacyCharacterSelectionActionName(a));}
void LegacyCharacterSelectionRootSource::QuickSave()
{
    if(SaveData().bytesAt77[2])CaptureTransition();
    d_.interaction.QuickSaveAt485FC0(0);
}
bool LegacyCharacterSelectionRootSource::QuickLoad()
{
    if(SaveData().bytesAt77[3])CaptureTransition();
    return d_.interaction.QuickLoadAt485CE0();
}
void LegacyCharacterSelectionRootSource::QueueLoadAfter()
{auto scenes=Scenes();scenes.ClearScenes();scenes.QueueScene("\x83\x8D\x81\x5B\x83\x68\x91\x49\x91\xF0\x8C\xE3\x8F\x88\x97\x9D");}
void LegacyCharacterSelectionRootSource::PostTitle()
{auto* e=d_.environment.CaptureAt4180C0();static_cast<void>(d_.postMessage(reinterpret_cast<HWND>(e->windowAt2C),WM_COMMAND,0x8015,0));}
int LegacyCharacterSelectionRootSource::Confirm(int mode,const char* fallback,unsigned flags)
{
    if(!ResolveEnvironmentObjectReference<void>(Root().InformationDisplaySlotAt3C()))
    {
        // 01:44F880/44FA10、04:44D7E0；08选择确认没有无Information回退。
        if(!fallback)
        {
            if(mode==3)fallback="\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x5A\x81\x5B\x83\x75\x82\xB5\x82\xDC\x82\xB7\x82\xA9\x81\x48";
            else if(mode==4)fallback="\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x8D\x81\x5B\x83\x68\x82\xB5\x82\xDC\x82\xB7\x82\xA9\x81\x48";
            else if(mode==1)fallback="\x83\x5E\x83\x43\x83\x67\x83\x8B\x82\xC9\x96\xDF\x82\xE8\x82\xDC\x82\xB7\x81\x42\x82\xE6\x82\xEB\x82\xB5\x82\xA2\x82\xC5\x82\xB7\x82\xA9\x81\x48";
            else throw std::logic_error("Character selection confirmation requires Information service");
        }
        auto* e=d_.environment.CaptureAt4180C0();const int result=d_.messageBox(reinterpret_cast<HWND>(e->windowAt2C),fallback,e->titleAt10.GetString(),flags);
        if(!result)return 0; // 原MessageBox失败0沿非接受分支，不伪造IDOK/IDYES。
        return result==IDYES||result==IDOK?6:7;
    }
    CaptureTransition();GeneralConfirmDialogNativeSession confirm(d_.confirmation,mode);confirm.ConsumeRootCaptureAt477C60();return confirm.RunAt5071D0();
}
void LegacyCharacterSelectionRootSource::PrepareLayers(){d_.interaction.ClearModalResultAt10();}
void LegacyCharacterSelectionRootSource::FinishScene()
{
    auto& r=Root();auto* draw=ResolveDrawManagerReferenceAt4175A0(r.DrawManagerSlotAt18());
    DrawLayersView(draw,d_.memory).WithNativeCallServices(r.PostWindow().BorrowNativeDrawCallServices()).ResetOwnedLayersAt47E940(d_.graphics);
    Scenes().PopThenActivatePendingAt4C1760();
}
bool LegacyCharacterSelectionRootSource::AccelerateTransition()
{return ReadScriptSuppressInputAt4C3430(d_.environment,d_.input)||ReadScriptAccelerateInputAt4AA970(d_.input);}
bool LegacyCharacterSelectionRootSource::CancelPressed()
{
    const auto* input=d_.input.CaptureAt418210();const auto* mouse=ResolveEnvironmentObjectReference<MouseManagerNativeBody3Ch>(input->mouseAt00);
    const auto* key=ResolveEnvironmentObjectReference<KeyboardManagerNativeBody438h>(input->keyboardAt18);
    return ReadKeyboardPressedAt47F4E0(*key,0)||(!mouse->previousAt30[1]&&mouse->pressedAt10[1]&&!mouse->suppressionAt28);
}
TransitRuntime& LegacyCharacterSelectionRootSource::Transition()
{return *ResolveDrawTransit(*ResolveDrawManagerReferenceAt4175A0(Root().DrawManagerSlotAt18()));}
void LegacyCharacterSelectionRootSource::Present(){d_.graphics.InvalidatedRectIfLoaded(nullptr);}
void LegacyCharacterSelectionRootSource::PlayBgmAndStopEffects(const char* name)
{
    auto* params=ResolveEnvironmentObjectReference<EnvironmentSoundParamsArchive>(SaveData().soundParamsAt1C);
    auto& post=Root().PostWindow();auto* receiver=post.SoundManagerAt24();auto& sound=post.SoundServices();
    auto& u=UnSave();u.StringAt2C()=name;
    static_cast<void>(sound.PlayBgmCapturedAt42BFE0(receiver,name,std::bit_cast<int>(*params->MusicVolumeAt14()),1));
    static_cast<void>(sound.ClearVoicesCapturedAt4384D0(receiver));sound.StopEffectsCapturedAt413270(receiver,&u.mappingsAt30_);
}
void LegacyCharacterSelectionRootSource::PlaySelectionCue(const char* name)
{
    auto* params=ResolveEnvironmentObjectReference<EnvironmentSoundParamsArchive>(SaveData().soundParamsAt1C);
    auto& post=Root().PostWindow();static_cast<void>(post.SoundServices().PlayCueAt4BCD70(post.SoundManagerAt24(),name,0,std::bit_cast<int>(*params->NormalEffectVolumeAt24()),1));
}
void LegacyCharacterSelectionRootSource::LoadAnimation(SpriteRuntime& sprite,const char* commands,const char* pictures)
{
    const auto logical=[](const char* p){return std::string("vrm:\\")+p;};
    const auto load=[&](const char* name,std::size_t member,std::uint8_t initialization)
    {
        // 每个成员在实际读取时才经同一Root的source取目录；冷装配不缓存catalog。
        ArchiveReaderFactoryRuntime reader(d_.archives,d_.transaction,d_.files,d_.readers,d_.codecs);
        const auto path=logical(name);
        if(!reader.LoadBorrowedAt483D90(path.c_str(),d_.key) ||
           !sprite.LoadArchiveReaderMemberAt4B0840(member,reader.Reader(),initialization,d_.codecs))throw 1;
    };
    load(commands,0,1);
    if(pictures)load(pictures,1,0);
    sprite.SetLogicalName(logical(commands));sprite.SetActive(true);
}
void LegacyCharacterSelectionRootSource::CaptureMasoGauge(GraphicsSurfaceReference& out)
{
    if(!CompiledGameProfile().loveHolicGauges)throw std::logic_error("Character selection: Maso gauge not bound");
    auto* draw=ResolveDrawManagerReferenceAt4175A0(Root().DrawManagerSlotAt18());
    auto& gauge=*reinterpret_cast<LoveHolicGaugeNativeBody*>(DrawLayerAt418C10(*draw,StartupMasoGaugeLayerIndex()));
    const auto bounds=ReadLoveHolicGaugeBounds(gauge);
    GraphicsSurfaceNativeScope scope(d_.graphics);
    if(!out){out.ConstructAt40C2E0();if(!out->CreateAt415010(d_.graphics,bounds.right-bounds.left,bounds.bottom-bounds.top,1,1))throw 1;out->originXAt10=bounds.left;out->originYAt14=bounds.top;}
    d_.graphics.SurfaceClearRaw(out->handleAt04,out->clearColorAt20,nullptr);
    // 08:4F7600在负数/>5时不画；局部画布要减自身原点，不改共享屏幕绘制。
    if(unsigned(gauge.services->state.ReadGaugeValue(gauge.services->profile.maso.valueName))>5u)return;
    UpdateLoveHolicGaugeNative(gauge);
    struct LocalDrawing final : IGraphicsWindowSurfaceApi
    {
        IGraphicsWindowSurfaceApi& api;POINT origin;
        LocalDrawing(IGraphicsWindowSurfaceApi& a,POINT p):api(a),origin(p){}
        std::byte* GetSurfacePointerRaw(int h) override{return api.GetSurfacePointerRaw(h);}
        int GetSurfacePitchRaw(int h) override{return api.GetSurfacePitchRaw(h);}
        void SurfaceClearRaw(int h,unsigned c,const RECT* r) override{api.SurfaceClearRaw(h,c,r);}
        int SurfaceDrawRaw(int h,int x,int y,const SIZE* z,const RECT* r,const RECT* c,int a) override{return api.SurfaceDrawRaw(h,x,y,z,r,c,a);}
        int SurfaceBlitRaw(int d,int s,int x,int y,const SIZE* z,const RECT* r,const RECT* c,int a) override{return api.SurfaceBlitRaw(d,s,x-origin.x,y-origin.y,z,r,c,a);}
    } drawing{d_.graphics,{out->originXAt10,out->originYAt14}};
    const auto& old=*gauge.services;
    LoveHolicGaugesNativeServices borrowed{old.resources,old.state,old.surfaces,drawing,old.strings,old.profile};
    auto target=gauge;target.services=&borrowed;CompositeLoveHolicGaugeNative(target,out.Get(),nullptr);
}
void LegacyCharacterSelectionRootSource::DrawBackgroundAndDialog(GuiDialogNativeBody5Ch& gui)
{
    auto& root=Root();auto* draw=ResolveDrawManagerReferenceAt4175A0(root.DrawManagerSlotAt18());
    const auto layers=DrawLayersView(draw,d_.memory).WithNativeCallServices(root.PostWindow().BorrowNativeDrawCallServices());
    if(!SuppressReadScriptDrawAt4B5A3A(d_.environment,d_.input))layers.AdvanceFramesAt46B240();
    layers.WindowZero().PropagateDirtyRectsAt4C4250();auto* list=ResolveDrawLayerListAt47AE80(*draw);
    auto* bg=list->vptrAt00->dirtyAt14(list);auto* fg=gui.vptrAt00->dirtyAt00(&gui);
    const auto snapshot=[](GuiDirtyRectNativeBody80h* d){const auto n=d->vptrAt00->countAt18(d);const auto* p=d->vptrAt00->rectsAt1C(d);return std::vector<RECT>(p,p+n);};
    const auto background=snapshot(bg),dialog=snapshot(fg);
    for(const auto& r:dialog)list->vptrAt00->addDirtyAt20(list,&r);
    for(const auto& r:background)fg->vptrAt00->addRectAt08(fg,&r);
    layers.RegisteredLayers().RenderDirtyRects(d_.graphics);gui.vptrAt00->drawAt60(&gui,nullptr);gui.vptrAt00->flushDirtyAt58(&gui);
}
}
