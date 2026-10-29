#pragma once
#include <Runtime/SettingDialogNativeApply.h>
#include <Runtime/FileManagerNative.h>
#include <Runtime/MessageOwnerNative.h>
#include <Runtime/RuntimeCString.h>
#include <bit>
#include <cstring>
#include <mbstring.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime::SettingDialogNativeApplyDetail
{
using Body=SettingDialogNativeBodyACh;using Slot=YtlReferenceSlotRuntime;
using Sound=SoundManagerNativeBody50h;using Params=EnvironmentSoundParamsArchive;
template<class T>T Read(const T& x) noexcept{return *static_cast<const volatile T*>(&x);}
template<class T>T& Field(void* p,unsigned offset) noexcept
{return *reinterpret_cast<T*>(reinterpret_cast<std::uintptr_t>(p)+offset);}
template<class T>T* Resolve(const Slot& r) noexcept
{
    const auto base=Read(r.baseAt00);if(!base)return nullptr;
    auto* c=static_cast<EnvironmentObjectReferenceControl*>(Read(r.controlAt08));
    const auto stride=c?Read(c->strideAt0C):0u;const auto bias=Read(r.biasAt04);
    return reinterpret_cast<T*>(base+bias*stride);
}
inline Slot& GameSystemSlot(ParamsRuntimeRoot* p) noexcept{return p->gameSystemAt04.Native();}
enum class ArchiveOrder{Ordinary,EndFirst,BeginFirst};
void* Registry(unsigned index,unsigned site,bool inlineCapture=false,ArchiveOrder order=ArchiveOrder::Ordinary);
Params* SavedParams(unsigned site,bool inlineCapture=false,ArchiveOrder order=ArchiveOrder::Ordinary);
Sound* RootSound(unsigned site);
DrawManagerNativeBody2Ch* RootDraw(unsigned site);
FileManagerNative28h* RootFiles(unsigned site);
ParamsGameSystem* GameSystem(unsigned site,bool inlined=true);
RuntimeCString* SnapshotAt(Body&,unsigned index) noexcept;
int SnapshotCount(Body&) noexcept;
inline int HeaderLength(const char* chars) noexcept{return *reinterpret_cast<const volatile int*>(chars-12);}
void ApplySoundPrefix(Body&);
void RestoreFirstMappings(Body&,unsigned& next);
void RestoreRemainingMappings(Body&,unsigned first);
void ApplyPolicyAndLayers(Body&);
void SetDefaultName(const char* heldName,bool first,unsigned site);
void AssignMapping(RuntimeCString& destination,const RuntimeCString& source);
bool NamesDifferInsensitive(const RuntimeCString&,const char* original);
// 无自动析构资格；用于433C50当前message隐藏返回串，成功返回之后才在该函数局部armed。
struct TextStorage final{union{RuntimeCString value;};TextStorage() noexcept{}~TextStorage() noexcept{}};
}
