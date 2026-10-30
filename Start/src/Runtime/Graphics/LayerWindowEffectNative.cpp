#include <Runtime/LayerWindowEffectNative.h>
#include <Runtime/LayerListNative.h>

#include <bit>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Body = LayerWindowEffectNativeBody2Ch;
using Table = LayerWindowEffectNativeVtable6;
using Direction = LayerWindowEffectDirection8h;
using Directions = LayerWindowEffectDirections;
using Slot = YtlReferenceSlotRuntime;
constexpr std::uint32_t MaximumDirections = 0x1FFFFFFFu;
thread_local ILayerWindowEffectNativeSource* currentSource = nullptr;

ILayerWindowEffectNativeSource& Source()
{
    if (!currentSource) throw std::logic_error("native LayerWindow effect source is not bound");
    return *currentSource;
}
std::int32_t Increment(std::int32_t value) noexcept
{ return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(value) + 1u); }
std::int32_t Multiply(std::int32_t a, std::int32_t b) noexcept
{ return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(a) * static_cast<std::uint32_t>(b)); }
std::uint32_t Bytes(const void* begin, const void* end) noexcept
{ return static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(end) - reinterpret_cast<std::uintptr_t>(begin)); }
template<class T> T* Offset(T* pointer, std::uint32_t bytes) noexcept
{ return reinterpret_cast<T*>(reinterpret_cast<std::uintptr_t>(pointer) + bytes); }

void DeleteDirections(Direction* begin, Direction* capacity) noexcept
{
    // 4C9DDA/4C879A按字节差向下对齐，不把损坏指针差改成现代容器检查。
    std::uint32_t bytes = Bytes(begin, capacity) & ~7u;
    void* raw = begin;
    if (bytes >= 0x1000u)
    {
        const auto address = reinterpret_cast<std::uintptr_t>(begin);
        std::memcpy(&raw, reinterpret_cast<const void*>(address - 4u), sizeof(raw));
        bytes += 0x23u;
        if (address - reinterpret_cast<std::uintptr_t>(raw) - 4u > 0x1Fu)
            _invalid_parameter_noinfo_noreturn();
    }
    ::operator delete(raw, static_cast<std::size_t>(bytes));
}
Direction* AllocateDirections(std::uint32_t capacity)
{
    const std::uint32_t bytes = capacity > MaximumDirections ? 0xFFFFFFFFu : capacity * 8u;
    if (bytes < 0x1000u) return bytes ? static_cast<Direction*>(::operator new(bytes)) : nullptr;
    const auto enlarged = bytes + 0x23u;
    void* raw = ::operator new(enlarged > bytes ? enlarged : 0xFFFFFFFFu);
    if (!raw) _invalid_parameter_noinfo_noreturn();
    const auto aligned = (reinterpret_cast<std::uintptr_t>(raw) + 0x23u) & ~std::uintptr_t{31};
    std::memcpy(reinterpret_cast<void*>(aligned - 4u), &raw, sizeof(raw));
    return reinterpret_cast<Direction*>(aligned);
}
std::int32_t Remainder(std::int32_t value, std::int32_t divisor)
{
    // 4D4367/68：保留Win32除零及商溢出硬件异常，不用C++ % 的UB或domain_error。
    std::int32_t result;
    __asm
    {
        mov eax, value
        cdq
        idiv divisor
        mov result, edx
    }
    return result;
}
void __fastcall DrawAt4D4250(Body* body, void*, const RECT* clip, const LayerWindowEffectLayerReferences* layers)
{
    auto* cursor = layers->begin;
    while (cursor != layers->end)
    {
        auto* layer = ResolveEnvironmentObjectReference<LayerItemNativeBody14h>(*cursor);
        if (layer->enabledAt0C)
        {
            layer = ResolveEnvironmentObjectReference<LayerItemNativeBody14h>(*cursor);
            const auto* table = reinterpret_cast<const LayerWindowEffectPatternVtable4*>(layer->vptrAt00);
            table->drawAtOffsetAt0C(layer, body->offsetXAt10, body->offsetYAt14, clip);
        }
        cursor = Offset(cursor, 12);
    }
}
void __fastcall NullAt4D4240(Body*, void*, std::uintptr_t, std::uintptr_t, std::uintptr_t) {}
void __fastcall ResetAt4D4C90(Body* body, void*)
{
    body->frameAt04 = 0;
    body->offsetYAt14 = 0;
    body->offsetXAt10 = 0;
    body->directionAt18 = 0;
    body->activeAt28 = 0;
}
RECT* __fastcall EmptyRectAt4CD210(Body*, void*, RECT* output, std::uintptr_t)
{ *output = {}; return output; }
RECT* __fastcall MotionAt4D42D0(Body* body, void*, RECT* output, std::uint8_t stop)
{
    *output = {};
    if (stop)
    {
        if (body->activeAt28)
        {
            body->vptrAt00->resetAt0C(body);
            const auto height = Source().ReadProgramHeightAt4180C0();
            const auto width = Source().ReadProgramWidthAt4180C0();
            *output = {0, 0, width, height};
        }
        return output;
    }
    auto frame = body->frameAt04;
    if (!Remainder(frame, body->periodAt08))
    {
        const auto* direction = Offset(body->directionsAt1C.begin, static_cast<std::uint32_t>(body->directionAt18) * 8u);
        const auto x = direction->xAt00;
        const auto y = direction->yAt04;
        body->offsetXAt10 = Multiply(x, body->amplitudeAt0C);
        body->offsetYAt14 = Multiply(y, body->amplitudeAt0C);
        Source().ClearScreenIfLoadedAt4182F0();
        const auto height = Source().ReadProgramHeightAt4180C0();
        const auto width = Source().ReadProgramWidthAt4180C0();
        body->directionAt18 = Increment(body->directionAt18);
        const auto count = LayerWindowEffectDirectionCount(body->directionsAt1C);
        *output = {0, 0, width, height};
        if (body->directionAt18 >= count) body->directionAt18 = 0;
        frame = body->frameAt04;
        body->activeAt28 = 1;
    }
    body->frameAt04 = Increment(frame);
    return output;
}
Body* __fastcall DeleteRoll(Body* body, void*, std::uint32_t flags) noexcept
{ return DeleteLayerWindowRollNativeAt4CAD40(body, flags); }
Body* __fastcall DeleteToss(Body* body, void*, std::uint32_t flags) noexcept
{ return DeleteLayerWindowTossNativeAt4CADA0(body, flags); }
Body* __fastcall DeleteBaseAt4CAEE0(Body* body, void*, std::uint32_t flags) noexcept
{
    body->vptrAt00 = &LayerWindowEffectBaseMethods();
    if (flags & 1) ::operator delete(body, std::size_t{8});
    return body;
}
[[noreturn]] void Pure() { _purecall(); std::terminate(); }
void __fastcall PureDraw(Body*, void*, const RECT*, const LayerWindowEffectLayerReferences*) { Pure(); }
void __fastcall PureNull(Body*, void*, std::uintptr_t, std::uintptr_t, std::uintptr_t) { Pure(); }
RECT* __fastcall PureMotion(Body*, void*, RECT*, std::uint8_t) { Pure(); }
void __fastcall PureReset(Body*, void*) { Pure(); }
RECT* __fastcall PureRectangle(Body*, void*, RECT*, std::uintptr_t) { Pure(); }

#define ENTRY(slot, method) reinterpret_cast<decltype(Table::slot)>(&method)
// 保留原生x86虚槽：ECX接收者、未读取的EDX占位及栈参数按对应槽布局桥接。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const Table baseTable{ENTRY(drawAt00, PureDraw), ENTRY(nullAt04, PureNull), ENTRY(motionAt08, PureMotion),
    ENTRY(resetAt0C, PureReset), ENTRY(emptyRectAt10, PureRectangle), ENTRY(deletingDestructorAt14, DeleteBaseAt4CAEE0)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
// 保留原生x86虚槽：ECX接收者、未读取的EDX占位及栈参数按对应槽布局桥接。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const Table rollTable{ENTRY(drawAt00, DrawAt4D4250), ENTRY(nullAt04, NullAt4D4240), ENTRY(motionAt08, MotionAt4D42D0),
    ENTRY(resetAt0C, ResetAt4D4C90), ENTRY(emptyRectAt10, EmptyRectAt4CD210), ENTRY(deletingDestructorAt14, DeleteRoll)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
// 保留原生x86虚槽：ECX接收者、未读取的EDX占位及栈参数按对应槽布局桥接。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const Table tossTable{ENTRY(drawAt00, DrawAt4D4250), ENTRY(nullAt04, NullAt4D4240), ENTRY(motionAt08, MotionAt4D42D0),
    ENTRY(resetAt0C, ResetAt4D4C90), ENTRY(emptyRectAt10, EmptyRectAt4CD210), ENTRY(deletingDestructorAt14, DeleteToss)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
#undef ENTRY

struct MemberUnwind final
{
    Body* body;
    bool active = true;
    ~MemberUnwind() noexcept { if (active) DestroyLayerWindowEffectNativeAt4C9DD0(body); }
};
struct RawUnwind final
{
    void* raw;
    bool active = true;
    ~RawUnwind() noexcept { if (active) ::operator delete(raw, std::size_t{0x2C}); }
};
Body* Construct(Body* body, bool roll)
{
    body->offsetXAt10 = 0; body->offsetYAt14 = 0;
    body->directionsAt1C = {};
    body->periodAt08 = 2; body->amplitudeAt0C = 10;
    body->frameAt04 = 0; body->offsetYAt14 = 0; body->offsetXAt10 = 0;
    body->directionAt18 = 0; body->activeAt28 = 0;
    MemberUnwind members{body};
    body->vptrAt00 = roll ? &rollTable : &tossTable;
    const Direction first = roll ? Direction{1, 0} : Direction{0, 1};
    const Direction second = roll ? Direction{-1, 0} : Direction{0, -1};
    AppendLayerWindowEffectDirection(body->directionsAt1C, first);
    AppendLayerWindowEffectDirection(body->directionsAt1C, second);
    members.active = false;
    return body;
}
Body* Create(bool roll)
{
    void* raw = ::operator new(0x2C);
    RawUnwind cleanup{raw};
    Body* body = raw ? Construct(::new (raw) Body, roll) : nullptr;
    cleanup.active = false;
    return body;
}
template<bool Roll>
class EffectControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit EffectControl(Body* body) noexcept : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        auto* body = static_cast<Body*>(payloadAt14);
        if (body) body->vptrAt00->deletingDestructorAt14(body, 1);
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override { RetireEnvironmentObjectControl(this, flags); }
};
static_assert(sizeof(EffectControl<true>) == 0x18 && sizeof(EffectControl<false>) == 0x18);
template<bool Roll> void Install(Slot& target, Body* body)
{
    ReleaseYtlReference(target);
    // 4D69D8/4D6B3C之后ECX固定为零，不能添加第二次成员control检查。
    if (!body) return;
    target.biasAt04 = 0;
    target.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    void* raw = ::operator new(0x18);
    auto* control = raw ? ::new (raw) EffectControl<Roll>(body) : nullptr;
    target.controlAt08 = control;
    control->referenceCountAt04 = 1;
    target.controlAt08->owningAt08 = 1;
    static_cast<EnvironmentObjectReferenceControl*>(target.controlAt08)->strideAt0C = 0x2C;
    static_cast<EnvironmentObjectReferenceControl*>(target.controlAt08)->countAt10 = 1;
}
}

LayerWindowEffectNativeScope::LayerWindowEffectNativeScope(ILayerWindowEffectNativeSource& source) noexcept
    : previous_(currentSource) { currentSource = &source; }
LayerWindowEffectNativeScope::~LayerWindowEffectNativeScope() noexcept { currentSource = previous_; }
const Table& LayerWindowEffectBaseMethods() noexcept { return baseTable; }
const Table& LayerWindowRollNativeMethods() noexcept { return rollTable; }
const Table& LayerWindowTossNativeMethods() noexcept { return tossTable; }

std::int32_t LayerWindowEffectDirectionCount(const Directions& directions) noexcept
{ return std::bit_cast<std::int32_t>(Bytes(directions.begin, directions.end)) >> 3; }
Direction* GrowLayerWindowEffectDirectionsAt4C8690(Directions& directions, Direction* position, const Direction& source)
{
    const auto index = static_cast<std::uint32_t>(std::bit_cast<std::int32_t>(Bytes(directions.begin, position)) >> 3);
    const auto count = static_cast<std::uint32_t>(LayerWindowEffectDirectionCount(directions));
    if (count == MaximumDirections) throw std::length_error("vector<T> too long");
    const auto required = count + 1u;
    const auto capacity = static_cast<std::uint32_t>(std::bit_cast<std::int32_t>(Bytes(directions.begin, directions.capacity)) >> 3);
    const auto grown = capacity > MaximumDirections - (capacity >> 1) ? required : capacity + (capacity >> 1);
    const auto nextCapacity = grown < required ? required : grown;
    auto* next = AllocateDirections(nextCapacity);
    auto* inserted = Offset(next, index * 8u);
    // 4C8739在分配回调之后才读source；先插入再搬旧段，最后释放旧区并发布三指针。
    inserted->xAt00 = source.xAt00;
    inserted->yAt04 = source.yAt04;
    if (position == directions.end)
        std::memmove(next, directions.begin, Bytes(directions.begin, directions.end));
    else
    {
        std::memmove(next, directions.begin, Bytes(directions.begin, position));
        std::memmove(Offset(inserted, 8), position, Bytes(position, directions.end));
    }
    if (directions.begin) DeleteDirections(directions.begin, directions.capacity);
    directions.begin = next;
    directions.end = Offset(next, required * 8u);
    directions.capacity = Offset(next, nextCapacity * 8u);
    return Offset(directions.begin, index * 8u);
}
void AppendLayerWindowEffectDirection(Directions& directions, const Direction& source)
{
    auto* position = directions.end;
    if (position == directions.capacity) { GrowLayerWindowEffectDirectionsAt4C8690(directions, position, source); return; }
    position->xAt00 = source.xAt00;
    position->yAt04 = source.yAt04;
    directions.end = Offset(directions.end, 8);
}
Body* ConstructLayerWindowRollNativeAt4D68FE(Body* body) { return Construct(body, true); }
Body* ConstructLayerWindowTossNativeAt4D6A64(Body* body) { return Construct(body, false); }
Body* CreateLayerWindowRollNativeAt4D68E0() { return Create(true); }
Body* CreateLayerWindowTossNativeAt4D6A46() { return Create(false); }
void DestroyLayerWindowEffectNativeAt4C9DD0(Body* body) noexcept
{
    if (body->directionsAt1C.begin)
    {
        DeleteDirections(body->directionsAt1C.begin, body->directionsAt1C.capacity);
        body->directionsAt1C = {};
    }
    body->vptrAt00 = &baseTable;
}
Body* DeleteLayerWindowRollNativeAt4CAD40(Body* body, std::uint32_t flags) noexcept
{
    DestroyLayerWindowEffectNativeAt4C9DD0(body);
    if (flags & 1) ::operator delete(body, std::size_t{0x2C});
    return body;
}
Body* DeleteLayerWindowTossNativeAt4CADA0(Body* body, std::uint32_t flags) noexcept
{
    DestroyLayerWindowEffectNativeAt4C9DD0(body);
    if (flags & 1) ::operator delete(body, std::size_t{0x2C});
    return body;
}
void InstallLayerWindowRollReferenceAt4D69A5(Slot& target, Body* body) { Install<true>(target, body); }
void InstallLayerWindowTossReferenceAt4D6B09(Slot& target, Body* body) { Install<false>(target, body); }
Body* ResolveLayerWindowEffectReference(const Slot& reference) noexcept
{ return ResolveEnvironmentObjectReference<Body>(reference); }
void ReleaseLayerWindowEffectReferenceAt4607A0(Slot& reference) noexcept { ReleaseYtlReference(reference); }
void ReplaceLayerWindowEffectAt4D6860(Slot& target, std::int32_t selector, ILayerWindowEffectSelectionSource& source)
{
    ReleaseYtlReference(target);
    if (selector == 1) InstallLayerWindowRollReferenceAt4D69A5(target, CreateLayerWindowRollNativeAt4D68E0());
    else if (selector == 0) InstallLayerWindowTossReferenceAt4D6B09(target, CreateLayerWindowTossNativeAt4D6A46());
    else if (selector == -1 && source.ReadSelectorAt3C() >= 0)
    {
        const auto height = source.ReadProgramHeightAt4180C0();
        const auto width = source.ReadProgramWidthAt4180C0();
        auto* list = ResolveLayerListReference(source.CaptureLayerListAt24());
        const RECT rectangle{0, 0, width, height};
        auto* dirty = list->vptrAt00->dirtyAt14(list);
        dirty->vptrAt00->addRectAt08(dirty, &rectangle);
    }
    source.WriteSelectorAt3C(selector);
}
}
