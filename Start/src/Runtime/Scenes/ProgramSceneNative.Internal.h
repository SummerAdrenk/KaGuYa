#pragma once
#include <Runtime/ProgramSceneNative.h>
#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <bit>
#include <cstring>

namespace KaGuYa::Start::Runtime::ProgramSceneNativeDetail
{
using Body=ProgramSceneNativeBody20h;
using Slot=YtlReferenceSlotRuntime;
template<class T>T Read(const T& value)noexcept{return *static_cast<const volatile T*>(&value);}
template<class T>T& Field(void* body,unsigned offset)noexcept{return *reinterpret_cast<T*>(reinterpret_cast<std::uintptr_t>(body)+offset);}
template<class T>T* Resolve(const Slot& slot)noexcept
{
    const auto base=Read(slot.baseAt00);if(!base)return nullptr;
    const auto* control=static_cast<const EnvironmentObjectReferenceControl*>(Read(slot.controlAt08));
    return reinterpret_cast<T*>(base+Read(slot.biasAt04)*(control?Read(control->strideAt0C):0u));
}
template<class Method>Method Virtual(void* receiver,unsigned offset)noexcept
{
    const auto table=Read(Field<const std::byte*>(receiver,0));Method result;
    std::memcpy(&result,table+offset,sizeof(result));return result;
}
inline void* Root(IProgramSceneNativeSource& source,unsigned ensure,unsigned read)
{source.EnsureRootAt413BE0(ensure);return source.RootAt6198C4(read);}
inline void* Scenes(IProgramSceneNativeSource& source,void* root,unsigned site)
{return Resolve<void>(source.RootScenesSlotAt30(root,site));}
inline void* Archive(IProgramSceneNativeSource& source,unsigned index,unsigned site)
{return ResolveEnvironmentArchiveAt417990(*source.CaptureEnvironmentAt4180C0(site),index);}
inline GuiDialogNativeBody5Ch* Dialog(Body& b)noexcept{return Resolve<GuiDialogNativeBody5Ch>(b.dialogAt08);}
void* InlineArchiveZero(EnvironmentRootStorage*)noexcept;
void Retire(Body&)noexcept;
void AdoptDialog(Slot&,GuiDialogNativeBody5Ch*,int mode,IEnvironmentRootMemory&);
void ReleaseRetainedNormal(Slot&)noexcept;
void ReleaseNormal(Slot&)noexcept;
inline void Copy(Slot& out,const Slot& in)noexcept
{
    out.baseAt00=Read(in.baseAt00);out.biasAt04=Read(in.biasAt04);out.controlAt08=Read(in.controlAt08);
    if(out.controlAt08)++out.controlAt08->referenceCountAt04;
}
struct UpdateFrame final
{
    IEnvironmentRootMemory& memory;
    int state=-1;
    Slot first{},argument{},confirmation{},second{};
    GuiDialogNativeBody5Ch* rawConfirmation=nullptr;
    ATL::CStringData* cachedText=nullptr;
    union Text{RuntimeCString value;Text()noexcept{}~Text()noexcept{}} text;
    ~UpdateFrame()noexcept;
};
bool Poll(Body&,ProgramSceneNativeServices&,UpdateFrame&);
void ApplySelection(Body&,ProgramSceneNativeServices&,UpdateFrame&,bool quickLoad);
void Modal(Body&,ProgramSceneNativeServices&,UpdateFrame&);
}
