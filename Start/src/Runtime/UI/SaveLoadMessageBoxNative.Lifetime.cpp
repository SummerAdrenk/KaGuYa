#include "SaveLoadMessageBoxNative.Internal.h"
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace SaveLoadMessageBoxNativeDetail;
thread_local SaveLoadMessageBoxNativeServices* activeServices;
Common* __fastcall Delete(Common* p, void*, std::uint32_t flags) noexcept
{ DeleteSaveLoadMessageBoxNativeAt4FDDA0(&Dialog(p), flags); return p; }
void __fastcall Begin(Common* p, void*) { BeginSaveLoadMessageBoxNativeAt50EAA0(p); }
std::int32_t __fastcall PollResult(Common* p, void*) { return PollSaveLoadMessageBoxNativeAt5253D0(p); }
}
const SaveLoadMessageBoxNativeVtable38& SaveLoadMessageBoxNativeVtable() noexcept
{
    static const auto table = []
    {
        // 原38槽与Modal表只有delete04、begin44、pollExtra90不同。
        // 继承入口只访问原80h前缀，不使用SystemDialog专属80/84字段。
        auto t = SystemDialogNativeModalVtable();
        t.common.deletingDestructorAt04 = reinterpret_cast<decltype(t.common.deletingDestructorAt04)>(&Delete);
        t.common.beginAt44 = reinterpret_cast<decltype(t.common.beginAt44)>(&Begin);
        t.pollExtraAt90 = reinterpret_cast<decltype(t.pollExtraAt90)>(&PollResult);
        return t;
    }();
    return table;
}
__declspec(noinline) SaveLoadMessageBoxNativeScope::SaveLoadMessageBoxNativeScope(
    SaveLoadMessageBoxNativeServices& s) noexcept : SaveLoadMessageBoxNativeScope(&s) {}
__declspec(noinline) SaveLoadMessageBoxNativeScope::SaveLoadMessageBoxNativeScope(
    SaveLoadMessageBoxNativeServices* s) noexcept
    : previous_(activeServices),
      modalServices_(s ? std::optional<SystemDialogNativeServices>{SystemDialogNativeServices{
          s->source, s->loader, s->surfaces, s->graphics, s->input, s->screen, s->strings}} : std::nullopt),
      modalScope_(modalServices_ ? &*modalServices_ : nullptr)
{ activeServices = s; }
__declspec(noinline) SaveLoadMessageBoxNativeScope::~SaveLoadMessageBoxNativeScope() noexcept
{ activeServices = previous_; }
__declspec(noinline) SaveLoadMessageBoxNativeServices& RequireSaveLoadMessageBoxNativeServices()
{
    if (!activeServices) throw std::logic_error("SaveLoadMessageBox native services are not bound");
    return *activeServices;
}
namespace SaveLoadMessageBoxNativeDetail
{
void DestroyCompletedMembers(Body& b) noexcept
{
    ReleaseYtlReference(b.digitsAt9C);
    ReleaseYtlReference(b.pageOnlyBaseAt90);
    ReleaseYtlReference(b.pageAndSlotBaseAt84);
    // 原4F80D0的80h Modal退役链：events74 -> background5C -> Dialog5C。
    // 通过Common原生ABI复用，不把A8正文强转为现代GuiModalDialogState。
    SystemDialogNativeModalVtable().common.deletingDestructorAt04(&b.common, 0);
}
}
void DestroySaveLoadMessageBoxNativeAt4F92E0(Body* b) noexcept
{
    b->common.vptrAt00 = &SaveLoadMessageBoxNativeVtable().common;
    DestroyCompletedMembers(*b);
}
Body* DeleteSaveLoadMessageBoxNativeAt4FDDA0(Body* b, std::uint32_t flags) noexcept
{
    DestroySaveLoadMessageBoxNativeAt4F92E0(b);
    if (flags & 1u) ::operator delete(b, std::size_t{0xA8});
    return b;
}
void BeginSaveLoadMessageBoxNativeAt50EAA0(Common* p)
{
    auto& b = Dialog(p);
    // 原循环每轮重读end；第二次解析保留第一轮EDX base，variant为DWORD回绕。
    for (auto* r = Read(p->childrenAt0C.begin); r != Read(p->childrenAt0C.end); ++r)
    {
        const auto base = Read(r->baseAt00);
        const auto variant = std::uint32_t(Read(ResolveWithBase<GuiControlNativeBody50h>(*r, base)->eventAt04)) - 18u;
        auto* child = ResolveWithBase<GuiControlNativeBody50h>(*r, base);
        const auto* table = Read(child->vptrAt00);
        const auto enabled = variant <= 3u ? std::uint32_t(Read(b.modeAt80)) == variant : true;
        table->writeEnabledAt4C(child, enabled);
    }
    // 不补写result68=-1；原50EAA0直接尾调用50CF50。
    SystemDialogNativeModalVtable().common.beginAt44(p);
}
std::int32_t PollSaveLoadMessageBoxNativeAt5253D0(Common* p)
{
    auto& b = Dialog(p);
    auto* current = Read(b.eventsAt74.begin);
    auto* const end = Read(b.eventsAt74.end);
    for (auto* r = current; r != end; ++r)
    {
        const auto base = Read(r->baseAt00);
        if (!Read(ResolveWithBase<GuiClickedEventNativeBody>(*r, base)->clickedFlagAt04)) continue;
        const auto event = Read(ResolveWithBase<GuiClickedEventNativeBody>(*r, base)->eventValueAt08);
        if (event == 22) { b.resultAt68 = 6; return 1; }
        if (event == 23) { b.resultAt68 = 7; return 1; }
        return 0; // 首个命中但未知的事件也截断扫描，flag不清。
    }
    return 0;
}
std::int32_t PollSaveLoadMessageBoxModalNativeAt52DBC0(Common* p)
{ return SystemDialogNativeModalVtable().common.pollAt48(p); }
std::int32_t EndSaveLoadMessageBoxNativeAt508E80(Common* p)
{ return SystemDialogNativeModalVtable().common.endAt4C(p); }
}
