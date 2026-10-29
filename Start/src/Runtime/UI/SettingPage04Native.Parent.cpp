#include "SettingPage04Native.Internal.h"

namespace KaGuYa::Start::Runtime::SettingPage04NativeDetail
{
void __fastcall ParentBegin(Common* p,void*)
{GuiDialogIntoBaseVtable().common.beginAt44(p);Field<int>(p,0x84)=-1;}
int __fastcall ParentResult(Common* p,void*){return Read(Field<int>(p,0x84));}
RECT* __fastcall ParentBounds(Common* p,void*,RECT* out){*out=Field<RECT>(p,0x68);return out;}
void __fastcall ParentReload(Common* p,void*)
{
    for(const auto& block:CurrentSettingOtherProfile().blocks)
    {
        if(block.kind==SettingOtherBlock::VersionInfo)continue;
        Fresh f;Into(p).findDialogAt90(p,&f.slot,block.dialogId);RefLife life{f.slot};
        if(Read(f.slot.baseAt00))
        {auto* c=Resolve<Common>(f.slot);CTable(c).loadAt98(c);if(block.kind==SettingOtherBlock::Name)BuildNameTextAt512200(Resolve<Common>(f.slot),1,0);}
        life.Normal();
    }
}
}
