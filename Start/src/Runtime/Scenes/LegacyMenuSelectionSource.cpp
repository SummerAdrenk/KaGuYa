#include <Runtime/LegacyMenuSelectionSource.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/ParamsRuntimeRoot.h>
#include <Runtime/ScriptArithmeticCommandNative.h>
#include <Runtime/ScriptNativeBody.h>
#include <Runtime/ReadScriptInputNative.h>
#include <Runtime/ReadScriptFrameAdvanceNative.h>
#include <Runtime/InputRootNative.h>
#include <Runtime/FileManagerNative.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/MessageWindowConfigRuntime.h>
#include <Runtime/GraphicsSurfaceCaptureRuntime.h>
#include <Runtime/GraphicsSurfaceReference.h>
#include <Runtime/NativeGraphicsRuntimeLibrary.h>
#include <Runtime/DrawManagerNative.h>
#include <Runtime/LayerListNative.h>
#include <vector>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
struct Image final
{
    YtlReferenceSlotRuntime slot{};
    ~Image() noexcept { ReleaseYtlReference(slot); }
};
}
const char* LegacyMenuSelectionSceneName() noexcept
{ return "\x83\x81\x83\x6A\x83\x85\x81\x5B\x91\x49\x91\xF0\x8F\x88\x97\x9D"; }
const char* LegacyMenuSelectionActionName(LegacyMenuSelectionAction action)
{
    switch(action)
    {
    case LegacyMenuSelectionAction::Save: return "\x83\x5A\x81\x5B\x83\x75\x89\xE6\x96\xCA";
    case LegacyMenuSelectionAction::Load: return "\x83\x8D\x81\x5B\x83\x68\x89\xE6\x96\xCA";
    case LegacyMenuSelectionAction::QuickSave: return "\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x5A\x81\x5B\x83\x75";
    case LegacyMenuSelectionAction::QuickLoad: return "\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x8D\x81\x5B\x83\x68";
    case LegacyMenuSelectionAction::Settings: return "\x90\xDD\x92\xE8\x89\xE6\x96\xCA";
    case LegacyMenuSelectionAction::Title: return "\x83\x5E\x83\x43\x83\x67\x83\x8B\x82\xD6";
    case LegacyMenuSelectionAction::Likability: return "\x8D\x44\x8A\xB4\x93\x78\x89\xE6\x96\xCA";
    }
    throw std::invalid_argument("Unknown legacy menu action");
}
RootRuntimePayload& LegacyMenuSelectionRootSource::Root()
{ d_.root.EnsureCreatedAt413BE0(); return *d_.root.ReadPublishedAt6198C4(); }
SceneManagerView LegacyMenuSelectionRootSource::Scenes()
{
    auto view=Root().PostWindow().BorrowSceneManagerAt30();
    if (!view.IsNative() || !view.NativeBody()) throw std::logic_error("Legacy menu requires native SceneManager");
    return {view.NativeBody(),view.NativeServices()};
}
MessageOwnerNative34h& LegacyMenuSelectionRootSource::Messages()
{
    auto* files=ResolveFileManagerAt479FF0(Root().FileManagerSlotAt00());
    auto* message=files ? ResolveMessageReference(files->messageAt10) : nullptr;
    if (!message) throw std::logic_error("Legacy menu has no current message owner");
    return *message;
}
int LegacyMenuSelectionRootSource::ReadValue(const char* name)
{
    auto& root=Root();
    const auto index=ResolveScriptNamedValueNativeAt498020(name,d_.names,d_.strings);
    return ReadScriptValueAt4C73A0(root.ParamsAt47BDB0(),index,d_.errors);
}
void LegacyMenuSelectionRootSource::WriteValue(const char* name,int value)
{
    auto& root=Root();
    const auto index=ResolveScriptNamedValueNativeAt498020(name,d_.names,d_.strings);
    WriteScriptValueAt4C8220(root.ParamsAt47BDB0(),index,value,d_.errors);
}
int LegacyMenuSelectionRootSource::ReadLabel(int index, MessageFormatSelectionTarget out)
{
    auto& message=Messages();
    auto reader=message.SourceWithContext({d_.messageFiles});
    if (reader.ReadVersionAt47DDE0()<4.0f) throw 0;
    MessageTextSelectionRuntime text(reader,message.flagsAt10[2]);
    return text.ReadTextAt4AD590(index,out);
}
void LegacyMenuSelectionRootSource::MarkLabelRead(int index)
{
    auto& message=Messages();
    auto& flags=message.flagsAt10[2];
    // 原0x4FE4C0：负值/超界不写；非0原值仍改1，但仅0->1持久化。
    if (index<0 || static_cast<unsigned>(index)>=flags.Size()) return;
    const auto previous=*flags.DataAt(static_cast<unsigned>(index));
    *flags.DataAt(static_cast<unsigned>(index))=1;
    if (!previous) message.ReadFlagsWithSources(d_.messageFiles,d_.messagePaths).PersistUnreadFlags();
}
COLORREF LegacyMenuSelectionRootSource::ReadColor(const char* name)
{
    const auto& params=d_.params.GetOrCreateAt418480().GameSystem();
    if (!params) throw std::logic_error("Menu colors require current GameSystem");
    auto* error=d_.tagErrors.CaptureAt47BCF0();
    const auto child=CaptureSettingChildAt477E90(error,params->settingTags[1].Native(),name);
    return ReadSettingNodeRgbColorAt4797C0(error,child.Native(),"Value",parser_);
}
UnSaveDataRuntime& LegacyMenuSelectionRootSource::UnSave()
{ return *static_cast<UnSaveDataRuntime*>(ResolveEnvironmentArchiveAt417990(*d_.environment.CaptureAt4180C0(),2)); }
SaveDataEnvironmentArchive& LegacyMenuSelectionRootSource::SaveData()
{ return *static_cast<SaveDataEnvironmentArchive*>(ResolveEnvironmentArchiveAt417990(*d_.environment.CaptureAt4180C0(),1)); }
void LegacyMenuSelectionRootSource::SetCommand(const char* name) { Scenes().WritePendingCommandAt1C(name); }
void LegacyMenuSelectionRootSource::ClearCommand() { Scenes().WritePendingCommandAt1C(""); }
bool LegacyMenuSelectionRootSource::Requested(LegacyMenuSelectionAction action)
{
    if (Scenes().ReadPendingCommandAt1C()==LegacyMenuSelectionActionName(action)) return true;
    // 原0x461310..0x4616F0的旧vector为16字节；语义action18..22复用统一12字节表。
    switch(action)
    {
    case LegacyMenuSelectionAction::Save: return ReadScriptSaveKeyAt47F830(d_.environment,d_.input);
    case LegacyMenuSelectionAction::Load: return ReadScriptLoadKeyAt47F650(d_.environment,d_.input);
    case LegacyMenuSelectionAction::QuickSave: return ReadScriptQuickSaveKeyAt47F780(d_.environment,d_.input,d_.root,d_.errors,d_.names,d_.strings);
    case LegacyMenuSelectionAction::QuickLoad: return ReadScriptQuickLoadKeyAt47F6D0(d_.environment,d_.input,d_.root,d_.errors,d_.names,d_.strings);
    case LegacyMenuSelectionAction::Likability: return ReadScriptLikabilityKeyAt47F530(d_.environment,d_.input,d_.root,0,d_.errors,d_.names,d_.strings);
    case LegacyMenuSelectionAction::Title:
    {
        const auto* key=ResolveEnvironmentObjectReference<KeyboardManagerNativeBody438h>(d_.input.CaptureAt418210()->keyboardAt18);
        return ReadKeyboardPressedAt47F4E0(*key,17); // 原0x4450F0的descriptor(73-5)/4。
    }
    case LegacyMenuSelectionAction::Settings: return false;
    }
    throw std::invalid_argument("Unknown legacy menu input");
}
void LegacyMenuSelectionRootSource::ResetScriptMenuState()
{
    Root().ParamsAt47BDB0().messageFormatIndexAt60=-1;
    auto* script=Root().ScriptAt47CD00();
    script->commandAt04.flags=0; script->commandAt04.reload[0]=0;
}
void LegacyMenuSelectionRootSource::ClearModalResult() { d_.interaction.ClearModalResultAt10(); }
bool LegacyMenuSelectionRootSource::ShouldAutoSave()
{ return Root().ScriptAt47CD00()->commandAt24.bytes[1]!=0 && SaveData().bytesAt74[2]!=0; }
void LegacyMenuSelectionRootSource::CapturePreview()
{
    GraphicsSurfaceNativeScope scope(d_.graphics);
    Image image;
    CaptureGraphicsScreenAt4ACDD0(image.slot,d_.graphics,d_.graphics,d_.graphics);
    auto preview=Root().CreateSavePreviewService(d_.graphics,d_.graphics,d_.graphics,d_.root.Files(),d_.preview);
    preview.SetCaptureAt48CFF0(image.slot);
}
void LegacyMenuSelectionRootSource::CaptureTransition()
{
    GraphicsSurfaceNativeScope scope(d_.graphics);
    Image image;
    // 原0x47D5D0等捕获已绘制画面，必须包含菜单；不能用仅draw图层合成代替。
    CaptureGraphicsScreenAt4ACDD0(image.slot,d_.graphics,d_.graphics,d_.graphics);
    auto& root=Root(); root.BindGraphicsSurfaceApi(d_.graphics);
    auto owned=image.slot; image.slot={};
    AssignGraphicsSurfaceReferenceAt4C1C60(root.TransitionFrameSlotAt1F0(),owned);
}
void LegacyMenuSelectionRootSource::AutoSave() { d_.interaction.AutoSaveAt46B280(0); }
void LegacyMenuSelectionRootSource::PushSystemScene(LegacyMenuSelectionAction action)
{
    if (action==LegacyMenuSelectionAction::Save || action==LegacyMenuSelectionAction::Load) d_.interaction.MarkRootSaveLoadAt1AF();
    Scenes().PushActiveScene(LegacyMenuSelectionActionName(action));
}
void LegacyMenuSelectionRootSource::QuickSave()
{ CaptureTransition(); d_.interaction.QuickSaveAt485FC0(0); }
bool LegacyMenuSelectionRootSource::QuickLoad()
{ CaptureTransition(); return d_.interaction.QuickLoadAt485CE0(); }
bool LegacyMenuSelectionRootSource::ConfirmTitle()
{
    if (!SaveData().bytesAt77[6]) return true;
    if (!ResolveEnvironmentObjectReference<void>(Root().InformationDisplaySlotAt3C()))
    {
        // 原0x463F00无Information时用0x52A2FC/MB_OKCANCEL，不能伪接受或吞失败。
        auto* env=d_.environment.CaptureAt4180C0();
        constexpr char text[]="\x83\x5E\x83\x43\x83\x67\x83\x8B\x82\xC9\x96\xDF\x82\xE8\x82\xDC\x82\xB7\x81\x42\x82\xE6\x82\xEB\x82\xB5\x82\xA2\x82\xC5\x82\xB7\x82\xA9\x81\x48";
        return d_.messageBox(reinterpret_cast<HWND>(env->windowAt2C),text,env->titleAt10.GetString(),0x21)==IDOK;
    }
    CaptureTransition();
    GeneralConfirmDialogNativeSession confirm(d_.confirmation,1);
    confirm.ConsumeRootCaptureAt477C60();
    return confirm.RunAt5071D0()==6;
}
void LegacyMenuSelectionRootSource::QueueLoadAfter()
{
    auto scenes=Scenes(); scenes.ClearScenes();
    scenes.QueueScene("\x83\x8D\x81\x5B\x83\x68\x91\x49\x91\xF0\x8C\xE3\x8F\x88\x97\x9D");
}
void LegacyMenuSelectionRootSource::PostTitle()
{
    auto* env=d_.environment.CaptureAt4180C0();
    static_cast<void>(d_.postMessage(reinterpret_cast<HWND>(env->windowAt2C),WM_COMMAND,0x8015,0));
}
void LegacyMenuSelectionRootSource::FinishScene()
{
    auto& root=Root();
    auto* draw=ResolveDrawManagerReferenceAt4175A0(root.DrawManagerSlotAt18());
    DrawLayersView(draw,d_.memory).WithNativeCallServices(root.PostWindow().BorrowNativeDrawCallServices()).ResetOwnedLayersAt47E940(d_.graphics);
    Scenes().PopThenActivatePendingAt4C1760();
}
bool LegacyMenuSelectionRootSource::SuppressBackgroundUpdate()
{ return SuppressReadScriptDrawAt4B5A3A(d_.environment,d_.input); }
void LegacyMenuSelectionRootSource::DrawBackgroundAndMenu(GuiDialogNativeBody5Ch& gui)
{
    GeneralConfirmDialogNativeScope scope(d_.confirmation);
    auto& root=Root();
    auto* draw=ResolveDrawManagerReferenceAt4175A0(root.DrawManagerSlotAt18());
    const auto layers=DrawLayersView(draw,d_.memory).WithNativeCallServices(root.PostWindow().BorrowNativeDrawCallServices());
    if (!SuppressBackgroundUpdate()) layers.AdvanceFramesAt46B240();
    layers.WindowZero().PropagateDirtyRectsAt4C4250();
    auto* list=ResolveDrawLayerListAt47AE80(*draw);
    auto* background=list->vptrAt00->dirtyAt14(list);
    auto* menu=gui.vptrAt00->dirtyAt00(&gui);
    // 原0x47C150先各自快照，再双向补dirty；避免新追加矩形回灌另一边。
    const auto snapshot=[](GuiDirtyRectNativeBody80h* dirty) {
        const auto count=dirty->vptrAt00->countAt18(dirty);
        const auto* rows=dirty->vptrAt00->rectsAt1C(dirty);
        return std::vector<RECT>(rows,rows+count);
    };
    const auto bg=snapshot(background), fg=snapshot(menu);
    for (const auto& rect:fg) list->vptrAt00->addDirtyAt20(list,&rect);
    for (const auto& rect:bg) menu->vptrAt00->addRectAt08(menu,&rect);
    layers.RegisteredLayers().RenderDirtyRects(d_.graphics);
    gui.vptrAt00->flushDirtyAt58(&gui);
}
}
