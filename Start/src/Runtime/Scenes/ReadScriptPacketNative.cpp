#include <Runtime/ReadScriptPacketNative.h>
#include <Runtime/ReadScriptPacketEffects.h>
#include <Runtime/EnvironmentRootRuntime.h>
#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/SaveDataEnvironmentArchive.h>

#include <bit>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Body = ReadScriptPacketNativeBody3Ch;
using Base = ReadScriptEffectNativeBase8h;
using Table = ReadScriptEffectNativeVtable;
using Slot = YtlReferenceSlotRuntime;
thread_local ReadScriptPacketNativeServices* currentServices = nullptr;
template<class T> T Read(const T& value) noexcept { return static_cast<const volatile T&>(value); }

template<class T> T* Resolve(const Slot& slot, std::uintptr_t base) noexcept
{
    if (!base) return nullptr;
    auto* control = static_cast<EnvironmentObjectReferenceControl*>(Read(slot.controlAt08));
    const auto stride = control ? Read(control->strideAt0C) : 0u;
    return reinterpret_cast<T*>(base + Read(slot.biasAt04) * stride);
}
template<class T> T* Resolve(const Slot& slot) noexcept { return Resolve<T>(slot, Read(slot.baseAt00)); }
template<class T> T* ResolveArchive(const Slot& slot) noexcept
{
    const auto base = Read(slot.baseAt00);
    if (!base) return nullptr;
    auto* control = static_cast<EnvironmentObjectReferenceControl*>(Read(slot.controlAt08));
    // 49BBE6/49BC2D等内联Environment路径先读bias，再读control+0C。
    const auto bias = Read(slot.biasAt04);
    const auto stride = control ? Read(control->strideAt0C) : 0u;
    return reinterpret_cast<T*>(base + bias * stride);
}

void Initialize(Body& body, const Table& table, std::int32_t opcode) noexcept
{
    body.baseAt00.readyAt04 = 0;
    body.baseAt00.vptrAt00 = &table;
    ::new (&body.packetAt08) TransitOperation;
    body.packetAt08.opcode = opcode;
}

std::int32_t ArchiveCount(EnvironmentRootStorage& root, Slot*& begin) noexcept
{
    // 四构造均先end后begin；x86差值按signed /12，再由调用点决定无符号门控。
    const auto end = reinterpret_cast<std::uintptr_t>(Read(root.archivesAt30.end));
    begin = Read(root.archivesAt30.begin);
    return std::bit_cast<std::int32_t>(end - reinterpret_cast<std::uintptr_t>(begin)) / 12;
}
void SetReady(Body& body, IReadScriptPacketEnvironmentSource& source)
{
    Slot* begin;
    const auto count = ArchiveCount(*source.CaptureEnvironmentAt4180C0(), begin);
    auto* const save = static_cast<std::uint32_t>(count) > 1u ?
        ResolveArchive<SaveDataEnvironmentArchive>(begin[1]) : nullptr;
    // 49BC01/49C051/49D757/49D96F不可合并到前一取根。497900不读ECX，
    // 但原slot0的base/control/stride/bias读取仍发生；volatile保留这些读取。
    if (ArchiveCount(*source.CaptureEnvironmentAt4180C0(), begin) != 0)
        static_cast<void>(ResolveArchive<void>(begin[0]));
    const auto index = Read(save->valueAt70);
    // 原PE 6018A0；497903 cmp/sbb/and为unsigned，不把负数当合法索引。
    constexpr std::int32_t steps[]{4, 8, 16, 32};
    body.stepAt38 = steps[index < 4u ? index : 0u];
    body.baseAt00.readyAt04 = 1;
}

ReadScriptPacketNativeServices& Services()
{
    if (!currentServices) throw std::logic_error("ReadScript packet native services are not bound");
    return *currentServices;
}
void Render(Body& body)
{
    auto& services = Services();
    // 4BB7BC/4BB934/4BBF0C/4BBFA3直接push this+8；不复制包，不缓存根或CTransit。
    const auto view = services.draw.CaptureDrawTransitionAt413BE0();
    static_cast<void>(view.RenderTransitionAt4C39E0(body.packetAt08));
}
void Invalidate() { Services().screen.InvalidatedRectIfLoaded(nullptr); }
void AddStep(Body& body) noexcept
{
    const auto step = Read(body.stepAt38);
    body.packetAt08.current = std::bit_cast<std::int32_t>(
        static_cast<std::uint32_t>(Read(body.packetAt08.current)) + static_cast<std::uint32_t>(step));
}
std::uint8_t Ascending(Body& body)
{
    auto& p = body.packetAt08;
    // 4BB754/4BBEA4：无Ready检查；整数ADD后有符号比较，不涉及SSE/MXCSR。
    if (Read(p.opcode) == -1 || Read(p.current) >= Read(p.target)) return 0;
    Render(body);
    AddStep(body);
    const auto target = Read(p.target);
    if (Read(p.current) >= target) p.current = target;
    Invalidate();
    return 1; // 原丢弃Render的AL，成功执行帧固定返回AL=1。
}
void Destroy(Body& body, const Table& table, std::int32_t finalValue, bool useTarget) noexcept
{
    const auto ready = Read(body.baseAt00.readyAt04);
    body.baseAt00.vptrAt00 = &table;
    if (ready)
    {
        body.packetAt08.current = useTarget ? Read(body.packetAt08.target) : finalValue;
        Render(body);
        Invalidate();
    }
    // 5EED60/5EF2D0/5EEE10/5EEEC0的EHFlags均为5；失败终止，不回写基类表。
    body.baseAt00.vptrAt00 = &ReadScriptEffectBaseNativeMethods();
}
template<void (*DestroyBody)(Body&) noexcept>
Body* Delete(Body& body, std::uint32_t flags) noexcept
{
    DestroyBody(body);
    if (flags & 1u) ::operator delete(&body, 0x3C);
    return &body;
}
std::uint8_t __fastcall Ready(Base* body, void*) { return Read(body->readyAt04); }
template<int Kind> void __fastcall Draw(Base*, void*) {} // 4B5520/4B5540/4B5840/4B5850仅ret。
template<std::uint8_t (*TickBody)(Body&)>
std::uint8_t __fastcall Tick(Base* body, void*) { return TickBody(*reinterpret_cast<Body*>(body)); }
template<Body* (*DeleteBody)(Body&, std::uint32_t) noexcept>
Base* __fastcall DeleteSlot(Base* body, void*, std::uint32_t flags) noexcept
{ return reinterpret_cast<Base*>(DeleteBody(*reinterpret_cast<Body*>(body), flags)); }

// 原PE四表：5B4D14、5B4D8C、5B4D28、5B4D3C，Ready都是4B0730。
const Table blend{
    reinterpret_cast<decltype(Table::tickAt00)>(&Tick<TickReadScriptBlendNativeAt4BB750>),
    reinterpret_cast<decltype(Table::drawAt04)>(&Draw<0>),
    reinterpret_cast<decltype(Table::readyAt08)>(&Ready),
    reinterpret_cast<decltype(Table::deleteAt0C)>(&DeleteSlot<DeleteReadScriptBlendNativeAt4A63D0>)};
const Table mosaic{
    reinterpret_cast<decltype(Table::tickAt00)>(&Tick<TickReadScriptMosaicNativeAt4BB8D0>),
    reinterpret_cast<decltype(Table::drawAt04)>(&Draw<1>),
    reinterpret_cast<decltype(Table::readyAt08)>(&Ready),
    reinterpret_cast<decltype(Table::deleteAt0C)>(&DeleteSlot<DeleteReadScriptMosaicNativeAt4A64E0>)};
const Table transit{
    reinterpret_cast<decltype(Table::tickAt00)>(&Tick<TickReadScriptTransitNativeAt4BBEA0>),
    reinterpret_cast<decltype(Table::drawAt04)>(&Draw<2>),
    reinterpret_cast<decltype(Table::readyAt08)>(&Ready),
    reinterpret_cast<decltype(Table::deleteAt0C)>(&DeleteSlot<DeleteReadScriptTransitNativeAt4A66D0>)};
const Table universal{
    reinterpret_cast<decltype(Table::tickAt00)>(&Tick<TickReadScriptUniversalNativeAt4BBF40>),
    reinterpret_cast<decltype(Table::drawAt04)>(&Draw<3>),
    reinterpret_cast<decltype(Table::readyAt08)>(&Ready),
    reinterpret_cast<decltype(Table::deleteAt0C)>(&DeleteSlot<DeleteReadScriptUniversalNativeAt4A67B0>)};

template<auto Construct, class... Args> Body* New(Args&&... args)
{
    void* storage = ::operator new(0x3C);
    auto* body = storage ? ::new (storage) Body : nullptr;
    if (body)
    {
        try { Construct(*body, args...); }
        catch (...)
        {
            // 工厂构造资格未完成时只交还裸3Ch，不调用带完成绘制的删除槽。
            ::operator delete(body, 0x3C);
            throw;
        }
    }
    return body;
}
}

EnvironmentRootStorage* RootReadScriptPacketEnvironmentSource::CaptureEnvironmentAt4180C0()
{ return owner_.CaptureAt4180C0(); }
ReadScriptPacketNativeScope::ReadScriptPacketNativeScope(ReadScriptPacketNativeServices& services) noexcept
    : previous_(currentServices) { currentServices = &services; }
ReadScriptPacketNativeScope::~ReadScriptPacketNativeScope() noexcept { currentServices = previous_; }
const Table& ReadScriptBlendNativeMethods() noexcept { return blend; }
const Table& ReadScriptMosaicNativeMethods() noexcept { return mosaic; }
const Table& ReadScriptTransitNativeMethods() noexcept { return transit; }
const Table& ReadScriptUniversalNativeMethods() noexcept { return universal; }

Body* ConstructReadScriptBlendNativeAt49BAC0(Body& body, const Slot& source, const Slot& previous,
    IReadScriptPacketEnvironmentSource& environment)
{
    Initialize(body, blend, 0);
    const auto base = Read(source.baseAt00); // 49BB5F早于state1，不能挪入try。
    // PE 5EED10：tryLow=tryHigh=1，catchHigh=2；state0仅基类展开，无完成析构。
    try
    {
        body.packetAt08.source = Resolve<GraphicsSurfaceNativeBody>(source, base);
        body.packetAt08.previous = Read(Resolve<GraphicsSurfaceNativeBody>(previous)->handleAt04);
        SetReady(body, environment);
    }
    catch (...) { /* 49BC77只返回this，保留已写前缀和未写step。 */ }
    return &body;
}
Body* ConstructReadScriptMosaicNativeAt49BEE0(Body& body, const Slot& source, const Slot& previous,
    const Slot& work, IReadScriptPacketEnvironmentSource& environment)
{
    Initialize(body, mosaic, 2);
    const auto base = Read(source.baseAt00); // 49BF7F在state1之前；5EF280的try=[1,1]。
    try
    {
        body.packetAt08.source = Resolve<GraphicsSurfaceNativeBody>(source, base);
        body.packetAt08.previous = Read(Resolve<GraphicsSurfaceNativeBody>(previous)->handleAt04);
        body.packetAt08.destination = Read(Resolve<GraphicsSurfaceNativeBody>(work)->handleAt04);
        body.packetAt08.current = 0;
        SetReady(body, environment);
    }
    catch (...) { /* 49C0C7不回滚。 */ }
    return &body;
}
Body* ConstructReadScriptTransitNativeAt49D640(Body& body, const Slot& source, std::int32_t opcode,
    IReadScriptPacketEnvironmentSource& environment)
{
    Initialize(body, transit, opcode);
    // 49D6E9首个base读取已在state1内，与另外三类不同；5EEDC0的try=[1,1]。
    try
    {
        body.packetAt08.source = Resolve<GraphicsSurfaceNativeBody>(source);
        SetReady(body, environment);
    }
    catch (...) { /* 49D7CD不清包、不初始化step。 */ }
    return &body;
}
Body* ConstructReadScriptUniversalNativeAt49D7F0(Body& body, const Slot& source, const Slot& previous,
    const Slot& mask, IReadScriptPacketEnvironmentSource& environment)
{
    Initialize(body, universal, 1);
    const auto base = Read(source.baseAt00); // 49D88F在state1之前；5EEE70的try=[1,1]。
    try
    {
        body.packetAt08.source = Resolve<GraphicsSurfaceNativeBody>(source, base);
        body.packetAt08.previous = Read(Resolve<GraphicsSurfaceNativeBody>(previous)->handleAt04);
        body.packetAt08.mask = Read(Resolve<GraphicsSurfaceNativeBody>(mask)->handleAt04);
        body.packetAt08.current = 255;
        body.packetAt08.range = 128;
        body.packetAt08.target = -128;
        SetReady(body, environment);
    }
    catch (...) { /* 49D9E5不回滚。 */ }
    return &body;
}

std::uint8_t TickReadScriptBlendNativeAt4BB750(Body& body) { return Ascending(body); }
std::uint8_t TickReadScriptTransitNativeAt4BBEA0(Body& body) { return Ascending(body); }
std::uint8_t TickReadScriptMosaicNativeAt4BB8D0(Body& body)
{
    if (Read(body.packetAt08.current) >= 512) return 0;
    Render(body);
    AddStep(body); // 不看opcode、不夹紧。
    Invalidate();
    return 1;
}
std::uint8_t TickReadScriptUniversalNativeAt4BBF40(Body& body)
{
    if (Read(body.packetAt08.current) < Read(body.packetAt08.target)) return 0;
    Render(body);
    const auto step = Read(body.stepAt38);
    body.packetAt08.current = std::bit_cast<std::int32_t>(
        static_cast<std::uint32_t>(Read(body.packetAt08.current)) - static_cast<std::uint32_t>(step));
    Invalidate();
    return 1;
}
void DestroyReadScriptBlendNativeAt4A3FC0(Body& body) noexcept { Destroy(body, blend, 255, false); }
void DestroyReadScriptMosaicNativeAt4A41C0(Body& body) noexcept { Destroy(body, mosaic, 512, false); }
void DestroyReadScriptTransitNativeAt4A4590(Body& body) noexcept { Destroy(body, transit, 255, false); }
void DestroyReadScriptUniversalNativeAt4A4650(Body& body) noexcept { Destroy(body, universal, 0, true); }
Body* DeleteReadScriptBlendNativeAt4A63D0(Body& body, std::uint32_t flags) noexcept
{ return Delete<DestroyReadScriptBlendNativeAt4A3FC0>(body, flags); }
Body* DeleteReadScriptMosaicNativeAt4A64E0(Body& body, std::uint32_t flags) noexcept
{ return Delete<DestroyReadScriptMosaicNativeAt4A41C0>(body, flags); }
Body* DeleteReadScriptTransitNativeAt4A66D0(Body& body, std::uint32_t flags) noexcept
{ return Delete<DestroyReadScriptTransitNativeAt4A4590>(body, flags); }
Body* DeleteReadScriptUniversalNativeAt4A67B0(Body& body, std::uint32_t flags) noexcept
{ return Delete<DestroyReadScriptUniversalNativeAt4A4650>(body, flags); }

Body* NewReadScriptBlendNative(const Slot& source, const Slot& previous, IReadScriptPacketEnvironmentSource& environment)
{ return New<ConstructReadScriptBlendNativeAt49BAC0>(source, previous, environment); }
Body* NewReadScriptMosaicNative(const Slot& source, const Slot& previous, const Slot& work,
    IReadScriptPacketEnvironmentSource& environment)
{ return New<ConstructReadScriptMosaicNativeAt49BEE0>(source, previous, work, environment); }
Body* NewReadScriptTransitNative(const Slot& source, std::int32_t opcode, IReadScriptPacketEnvironmentSource& environment)
{ return New<ConstructReadScriptTransitNativeAt49D640>(source, opcode, environment); }
Body* NewReadScriptUniversalNative(const Slot& source, const Slot& previous, const Slot& mask,
    IReadScriptPacketEnvironmentSource& environment)
{ return New<ConstructReadScriptUniversalNativeAt49D7F0>(source, previous, mask, environment); }
}
