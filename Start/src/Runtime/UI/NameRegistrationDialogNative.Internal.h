#pragma once
#include <Runtime/NameRegistrationDialogNative.h>
#include <Runtime/GraphicsSurfaceReference.h>
#include <Runtime/GraphicsSurfaceRuntime.h>
#include <bit>
#include <stdexcept>

namespace KaGuYa::Start::Runtime::NameRegistrationDialogNativeDetail
{
using Body = NameRegistrationDialogNativeBody94h;
using Common = GuiDialogNativeBody5Ch;
using Child = GuiControlNativeBody50h;
using Slot = YtlReferenceSlotRuntime;
inline Body& Dialog(Common* p) noexcept { return *reinterpret_cast<Body*>(p); }
template<class T> T& Field(void* p, std::size_t offset)
{
    if (!p) throw std::logic_error("NameRegist native null dereference");
    return *reinterpret_cast<T*>(static_cast<std::byte*>(p) + offset);
}
inline void* Resolve(const Slot& r) noexcept { return ResolveEnvironmentObjectReference<void>(r); }
inline std::int32_t Add(std::int32_t a, std::int32_t b) noexcept
{ return std::bit_cast<std::int32_t>(std::uint32_t(a) + std::uint32_t(b)); }
struct UnwrittenReference final { union { Slot slot; }; UnwrittenReference() noexcept {} };
struct UnwrittenText final { union { RuntimeCString text; }; UnwrittenText() noexcept {} ~UnwrittenText() noexcept {} };
inline Slot Copy(const Slot& r) noexcept
{ UnwrittenReference out; CopyGuiClickedEventReference(out.slot, r); return out.slot; }
// EH重读槽；正常尾分别缓存control/重读control，clear只清原明确写的base/control。
struct LocalReference final
{
    Slot& slot;
    bool active = true;
    ~LocalReference() noexcept { if (active) ReleaseYtlReference(slot); }
    void Captured(YtlReferenceControlRuntime* c, bool clear = true) noexcept
    {
        active = false;
        if (!c) return;
        if (--c->referenceCountAt04 == 0)
        { if (c->owningAt08) c->DestroyPayloadAtVslot00(); c->DeleteControlAtVslot04(1); }
        if (clear) { slot.baseAt00 = 0; slot.controlAt08 = nullptr; }
    }
    void Reload(bool clear = true) noexcept
    {
        active = false;
        auto* c = slot.controlAt08;
        if (!c) return;
        if (--c->referenceCountAt04 == 0)
        {
            if (c->owningAt08) c->DestroyPayloadAtVslot00();
            if (slot.controlAt08) slot.controlAt08->DeleteControlAtVslot04(1);
        }
        if (clear) { slot.baseAt00 = 0; slot.controlAt08 = nullptr; }
    }
};
struct LocalText final
{
    RuntimeCString& text;
    bool active;
    ~LocalText() noexcept { if (active) DestroyRuntimeCStringAt40CD40(&text); }
    // 正常TEXT Free可抛；原调用点仍可能保有edit Ref的EH态，不能强加terminate。
    void Finish() { active = false; DestroyRuntimeCStringAt40CD40(&text); }
};
inline void* Root(INameRegistrationDialogNativeSource& s)
{ s.EnsureRootAt413BE0(); return s.RootAt6198C4(); }
inline void* Registry1(INameRegistrationDialogNativeSource& s)
{ return s.RegistryAt417990(s.SystemAt4180C0(), 1); }
inline void* GameSystem(INameRegistrationDialogNativeSource& s)
{ auto* p = s.ParamsAt418480(); return Resolve(s.ParamsGameSystemSlotAt04(p)); }
// 98h端口只有原生正文才能使用；根从不进入Field。
inline void* Edit(const Slot& r) { auto* p = Resolve(r); if (!p) throw std::logic_error("NameRegist missing native edit98"); return p; }
inline HWND Window(void* p) { return Field<HWND>(p, 4); }
inline HWND EditWindow(void* p) { return Field<HWND>(p, 0x28); }
inline const NameRegistrationDialogNativeVtable38& Table(Common* p) noexcept
{ return *reinterpret_cast<const NameRegistrationDialogNativeVtable38*>(p->vptrAt00); }
inline Slot& Find(Common* p, Slot& out, std::int32_t id)
{ p->vptrAt00->findChildAt14(p, &out, id); return out; }
void SetRadio(Body&, std::int32_t id, std::int32_t invalidate);
std::int32_t RadioChoice(Body&);
std::int32_t FocusEdit(Body&);
std::int32_t Outside(Common*);
void BeginModal(Common*);
void SetEditBackground(void* nativeEdit, COLORREF color);
std::int32_t __fastcall UpperInput(Common*, void*);
std::int32_t __fastcall ActiveInput(Common*, void*);
void __fastcall ResetInput(Common*, void*);
void __fastcall Begin(Common*, void*);
std::int32_t __fastcall Poll(Common*, void*);
std::int32_t __fastcall End(Common*, void*);
std::int32_t __fastcall Result(Common*, void*);
void __fastcall ProcessInput(Common*, void*);
void __fastcall FlushDirty(Common*, void*);
std::int32_t __fastcall PollExtra(Common*, void*);
void __fastcall Noop(Common*, void*);
}
