#include <Runtime/ReadScriptFlashShakeNative.h>
#include <Runtime/GraphicsSurfaceReference.h>
#include <Runtime/GraphicsSurfaceRuntime.h>

#include <bit>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Flash = ReadScriptFlashNativeBody34h;
using Shake = ReadScriptShakeNativeBody28h;
using Base = ReadScriptEffectNativeBase8h;
using Slot = YtlReferenceSlotRuntime;
using Table = ReadScriptEffectNativeVtable;
thread_local ReadScriptFlashShakeNativeServices* currentServices = nullptr;
template<class T> T Read(const T& value) noexcept { return static_cast<const volatile T&>(value); }
std::int32_t Add(std::int32_t a, std::int32_t b) noexcept
{ return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(a) + static_cast<std::uint32_t>(b)); }
std::int32_t Multiply(std::int32_t a, std::int32_t b) noexcept
{ return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(a) * static_cast<std::uint32_t>(b)); }
ReadScriptFlashShakeNativeServices& Services()
{
    if (!currentServices) throw std::logic_error("Flash/Shake native services are not bound");
    return *currentServices;
}
template<class T> T* Resolve(const Slot& slot) noexcept
{
    const auto base = Read(slot.baseAt00);
    if (!base) return nullptr;
    const auto* control = static_cast<const EnvironmentObjectReferenceControl*>(Read(slot.controlAt08));
    const auto stride = control ? Read(control->strideAt0C) : 0u;
    return reinterpret_cast<T*>(base + Read(slot.biasAt04) * stride);
}
std::int32_t IntervalCount(const Flash& body) noexcept
{
    const auto* control = static_cast<const EnvironmentObjectReferenceControl*>(
        Read(body.intervalsAt20.RawSlot().controlAt08));
    return control ? std::bit_cast<std::int32_t>(Read(control->countAt10)) : 0;
}
void DrawSurface(const Slot& slot, std::int32_t x, std::int32_t y)
{
    // 4419A6先取固定正文handle；只有非零才进入4182F0/module门控，不能预查ModuleLoaded。
    const auto handle = Read(Resolve<GraphicsSurfaceNativeBody>(slot)->handleAt04);
    if (handle) (void)Services().drawing.SurfaceDrawRaw(handle, x, y, nullptr, nullptr, nullptr, 0);
}
struct Temporary final
{
    union { Slot value; };
    Temporary() noexcept {}
};
struct TemporaryUnwind final
{
    Slot& value;
    ~TemporaryUnwind() noexcept { ReleaseYtlReference(value); }
};
void ReleaseTemporaryNormally(Slot& value) noexcept
{
    // 49BE6E..94/49D5F0..616：正常尾重读control但不清临时位模式；state6/3为terminate。
    auto* control = value.controlAt08;
    if (control && --control->referenceCountAt04 == 0)
    {
        if (control->owningAt08) control->DestroyPayloadAtVslot00();
        control = value.controlAt08;
        if (control) control->DeleteControlAtVslot04(1);
    }
}
void CapturePrevious(Slot& target, IReadScriptFlashShakeNativeSource& source)
{
    Temporary temporary;
    Slot* incoming = source.CaptureScreenAt4ACDD0(temporary.value);
    try { CopyAssignGraphicsSurfaceReferenceAt465870(target, *incoming); }
    catch (...)
    {
        // 仅Capture返回之后的赋值失败才清临时槽；来源返回指针与临时存储不可混用。
        TemporaryUnwind unwind{temporary.value};
        throw;
    }
    ReleaseTemporaryNormally(temporary.value);
}

// 原PE 602120的十五项{count,POINT*}，200个POINT逐项读取；无随机表生成或rand调用。
constexpr POINT vertical[]{{0,1},{0,-1},{0,1},{0,-1},{0,1},{0,-1},{0,1},{0,-1}};
constexpr POINT horizontal[]{{1,0},{-1,0},{1,0},{-1,0},{1,0},{-1,0},{1,0},{-1,0}};
constexpr POINT corners[]{{1,1},{1,-1},{-1,1},{-1,-1},{1,1},{1,-1},{-1,1},{-1,-1}};
constexpr POINT down[]{{0,128},{0,0},{0,96},{0,0},{0,64},{0,0},{0,48},{0,0},{0,32},{0,0},{0,16},{0,0},{0,8},{0,0},{0,4},{0,0}};
constexpr POINT up[]{{0,-128},{0,0},{0,-96},{0,0},{0,-64},{0,0},{0,-48},{0,0},{0,-32},{0,0},{0,-16},{0,0},{0,-8},{0,0},{0,-4},{0,0}};
constexpr POINT right[]{{128,0},{0,0},{96,0},{0,0},{64,0},{0,0},{48,0},{0,0},{32,0},{0,0},{16,0},{0,0},{8,0},{0,0},{4,0},{0,0}};
constexpr POINT left[]{{-128,0},{0,0},{-96,0},{0,0},{-64,0},{0,0},{-48,0},{0,0},{-32,0},{0,0},{-16,0},{0,0},{-8,0},{0,0},{-4,0},{0,0}};
constexpr POINT downRight[]{{128,128},{0,0},{96,96},{0,0},{64,64},{0,0},{48,48},{0,0},{32,32},{0,0},{16,16},{0,0},{8,8},{0,0},{4,4},{0,0}};
constexpr POINT downLeft[]{{-128,128},{0,0},{-96,96},{0,0},{-64,64},{0,0},{-48,48},{0,0},{-32,32},{0,0},{-16,16},{0,0},{-8,8},{0,0},{-4,4},{0,0}};
constexpr POINT upRight[]{{128,-128},{0,0},{96,-96},{0,0},{64,-64},{0,0},{48,-48},{0,0},{32,-32},{0,0},{16,-16},{0,0},{8,-8},{0,0},{4,-4},{0,0}};
constexpr POINT upLeft[]{{-128,-128},{0,0},{-96,-96},{0,0},{-64,-64},{0,0},{-48,-48},{0,0},{-32,-32},{0,0},{-16,-16},{0,0},{-8,-8},{0,0},{-4,-4},{0,0}};
constexpr POINT diagonal[]{{1,1},{-1,-1},{1,1},{-1,-1},{1,1},{-1,-1},{1,1},{-1,-1}};
constexpr POINT antiDiagonal[]{{1,-1},{-1,1},{1,-1},{-1,1},{1,-1},{-1,1},{1,-1},{-1,1}};
constexpr POINT counterClockwise[]{{0,-1},{0,1},{-1,-1},{1,1},{-1,0},{1,0},{-1,1},{1,-1},{0,1},{0,-1},{1,1},{-1,-1},{1,0},{-1,0},{1,-1},{-1,1}};
constexpr POINT clockwise[]{{0,-1},{0,1},{1,-1},{-1,1},{1,0},{-1,0},{1,1},{-1,-1},{0,1},{0,-1},{-1,1},{1,-1},{-1,0},{1,0},{-1,-1},{1,1}};
struct Path final { std::int32_t count; const POINT* points; };
static_assert(sizeof(Path) == 8);
constexpr Path paths[]{{8,vertical},{8,horizontal},{8,corners},{16,down},{16,up},{16,right},{16,left},
    {16,downRight},{16,downLeft},{16,upRight},{16,upLeft},{8,diagonal},{8,antiDiagonal},
    {16,counterClockwise},{16,clockwise}};
const Path& PathAt(std::int32_t mode) noexcept
{
    return *reinterpret_cast<const Path*>(reinterpret_cast<std::uintptr_t>(paths) +
        static_cast<std::uint32_t>(mode) * 8u);
}
std::int32_t Remainder(std::int32_t counter, std::int32_t interval) noexcept
{
    // 原4BBE16的IDIV，包括除零/INT_MIN除-1的硬件故障，不改成C++异常或有符号UB。
    std::int32_t remainder;
    __asm
    {
        mov eax, counter
        cdq
        idiv interval
        mov remainder, edx
    }
    return remainder;
}

std::uint8_t __fastcall FlashTick(Base* body, void*)
{ return TickReadScriptFlashNativeAt4BB7F0(*reinterpret_cast<Flash*>(body)); }
std::uint8_t __fastcall ShakeTick(Base* body, void*)
{ return TickReadScriptShakeNativeAt4BBDF0(*reinterpret_cast<Shake*>(body)); }
void __fastcall Draw(Base*, void*) {}
std::uint8_t __fastcall Ready(Base* body, void*) { return Read(body->readyAt04); }
Base* __fastcall FlashDelete(Base* body, void*, std::uint32_t flags) noexcept
{ (void)DeleteReadScriptFlashNativeAt4A64B0(*reinterpret_cast<Flash*>(body), flags); return body; }
Base* __fastcall ShakeDelete(Base* body, void*, std::uint32_t flags) noexcept
{ (void)DeleteReadScriptShakeNativeAt4A65F0(*reinterpret_cast<Shake*>(body), flags); return body; }
// 保留原生x86虚槽：ECX接收者、未读取的EDX占位及栈参数按对应槽布局桥接。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const Table flashMethods{
    reinterpret_cast<decltype(Table::tickAt00)>(&FlashTick), reinterpret_cast<decltype(Table::drawAt04)>(&Draw),
    reinterpret_cast<decltype(Table::readyAt08)>(&Ready), reinterpret_cast<decltype(Table::deleteAt0C)>(&FlashDelete)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
// 保留原生x86虚槽：ECX接收者、未读取的EDX占位及栈参数按对应槽布局桥接。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const Table shakeMethods{
    reinterpret_cast<decltype(Table::tickAt00)>(&ShakeTick), reinterpret_cast<decltype(Table::drawAt04)>(&Draw),
    reinterpret_cast<decltype(Table::readyAt08)>(&Ready), reinterpret_cast<decltype(Table::deleteAt0C)>(&ShakeDelete)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
}

const Table& ReadScriptFlashNativeMethods() noexcept { return flashMethods; }
const Table& ReadScriptShakeNativeMethods() noexcept { return shakeMethods; }
ReadScriptFlashShakeNativeScope::ReadScriptFlashShakeNativeScope(ReadScriptFlashShakeNativeServices& services) noexcept
    : previous_(currentServices), surfaces_(services.surfaces) { currentServices = &services; }
ReadScriptFlashShakeNativeScope::~ReadScriptFlashShakeNativeScope() noexcept { currentServices = previous_; }

Flash* ConstructReadScriptFlashNativeAt49BCA0(Flash& body, const char* name, std::int32_t count,
    IReadScriptFlashShakeNativeSource& source, ReadScriptFlashShakeNativeServices& services)
{
    ReadScriptFlashShakeNativeScope scope(services);
    body.baseAt00.readyAt04 = 0;
    body.baseAt00.vptrAt00 = &flashMethods;
    body.flashAt08 = {};
    body.previousAt14 = {};
    ::new (&body.intervalsAt20) ByteArrayReferenceRuntime;
    try
    {
        if (!source.LoadFlashAt480000(body.flashAt08, name)) throw 1;
        body.intervalsAt20.AllocateAt412700(static_cast<std::uint32_t>(count) * 2u - 1u);
        for (std::int32_t i = 0; i < IntervalCount(body); i = Add(i, 1))
        {
            const auto& slot = body.intervalsAt20.RawSlot();
            const auto base = Read(slot.baseAt00);
            std::uintptr_t address = 0;
            if (base)
            {
                const auto* control = static_cast<const EnvironmentObjectReferenceControl*>(Read(slot.controlAt08));
                const auto stride = control ? Read(control->strideAt0C) : 0u;
                address = base + (Read(slot.biasAt04) + static_cast<std::uint32_t>(i)) * stride;
            }
            *reinterpret_cast<std::uint8_t*>(address) = (i & 1) ? 2 : 4;
        }
        body.indexAt30 = 0;
        body.remainingAt2C = 0;
        CapturePrevious(body.previousAt14, source);
        body.baseAt00.readyAt04 = 1;
    }
    catch (...) {} // 5EF1BC：局部catch只吞业务异常，保留成员及未提交字节。
    return &body;
}

Shake* ConstructReadScriptShakeNativeAt49D4E0(Shake& body, std::int32_t mode, std::int32_t scale,
    std::int32_t interval, IReadScriptFlashShakeNativeSource& source, ReadScriptFlashShakeNativeServices& services)
{
    ReadScriptFlashShakeNativeScope scope(services);
    body.baseAt00.readyAt04 = 0;
    body.baseAt00.vptrAt00 = &shakeMethods;
    body.previousAt08 = {};
    body.modeAt18 = mode;
    body.counterAt1C = 0;
    body.intervalAt14 = interval ? interval : 1;
    body.indexAt20 = 0;
    body.scaleAt24 = scale;
    try { CapturePrevious(body.previousAt08, source); }
    catch (...)
    {
        struct Unwind final
        {
            Shake& body;
            ~Unwind() noexcept
            {
                ReleaseYtlReference(body.previousAt08);
                body.baseAt00.vptrAt00 = &ReadScriptEffectBaseNativeMethods();
            }
        } unwind{body};
        throw;
    }
    body.baseAt00.readyAt04 = 1;
    return &body;
}

Flash* NewReadScriptFlashNative(const char* name, std::int32_t count,
    IReadScriptFlashShakeNativeSource& source, ReadScriptFlashShakeNativeServices& services)
{
    void* raw = ::operator new(0x34);
    if (!raw) return nullptr;
    // 原外层申请在构造catch之外；构造本身已吞业务异常，不能另加候选拥有者。
    return ConstructReadScriptFlashNativeAt49BCA0(*::new (raw) Flash, name, count, source, services);
}
Shake* NewReadScriptShakeNative(std::int32_t mode, std::int32_t scale, std::int32_t interval,
    IReadScriptFlashShakeNativeSource& source, ReadScriptFlashShakeNativeServices& services)
{
    void* raw = ::operator new(0x28);
    if (!raw) return nullptr;
    try { return ConstructReadScriptShakeNativeAt49D4E0(*::new (raw) Shake, mode, scale, interval, source, services); }
    catch (...)
    {
        struct Unwind final { void* raw; ~Unwind() noexcept { ::operator delete(raw, std::size_t{0x28}); } } unwind{raw};
        throw;
    }
}

std::uint8_t TickReadScriptFlashNativeAt4BB7F0(Flash& body)
{
    const auto remaining = Read(body.remainingAt2C);
    if (remaining) { body.remainingAt2C = Add(remaining, -1); return 1; }
    const auto count = IntervalCount(body);
    const auto index = Read(body.indexAt30);
    if (index >= count) return 0;
    DrawSurface((index & 1) ? body.previousAt14 : body.flashAt08, 0, 0);
    Services().screen.InvalidatedRectIfLoaded(nullptr);
    const auto& slot = body.intervalsAt20.RawSlot();
    std::uintptr_t address = 0;
    std::int32_t nextIndex;
    if (!Read(slot.baseAt00)) nextIndex = Read(body.indexAt30);
    else
    {
        const auto* control = static_cast<const EnvironmentObjectReferenceControl*>(Read(slot.controlAt08));
        const auto stride = control ? Read(control->strideAt0C) : 0u;
        nextIndex = Read(body.indexAt30);
        const auto offset = (Read(slot.biasAt04) + static_cast<std::uint32_t>(nextIndex)) * stride;
        address = Read(slot.baseAt00) + offset;
    }
    const auto interval = *reinterpret_cast<const std::uint8_t*>(address);
    body.indexAt30 = Add(nextIndex, 1);
    body.remainingAt2C = static_cast<std::int32_t>(interval) - 1;
    return 1;
}

std::uint8_t TickReadScriptShakeNativeAt4BBDF0(Shake& body)
{
    const auto mode = Read(body.modeAt18);
    const auto index = Read(body.indexAt20);
    const auto& path = PathAt(mode);
    if (index >= Read(path.count)) return 0;
    const auto counter = Read(body.counterAt1C);
    if (Remainder(counter, Read(body.intervalAt14)) == 0)
    {
        const auto* point = reinterpret_cast<const POINT*>(reinterpret_cast<std::uintptr_t>(Read(path.points)) +
            static_cast<std::uint32_t>(index) * 8u);
        const auto scale = Read(body.scaleAt24);
        const auto x = Multiply(scale, Read(point->x));
        const auto y = Multiply(scale, Read(point->y));
        Services().screen.ClearScreenIfLoaded(nullptr);
        DrawSurface(body.previousAt08, x, y);
        Services().screen.InvalidatedRectIfLoaded(nullptr);
        body.indexAt20 = Add(Read(body.indexAt20), 1);
        body.counterAt1C = Add(Read(body.counterAt1C), 1);
    }
    else body.counterAt1C = Add(counter, 1);
    return 1;
}

void DestroyReadScriptFlashNativeAt4A4080(Flash& body) noexcept
{
    const auto ready = Read(body.baseAt00.readyAt04);
    body.baseAt00.vptrAt00 = &flashMethods;
    if (ready)
    {
        DrawSurface(body.previousAt14, 0, 0);
        Services().screen.InvalidatedRectIfLoaded(nullptr);
    }
    // 5EF24C EHFlags=5：恢复绘制和三个成员退役均终止，不重试或展开后续槽。
    ReleaseYtlReference(body.intervalsAt20.RawSlot());
    ReleaseYtlReference(body.previousAt14);
    ReleaseYtlReference(body.flashAt08);
    body.baseAt00.vptrAt00 = &ReadScriptEffectBaseNativeMethods();
}
Flash* DeleteReadScriptFlashNativeAt4A64B0(Flash& body, std::uint32_t flags) noexcept
{
    DestroyReadScriptFlashNativeAt4A4080(body);
    if (flags & 1u) ::operator delete(&body, std::size_t{0x34});
    return &body;
}
Shake* DeleteReadScriptShakeNativeAt4A65F0(Shake& body, std::uint32_t flags) noexcept
{
    const auto ready = Read(body.baseAt00.readyAt04);
    body.baseAt00.vptrAt00 = &shakeMethods;
    if (ready)
    {
        DrawSurface(body.previousAt08, 0, 0);
        Services().screen.InvalidatedRectIfLoaded(nullptr);
    }
    // 5EF188 EHFlags=5；完成帧不ClearScreen、不清ready，flags=0也退役Ref及基类表。
    ReleaseYtlReference(body.previousAt08);
    body.baseAt00.vptrAt00 = &ReadScriptEffectBaseNativeMethods();
    if (flags & 1u) ::operator delete(&body, std::size_t{0x28});
    return &body;
}
}
