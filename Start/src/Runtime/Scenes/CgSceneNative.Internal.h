#pragma once
#include <Runtime/CgSceneNative.h>
#include <Runtime/DrawManagerNative.h>
#include <Runtime/LayerListNative.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/GraphicsSurfaceReference.h>
#include <Runtime/SceneGroupReferenceNative.h>
#include <bit>

namespace KaGuYa::Start::Runtime::CgSceneNativeDetail
{
using Body=CgSceneNativeBodyB4h;
using Slot=YtlReferenceSlotRuntime;
using Groups=EnvironmentVectorStorage<Slot>;
template<class T>T Read(const T& value) noexcept{return *static_cast<const volatile T*>(&value);}
template<class T>T& Field(void* p,std::uint32_t offset) noexcept
{return *reinterpret_cast<T*>(reinterpret_cast<std::uintptr_t>(p)+offset);}
template<class T>T* Resolve(const Slot& value) noexcept
{
    const auto base=Read(value.baseAt00);if(!base)return nullptr;
    const auto* control=static_cast<const EnvironmentObjectReferenceControl*>(Read(value.controlAt08));
    return reinterpret_cast<T*>(base+Read(value.biasAt04)*(control?Read(control->strideAt0C):0));
}
template<class T>T* ResolveWithBase(const Slot& value,std::uintptr_t base) noexcept
{
    if(!base)return nullptr;
    const auto* control=static_cast<const EnvironmentObjectReferenceControl*>(Read(value.controlAt08));
    return reinterpret_cast<T*>(base+Read(value.biasAt04)*(control?Read(control->strideAt0C):0));
}
template<class T>T* Advance(T* p,int index) noexcept
{return reinterpret_cast<T*>(reinterpret_cast<std::uintptr_t>(p)+std::uint32_t(index)*sizeof(T));}
inline std::int32_t Add(std::int32_t a,std::int32_t b) noexcept
{return std::bit_cast<std::int32_t>(std::uint32_t(a)+std::uint32_t(b));}
inline std::int32_t Sub(std::int32_t a,std::int32_t b) noexcept
{return std::bit_cast<std::int32_t>(std::uint32_t(a)-std::uint32_t(b));}
void* Root(ICgSceneNativeSource&,std::uint32_t callSite,std::uint32_t readSite=0);
void* Draw(ICgSceneNativeSource&,std::uint32_t callSite);
void* Scenes(ICgSceneNativeSource&,std::uint32_t callSite,std::uint32_t readSite=0);
void* Registry(ICgSceneNativeSource&,unsigned index,unsigned site,bool inlineLookup=false);
void ResetOperation(TransitOperation&) noexcept;
void Retire(Body&) noexcept;
inline void AppendGroup(Groups& groups,const Slot& slot){AppendSceneGroupReferenceNativeAt499030(groups,slot);}
inline void DestroyGroups(Groups& groups)noexcept{DestroySceneGroupReferencesNativeAt4A3950(groups);}
void AdoptDisplay(Slot&,CgDisplayWindowNativeBody60h*,IEnvironmentRootMemory&);
void AdoptOperation(Slot&,CgOperationWindowNativeBody30h*,IEnvironmentRootMemory&);
void CaptureScreen(Body&,ICgSceneNativeSource&,unsigned site,bool inlineNormal);
void ReleaseTemporaryRetained(Slot&) noexcept;
void ReleaseTemporaryCaptured(YtlReferenceControlRuntime*) noexcept;
const char* SettingName() noexcept;
const char* SceneName() noexcept;
const char* MusicName() noexcept;
const char* TitleName() noexcept;
CgButtonNativeBody84h* Group(Body&) noexcept;
const CgButtonNativeVtable15* GroupMethods(CgButtonNativeBody84h&) noexcept;
CgDisplayWindowNativeBody60h* Display(Body&) noexcept;
CgOperationWindowNativeBody30h* Operation(Body&) noexcept;
bool PollSelection(Body&,CgSceneNativeServices&);
void PollDisplay(Body&,CgSceneNativeServices&);
void PollHidden(Body&,CgSceneNativeServices&);
}
