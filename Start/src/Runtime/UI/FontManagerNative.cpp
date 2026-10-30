#include <Runtime/FontManagerNative.h>
#include <Runtime/FontManagerRuntime.h>

#include <algorithm>
#include <bit>
#include <cstdlib>
#include <cstring>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
thread_local IFontResourcePlatformApi* activePlatform = nullptr;

__declspec(noinline) IFontResourcePlatformApi& Platform()
{
    if (!activePlatform) throw std::logic_error("Native FontManager requires its external FontManagerNativeScope");
    return *activePlatform;
}

template<class Slot>
std::int32_t Count(const Slot* begin, const Slot* end) noexcept
{
    const auto bytes = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(end) -
        reinterpret_cast<std::uintptr_t>(begin));
    return std::bit_cast<std::int32_t>(bytes) / 12;
}

template<class Slot>
void DeleteStorage(Slot* begin, Slot* capacity) noexcept
{
    // 461C55/4621C9：回调后当前capacity/begin，先有符号除12，再保留低32位乘回。
    auto bytes = static_cast<std::uint32_t>(Count(begin, capacity)) * 12u;
    void* raw = begin;
    if (bytes >= 0x1000u)
    {
        const auto address = reinterpret_cast<std::uintptr_t>(begin);
        std::memcpy(&raw, reinterpret_cast<const void*>(address - 4u), sizeof(raw));
        bytes += 0x23u;
        if (address - reinterpret_cast<std::uintptr_t>(raw) - 4u > 0x1Fu)
            _invalid_parameter_noinfo_noreturn();
    }
    ::operator delete(raw, bytes);
}

void Destroy(FontManagerNativeBody1Ch& body, bool* modernSecondaryDestroyed) noexcept
{
    body.vptrAt00 = &FontManagerNativeMethods();
    auto* current = body.resourcesAt04.begin;
    auto* end = body.resourcesAt04.end;
    if (Count(current, end) != 0)
    {
        while (current != end)
        {
            std::uintptr_t address = 0;
            if (current->baseAt00)
            {
                const auto stride = current->controlAt08 ? current->controlAt08->strideAt0C : 0u;
                address = current->baseAt00 + current->biasAt04 * stride;
            }
            const char* name;
            // 463109只读解析正文+04。非法空正文仍不是正常可跳过项。
            std::memcpy(&name, reinterpret_cast<const void*>(address + 4u), sizeof(name));
            static_cast<void>(Platform().RemoveFontResource(name, 0x10, nullptr));
            ++current; // iterator不因回调更换begin而重新定位。
            end = body.resourcesAt04.end;
        }
        // 463120当前begin，end取最后一次Remove后的读值；释放中不跟随end变化。
        ReleaseFontResourceRangeAt48FA30(body.resourcesAt04.begin, end);
        body.resourcesAt04.end = body.resourcesAt04.begin;
    }
    DestroyFontSecondaryStorageAt462140(body.secondaryAt10);
    if (modernSecondaryDestroyed) *modernSecondaryDestroyed = true;
    DestroyFontResourceStorageAt461C20(body.resourcesAt04);
}

FontManagerNativeBody1Ch* __fastcall DeleteEntry(FontManagerNativeBody1Ch* body, void*, std::uint32_t flags)
{ return DeleteFontManagerAt468AE0(*body, flags); }
// 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
// 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const FontManagerNativeVtable methods{
    reinterpret_cast<decltype(FontManagerNativeVtable::deleteAt00)>(&DeleteEntry)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif

class FontManagerControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit FontManagerControl(FontManagerNativeBody1Ch* body) noexcept
        : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        // 492C40判payload null，随后读取正文当前表+00，不固定调用468AE0。
        auto* const body = static_cast<FontManagerNativeBody1Ch*>(payloadAt14);
        if (body) body->vptrAt00->deleteAt00(body, 1);
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        // 467A40先退5ADD18；派生delete18h，退役后再调+04则412460 delete14h。
        RetireEnvironmentObjectControl(this, flags);
    }
};
static_assert(sizeof(FontManagerControl) == 0x18);

template<class Slot>
Slot* AllocateModernStorage(std::uint32_t capacity)
{
    const auto bytes = capacity * 12u;
    if (bytes < 0x1000u) return static_cast<Slot*>(::operator new(bytes));
    if (bytes > 0xFFFFFFFFu - 0x23u) throw std::bad_array_new_length();
    void* raw = ::operator new(bytes + 0x23u);
    if (!raw) throw std::bad_alloc();
    const auto aligned = (reinterpret_cast<std::uintptr_t>(raw) + 0x23u) & ~std::uintptr_t{31};
    std::memcpy(reinterpret_cast<void*>(aligned - 4u), &raw, sizeof(raw));
    return reinterpret_cast<Slot*>(aligned);
}

template<class Slot>
void AdoptModern(FontReferenceStorage<Slot>& storage, Slot reference)
{
    if (storage.end != storage.capacity)
    {
        *storage.end = reference;
        ++storage.end;
        return;
    }
    constexpr std::uint32_t maximum = 0x15555555u;
    const auto size = static_cast<std::uint32_t>(Count(storage.begin, storage.end));
    const auto capacity = static_cast<std::uint32_t>(Count(storage.begin, storage.capacity));
    if (size >= maximum) throw std::length_error("font reference vector too long");
    const auto grown = capacity > maximum - capacity / 2u ? size + 1u : capacity + capacity / 2u;
    const auto nextCapacity = (std::max)(size + 1u, grown);
    auto* next = AllocateModernStorage<Slot>(nextCapacity);
    if (!next) throw std::bad_alloc();
    if (size) std::memcpy(next, storage.begin, size * sizeof(Slot));
    next[size] = reference;
    // 现代转移口仅搬运已拥有的Ref，不额外增减count；不是原population地址。
    if (storage.begin) DeleteStorage(storage.begin, storage.capacity);
    storage.begin = next;
    storage.end = next + size + 1u;
    storage.capacity = next + nextCapacity;
}
}

__declspec(noinline) FontManagerNativeScope::FontManagerNativeScope(IFontResourcePlatformApi& platform) noexcept
    : previous_(activePlatform) { activePlatform = &platform; }
__declspec(noinline) FontManagerNativeScope::~FontManagerNativeScope() noexcept { activePlatform = previous_; }

std::span<FontResourceReferenceSlotRuntime> FontManagerNativeBody1Ch::ResourceReferences() noexcept
{ return {resourcesAt04.begin, static_cast<std::size_t>(Count(resourcesAt04.begin, resourcesAt04.end))}; }
std::span<YtlReferenceSlotRuntime> FontManagerNativeBody1Ch::SecondaryReferences() noexcept
{ return {secondaryAt10.begin, static_cast<std::size_t>(Count(secondaryAt10.begin, secondaryAt10.end))}; }
const FontManagerNativeVtable& FontManagerNativeMethods() noexcept { return methods; }

FontManagerNativeBody1Ch* ConstructFontManagerAt48ACA9(FontManagerNativeBody1Ch& body) noexcept
{
    body.vptrAt00 = &methods;
    body.resourcesAt04.begin = nullptr;
    body.resourcesAt04.end = nullptr;
    body.resourcesAt04.capacity = nullptr;
    body.secondaryAt10.begin = nullptr;
    body.secondaryAt10.end = nullptr;
    body.secondaryAt10.capacity = nullptr;
    ReleaseFontResourceRangeAt48FA30(body.resourcesAt04.begin, body.resourcesAt04.end);
    body.resourcesAt04.end = body.resourcesAt04.begin;
    return &body;
}
FontManagerNativeBody1Ch* NewFontManagerAt48AC96()
{
    void* storage = ::operator new(0x1C);
    auto* body = static_cast<FontManagerNativeBody1Ch*>(storage);
    return body ? ConstructFontManagerAt48ACA9(*body) : nullptr;
}
void DestroyFontManagerAt463090(FontManagerNativeBody1Ch& body) noexcept { Destroy(body, nullptr); }
void DestroyFontManagerModernOwner(FontManagerNativeBody1Ch& body, bool& secondaryStorageDestroyed) noexcept
{ Destroy(body, &secondaryStorageDestroyed); }
FontManagerNativeBody1Ch* DeleteFontManagerAt468AE0(FontManagerNativeBody1Ch& body, std::uint32_t flags) noexcept
{
    DestroyFontManagerAt463090(body);
    if ((flags & 1u) != 0) ::operator delete(&body, std::size_t{0x1C});
    return &body;
}

void CreateAndReplaceFontManagerAt48AC96(YtlReferenceSlotRuntime& slot)
{
    auto* const created = NewFontManagerAt48AC96();
    ReleaseYtlReference(slot);
    if (!created) return;
    if (slot.controlAt08) slot.controlAt08->DeleteControlAtVslot04(1);
    slot.biasAt04 = 0;
    slot.baseAt00 = reinterpret_cast<std::uintptr_t>(created);
    void* storage = ::operator new(0x18);
    auto* control = storage ? ::new (storage) FontManagerControl(created) : nullptr;
    slot.controlAt08 = control;
    // 48AD68/72/79/83：先发布，按步读取当前control；保留+09..0B未写字节。
    control->referenceCountAt04 = 1;
    slot.controlAt08->owningAt08 = 1;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->strideAt0C = 0x1C;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->countAt10 = 1;
}
void ReleaseFontManagerReferenceAt482749(YtlReferenceSlotRuntime& slot) { ReleaseYtlReference(slot); }
void ReleaseFontManagerReferenceAt4642B1(YtlReferenceSlotRuntime& slot) noexcept { ReleaseYtlReference(slot); }
FontManagerNativeBody1Ch* ResolveFontManagerReference(const YtlReferenceSlotRuntime& slot) noexcept
{ return ResolveEnvironmentObjectReference<FontManagerNativeBody1Ch>(slot); }

void ReleaseFontResourceRangeAt48FA30(FontResourceReferenceSlotRuntime* begin,
    FontResourceReferenceSlotRuntime* end) noexcept
{
    for (; begin != end; ++begin)
    {
        auto* control = begin->controlAt08;
        if (!control) continue;
        if (--control->referenceCountAt04 == 0)
        {
            if (control->owningAt08) control->DestroyPayloadAtVslot00();
            control = begin->controlAt08;
            if (control) control->DeleteControlAtVslot04(1);
        }
        begin->controlAt08 = nullptr;
        begin->baseAt00 = 0;
    }
}
void DestroyFontResourceStorageAt461C20(FontResourceStorage& storage) noexcept
{
    if (!storage.begin) return;
    ReleaseFontResourceRangeAt48FA30(storage.begin, storage.end);
    DeleteStorage(storage.begin, storage.capacity);
    storage.begin = nullptr;
    storage.end = nullptr;
    storage.capacity = nullptr;
}
void DestroyFontSecondaryStorageAt462140(FontSecondaryStorage& storage) noexcept
{
    auto* current = storage.begin;
    if (!current) return;
    auto* const end = storage.end;
    for (; current != end; ++current) ReleaseYtlReference(*current);
    DeleteStorage(storage.begin, storage.capacity);
    storage.begin = nullptr;
    storage.end = nullptr;
    storage.capacity = nullptr;
}
void AdoptFontResourceReferenceModern(FontResourceStorage& storage, FontResourceReferenceSlotRuntime reference)
{ AdoptModern(storage, reference); }
void AdoptFontSecondaryReferenceModern(FontSecondaryStorage& storage, YtlReferenceSlotRuntime reference)
{ AdoptModern(storage, reference); }
}
