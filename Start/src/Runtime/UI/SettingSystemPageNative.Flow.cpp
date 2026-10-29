#include "SettingSystemPageNative.Internal.h"
#include <Runtime/SettingShellResourceProfile.h>

namespace KaGuYa::Start::Runtime::SettingSystemPageNativeDetail
{
void __fastcall Begin(Common* p, void*)
{
    ClearGuiClickedEventFlagsAt532200(*Manager(Page(p)));
    Read(p->vptrAt00)->resetChildrenAt68(p);
    const auto gatedButton = CurrentSettingShellProfile().modeGateButton;
    if (gatedButton < 0) return;
    FreshReference child;
    Read(p->vptrAt00)->findChildAt14(p, &child.slot, gatedButton);
    LocalReference lifetime{child.slot};
    if (Read(child.slot.baseAt00))
    {
        auto* selected = Resolve<GuiControlNativeBody50h>(child.slot);
        auto& source = RequireSettingSystemPageNativeServices().source;
        auto* root = Root(source);
        const auto* table = Read(selected->vptrAt00); // 50F936，读取218之前固定table。
        const auto value = source.RootByteAt218(root);
        table->writeStateAt44(selected, value != 0);
    }
    lifetime.Reload(false); // state1正常尾不清局部槽；异常展开是4F6460。
}
std::int32_t __fastcall Poll(Common* p, void*)
{
    Page(p).resultAt5C = -1;
    Read(p->vptrAt00)->processInputAt54(p);
    auto* manager = Manager(Page(p));
    // 原50FDB0未导出公共核；这里只恢复同一算法，不调用现代ClickedEventManager。
    auto* position = Read(manager->begin);
    const auto* end = Read(manager->end);
    for (; position != end; ++position)
    {
        const auto base = Read(position->baseAt00); // 固定ESI，后两次解析不重读base。
        if (!Read(ResolveWithBase<GuiClickedEventNativeBody>(*position, base)->clickedFlagAt04)) continue;
        const auto value = Read(ResolveWithBase<GuiClickedEventNativeBody>(*position, base)->eventValueAt08);
        auto* clicked = ResolveWithBase<GuiClickedEventNativeBody>(*position, base);
        Read(clicked->vptrAt00)->clearAt04(clicked);
        Page(p).resultAt5C = value;
        return 1;
    }
    Page(p).resultAt5C = -1;
    return 1;
}
std::int32_t __fastcall End(Common*, void*) { return 1; } // 509540原真mov eax,1;ret。
std::int32_t __fastcall Result(Common* p, void*) { return Read(Page(p).resultAt5C); }
}
