#pragma once
#include <Runtime/SettingDialogNativeConfirm.h>
#include <Runtime/GraphicsSurfaceNativeBody.h>
#include <Runtime/ParamsGameSystemRuntime.h>
#include <bit>
#include <cstring>
#include <stdexcept>

// 只读复用已验04的直接512200核。原是direct call，不改成相似gate虚槽或modern回调。
namespace KaGuYa::Start::Runtime::SettingPage04NativeDetail
{void BuildNameTextAt512200(GuiDialogNativeBody5Ch*,int,int);}

namespace KaGuYa::Start::Runtime::SettingDialogNativeConfirmDetail
{
using Body=SettingDialogNativeBodyACh;using Common=GuiDialogNativeBody5Ch;using Slot=YtlReferenceSlotRuntime;
template<class T>T Read(const T& x) noexcept{return *static_cast<const volatile T*>(&x);}
template<class T>T& Field(void* p,unsigned offset) noexcept{return *reinterpret_cast<T*>(reinterpret_cast<std::uintptr_t>(p)+offset);}
template<class T>T* ResolveWithBase(const Slot& ref,std::uintptr_t base) noexcept
{
    if(!base)return nullptr;auto* control=static_cast<EnvironmentObjectReferenceControl*>(Read(ref.controlAt08));
    const auto stride=control?Read(control->strideAt0C):0u;return reinterpret_cast<T*>(base+Read(ref.biasAt04)*stride);
}
template<class T>T* Resolve(const Slot& ref) noexcept{return ResolveWithBase<T>(ref,Read(ref.baseAt00));}
struct Fresh final{union{Slot slot;};Fresh() noexcept{}~Fresh() noexcept{}};
// EH重读槽；正常Modal尾缓存control/不清槽，查页/块normal则payload后重读control并清base。
void ReleaseReloadClear(Slot&) noexcept;
void ReleaseReloadRetain(Slot&) noexcept;
void ReleaseCaptured(YtlReferenceControlRuntime*) noexcept;
struct RefLife final
{
    Slot& slot;bool armed=true;
    ~RefLife() noexcept{if(armed)ReleaseReloadClear(slot);}
    void NormalReload() noexcept{armed=false;ReleaseReloadClear(slot);}
    void NormalReloadRetain() noexcept{armed=false;ReleaseReloadRetain(slot);}
    void NormalCaptured(YtlReferenceControlRuntime* c) noexcept{armed=false;ReleaseCaptured(c);}
};
void* Registry(unsigned index,unsigned site,bool ordinary=false);
void* ModalReceiver(unsigned site);
std::int32_t ModalAt5071D0(void* unusedThis,Slot&);
void FindDialog(Common*,Slot&,int);
void ReloadPage(Common*);
void AddBackgroundBounds(Body&);
void ApplyWrites(std::span<const SettingDialogNativeConfirmDefaultWrite>);
void DestroySerializer(SettingDialogNativeConfirmSerialize18h&);
}
