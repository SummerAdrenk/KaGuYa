#include "NativeSerialize18.Internal.h"
#include <Runtime/SoundParamsEnvironmentArchive.h>
#include <Runtime/EnvironmentArchiveVectorRuntime.h>
#include <Runtime/SettingDialogNativeConfirm.h>
#include <mbstring.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace NativeSerialize18Detail;using Ref=YtlReferenceSlotRuntime;
template<class T>T* Resolve(const Ref& r) noexcept
{
    const U base=ReadAt<U>(&r);if(!base)return nullptr;auto* control=ReadAt<const void*>(&r,8);
    const U stride=control?ReadAt<U>(control,0xC):0;return reinterpret_cast<T*>(base+ReadAt<U>(&r,4)*stride);
}
U RefCount(const EnvironmentVectorStorage<Ref>& v) noexcept
{return static_cast<U>(std::bit_cast<int>(Address(v.end)-Address(v.begin))/12);}
bool Same(const char* left,const char* right)
{if(!right)throw ATL::CAtlException(E_FAIL);return _mbscmp(reinterpret_cast<const unsigned char*>(left),reinterpret_cast<const unsigned char*>(right))==0;}
void InvokeArchive(void* body,void* serializer)
{
    using Serialize=void(__thiscall*)(void*,void*);
    Slot<Serialize>(body,0)(body,serializer);
}
void WriteList(EnvironmentVectorStorage<Ref>& v,void* p)
{
    U count=RefCount(v);Argument(p,0x58,&count);
    for(auto* it=v.begin;it!=v.end;++it)InvokeArchive(Resolve<void>(*it),p); // 原end与正文slot00逐项重读。
}
void ReadTypeList(EnvironmentVectorStorage<Ref>& target,void* p,bool master)
{
    U count;Argument(p,0x58,&count);auto& s=RequireNativeSerialize18Services();
    for(U i=0;std::bit_cast<int>(i)<std::bit_cast<int>(count);++i)
    {
        Ref local;SoundTypeNative* body;
        if(master){body=CreateSoundTypeAt46FEBD(s.strings,s.memory);AdoptSoundNativeReference(local,body,s.memory);}
        else{auto* se=CreateSoundSeForFileAt48C7C0(s.strings,s.memory);body=se;AdoptSoundNativeReference(local,se,s.memory);}
        auto* cached=local.controlAt08;SoundNativeTemporary life(local);InvokeArchive(body,p);
        for(auto* it=target.begin;it!=target.end;++it)
        {
            if(!Same(body->keyAt0C.GetString(),Resolve<SoundTypeNative>(*it)->keyAt0C.GetString()))continue;
            Resolve<SoundTypeNative>(*it)->valueAt04=body->valueAt04;
            Resolve<SoundTypeNative>(*it)->valueAt08=body->valueAt08;break; // 两个标量分别重解原目标Ref，不替换名称/配置对象。
        }
        life.Finish(cached); // normal缓存control/terminate，EH从局部Ref当前control展开。
    }
}
}
void SerializeSoundTypeNative18(SoundTypeNative& body,void* p)
{
    using namespace NativeSerialize18Detail;
    Argument(p,0x4C,&body.keyAt0C);Argument(p,0x54,&body.valueAt04);Argument(p,0x44,&body.valueAt08);
    // SoundSe/SeEx继承同一type_params序列化；text/file_keys/state/strings不是这里的落盘字段。
}
void SerializeSoundCharacterNative18(SoundCharacterNative& body,void* p)
{
    using namespace NativeSerialize18Detail;
    Argument(p,0x54,&body.valueAt04);Argument(p,0x4C,&body.keyAt08);Argument(p,0x54,&body.valueAt05);
    if(Mode(p)){WriteList(body.parametersAt0C,p);return;}
    // 48C2D1先Clear旧元素(保留capacity)，再读count；失败保留清空/新提交前缀。
    ClearEnvironmentArchiveReferencesAt498312(body.parametersAt0C);U count;Argument(p,0x58,&count);auto& s=RequireNativeSerialize18Services();
    for(U i=0;std::bit_cast<int>(i)<std::bit_cast<int>(count);++i)
    {
        auto* incoming=CreateSoundSeExForFileAt48C310(s.strings,s.memory);Ref local;AdoptSoundNativeReference(local,incoming,s.memory);
        auto* cached=local.controlAt08;SoundNativeTemporary life(local);InvokeArchive(incoming,p);
        Ref copied{reinterpret_cast<std::uintptr_t>(incoming),0,cached};if(cached)++cached->referenceCountAt04;
        SoundNativeTemporary copiedLife(copied);AppendEnvironmentArchiveReferenceAt49852D(body.parametersAt0C,copied,s.memory);
        copiedLife.Finish(copied.controlAt08);life.Finish(cached);
    }
}
void SerializeSoundTypeGroupNative18(SoundTypeGroupNative& body,void* p)
{
    using namespace NativeSerialize18Detail;
    if(Mode(p)){WriteList(body.parametersAt08,p);return;}
    ReadTypeList(body.parametersAt08,p,false); // group masterAt04不在此文件中。
}
void SerializeSoundCharacterGroupNative18(SoundCharacterGroupNative& body,void* p)
{
    using namespace NativeSerialize18Detail;Argument(p,0x54,&body.valueAt05);
    if(Mode(p)){WriteList(body.charactersAt08,p);return;}
    U count;Argument(p,0x58,&count);auto& s=RequireNativeSerialize18Services();
    for(U i=0;std::bit_cast<int>(i)<std::bit_cast<int>(count);++i)
    {
        auto* incoming=CreateSoundCharacterForFileAt48C570(s.strings,s.memory);Ref local;AdoptSoundNativeReference(local,incoming,s.memory);
        auto* cached=local.controlAt08;SoundNativeTemporary life(local);InvokeArchive(incoming,p);
        for(auto* it=body.charactersAt08.begin;it!=body.charactersAt08.end;++it)
        {
            if(!Same(incoming->keyAt08.GetString(),Resolve<SoundCharacterNative>(*it)->keyAt08.GetString()))continue;
            incoming->MergeIntoAt48EE20(*it);break; // 正式native字段/Ref合并核，不含任何SerializeBuffer或codec。
        }
        life.Finish(cached);
    }
}
void SerializeSoundParamsNative18At48B750(EnvironmentSoundParamsArchive& body,void* p)
{
    using namespace NativeSerialize18Detail;auto& table=body.typeParametersAt20.Native();
    if(Mode(p))WriteList(table,p);else ReadTypeList(table,p,true);
    InvokeArchive(Resolve<void>(body.seByCharsAt38.Native()),p); // 48B9C9 raw slot00。
    InvokeArchive(Resolve<void>(body.seByTypeAt2C.Native()),p); // 48B9EF raw slot00。
}
void SerializeSoundParamsForSettingConfirmNative18(void* nativeSound,SettingDialogNativeConfirmSerialize18h& serializer,unsigned auditSite)
{
    static_assert(sizeof(SettingDialogNativeConfirmSerialize18h)==sizeof(NativeSerialize18Body));
    static_assert(offsetof(SettingDialogNativeConfirmSerialize18h,cursorAt14)==offsetof(NativeSerialize18Body,cursorAt14));
    static_cast<void>(auditSite); // 本具名叶的合同限定真44h；site只审计，不用于猜归档动态类型。
    SerializeSoundParamsNative18At48B750(*static_cast<EnvironmentSoundParamsArchive*>(nativeSound),&serializer);
}
void SerializeKnownArchiveNative18(NativeSerialize18ArchiveKind kind,void* archive,void* p)
{
    switch(kind)
    {
    case NativeSerialize18ArchiveKind::SoundParams44:SerializeSoundParamsNative18At48B750(*static_cast<EnvironmentSoundParamsArchive*>(archive),p);return;
    case NativeSerialize18ArchiveKind::SoundType10:SerializeSoundTypeNative18(*static_cast<SoundTypeNative*>(archive),p);return;
    case NativeSerialize18ArchiveKind::SoundCharacter18:SerializeSoundCharacterNative18(*static_cast<SoundCharacterNative*>(archive),p);return;
    case NativeSerialize18ArchiveKind::SoundTypeGroup14:SerializeSoundTypeGroupNative18(*static_cast<SoundTypeGroupNative*>(archive),p);return;
    case NativeSerialize18ArchiveKind::SoundCharacterGroup14:SerializeSoundCharacterGroupNative18(*static_cast<SoundCharacterGroupNative*>(archive),p);return;
    }
    throw std::invalid_argument("NativeSerialize18 archive kind is not registered");
}
}
