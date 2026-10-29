#include "ProgramMain00DialogNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
namespace { thread_local ProgramMain00DialogNativeServices* selected = nullptr; }
ProgramMain00DialogNativeScope::ProgramMain00DialogNativeScope(ProgramMain00DialogNativeServices& s) noexcept
    : ProgramMain00DialogNativeScope(&s) {}
ProgramMain00DialogNativeScope::ProgramMain00DialogNativeScope(ProgramMain00DialogNativeServices* s) noexcept
    : previous_(selected)
{
    selected = s;
    if (s)
    {
        surfaces_.emplace(s->shared.surfaces);
        graphics_.emplace(s->shared.graphics);
        input_.emplace(s->shared.input);
        screen_.emplace(s->shared.screen);
    }
}
ProgramMain00DialogNativeScope::~ProgramMain00DialogNativeScope() noexcept { selected = previous_; }
ProgramMain00DialogNativeServices* SelectedProgramMain00DialogNativeServices() noexcept { return selected; }
ProgramMain00DialogNativeServices& RequireProgramMain00DialogNativeServices()
{ if (!selected) std::terminate(); return *selected; }

namespace ProgramMain00NativeDetail
{
void ReleaseReload(Slot& slot, bool clear)
{
    auto* control = Read(slot.controlAt08);
    if (!control) return;
    if (--control->referenceCountAt04 == 0)
    {
        if (control->owningAt08) control->DestroyPayloadAtVslot00();
        if (auto* current = Read(slot.controlAt08)) current->DeleteControlAtVslot04(1);
    }
    if (clear) { slot.controlAt08 = nullptr; slot.baseAt00 = 0; }
}
void RefFrame::Captured(Control* control, bool clear) noexcept
{
    active = false;
    if (!control) return;
    if (--control->referenceCountAt04 == 0)
    {
        if (control->owningAt08) control->DestroyPayloadAtVslot00();
        control->DeleteControlAtVslot04(1);
    }
    if (clear) { slot.controlAt08 = nullptr; slot.baseAt00 = 0; }
}
void RefFrame::Reload(bool clear) noexcept { active = false; ReleaseReload(slot, clear); }
void RefFrame::ReloadUnchecked(bool clear) noexcept
{ CapturedReloadUnchecked(Read(slot.controlAt08), clear); }
void RefFrame::CapturedReloadUnchecked(Control* control, bool clear) noexcept
{
    active = false;
    if (!control) return;
    if (--control->referenceCountAt04 == 0)
    {
        if (control->owningAt08)
        {
            control->DestroyPayloadAtVslot00();
            control = Read(slot.controlAt08);
        }
        // 4DD04E/505F87：payload后重读但无null检查，不增补兜底。
        control->DeleteControlAtVslot04(1);
    }
    if (clear) { slot.controlAt08 = nullptr; slot.baseAt00 = 0; }
}
void* Archive(EnvironmentRootStorage* environment, std::uint32_t index) noexcept
{
    auto* begin = Read(environment->archivesAt30.begin);
    const auto bytes = std::uint32_t(reinterpret_cast<std::uintptr_t>(Read(environment->archivesAt30.end)) - reinterpret_cast<std::uintptr_t>(begin));
    return std::uint32_t(std::bit_cast<std::int32_t>(bytes) / 12) > index ? Resolve<void>(begin[index]) : nullptr;
}
void* Root(std::uint32_t ensure, std::uint32_t read)
{
    auto& source = RequireProgramMain00DialogNativeServices().shared.source;
    source.EnsureRootAt413BE0(ensure);
    return source.RootAt6198C4(read);
}
void* BorrowInlineEnvironment(std::uint32_t ensureSite, std::uint32_t readSite)
{
    auto& source = RequireProgramMain00DialogNativeServices().source;
    source.EnsureEnvironmentInline(ensureSite);
    return Archive(source.EnvironmentAt6198C8(readSite), 0);
}
void* BorrowEnvironment(std::uint32_t site)
{ return Archive(RequireProgramMain00DialogNativeServices().shared.source.CaptureEnvironmentAt4180C0(site), 0); }
std::int32_t ReadNamed(void* root, void* archive, const char* name,
    std::uint32_t resolve, std::uint32_t params, std::uint32_t read)
{
    auto& s = RequireProgramMain00DialogNativeServices().shared.source;
    const auto index = s.ResolveNamedAt498020(archive, name, resolve);
    auto* receiver = s.ParamsAt47BDB0(root, params);
    return s.ReadValueAt4C73A0(receiver, index, read);
}
void WriteNamed(void* root, void* archive, const char* name, std::int32_t value,
    std::uint32_t resolve, std::uint32_t params, std::uint32_t write)
{
    auto& s = RequireProgramMain00DialogNativeServices().shared.source;
    const auto index = s.ResolveNamedAt498020(archive, name, resolve);
    auto* receiver = s.ParamsAt47BDB0(root, params);
    s.WriteValueAt4C8220(receiver, index, value, write);
}
void PlaySe(SoundReceiverOrder order,std::uint32_t nameAddress, std::uint32_t environmentSite,
    std::uint32_t registrySite, std::uint32_t ensureSite, std::uint32_t rootSite,
    std::uint32_t playSite)
{
    auto& services = RequireProgramMain00DialogNativeServices();
    auto& s = services.shared.source;
    auto* registry = s.RegistryAt417990(s.CaptureEnvironmentAt4180C0(environmentSite), 1, registrySite);
    auto* params = Resolve<void>(*reinterpret_cast<Slot*>(static_cast<std::byte*>(registry) + 0x1C));
    const auto* name = services.source.SoundNameAt61A6xx(nameAddress);
    auto* root = Root(ensureSite, rootSite);
    // 52CB35/52CBB1先读音量，之后4188F0才解固定Root的Sound；另外四处相反。
    // 音量虚调用可重绑同Root的Sound槽，两种顺序不能合并。
    void* sound=nullptr;
    if(order==SoundReceiverOrder::BeforeVolume)sound=Resolve<void>(s.RootSoundSlotAt24(root));
    const auto volume = *services.source.SeVolumeAtVslot24(params);
    if(order==SoundReceiverOrder::AfterVolume)sound=Resolve<void>(s.RootSoundSlotAt24(root));
    (void)services.source.PlaySeAt4BCD70(sound, name, 0, volume, 1, playSite);
}
void InvalidateScreen(Body& b, bool inlineEnvironment, std::uint32_t heightSite, std::uint32_t widthSite,
    std::uint32_t heightRead, std::uint32_t widthRead)
{
    auto& services = RequireProgramMain00DialogNativeServices();
    auto capture = [&](std::uint32_t site, std::uint32_t readSite) {
        if (!inlineEnvironment) return services.shared.source.CaptureEnvironmentAt4180C0(site);
        services.source.EnsureEnvironmentInline(site);
        return services.source.EnvironmentAt6198C8(readSite);
    };
    const auto height = Read(capture(heightSite, heightRead)->heightAt1C);
    const auto width = Read(capture(widthSite, widthRead)->widthAt18);
    Methods(b).invalidateAt64(&b.common, {0, 0, std::bit_cast<std::int32_t>(width), std::bit_cast<std::int32_t>(height)});
}
void Blit(GraphicsSurfaceNativeBody* destination, GraphicsSurfaceNativeBody* source,
    std::int32_t x, std::int32_t y)
{
    const auto destinationHandle = Read(destination->handleAt04);
    if (!destinationHandle) throw 1;
    const auto sourceHandle = Read(source->handleAt04);
    if (!sourceHandle) throw 1;
    auto& graphics = RequireProgramMain00DialogNativeServices().shared.graphics.CompositionAt4182F0();
    if (!graphics.BlitSurface(destinationHandle, sourceHandle, x, y, nullptr, nullptr, nullptr, 0)) throw 1;
}
void Clear(GraphicsSurfaceNativeBody* surface)
{
    const auto handle = Read(surface->handleAt04);
    if (!handle) return;
    const auto color = Read(surface->clearColorAt20);
    RequireProgramMain00DialogNativeServices().shared.graphics.CompositionAt4182F0().ClearSurface(handle, color, nullptr);
}
RECT SurfaceRect(const Slot& slot)
{
    auto* surface = Resolve<GraphicsSurfaceNativeBody>(slot);
    const auto width = Read(surface->widthAt08), height = Read(surface->heightAt0C);
    surface = Resolve<GraphicsSurfaceNativeBody>(slot);
    const auto x = Read(surface->originXAt10), y = Read(surface->originYAt14);
    return {x, y, Add(x, width), Add(y, height)};
}
void LoadFresh(Slot& output, const char* name)
{
    auto& s = RequireProgramMain00DialogNativeServices();
    void* receiver = s.loader.CaptureLoadReceiverAt413BE0();
    AdoptGraphicsSurfaceAt407A80(output, NewGraphicsSurfaceAt40C2E0(s.shared.surfaces, s.shared.strings));
    if (!s.loader.LoadIntoAt4800A0(receiver, *Resolve<GraphicsSurfaceNativeBody>(output), name)) throw 1;
}
void AppendStaticSurface(Body& b, const Slot& surface, std::int32_t event, const RECT& bounds, bool virtualAppend)
{
    Slot group;
    ConstructGuiControlResourceReferenceAt4D76A0(group, NewGuiControlResourceAt5047DD(), 1);
    RefFrame groupFrame{group};
    auto* capturedGroupControl = Read(group.controlAt08);
    AppendGuiControlSurfaceArgumentAt538350(*Resolve<GuiControlResourceNativeBody38h>(group), 0, Copy(surface));
    ::CopyRect(&Resolve<GuiControlResourceNativeBody38h>(group)->combinedRectAt28, &bounds);
    void* raw = ::operator new(0x50);
    GuiControlNativeBody50h* control = nullptr;
    try { if (raw) control = ConstructGuiStaticAt4F4050(static_cast<GuiControlNativeBody50h*>(raw), &b.common, event, Copy(group)); }
    catch (...) { ::operator delete(raw, std::size_t{0x50}); throw; }
    Slot child;
    ConstructGuiStaticReferenceAt4D7940(child, control, 1);
    RefFrame childFrame{child};
    if (virtualAppend) Methods(b).appendChildAt10(&b.common, Copy(child));
    else
    {
        Slot argument = Copy(child);
        RefFrame argumentFrame{argument};
        AppendGuiControlChildReference(b.common.childrenAt0C, argument);
        argumentFrame.Reload();
    }
    childFrame.ReloadUnchecked(false);
    groupFrame.Captured(capturedGroupControl, false);
}
}
}
