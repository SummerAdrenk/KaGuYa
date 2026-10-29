#pragma once

#include <Runtime/ProgramMain00DialogNative.h>
#include <Runtime/ProgramSelectDialogNative.h>
#include <Runtime/GraphicsSurfaceRuntime.h>
#include <bit>
#include <exception>
#include <new>

namespace KaGuYa::Start::Runtime::ProgramMain00NativeDetail
{
using Body = ProgramMain00DialogNativeBodyC0h;
using Parts = ProgramTrainingPartsNativeBody74h;
using Common = GuiDialogNativeBody5Ch;
using Slot = YtlReferenceSlotRuntime;
using Control = YtlReferenceControlRuntime;
template<class T> T Read(const T& v) noexcept { return *static_cast<const volatile T*>(&v); }
template<class T> T* Resolve(const Slot& s) noexcept
{
    const auto base = Read(s.baseAt00);
    if (!base) return nullptr;
    auto* control = Read(s.controlAt08);
    const auto stride = control ? Read(static_cast<EnvironmentObjectReferenceControl*>(control)->strideAt0C) : 0u;
    return reinterpret_cast<T*>(base + std::uint32_t(Read(s.biasAt04) * stride));
}
inline Slot Copy(const Slot& s) noexcept
{
    Slot r{Read(s.baseAt00), Read(s.biasAt04), Read(s.controlAt08)};
    if (r.controlAt08) ++r.controlAt08->referenceCountAt04;
    return r;
}
inline std::int32_t Add(std::int32_t a, std::int32_t b) noexcept
{ return std::bit_cast<std::int32_t>(std::uint32_t(a) + std::uint32_t(b)); }
inline std::int32_t Sub(std::int32_t a, std::int32_t b) noexcept
{ return std::bit_cast<std::int32_t>(std::uint32_t(a) - std::uint32_t(b)); }
inline std::int32_t Count(const GuiControlSurfaceReferences& v) noexcept
{ return std::bit_cast<std::int32_t>(std::uint32_t(reinterpret_cast<std::uintptr_t>(Read(v.end)) - reinterpret_cast<std::uintptr_t>(Read(v.begin)))) / 12; }
inline Body& Dialog(Common* p) noexcept { return *reinterpret_cast<Body*>(p); }
inline const GuiDialogNativeVtable35& Methods(Body& b) noexcept { return *Read(b.common.vptrAt00); }
inline Parts& Part(Body& b) noexcept { return *Resolve<Parts>(b.partsAt74); }
struct LeafScope final
{
    ProgramMain00DialogNativeServices& services;
    ProgramMain00DialogNativeScope scope;
    LeafScope() noexcept : services(RequireProgramMain00DialogNativeServices()), scope(services) {}
};
// EH释放重读槽；正常尾可按ASM选定缓存control或重读control，二者不可合并。
struct RefFrame final
{
    Slot& slot;
    bool active = true;
    ~RefFrame() noexcept { if (active) ReleaseYtlReference(slot); }
    void Captured(Control* control, bool clear = true) noexcept;
    void Reload(bool clear = true) noexcept;
    void ReloadUnchecked(bool clear = true) noexcept;
    void CapturedReloadUnchecked(Control*, bool clear = true) noexcept;
};
struct VectorFrame final
{
    GuiControlSurfaceReferences& value;
    ~VectorFrame() noexcept { DestroyGuiControlSurfaceReferencesAt43EAA0(value); }
};
void ReleaseReload(Slot&, bool clear);
void* Root(std::uint32_t ensure, std::uint32_t read);
void* Archive(EnvironmentRootStorage*, std::uint32_t index) noexcept;
void* BorrowInlineEnvironment(std::uint32_t ensureSite, std::uint32_t readSite);
void* BorrowEnvironment(std::uint32_t site);
std::int32_t ReadNamed(void* root, void* archive, const char*, std::uint32_t resolve,
    std::uint32_t params, std::uint32_t read);
void WriteNamed(void* root, void* archive, const char*, std::int32_t,
    std::uint32_t resolve, std::uint32_t params, std::uint32_t write);
enum class SoundReceiverOrder {BeforeVolume,AfterVolume};
void PlaySe(SoundReceiverOrder,std::uint32_t nameAddress, std::uint32_t environmentSite,
    std::uint32_t registrySite, std::uint32_t ensureSite, std::uint32_t rootSite,
    std::uint32_t playSite);
void InvalidateScreen(Body&, bool inlineEnvironment, std::uint32_t heightSite, std::uint32_t widthSite,
    std::uint32_t heightRead = 0, std::uint32_t widthRead = 0);
void Blit(GraphicsSurfaceNativeBody* destination, GraphicsSurfaceNativeBody* source,
    std::int32_t x, std::int32_t y);
void Clear(GraphicsSurfaceNativeBody*);
RECT SurfaceRect(const Slot&);
void LoadFresh(Slot&, const char*);
void AppendStaticSurface(Body&, const Slot&, std::int32_t event, const RECT&, bool virtualAppend);

namespace Sample18
{
extern const GuiStaticDescriptorNative24h initial[3];
extern const GuiButtonDescriptorNative2Ch buttons[8];
extern const GuiStaticDescriptorNative24h statics[31];
extern const char* const params[18];
extern const char* const other[6];
extern const char* const resources[20];
extern const char* const markers[4];
extern const char* const digits[10];
extern const char* const time[2];
extern const POINT numberPoints[2];
extern const POINT datePoints[3];
extern const std::int32_t events[36];
extern const std::int32_t extraEvents[3];
extern const GuiDialogNavigationMapping navigation0[9], navigation1[9];
}
}
