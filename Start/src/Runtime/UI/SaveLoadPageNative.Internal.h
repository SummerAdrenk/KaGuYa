#pragma once
#include <Runtime/SaveLoadPageNative.h>
#include <Runtime/GuiClickedEventNative.h>
#include <bit>
#include <cstring>
#include <new>

namespace KaGuYa::Start::Runtime::SaveLoadPageNativeDetail
{
using Body=SaveLoadPageSecondNativeBodyACh;using Common=GuiDialogNativeBody5Ch;using Slot=YtlReferenceSlotRuntime;
inline Body& Page(Common* body) noexcept{return *reinterpret_cast<Body*>(body);}
inline SaveLoadDialogNativeBody184h& Parent(Common* body) noexcept
{return *reinterpret_cast<SaveLoadDialogNativeBody184h*>(body->parentAt04);}
template<class T>T Read(const T& value) noexcept{return *static_cast<const volatile T*>(&value);}
template<class T>T* Resolve(const Slot& value) noexcept
{
    const auto base=Read(value.baseAt00);if(!base)return nullptr;
    auto* control=static_cast<EnvironmentObjectReferenceControl*>(Read(value.controlAt08));
    const auto stride=control?Read(control->strideAt0C):0u;
    return reinterpret_cast<T*>(base+Read(value.biasAt04)*stride);
}
inline Slot Copy(const Slot& source) noexcept{Slot out;CopyGuiClickedEventReference(out,source);return out;}
inline void ReleaseUncleared(Slot& slot)
{
    auto* control=slot.controlAt08;
    if(control&&--control->referenceCountAt04==0)
    {
        if(control->owningAt08)control->DestroyPayloadAtVslot00();
        if(slot.controlAt08)slot.controlAt08->DeleteControlAtVslot04(1);
    }
}
struct LocalReference
{
    Slot& slot;bool armed=true;
    ~LocalReference() noexcept{if(armed)ReleaseYtlReference(slot);}
    void Reload(bool clear=true) noexcept
    {armed=false;if(clear)ReleaseYtlReference(slot);else ReleaseUncleared(slot);}
    void Captured(YtlReferenceControlRuntime* control,bool clear=true) noexcept
    {
        armed=false;if(!control)return;
        if(--control->referenceCountAt04==0)
        {if(control->owningAt08)control->DestroyPayloadAtVslot00();control->DeleteControlAtVslot04(1);}
        if(clear){slot.baseAt00=0;slot.controlAt08=nullptr;}
    }
};
inline std::int32_t Add(std::int32_t a,std::int32_t b) noexcept
{return std::bit_cast<std::int32_t>(std::uint32_t(a)+std::uint32_t(b));}
inline std::int32_t Sub(std::int32_t a,std::int32_t b) noexcept
{return std::bit_cast<std::int32_t>(std::uint32_t(a)-std::uint32_t(b));}
SaveFileStateNativeArchive* CaptureSave(SaveLoadPageNativeServices&);
void DrawSaveLoadPageNumbers(Body&,std::int32_t page,SaveLoadPageNativeServices&);
void WriteSaveLoadPageButtons(Common&,std::int32_t page);
void NotifySaveLoadPage(Common&,SaveLoadPageNativeServices&);
}
