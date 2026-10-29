#pragma once
#include <Runtime/MovieSceneNative.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/EnvironmentObjectReference.h>
#include <bit>
#include <cstring>

namespace KaGuYa::Start::Runtime::MovieSceneNativeDetail
{
using Body=MovieSceneNativeBody0Ch;
using Slot=YtlReferenceSlotRuntime;
template<class T>T Read(const T& value)noexcept{return *static_cast<const volatile T*>(&value);}
template<class T>T& Field(void* body,std::uint32_t offset)noexcept
{return *reinterpret_cast<T*>(reinterpret_cast<std::uintptr_t>(body)+offset);}
template<class T>T* Resolve(const Slot& slot)noexcept
{
    const auto base=Read(slot.baseAt00);if(!base)return nullptr;
    const auto* control=static_cast<const EnvironmentObjectReferenceControl*>(Read(slot.controlAt08));
    const auto stride=control?Read(control->strideAt0C):0u;
    return reinterpret_cast<T*>(base+Read(slot.biasAt04)*stride);
}
template<class Function>Function Virtual(void* receiver,std::uint32_t offset)noexcept
{
    const auto* table=Read(Field<const std::byte*>(receiver,0));
    Function result;std::memcpy(&result,table+offset,sizeof(result));return result;
}
inline void* Root(IMovieSceneNativeSource& source,unsigned ensureSite,unsigned readSite)
{source.EnsureRootAt413BE0(ensureSite);return source.RootAt6198C4(readSite);}
inline DirectMovieNativeBody38h* Movie(IMovieSceneNativeSource& source,unsigned ensureSite,unsigned readSite,unsigned slotSite)
{
    auto* root=Root(source,ensureSite,readSite);
    return Resolve<DirectMovieNativeBody38h>(source.RootMovieSlotAt48(root,slotSite));
}
inline void* InlineArchive(EnvironmentRootStorage* environment,unsigned index)noexcept
{
    const auto end=reinterpret_cast<std::uintptr_t>(Read(environment->archivesAt30.end));
    const auto begin=reinterpret_cast<std::uintptr_t>(Read(environment->archivesAt30.begin));
    const auto count=std::uint32_t(std::bit_cast<std::int32_t>(std::uint32_t(end-begin))/12);
    if(count<=index)return nullptr;
    const auto* slot=reinterpret_cast<const Slot*>(begin+index*12u);
    const auto base=Read(slot->baseAt00);if(!base)return nullptr;
    const auto* control=static_cast<const EnvironmentObjectReferenceControl*>(Read(slot->controlAt08));
    // 原slot0内联路径4A0F52先读bias，再读control stride。
    if(index==0)
    {
        const auto bias=Read(slot->biasAt04);
        return reinterpret_cast<void*>(base+bias*(control?Read(control->strideAt0C):0u));
    }
    const auto stride=control?Read(control->strideAt0C):0u;
    return reinterpret_cast<void*>(base+Read(slot->biasAt04)*stride);
}
inline void* Archive(IMovieSceneNativeSource& source,unsigned index,unsigned site)
{return InlineArchive(source.CaptureEnvironmentAt4180C0(site),index);}

struct CompletionSites final
{
    unsigned environment,ensureRoot,rootRead,scenesSlot,advance,graphics,invalidate;
};
inline constexpr CompletionSites constructorCompletion{0x4A1108,0x4A1124,0x4A1129,0x4A112F,0x4A1150,0x4A1157,0x4A115E};
inline constexpr CompletionSites updateCompletion{0x4B966F,0x4B9688,0x4B968D,0x4B9693,0x4B96B4,0x4B96BB,0x4B96C2};
void RestoreAdvanceInvalidate(Body&,IMovieSceneNativeSource&,const CompletionSites&);
void Retire(Body&)noexcept;

union TextStorage
{
    RuntimeCString value;
    TextStorage()noexcept{}
    ~TextStorage()noexcept{}
};
static_assert(sizeof(TextStorage)==4);
inline ATL::CStringData* Header(const char* chars)noexcept
{return reinterpret_cast<ATL::CStringData*>(reinterpret_cast<std::uintptr_t>(chars)-16u);}
inline const char* Chars(const RuntimeCString& value)noexcept
{const char* result;std::memcpy(&result,&value,4);return result;}

// 仅是Scene0Ch正文之外的栈展开资格，不是第二套业务状态机。
struct ConstructionFrame final
{
    Body& body;
    IEnvironmentRootMemory& memory;
    DirectMovieNativeBody38h* raw=nullptr;
    unsigned state=0;
    bool pathOwned=false,active=true;
    TextStorage procOrPrefix,path,alternatePrefix,alternatePath;
    ~ConstructionFrame()noexcept;
    void ReleaseNormal(RuntimeCString& text,unsigned nextState)
    {
        auto* header=Header(Chars(text));state=nextState;
        ReleaseRuntimeCStringHeader(header);
    }
};
}
