#include "SaveLoadPopupNative.Internal.h"
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace SaveLoadPopupNativeDetail;
thread_local SaveLoadPopupNativeServices* activeServices;
Common* __fastcall Delete(Common* p, void*, std::uint32_t flags) noexcept
{ DeleteSaveLoadPopupNativeAt4FDFE0(&Popup(p), flags); return p; }
constexpr std::array<std::uint32_t, 35> slots{{
    0x509ED0,0x4FDFE0,0x5025A0,0x50A5D0,0x4FF020,0x509D20,0x536310,
    0x536500,0x50FF40,0x538660,0x510630,0x500800,0x532170,0x51FC00,
    0x511300,0x5107D0,0x511610,0x50EBB0,0x52FBB0,0x5094C0,0x50ABC0,
    0x51A040,0x525FB0,0x5249C0,0x524A90,0x50FC80,0x522E90,0x510030,
    0x5100E0,0x50A6A0,0x50B920,0x510220,0x510170,0x5102B0,0x518A80}};
}
const GuiDialogNativeVtable35& SaveLoadPopupNativeVtable() noexcept
{
    static const auto table = []
    {
        auto t = SaveLoadPageBaseNativeVtable();
        // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 void* 仅占 EDX、不读取其输入。
        // flags/clip 仍各占 4 字节栈实参并由被调方清栈，其余槽无栈实参；各返回类型一致。
        // Delete 的 noexcept 仅向可抛槽类型放宽，其余异常约定不变。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
#define BIND(field, entry) t.field = reinterpret_cast<decltype(t.field)>(&entry)
        BIND(deletingDestructorAt04, Delete);
        BIND(upperInputAt34, UpperInput);
        BIND(activeInputAt38, ActiveInput);
        BIND(resetInputAt3C, ResetInput);
        BIND(priorityInputAt40, PriorityInput);
        BIND(beginAt44, Begin);
        BIND(pollAt48, Poll);
        BIND(processInputAt54, ProcessInput);
        BIND(drawAt60, Draw);
#undef BIND
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
        return t;
    }();
    return table;
}
std::span<const std::uint32_t,35> SaveLoadPopupNativeOriginalSlots() noexcept { return slots; }
namespace SaveLoadPopupNativeDetail
{
void DestroyConstructedMembers(Body& p) noexcept
{
    DestroyNumbersAt4F7080(p.numbersAtA0);
    DestroyGuiControlSurfaceReferencesAt43EAA0(p.markersAt94);
    DestroyGuiControlSurfaceReferencesAt43EAA0(p.digitsAt88);
    p.common.vptrAt00 = &SaveLoadPageBaseNativeVtable();
    ReleaseYtlReference(p.managerAt70);
    DestroyGuiDialogAt4F7AC0(&p.common);
}
}
void DestroySaveLoadPopupNative(Body* p) noexcept
{
    p->common.vptrAt00 = &SaveLoadPopupNativeVtable();
    DestroyConstructedMembers(*p);
}
Body* DeleteSaveLoadPopupNativeAt4FDFE0(Body* p, std::uint32_t flags) noexcept
{
    DestroySaveLoadPopupNative(p);
    if (flags & 1u) ::operator delete(p, std::size_t{0xC0});
    return p;
}
__declspec(noinline) SaveLoadPopupNativeScope::SaveLoadPopupNativeScope(SaveLoadPopupNativeServices& s) noexcept
    : SaveLoadPopupNativeScope(&s) {}
__declspec(noinline) SaveLoadPopupNativeScope::SaveLoadPopupNativeScope(SaveLoadPopupNativeServices* s) noexcept
    : previous_(activeServices), surfaces_(s ? &s->surfaces : nullptr), graphics_(s ? &s->graphics : nullptr),
      input_(s ? &s->input : nullptr), screen_(s ? &s->screen : nullptr)
{ activeServices = s; }
__declspec(noinline) SaveLoadPopupNativeScope::~SaveLoadPopupNativeScope() noexcept { activeServices = previous_; }
__declspec(noinline) SaveLoadPopupNativeServices& RequireSaveLoadPopupNativeServices()
{
    if (!activeServices) throw std::logic_error("SaveLoadPopup native services are not bound");
    return *activeServices;
}
EnvironmentRootStorage* RootSaveLoadPopupNativeSource::CaptureEnvironmentAt4180C0()
{ return environment_.CaptureAt4180C0(); }
InputRootNativeBody24h* RootSaveLoadPopupNativeSource::InputAt418210()
{ return input_.CaptureAt418210(); }
}
