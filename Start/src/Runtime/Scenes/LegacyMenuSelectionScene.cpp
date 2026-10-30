#include <Runtime/LegacyMenuSelectionNative.h>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
std::uint8_t __fastcall Run(LegacyMenuSelectionScene* b, void*, int)
{ b->vptr->update(b); b->vptr->draw(b); return 1; }
LegacyMenuSelectionScene* __fastcall Delete(LegacyMenuSelectionScene* b, void*, unsigned flags) noexcept
{
    DestroyLegacyMenuSelectionScene(*b);
    if (flags&1u) ::operator delete(b, sizeof(*b));
    return b;
}
void __fastcall Update(LegacyMenuSelectionScene* b, void*) { UpdateLegacyMenuSelectionScene(*b); }
void __fastcall Draw(LegacyMenuSelectionScene* b, void*)
{ if (b->drawEnabled) b->services->source.DrawBackgroundAndMenu(*b->dialog); }
// 原生x86虚槽桥接：对象走ECX，额外void*只占EDX且不消费，其余参数/返回与原槽对应。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const LegacyMenuSelectionSceneVtable Methods{
    reinterpret_cast<decltype(Methods.run)>(&Run), reinterpret_cast<decltype(Methods.destroy)>(&Delete),
    reinterpret_cast<decltype(Methods.update)>(&Update), reinterpret_cast<decltype(Methods.draw)>(&Draw)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
bool PreserveSkip(SaveDataEnvironmentArchive& save) { return (save.bytesAt34[4]&8u)!=0; }
}
LegacyMenuSelectionScene* ConstructLegacyMenuSelectionScene(void* raw, LegacyMenuSelectionServices& s)
{
    RequireLegacyMenuSelectionProfile(s.profile);
    auto* b = ::new(raw) LegacyMenuSelectionScene{&Methods, &s, nullptr, 0, 0, true};
    void* gui = ::operator new(LegacyMenuSelectionDialogStorageSize());
    try { b->dialog=ConstructLegacyMenuSelectionDialog(gui,s); }
    catch (...) { ::operator delete(gui); throw; }
    try
    {
        // 原0x47BD20：GUI构造成功后才清auto/等待显示、保存并屏蔽skip、重置脚本状态。
        s.source.UnSave().FlagsAt18() &= 2u;
        s.source.UnSave().DwordAt0C() &= 0xFFFFFF00u;
        b->savedSkip=s.source.UnSave().FlagsAt14();
        s.source.UnSave().FlagsAt14() &= 2u;
        s.source.ResetScriptMenuState();
    }
    catch (...) { DestroyLegacyMenuSelectionScene(*b); throw; }
    return b;
}
void UpdateLegacyMenuSelectionScene(LegacyMenuSelectionScene& b)
{
    auto& source=b.services->source;
    auto& gui=*b.dialog;
    if (b.phase==0)
    {
        source.ClearModalResult();
        gui.vptrAt00->beginAt44(&gui);
        b.phase=13;
        return;
    }
    if (b.phase==13)
    {
        // 原0x47C5B0..：脚本+25与设置+7A的联合门；只有第一次Begin后的入口执行。
        if (source.ShouldAutoSave()) { source.CapturePreview(); source.AutoSave(); }
        b.phase=1;
    }
    if (b.phase==12)
    {
        // 原0x47C4F2：从设置返回只保留savedSkip的bit1，再刷新system。
        source.UnSave().FlagsAt14()=PreserveSkip(source.SaveData()) ? b.savedSkip&2u : 0;
        ReopenLegacyMenuSelectionSystem(gui); b.phase=1;
    }
    if (b.phase==2)
    {
        // 原0x47D0B7：结果及正文已读已由End提交；先重置draw/弹出场景，再恢复环境。
        source.FinishScene();
        source.UnSave().ModeAt04()=1;
        source.UnSave().FlagsAt18()|=1u;
        source.UnSave().DwordAt0C()=(source.UnSave().DwordAt0C()&0xFFFFFF00u)|1u;
        source.UnSave().FlagsAt14()=PreserveSkip(source.SaveData()) ? b.savedSkip : 1u;
        b.phase=3; return;
    }
    if (b.phase!=1) return;
    for (auto action : {LegacyMenuSelectionAction::Save, LegacyMenuSelectionAction::Load,
        LegacyMenuSelectionAction::QuickSave, LegacyMenuSelectionAction::QuickLoad,
        LegacyMenuSelectionAction::Settings, LegacyMenuSelectionAction::Title, LegacyMenuSelectionAction::Likability})
    {
        if (!source.Requested(action)) continue;
        if (action==LegacyMenuSelectionAction::Save || action==LegacyMenuSelectionAction::Load
            || action==LegacyMenuSelectionAction::QuickSave) source.CapturePreview();
        source.ClearCommand();
        switch (action)
        {
        case LegacyMenuSelectionAction::Save: case LegacyMenuSelectionAction::Load:
            source.PushSystemScene(action); return;
        case LegacyMenuSelectionAction::QuickSave: source.QuickSave(); return;
        case LegacyMenuSelectionAction::QuickLoad:
            if (source.QuickLoad()) { source.QueueLoadAfter(); source.UnSave().ModeAt04()=0; b.phase=3; }
            return;
        case LegacyMenuSelectionAction::Settings: source.PushSystemScene(action); b.phase=12; return;
        case LegacyMenuSelectionAction::Title:
            if (source.ConfirmTitle()) { source.PostTitle(); source.UnSave().ModeAt04()=0; b.phase=3; }
            return;
        case LegacyMenuSelectionAction::Likability: source.PushSystemScene(action); return;
        }
    }
    if (!gui.vptrAt00->pollAt48(&gui)) { gui.vptrAt00->endAt4C(&gui); b.phase=2; }
}
void DestroyLegacyMenuSelectionScene(LegacyMenuSelectionScene& b) noexcept
{
    if (b.dialog) { b.dialog->vptrAt00->deletingDestructorAt04(b.dialog,1); b.dialog=nullptr; }
}
}
