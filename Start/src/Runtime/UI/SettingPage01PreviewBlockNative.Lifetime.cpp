#include "SettingPage01PreviewBlockNative.Internal.h"
#include <Runtime/NativeGraphicsRuntimeLibrary.h>
#include <new>

namespace KaGuYa::Start::Runtime
{
using namespace SettingPage01PreviewBlockNativeDetail;
namespace
{
thread_local SettingPage01PreviewBlockNativeServices* active = nullptr;
Common* __fastcall Delete(Common* p, void*, std::uint32_t flags) noexcept
{ DeleteSettingPage01PreviewBlockNativeAt4FCFD0(&B(p), flags); return p; }
}
const SettingPageChoiceNativeVtable38& SettingPage01PreviewBlockNativeVtable() noexcept
{
    static const auto table = [] {
        auto t = ChoiceBaseTable(); // 5BF0CC：确切继承35槽，不用成功占位函数填表。
        t.common.deletingDestructorAt04 = reinterpret_cast<decltype(t.common.deletingDestructorAt04)>(&Delete);
        t.common.pollAt48 = reinterpret_cast<decltype(t.common.pollAt48)>(&Poll);
        t.common.endAt4C = reinterpret_cast<decltype(t.common.endAt4C)>(&End);
        t.common.drawAt60 = reinterpret_cast<decltype(t.common.drawAt60)>(&Draw);
        t.common.compositeAt5C = reinterpret_cast<decltype(t.common.compositeAt5C)>(&Composite);
        t.loadAt90 = reinterpret_cast<decltype(t.loadAt90)>(&Load);
        t.storeAt94 = reinterpret_cast<decltype(t.storeAt94)>(&Store);
        return t;
    }(); return table;
}
namespace SettingPage01PreviewBlockNativeDetail
{
void DestroyMembers(Body& b) noexcept
{
    // Ctor的state3/2/1/0逆序；此入口不调用store，避免失败构造保存半成品。
    ReleaseYtlReference(b.textAtA8);
    DestroyGuiControlChildrenAt4F6C60(b.previewsAt8C); // 4F6BB0/4D7E70同12字节Ref算法及terminate边界。
    b.transparencyAt80.DestroyStorage(); // 4F6A30/48F600与既有DWORD vector同存储核。
    DestroyChoice(&b.choice);
}
}
void DestroySettingPage01PreviewBlockNativeAt4FCFD0(Body* b) noexcept
{
    b->choice.common.vptrAt00 = &SettingPage01PreviewBlockNativeVtable().common;
    Store(&b->choice.common, nullptr); // 5F7F94(flags5)：异常终止，不吞store错误。
    DestroyMembers(*b);
}
Body* DeleteSettingPage01PreviewBlockNativeAt4FCFD0(Body* b, std::uint32_t flags) noexcept
{
    DestroySettingPage01PreviewBlockNativeAt4FCFD0(b);
    if (flags & 1u) ::operator delete(b, std::size_t{0xB4});
    return b;
}
SettingPage01PreviewBlockNativeScope::SettingPage01PreviewBlockNativeScope(
    SettingPage01PreviewBlockNativeServices& s) noexcept : previous_(active) { active = &s; }
SettingPage01PreviewBlockNativeScope::~SettingPage01PreviewBlockNativeScope() noexcept { active = previous_; }
SettingPage01PreviewBlockNativeServices& RequireSettingPage01PreviewBlockNativeServices()
{ if (!active) throw std::logic_error("Setting preview native services are not bound"); return *active; }
const char* SettingPage01PreviewBlockNativeGlobals::FontFaceAt61A644()
{ return Read(*face_); }
const char* SettingPage01PreviewBlockNativeGlobals::SampleCharactersAt61A648()
{ return Read(*sample_); }
bool SettingPage01PreviewBlockNativeText::CreateFontAt4388B0(const GraphicsTextNativeFontParameters* p)
{ return graphics_.TextCalls().CreateTextFontNativeAt4388B0(p); }
bool SettingPage01PreviewBlockNativeText::InitializeAt4389D0(const char* p, std::int32_t x, std::int32_t y)
{ return graphics_.TextCalls().InitializeText(p, x, y); }
bool SettingPage01PreviewBlockNativeText::BlitAllAt53A980(std::int32_t h, RECT* p)
{ return graphics_.TextCalls().BlitAllTextToSurface(h, p); }
Common* SettingPage01PreviewBlockNativeConstruction::ConstructBlock02At4E5510(void* raw, Common* p, std::int32_t id)
{ return &ConstructSettingPage01PreviewBlockNativeAt4E5510(raw, p, id)->choice.common; }
}
