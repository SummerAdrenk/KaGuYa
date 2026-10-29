#include "SettingDialogNativeConfirm.Internal.h"
#include <cstdlib>

namespace KaGuYa::Start::Runtime::SettingDialogNativeConfirmDetail
{
void ApplyWrites(std::span<const SettingDialogNativeConfirmDefaultWrite> writes)
{
    void* archive=nullptr;unsigned site=0;
    for(const auto& write:writes)
    {
        if(site!=write.captureSite){site=write.captureSite;archive=Registry(write.archiveIndex,site);}
        using K=SettingDialogNativeConfirmWriteKind;
        switch(write.kind)
        {
        case K::Byte:*static_cast<volatile std::uint8_t*>(&Field<std::uint8_t>(archive,write.offset))=static_cast<std::uint8_t>(write.value);break;
        case K::Word:*static_cast<volatile std::uint16_t*>(&Field<std::uint16_t>(archive,write.offset))=static_cast<std::uint16_t>(write.value);break;
        case K::Dword:*static_cast<volatile unsigned*>(&Field<unsigned>(archive,write.offset))=write.value;break;
        case K::And:*static_cast<volatile unsigned*>(&Field<unsigned>(archive,write.offset))=Read(Field<unsigned>(archive,write.offset))&write.value;break;
        case K::Or:*static_cast<volatile unsigned*>(&Field<unsigned>(archive,write.offset))=Read(Field<unsigned>(archive,write.offset))|write.value;break;
        }
    }
}
void DestroySerializer(SettingDialogNativeConfirmSerialize18h& serializer)
{
    auto& s=RequireSettingDialogNativeConfirmServices();serializer.vptrAt00=s.source.SerializeMethodsAt5B22F4();
    auto* begin=Read(serializer.beginAt04);
    if(begin)
    {
        unsigned bytes=static_cast<unsigned>(reinterpret_cast<std::uintptr_t>(Read(serializer.capacityAt0C))-reinterpret_cast<std::uintptr_t>(begin));
        void* raw=begin;
        if(bytes>=0x1000)
        {
            raw=*reinterpret_cast<void**>(begin-4);bytes+=0x23;
            if(static_cast<unsigned>(reinterpret_cast<std::uintptr_t>(begin)-reinterpret_cast<std::uintptr_t>(raw)-4)>0x1F)
                _invalid_parameter_noinfo_noreturn();
        }
        s.memory.DeleteVector(raw,bytes);serializer.beginAt04=nullptr;serializer.endAt08=nullptr;serializer.capacityAt0C=nullptr;
    }
    serializer.vptrAt00=s.source.ISerializeMethods(); // 原normal/EH均保留mode/cursor，不清未写padding。
}
namespace
{
struct SerializeLife final
{
    SettingDialogNativeConfirmSerialize18h& value;bool armed=true;
    ~SerializeLife() noexcept{if(armed)DestroySerializer(value);}
    void Normal(){armed=false;DestroySerializer(value);} // 5392B8先撤state0，normal delete失败不重试。
};
ParamsGameSystem* GameSystem(unsigned site)
{
    auto* p=RequireSettingDialogNativeConfirmServices().source.CaptureParamsAt418480(site);
    return Resolve<ParamsGameSystem>(p->gameSystemAt04.Native());
}
}
}

namespace KaGuYa::Start::Runtime
{
void ResetSettingDialogDefaultsNativeAt538BB0(SettingDialogNativeBodyACh& b)
{
    using namespace SettingDialogNativeConfirmDetail;auto& s=RequireSettingDialogNativeConfirmServices();
    const auto prefix=SettingDialogNativeConfirmDefaultPrefixSample18();ApplyWrites(prefix.first(12));
    auto* original=Registry(0,0x538F94);auto* destination=Registry(1,0x538FDB);
    Field<unsigned>(destination,0x44)=Read(Field<unsigned>(original,0x44)); // 旧slot0 +44在第二次Env捕获之后才读。
    original=Registry(0,0x53902C);destination=Registry(1,0x539073);
    Field<unsigned>(destination,0x4C)=Read(Field<unsigned>(original,0x44));
    ApplyWrites(prefix.subspan(12));

    auto* fixedSave=Registry(1,0x5391D7);SettingDialogNativeConfirmSerialize18h serializer;
    serializer.vptrAt00=s.source.SerializeMethodsAt5B22F4();serializer.beginAt04=nullptr;serializer.capacityAt0C=nullptr;
    serializer.endAt08=nullptr;serializer.cursorAt14=0;serializer.writingAt10=1;SerializeLife serialLife{serializer};
    s.source.SerializeArchiveAtVslot00(Resolve<void>(Field<Slot>(fixedSave,0x28)),serializer,0x539275);
    auto& sound=Field<Slot>(fixedSave,0x1C);const auto soundBase=Read(sound.baseAt00);
    serializer.writingAt10=0;serializer.cursorAt14=0; // 53927A先固定sound base，再翻mode/cursor，stride/bias后读。
    s.source.SerializeArchiveAtVslot00(ResolveWithBase<void>(sound,soundBase),serializer,0x5392AC);
    Field<std::uint8_t>(fixedSave,0x81)=0;serialLife.Normal();

    const auto tail=SettingDialogNativeConfirmDefaultTailSample18();ApplyWrites(tail.first(2));
    auto* first=GameSystem(0x539362);destination=Registry(1,0x539388);
    AssignRuntimeCStringAt410220(Field<RuntimeCString>(destination,0x58),first->defaultGivenName.Native());
    auto* second=GameSystem(0x5393DF);destination=Registry(1,0x539405);
    AssignRuntimeCStringAt410220(Field<RuntimeCString>(destination,0x5C),second->defaultFamilyName.Native());
    ApplyWrites(tail.subspan(2));
    Fresh reused,finalPage;
    for(int id:{1,2,3,4})
    {
        // 5396E7..539827复用var_1C，第四页改用var_34；正常尾不清，EH才走4F65B0。
        Slot& page=id==4?finalPage.slot:reused.slot;
        FindDialog(&b.common,page,id);RefLife life{page};
        if(Read(page.baseAt00))ReloadPage(Resolve<Common>(page));
        life.NormalReloadRetain();
    }
}
}
