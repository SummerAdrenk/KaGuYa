#include "SystemDialogNative.Internal.h"
#include <Runtime/SceneManagerNative.h>

namespace KaGuYa::Start::Runtime::SystemDialogNativeDetail
{
namespace
{
void AddBackgroundBounds(Common* p)
{
    auto& r = Dialog(p).backgroundAt5C;
    const auto base = Read(r.baseAt00); // 52DBC0：两次解析共用原ECX base。
    auto* first = ResolveWithBase<GraphicsSurfaceNativeBody>(r, base);
    const auto width = Read(first->widthAt08), height = Read(first->heightAt0C);
    auto* second = ResolveWithBase<GraphicsSurfaceNativeBody>(r, base);
    const auto x = Read(second->originXAt10), y = Read(second->originYAt14);
    auto* dirty = p->vptrAt00->dirtyAt00(p);
    dirty->vptrAt00->addXYWHAt10(dirty, x, y, width, height);
}
}
void BeginModalAt50CF50(Common* p)
{
    auto& b = Dialog(p);
    auto& s = RequireSystemDialogNativeServices().source;
    if (!Read(b.backgroundAt5C.baseAt00))
    {
        auto* fixedRoot = Root(s);
        auto* draw = Resolve<void>(s.RootDrawSlotAt18(fixedRoot));
        FreshReference background;
        auto* returned = s.CaptureBackgroundAt4ACDD0(draw, &background.slot);
        LocalReference lifetime{background.slot};
        CopyAssignGraphicsSurfaceReferenceAt465870(b.backgroundAt5C, *returned);
        lifetime.Reload(false); // 原state1正常终止，不清局部位型。
    }
    p->vptrAt00->writeEnabledAt24(p, 1);
    ClearGuiClickedEventFlagsAt532200(b.eventsAt74);
    b.phaseAt6C = 0;
    p->vptrAt00->resetInputAt3C(p);
    p->vptrAt00->resetChildrenAt68(p);
    p->activeAt4C = nullptr;
    Table(p).beginExtraAt8C(p);
    auto* fixedRoot = Root(s);
    s.RootModalFlagAt1B8(fixedRoot) = 1;
}
void __fastcall Begin(Common* p, void*)
{
    BeginModalAt50CF50(p);
    auto& s = RequireSystemDialogNativeServices().source;
    auto* fixedRoot = Root(s);
    const auto mode = Read(Resolve<WindowModeNative2Ch>(s.RootWindowModeSlotAt0C(fixedRoot))->modeAt04);
    for (int id = 1; id != 4; ++id)
    {
        FreshReference r;
        p->vptrAt00->findChildAt14(p, &r.slot, id);
        LocalReference lifetime{r.slot}; // 50F990 state0/2/4：分别保护各次已返回Find。
        if (Read(r.slot.baseAt00))
        {
            auto* child = Resolve<GuiControlNativeBody50h>(r.slot);
            if (id == 3)
            {
                fixedRoot = Root(s);
                const auto* selectedTable = Read(child->vptrAt00); // 50FB10：Ensure后、219读取前缓存ESI。
                const auto state = s.RootMenuStateAt219(fixedRoot);
                selectedTable->writeStateAt44(child, state != 0);
            }
            else child->vptrAt00->writeStateAt44(child, id == 1 ? mode != 0 : mode == 0);
        }
        lifetime.Reload(false); // state1/3/5为正常尾terminate。
    }
    Dialog(p).resultAt68 = -1; // 三次Find全部完成之后才发布。
}
std::int32_t __fastcall Poll(Common* p, void*)
{
    auto& b = Dialog(p);
    switch (Read(b.phaseAt6C))
    {
    case 0:
        AddBackgroundBounds(p);
        b.phaseAt6C = 3;
        break;
    case 3:
        p->vptrAt00->processInputAt54(p);
        if (Table(p).pollExtraAt90(p)) b.phaseAt6C = 4;
        break;
    case 4:
        AddBackgroundBounds(p);
        p->vptrAt00->writeEnabledAt24(p, 0);
        Table(p).endExtraAt94(p);
        b.phaseAt6C = -1;
        p->vptrAt00->flushDirtyAt58(p);
        return 1;
    default: return 0;
    }
    p->vptrAt00->flushDirtyAt58(p);
    return 1;
}
std::int32_t EndModalAt508E80(Common* p)
{
    // 无局部EH状态，不捕获业务叶异常；先background再Ensure、最后写modal0。
    auto& background = Dialog(p).backgroundAt5C;
    if (background.controlAt08)
    {
        ReleaseWithoutClearing(background);
        background.controlAt08 = nullptr;
        background.baseAt00 = 0;
    }
    auto& s = RequireSystemDialogNativeServices().source;
    auto* fixedRoot = Root(s);
    s.RootModalFlagAt1B8(fixedRoot) = 0;
    return 1;
}
std::int32_t __fastcall End(Common* p, void*)
{
    (void)EndModalAt508E80(p);
    auto& s = RequireSystemDialogNativeServices().source;
    const auto result = Read(Dialog(p).resultAt68);
    switch (result)
    {
    case 1: case 2:
    {
        auto* system = s.SystemAt4180C0();
        const auto window = s.SystemWindowAt2C(system);
        (void)s.PostMessageA(window, 0x111, result == 1 ? 0x800C : 0x800B, 0);
        break;
    }
    case 3:
    {
        auto* fixedRoot = Root(s);
        // 4186D0只解析根30h，不Ensure或增持；目标是原48h SceneManager而非CCh Script。
        auto* scenes = Resolve<SceneManagerNativeBody48h>(s.RootSceneManagerSlotAt30(fixedRoot));
        const auto bytes = SystemDialogNativeSettingsCommandSample18();
        SetRuntimeCStringAt437680(scenes->commandAt1C, bytes.data(), 8);
        Dialog(p).suppressOnceAt84 = 1; // 字符串失败不得提前置1。
        break;
    }
    case 4:
    {
        auto* system = s.SystemAt4180C0();
        (void)s.PostMessageA(s.SystemWindowAt2C(system), 0x10, 0, 0);
        break;
    }
    default: break;
    }
    return 0; // 即使PostMessage失败也原样返回0，5071D0据此走取消结果2。
}
}
