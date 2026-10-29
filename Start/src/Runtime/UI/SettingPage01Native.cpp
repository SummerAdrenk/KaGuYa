#include "SettingPage02Native.Internal.h"

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace SettingPage02NativeDetail;
Common* __fastcall Delete(Common* p, void*, std::uint32_t flags) noexcept
{ DeleteSettingPage01NativeAt4FE320(reinterpret_cast<Parent*>(p), flags); return p; }
}
const SettingPageNativeVtable39& SettingPage01NativeVtable() noexcept
{
    static const auto t = [] {
        auto v = SettingPageNativeBaseVtable();
        v.common.common.deletingDestructorAt04 = reinterpret_cast<decltype(v.common.common.deletingDestructorAt04)>(&Delete);
        v.reloadAt94 = reinterpret_cast<decltype(v.reloadAt94)>(&SettingPage02NativeDetail::Reload01);
        return v;
    }(); return t;
}
SettingPageRadioContainerNativeBody7Ch* ConstructSettingPage01Block00NativeAt4E50B0(void* raw, Common* parent, std::int32_t id)
{ return ConstructContainer(raw, parent, id, SettingPageRadioContainerKind::Page01Block00); }
SettingPageRadioContainerNativeBody7Ch* ConstructSettingPage01Block01NativeAt4E52E0(void* raw, Common* parent, std::int32_t id)
{ return ConstructContainer(raw, parent, id, SettingPageRadioContainerKind::Page01Block01); }
SettingPage01NativeBody84h* ConstructSettingPage01NativeAt4F1EE0(void* raw, Common* parent, std::int32_t id,
    ISettingPage01Block02NativeConstruction& preview)
{ return ConstructParent(raw, parent, id, true, &preview); }
SettingPage01NativeBody84h* DeleteSettingPage01NativeAt4FE320(SettingPage01NativeBody84h* p, std::uint32_t flags) noexcept
{
    p->common.vptrAt00 = &SettingPage01NativeVtable().common.common;
    DestroySettingPageNativeAt4F9840(p);
    if (flags & 1u) ::operator delete(p, std::size_t{0x84});
    return p;
}
GuiDialogNativeBody5Ch* SettingPage01And02NativeConstruction::ConstructPage01At4F1EE0(void* raw, Common* p, std::int32_t id)
{ return &ConstructSettingPage01NativeAt4F1EE0(raw, p, id, preview_)->common; }
GuiDialogNativeBody5Ch* SettingPage01And02NativeConstruction::ConstructPage02At4F23B0(void* raw, Common* p, std::int32_t id)
{ return &ConstructSettingPage02NativeAt4F23B0(raw, p, id)->common; }
}
