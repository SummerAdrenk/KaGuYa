#include "LikabilityDialogNative.Internal.h"
#include "LikabilityDialogNative.Profile.h"
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
thread_local LikabilityDialogNativeServices* services = nullptr;
using namespace LikabilityNativeDetail;
GuiDirtyRectNativeBody80h* __fastcall Dirty(Common* p, void*) { return Resolve<GuiDirtyRectNativeBody80h>(p->dirtyAt50); }
Common* __fastcall Delete(Common* p, void*, std::uint32_t flags)
{ return &DeleteLikabilityDialogNativeAt4FC750(&Derived(p), flags)->common; }
void __fastcall Begin(Common* p, void*) { BeginLikabilityDialogNativeAt50C950(Derived(p)); }
std::int32_t __fastcall Poll(Common* p, void*) { return PollLikabilityDialogNativeAt52D4A0(Derived(p)); }
std::int32_t __fastcall End(Common*, void*) { return 1; }
void __fastcall Input(Common* p, void*) { ProcessLikabilityDialogInputNativeAt518F30(Derived(p)); }
void __fastcall Flush(Common* p, void*) { FlushLikabilityDialogDirtyNativeAt5261A0(Derived(p)); }
}
LikabilityDialogNativeScope::LikabilityDialogNativeScope(LikabilityDialogNativeServices& s) noexcept
    : previous_(services), surfaces_(s.surfaces), graphics_(s.graphics), input_(s.input), screen_(s.screen)
{ services = &s; }
LikabilityDialogNativeScope::~LikabilityDialogNativeScope() noexcept { services = previous_; }
LikabilityDialogNativeServices& RequireLikabilityDialogNativeServices()
{
    if (!services) throw std::logic_error("Likability native services are not bound");
    return *services;
}
const LikabilityDialogNativeVtable35& LikabilityDialogNativeVtable() noexcept
{
    static const auto table = [] {
        auto t = GuiDialogBaseVtable();
        t.dirtyAt00 = reinterpret_cast<decltype(t.dirtyAt00)>(&Dirty);
        t.deletingDestructorAt04 = reinterpret_cast<decltype(t.deletingDestructorAt04)>(&Delete);
        t.beginAt44 = reinterpret_cast<decltype(t.beginAt44)>(&Begin);
        t.pollAt48 = reinterpret_cast<decltype(t.pollAt48)>(&Poll);
        t.endAt4C = reinterpret_cast<decltype(t.endAt4C)>(&End);
        t.processInputAt54 = reinterpret_cast<decltype(t.processInputAt54)>(&Input);
        t.flushDirtyAt58 = reinterpret_cast<decltype(t.flushDirtyAt58)>(&Flush);
        return t;
    }();
    return table;
}
void DestroyLikabilityDialogNative(LikabilityDialogNativeBody94h& b) noexcept
{
    b.common.vptrAt00 = &LikabilityDialogNativeVtable();
    DestroyLikabilityProfileState(b.profileState);
    b.profileState = nullptr;
    ReleaseYtlReference(b.canvasAt84);
    ReleaseYtlReference(b.backgroundAt78);
    ReleaseYtlReference(b.clickedManagerAt6C);
    DestroyGuiDialogAt4F7AC0(&b.common);
}
LikabilityDialogNativeBody94h* DeleteLikabilityDialogNativeAt4FC750(Body* b, std::uint32_t flags) noexcept
{
    DestroyLikabilityDialogNative(*b);
    if (flags & 1u) ::operator delete(b, sizeof(Body));
    return b;
}
Body* ConstructLikabilityDialogNativeAt4E0E30(Body& b, LikabilityDialogNativeServices& s)
{ LikabilityDialogNativeScope scope(s); return ConstructLikabilityDialogNativeAt4E0E30(b); }
GuiDialogNativeBody5Ch* ConstructLikabilityDialogNativeAt4E0E30(void* raw, LikabilityDialogNativeServices& s)
{ return &ConstructLikabilityDialogNativeAt4E0E30(*::new(raw) Body, s)->common; }

namespace LikabilityNativeDetail
{
void ReleaseCaptured(Control* c)
{
    if (c && --c->referenceCountAt04 == 0)
    { if (c->owningAt08) c->DestroyPayloadAtVslot00(); c->DeleteControlAtVslot04(1); }
}
void ReleaseCurrent(Slot& r, bool clear)
{
    auto* c = Read(r.controlAt08);
    if (!c) return;
    if (--c->referenceCountAt04 == 0)
    {
        if (c->owningAt08) c->DestroyPayloadAtVslot00();
        if (Read(r.controlAt08)) r.controlAt08->DeleteControlAtVslot04(1);
    }
    if (clear) { r.controlAt08 = nullptr; r.baseAt00 = 0; }
}
void Assign(Slot& target, const Slot& from, bool clear)
{
    if (&target == &from) return;
    ReleaseCurrent(target, clear);
    target.baseAt00 = Read(from.baseAt00);
    target.biasAt04 = Read(from.biasAt04);
    target.controlAt08 = Read(from.controlAt08);
    if (target.controlAt08) ++target.controlAt08->referenceCountAt04;
}
void BorrowArchive0(EnvironmentRootStorage* env)
{
    // 原内联count是signed /12，仅等于0跳过；虽498020不使用ECX，也不能省去这些物理读取。
    const auto& archives = env->archivesAt30;
    auto* begin = Read(archives.begin);
    if (Count(archives) != 0) static_cast<void>(Resolve<void>(begin[0]));
}
void* CaptureDraw(std::uint32_t ensureSite, std::uint32_t rootSite)
{
    auto& source = RequireLikabilityDialogNativeServices().source;
    source.EnsureRootAt413BE0(ensureSite);
    auto* root = source.RootAt6198C4(rootSite);
    return Resolve<void>(source.RootDrawSlotAt18(root));
}
std::int32_t NamedValue(void* root, const char* name, std::uint32_t paramsSite)
{
    auto& s = RequireLikabilityDialogNativeServices();
    const auto index = ResolveScriptNamedValueNativeAt498020(name, s.names, s.strings);
    auto* params = s.source.ParamsAt47BDB0(root, paramsSite);
    return ReadScriptValueAt4C73A0(*params, index, s.errors);
}
void BindEventAt536DB0(GuiControlNativeBody50h* child, Slot owned)
{
    Local argument{owned};
    Assign(child->eventAt3C, owned, false);
    child->eventEnabledAt48 = 1;
    argument.Current();
}
void AppendEventAt4FEF80(GuiClickedEventReferences* manager, Slot owned)
{
    Local argument{owned};
    AppendGuiClickedEventReference(*manager, owned);
    argument.Current();
}
std::int32_t PollEventsAt50FDB0(GuiClickedEventReferences* manager)
{
    auto* at = Read(manager->begin);
    auto* const end = Read(manager->end);
    for (; at != end; ++at)
    {
        const auto base = Read(at->baseAt00);
        if (!Read(ResolveWithBase<GuiClickedEventNativeBody>(*at, base)->clickedFlagAt04)) continue;
        const auto value = Read(ResolveWithBase<GuiClickedEventNativeBody>(*at, base)->eventValueAt08);
        auto* event = ResolveWithBase<GuiClickedEventNativeBody>(*at, base);
        event->vptrAt00->clearAt04(event);
        return value;
    }
    return -1;
}
void Blit(Body& b, Vector& digits, std::int32_t digit, POINT at)
{
    // 4425C0目标handle先读；digit沿原signed物理索引，不钳制/取绝对值/模10兜底。
    const auto address = reinterpret_cast<std::uintptr_t>(Read(digits.begin)) + static_cast<std::uint32_t>(digit) * 12u;
    auto* source = Resolve<Surface>(*reinterpret_cast<const Slot*>(address));
    auto* target = Resolve<Surface>(b.canvasAt84);
    const auto destination = Read(target->handleAt04);
    if (!destination) return;
    const auto input = Read(source->handleAt04);
    if (!input) return;
    auto& api = RequireLikabilityDialogNativeServices().graphics.CompositionAt4182F0();
    static_cast<void>(api.BlitSurface(destination, input, at.x, at.y, nullptr, nullptr, nullptr, 0));
}
}
}
