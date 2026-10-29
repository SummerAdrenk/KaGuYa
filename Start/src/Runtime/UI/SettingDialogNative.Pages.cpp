#include "SettingDialogNative.Internal.h"
#include <Runtime/SettingShellResourceProfile.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace SettingDialogNativeDetail;
Common* NewPage(Body& b, int id, SettingDialogNativeServices& s)
{
    const auto bytes = id == 3 ? 0x8Cu : id == 4 ? 0x88u : 0x84u;
    auto* raw = s.memory.AllocateBody(bytes);
    struct Raw final
    { void* p; std::size_t bytes; IEnvironmentRootMemory& memory; bool active = true;
      ~Raw() noexcept { if (active) memory.DeleteBody(p, bytes); } } cleanup{raw, bytes, s.memory};
    Common* result = nullptr;
    if (raw)
    {
        switch (id)
        {
        case 1: result = s.pages.ConstructPage01At4F1EE0(raw, &b.common, id); break;
        case 2: result = s.pages.ConstructPage02At4F23B0(raw, &b.common, id); break;
        case 3: result = s.pages.ConstructPage03At4F2910(raw, &b.common, id); break;
        case 4: result = s.pages.ConstructPage04At4F2FE0(raw, &b.common, id); break;
        }
    }
    cleanup.active = false;
    return result;
}
}
void SelectSettingDialogNativePageAt5352F0(SettingDialogNativeBodyACh& b, std::int32_t selection)
{
    using namespace SettingDialogNativeDetail;
    auto& s = RequireSettingDialogNativeServices();
    auto* p = &b.common;
    {
        FreshReference system;
        Table(p).findDialogAt90(p, &system.slot, 0);
        LocalReference systemLifetime{system.slot};
        if (Read(system.slot.baseAt00))
        {
            for (const auto& tab : CurrentSettingShellProfile().tabs)
            {
                auto* page = Resolve<Common>(system.slot);
                FreshReference child;
                Read(page->vptrAt00)->findChildAt14(page, &child.slot, tab.button);
                LocalReference childLifetime{child.slot};
                if (Read(child.slot.baseAt00))
                {
                    auto* selected = Resolve<GuiControlNativeBody50h>(child.slot);
                    if (static_cast<int>(tab.page) == selection)
                    {
                        Read(selected->vptrAt00)->resetAt08(selected);
                        selected = Resolve<GuiControlNativeBody50h>(child.slot);
                        Read(selected->vptrAt00)->writeStateAt44(selected, 0);
                    }
                    else if (!Read(selected->vptrAt00)->readStateAt48(selected))
                    {
                        selected = Resolve<GuiControlNativeBody50h>(child.slot);
                        Read(selected->vptrAt00)->writeStateAt44(selected, 1);
                    }
                }
                childLifetime.Reload();
            }
        }
        systemLifetime.Reload(false);
    }
    // Mamatomo的顺序为Text/Display/Other/Sound；每款明确提供自己的可达页。
    for (const auto kind : CurrentSettingShellProfile().pageVisitOrder)
    {
        const auto pageSelection = static_cast<int>(kind);
        const auto id = pageSelection + 1;
        FreshReference page;
        Table(p).findDialogAt90(p, &page.slot, id);
        LocalReference lifetime{page.slot};
        if (!Read(page.slot.baseAt00))
        {
            if (selection != pageSelection) { lifetime.Reload(false); continue; }
            auto* created = NewPage(b, id, s);
            ReleaseYtlReference(page.slot);
            // raw保护已经撤去；new18抛出时不补回收已完成动态页正文。
            ConstructOwnedDialogReference(page.slot, created, id == 3 ? 0x8C : id == 4 ? 0x88 : 0x84, s.memory);
            auto* selected = Resolve<Common>(page.slot);
            Read(selected->vptrAt00)->beginAt44(selected);
            Slot owned = Copy(page.slot);
            Table(p).appendDialogAt8C(p, owned); // callee拥有/消费按值Ref；不新添caller保护。
        }
        auto* selected = Resolve<Common>(page.slot);
        Read(selected->vptrAt00)->writeEnabledAt24(selected, selection == pageSelection);
        lifetime.Reload(false);
    }
    b.pageAt78 = selection; // 535AEE：所有查找/构造/启用和正常尾结束才发布。
}
}
