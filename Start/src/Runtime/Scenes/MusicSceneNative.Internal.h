#pragma once
#include <Runtime/MusicSceneNative.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/GraphicsSurfaceReference.h>
#include <Runtime/EnvironmentObjectReference.h>
#include <bit>
#include <cstring>

namespace KaGuYa::Start::Runtime::MusicSceneNativeDetail
{
using Body=MusicSceneNativeBody64h;
using Slot=YtlReferenceSlotRuntime;
template<class T>T Read(const T& value) noexcept{return *static_cast<const volatile T*>(&value);}
template<class T>T& Field(void* p,std::size_t n) noexcept
{return *reinterpret_cast<T*>(reinterpret_cast<std::uintptr_t>(p)+n);}
template<class T>T* ResolveWithBase(const Slot& r,std::uintptr_t base) noexcept
{
    if(!base)return nullptr;
    auto* control=Read(r.controlAt08);
    const auto stride=control?Read(static_cast<EnvironmentObjectReferenceControl*>(control)->strideAt0C):0u;
    return reinterpret_cast<T*>(base+Read(r.biasAt04)*stride);
}
template<class T>T* Resolve(const Slot& r) noexcept{return ResolveWithBase<T>(r,Read(r.baseAt00));}
inline std::int32_t Add(std::int32_t a,std::int32_t b) noexcept
{return std::bit_cast<std::int32_t>(std::uint32_t(a)+std::uint32_t(b));}
inline std::int32_t Sub(std::int32_t a,std::int32_t b) noexcept
{return std::bit_cast<std::int32_t>(std::uint32_t(a)-std::uint32_t(b));}
struct RefLife final
{
    Slot& slot;bool active=true;
    ~RefLife() noexcept{if(active)ReleaseYtlReference(slot);}
    // 4A097D/4B8E6B正常尾不清槽，payload返回后才重读control。
    void NormalTerminate() noexcept;
};
void* Root(IMusicSceneNativeSource&,std::uint32_t);
void* Scenes(IMusicSceneNativeSource&,std::uint32_t);
void* Draw(IMusicSceneNativeSource&,std::uint32_t);
void* Registry(IMusicSceneNativeSource&,std::uint32_t index,std::uint32_t site,bool inlineLookup=false);
void ResetOperation(TransitOperation&) noexcept;
void Retire(Body&) noexcept;
void AdoptButton(Slot&,void*);
void CaptureScreen(Body&,IMusicSceneNativeSource&,std::uint32_t);
const char* BlackName() noexcept;
const char* SettingName() noexcept;
const char* CgName() noexcept;
const char* SceneName() noexcept;
const char* TitleName() noexcept;
}
