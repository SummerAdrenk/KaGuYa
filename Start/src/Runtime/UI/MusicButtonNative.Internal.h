#pragma once
#include <Runtime/MusicButtonNative.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/GuiDirtyRectNative.h>
#include <bit>
#include <cstring>

namespace KaGuYa::Start::Runtime::MusicButtonNativeDetail
{
using Body=MusicButtonNativeBody70h;using Slot=YtlReferenceSlotRuntime;
template<class T>T Read(const T& x) noexcept{return *static_cast<const volatile T*>(&x);}
template<class T>T& Field(void* p,std::size_t offset) noexcept{return *reinterpret_cast<T*>(reinterpret_cast<std::uintptr_t>(p)+offset);}
template<class T>T* Resolve(const Slot& r) noexcept
{
    const auto base=Read(r.baseAt00);if(!base)return nullptr;auto* c=Read(r.controlAt08);
    const auto stride=c?Read(static_cast<EnvironmentObjectReferenceControl*>(c)->strideAt0C):0u;
    return reinterpret_cast<T*>(base+Read(r.biasAt04)*stride);
}
inline int Add(int a,int b) noexcept{return std::bit_cast<int>(std::uint32_t(a)+std::uint32_t(b));}
inline int Sub(int a,int b) noexcept{return std::bit_cast<int>(std::uint32_t(a)-std::uint32_t(b));}
inline GuiDialogNativeBody5Ch* Dialog(Body& p) noexcept{return Resolve<GuiDialogNativeBody5Ch>(p.dialogAt50);}
inline GuiDirtyRectNativeBody80h* Dirty(Body& p) noexcept{return Resolve<GuiDirtyRectNativeBody80h>(p.common.dirtyAt24);}
inline int PublicCount(const Body& p) noexcept
{auto* c=Read(p.common.publicGuiAt44.controlAt08);return c?std::bit_cast<int>(Read(static_cast<EnvironmentObjectReferenceControl*>(c)->countAt10)):0;}
inline CgUiButtonNativeBody6Ch* PublicAt(Body& p,unsigned index) noexcept
{
    auto& r=p.common.publicGuiAt44;const auto base=Read(r.baseAt00);if(!base)return nullptr;auto* c=Read(r.controlAt08);
    const auto stride=c?Read(static_cast<EnvironmentObjectReferenceControl*>(c)->strideAt0C):0u;
    return reinterpret_cast<CgUiButtonNativeBody6Ch*>(base+(Read(r.biasAt04)+index)*stride);
}
void* Root(IMusicButtonNativeSource&,unsigned site);
void* Scenes(IMusicButtonNativeSource&,unsigned site);
void* Sound(IMusicButtonNativeSource&,unsigned site);
void* Registry(IMusicButtonNativeSource&,unsigned index,unsigned site,bool inlineLookup=false);
void AdoptDialog(Slot&,GuiDialogNativeBody5Ch*);
}
