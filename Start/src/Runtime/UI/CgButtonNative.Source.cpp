#include "CgButtonNative.Internal.h"

namespace KaGuYa::Start::Runtime::CgButtonNativeDetail
{
void* Root(unsigned ensureSite, unsigned readSite)
{
    auto& s = RequireCgButtonNativeServices().source;
    s.EnsureRootAt413BE0(ensureSite);
    return s.RootAt6198C4(readSite);
}
void* Draw(unsigned ensureSite, unsigned readSite)
{
    auto& s = RequireCgButtonNativeServices().source;
    auto* root = Root(ensureSite, readSite);
    return Resolve<void>(s.RootDrawSlotAt18(root));
}
void* System(unsigned site)
{
    auto* root = RequireCgButtonNativeServices().source.CaptureEnvironmentAt4180C0(site);
    auto* registry = ResolveEnvironmentArchiveAt417990(*root, 1);
    return Resolve<void>(Field<Slot>(registry, 0x1C));
}
void Cue(unsigned address, int channel, bool wrapped, unsigned environmentSite, unsigned ensureSite, unsigned readSite)
{
    auto& s = RequireCgButtonNativeServices();
    auto* system = System(environmentSite);
    auto* name = s.source.CueNameAt(address);
    auto* root = Root(ensureSite, readSite);
    using Query = const int* (__thiscall*)(void*);
    auto query = [&]() { return Read(*reinterpret_cast<const int*>(
        Read(Field<Query>(Read(Field<void*>(system, 0)), 0x24))(system))); };
    if (wrapped)
    {
        // 4188C0/4188F0包装链：先+24取值，后解Root+24；不能与4BEAF0交换。
        const auto value = query();
        auto* sound = Resolve<void>(s.source.RootSoundSlotAt24(root));
        s.source.PlayCueAt4BCD70(sound, name, channel, value, 1);
    }
    else
    {
        auto* sound = Resolve<void>(s.source.RootSoundSlotAt24(root));
        const auto value = query();
        s.source.PlayCueAt4BCD70(sound, name, channel, value, 1);
    }
}
RECT ButtonRect(Button& b)
{
    RECT r{0, 0, Read(b.extentAt38.cx), Read(b.extentAt38.cy)};
    const auto* p = Read(b.useNaturalAt2C) ? &b.naturalAt30 : &b.positionAt24;
    const auto y = Read(p->y);
    const auto x = Read(p->x);
    ::OffsetRect(&r, x, y);
    return r;
}
void Blit(Surface* target, Surface* source, int x, int y, const RECT* clip)
{
    // 4425C0先读取目标+04，再读取源+04；零handle不捕获Graphics。
    const auto destination = Read(target->handleAt04);
    if (!destination) return;
    const auto handle = Read(source->handleAt04);
    if (!handle) return;
    static_cast<void>(RequireCgButtonNativeServices().mode.buttons.graphics.SurfaceBlitRaw(
        destination, handle, x, y, nullptr, nullptr, clip, 0));
}
namespace
{
Surface* StateSurface(Button& b)
{
    const auto state = Read(b.stateAt04);
    if (state < 0) return nullptr;
    const auto slot = Read(*Advance(b.slotsAt40, state));
    if (slot < 0) return nullptr;
    return ResolveCgUiButtonSurfaceNativeAt4ADED0(b, slot);
}
POINT Position(Button& b, Surface& s)
{
    const auto dx = Sub(Read(s.originXAt10), Read(b.naturalAt30.x));
    const auto dy = Sub(Read(s.originYAt14), Read(b.naturalAt30.y));
    const auto* p = Read(b.useNaturalAt2C) ? &b.naturalAt30 : &b.positionAt24;
    const auto x = Add(dx, Read(p->x));
    const auto y = Add(dy, Read(p->y));
    return {x, y};
}
}
void DrawButton(Button& b, const RECT* clip)
{
    auto* surface = StateSurface(b);
    if (!surface) return;
    const auto p = Position(b, *surface);
    const auto handle = Read(surface->handleAt04);
    if (!handle) return;
    static_cast<void>(RequireCgButtonNativeServices().mode.buttons.graphics.SurfaceDrawRaw(
        handle, p.x, p.y, nullptr, nullptr, clip, 0));
}
void CompositeButton(Button& b, Surface* target, const RECT* clip)
{
    auto* surface = StateSurface(b);
    if (!surface) return;
    const auto p = Position(b, *surface);
    Blit(target, surface, p.x, p.y, clip);
}
}
