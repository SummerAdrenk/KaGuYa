#include "SettingDialogNativeApply.Internal.h"

namespace KaGuYa::Start::Runtime::SettingDialogNativeApplyDetail
{
void ApplyPolicyAndLayers(Body& b)
{
    const unsigned choice=Read(Field<unsigned>(Registry(1,0x51DF71),0x8C));
    auto* policy=Registry(5,0x51DF85);
    for(const auto& write:SettingDialogNativeApplyPolicyWritesSample18(choice))
        *static_cast<volatile unsigned*>(&Field<unsigned>(policy,write.offset))=write.value;

    auto* save=Registry(1,0x51E069);
    bool colors=Read(b.savedAt90)!=Read(Field<unsigned>(save,0x44));
    if(!colors){save=Registry(1,0x51E087);colors=Read(b.savedAt94)!=Read(Field<unsigned>(save,0x4C));}
    if(colors)
    {
        auto* manager=RootDraw(0x51E0A5);
        auto* layer=reinterpret_cast<MessageWindowLayerNativeBody50h*>(DrawLayerAt418C10(*manager,1));
        RefreshMessageWindowLayerNativeAt5325F0(*layer,-1);
    }

    save=Registry(1,0x51E0E5);
    bool text=Read(b.savedBytesAtA0[0])!=Read(Field<std::uint8_t>(save,0x60));
    if(!text){save=Registry(1,0x51E100);text=Read(b.savedBytesAtA0[1])!=Read(Field<std::uint8_t>(save,0x61));}
    if(!text)
    {
        auto* manager=RootDraw(0x51E11E);
        auto* window=reinterpret_cast<TextWindowLayerNativeBodyE8h*>(DrawLayerAt418C10(*manager,3));
        text=Read(window->glyphCursorAt50)>=0;
    }
    save=Registry(1,0x51E15D);
    if(Read(b.savedBytesAtA0[3])!=Read(Field<std::uint8_t>(save,0x6C)))text=true;

    const char* first=Field<RuntimeCString>(Registry(1,0x51E17D),0x58).GetString();
    bool names=NamesDifferInsensitive(b.firstNameAt7C,first);
    if(!names)
    {
        const char* second=Field<RuntimeCString>(Registry(1,0x51E1A6),0x5C).GetString();
        names=NamesDifferInsensitive(b.secondNameAt80,second);
    }
    if(names)
    {
        // 两个参数分别在各自Graphics获取之前借用，不能合并成一次名字/模块快照。
        const char* heldFirst=Field<RuntimeCString>(Registry(1,0x51E1D6),0x58).GetString();
        SetDefaultName(heldFirst,true,0x51E1E5);
        const char* heldSecond=Field<RuntimeCString>(Registry(1,0x51E218),0x5C).GetString();
        SetDefaultName(heldSecond,false,0x51E227);text=true;
    }
    const auto flags=Read(Field<unsigned>(Registry(1,0x51E25F),0x38));
    if(static_cast<unsigned>(Read(b.savedBytesAtA0[2]))!=((flags>>4)&1u)||text)
    {
        auto* manager=RootDraw(0x51E288);
        auto* window=reinterpret_cast<TextWindowLayerNativeBodyE8h*>(DrawLayerAt418C10(*manager,3));
        RestoreSettingDialogTextNativeAt433C50(*window);
    }
    // 原Apply不刷新first/second、84 names或90/A0 Snapshot，本函数不是新Snapshot事务。
}
}
