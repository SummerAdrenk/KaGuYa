#pragma once
#include <Runtime/SettingPage01PreviewBlockNative.h>
#include <Runtime/GraphicsSurfaceRuntime.h>
#include "SettingPage02Native.Internal.h"

namespace KaGuYa::Start::Runtime::SettingPage01PreviewBlockNativeDetail
{
using namespace SettingPage02NativeDetail;
using Body = SettingPage01PreviewBlockNativeBodyB4h;
using Control = GuiControlNativeBody50h;
using Surface = GraphicsSurfaceNativeBody;
using Color = ColorTransWindowNativeBody38h;
inline Body& B(Common* p) noexcept { return *reinterpret_cast<Body*>(p); }
inline const GuiSliderNativeVtable31& Slider(Control* p) noexcept
{ return *reinterpret_cast<const GuiSliderNativeVtable31*>(Read(p->vptrAt00)); }
void DestroyMembers(Body&) noexcept;
void PopulateSlider(Body&, const GuiSliderDescriptorNative6Ch&, RECT&);
void __fastcall Load(Common*, void*);
void __fastcall Store(Common*, void*);
std::int32_t __fastcall Poll(Common*, void*);
std::int32_t __fastcall End(Common*, void*);
void __fastcall Draw(Common*, void*, const RECT*);
void __fastcall Composite(Common*, void*, Slot, const RECT*);
inline void Invalidate(Common& p, RECT rect) { Read(p.vptrAt00)->invalidateAt64(&p, rect); }
inline void InvalidateChild(Common& p, Slot& slot)
{
    auto* child = Resolve<Control>(slot);
    auto* table = Read(p.vptrAt00); // 父虚表先于child的rect虚调用固定。
    RECT out;
    auto* rect = Read(child->vptrAt00)->rectAt3C(child, &out);
    table->invalidateAt64(&p, *rect);
}
inline std::int32_t Signed(std::uint32_t v) noexcept { return std::bit_cast<std::int32_t>(v); }
}
