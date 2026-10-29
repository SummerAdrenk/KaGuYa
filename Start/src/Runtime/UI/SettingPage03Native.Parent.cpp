#include "SettingPage03Native.Internal.h"

namespace KaGuYa::Start::Runtime::SettingPage03NativeDetail
{
std::int32_t ParentChecked(Common* p, std::int32_t id)
{
    Fresh f; Into(p).findDialogAt90(p, &f.slot, 15); RefLife life{f.slot};
    int result = 0;
    if (Read(f.slot.baseAt00)) { auto* c = Resolve<Common>(f.slot); result = Table(c).checkedAt98(c, id); }
    life.Normal(); return result;
}
std::int32_t ParentValue(Common* p, std::int32_t id)
{
    Fresh f; Into(p).findDialogAt90(p, &f.slot, 15); RefLife life{f.slot};
    int result = 0;
    if (Read(f.slot.baseAt00)) { auto* c = Resolve<Common>(f.slot); result = Table(c).valueAt9C(c, id, 0); }
    life.Normal(); return result;
}
std::int32_t ParentPage(Common* p)
{
    Fresh f; Into(p).findDialogAt90(p, &f.slot, 19); RefLife life{f.slot};
    int result = Read(f.slot.baseAt00) ? Read(Field<int>(Resolve<Common>(f.slot), 0x80)) : -1;
    life.Normal(); return result;
}
void __fastcall ParentBegin(Common* p, void*)
{
    GuiDialogIntoBaseVtable().common.beginAt44(p);
    Fresh type; Into(p).findDialogAt90(p, &type.slot, 16);
    {
        RefLife life{type.slot};
        if (Read(type.slot.baseAt00))
        {
            auto* fixed = Resolve<Common>(type.slot); auto* v = Read(fixed->vptrAt00);
            const auto enabled = ParentChecked(p, SettingSoundMasterRowFor(SettingSoundMaster::Effects).checkbox); v->writeStateAt1C(fixed, enabled);
        }
        life.Normal();
    }
    Fresh chars; Into(p).findDialogAt90(p, &chars.slot, 17);
    {
        RefLife life{chars.slot};
        if (Read(chars.slot.baseAt00))
        {
            auto* fixed = Resolve<Common>(chars.slot); auto* v = Read(fixed->vptrAt00);
            const auto enabled = ParentChecked(p, SettingSoundMasterRowFor(SettingSoundMaster::Voice).checkbox); v->writeStateAt1C(fixed, enabled);
            Fresh indicator; Into(p).findDialogAt90(p, &indicator.slot, 19); RefLife indicatorLife{indicator.slot};
            if (Read(indicator.slot.baseAt00))
            {
                auto* c = Resolve<Common>(indicator.slot);
                if (Read(c->vptrAt00)->readEnabledAt28(c)) SelectCharacterPageAt503090(Resolve<Common>(chars.slot), 0, 0);
            }
            indicatorLife.Normal();
        }
        life.Normal();
    }
    Fresh loop; Into(p).findDialogAt90(p, &loop.slot, 18);
    {
        RefLife life{loop.slot};
        if (Read(loop.slot.baseAt00))
        {
            auto* fixed = Resolve<Common>(loop.slot); const auto* v = &ContainerTable(fixed);
            const auto enabled = ParentChecked(p, SettingSoundMasterRowFor(SettingSoundMaster::Voice).checkbox); v->setGateAtA0(fixed, enabled, 0);
        }
        life.Normal();
    }
    Field<int>(p, 0x84) = -1; Field<int>(p, 0x88) = -1;
}
void __fastcall ParentReload(Common* p, void*)
{
    Fresh master; Into(p).findDialogAt90(p, &master.slot, 15);
    {
        RefLife life{master.slot};
        if (Read(master.slot.baseAt00))
        {
            auto* c = Resolve<Common>(master.slot); Table(c).loadAt90(c);
            SyncMusicAt532740(Resolve<Common>(master.slot));
        }
        life.Normal();
    }
    for (int id : {16,17})
    {
        Fresh f; Into(p).findDialogAt90(p, &f.slot, id); RefLife life{f.slot};
        if (Read(f.slot.baseAt00))
        {
            auto* c = Resolve<Common>(f.slot); Table(c).loadAt90(c);
            c = Resolve<Common>(f.slot); const auto* v = Read(c->vptrAt00);
            const auto enabled = ParentChecked(p, SettingSoundMasterRowFor(id == 16 ? SettingSoundMaster::Effects : SettingSoundMaster::Voice).checkbox); v->writeStateAt1C(c, enabled);
        }
        life.Normal();
    }
    Fresh loop; Into(p).findDialogAt90(p, &loop.slot, 18); RefLife life{loop.slot};
    if (Read(loop.slot.baseAt00))
    {
        auto* c = Resolve<Common>(loop.slot); ContainerTable(c).loadAt98(c);
        c = Resolve<Common>(loop.slot); const auto* v = &ContainerTable(c);
        const auto enabled = ParentChecked(p, SettingSoundMasterRowFor(SettingSoundMaster::Voice).checkbox); v->setGateAtA0(c, enabled, 0);
    }
    life.Normal();
}
std::int32_t __fastcall ParentPoll(Common* p, void*)
{
    const auto& effects = SettingSoundMasterRowFor(SettingSoundMaster::Effects);
    const auto& voice = SettingSoundMasterRowFor(SettingSoundMaster::Voice);
    const auto oldType = ParentChecked(p, effects.checkbox), oldChars = ParentChecked(p, voice.checkbox);
    (void)ParentValue(p, effects.slider); (void)ParentValue(p, voice.slider);
    const auto oldPage = ParentPage(p);
    GuiDialogIntoBaseVtable().common.pollAt48(p);
    if (oldType != ParentChecked(p, effects.checkbox))
    {
        Fresh f; Into(p).findDialogAt90(p, &f.slot, 16); RefLife life{f.slot};
        if (Read(f.slot.baseAt00))
        {
            auto* c = Resolve<Common>(f.slot); Read(c->vptrAt00)->writeStateAt1C(c, oldType == 0);
            c = Resolve<Common>(f.slot); auto* fixed = Read(p->vptrAt00);
            RECT out; auto* returned = Table(c).boundsAt8C(c, &out); fixed->invalidateAt64(p, *returned);
        }
        life.Normal();
    }
    if (oldChars != ParentChecked(p, voice.checkbox))
    {
        Fresh f; Into(p).findDialogAt90(p, &f.slot, 17);
        {
            RefLife life{f.slot};
            if (Read(f.slot.baseAt00))
            {
                auto* c = Resolve<Common>(f.slot); Read(c->vptrAt00)->writeStateAt1C(c, oldChars == 0);
                c = Resolve<Common>(f.slot); auto* fixed = Read(p->vptrAt00);
                RECT out; auto* returned = Table(c).boundsAt8C(c, &out); fixed->invalidateAt64(p, *returned);
            }
            life.Normal();
        }
        Fresh loop; Into(p).findDialogAt90(p, &loop.slot, 18); RefLife life{loop.slot};
        if (Read(loop.slot.baseAt00)) { auto* c = Resolve<Common>(loop.slot); ContainerTable(c).setGateAtA0(c, oldChars == 0, 1); }
        life.Normal();
    }
    if (oldPage != ParentPage(p))
    {
        Fresh f; Into(p).findDialogAt90(p, &f.slot, 17); RefLife life{f.slot};
        if (Read(f.slot.baseAt00))
        { auto* fixed = Resolve<Common>(f.slot); const auto page = ParentPage(p); SelectCharacterPageAt503090(fixed, page, 1); }
        life.Normal();
    }
    return 1;
}
void __fastcall ParentDraw(Common* p, void*, const RECT* clip)
{
    if (Read(p->enabledAt38))
    { auto* footer = Resolve<Control>(reinterpret_cast<Parent*>(p)->footerAt78); CTable(footer).drawAt0C(footer, clip); }
    GuiDialogIntoBaseVtable().common.drawAt60(p, clip);
}
void __fastcall ParentComposite(Common* p, void*, Slot owned, const RECT* clip)
{
    RefLife life{owned};
    if (Read(p->enabledAt38))
    { auto* footer = Resolve<Control>(reinterpret_cast<Parent*>(p)->footerAt78); CTable(footer).compositeAt10(footer, Copy(owned), clip); }
    GuiDialogIntoBaseVtable().common.compositeAt5C(p, Copy(owned), clip); life.Normal();
}
RECT* __fastcall ParentBounds(Common* p, void*, RECT* out) { *out = Field<RECT>(p, 0x68); return out; }
}
