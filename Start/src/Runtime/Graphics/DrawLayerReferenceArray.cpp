#include <Runtime/DrawLayerReferenceArray.h>
#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/LayerItemNative.h>

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
using Slot = YtlReferenceSlotRuntime;
constexpr std::uint32_t MaximumReferences = 0x15555555u;

std::int32_t Distance(const Slot* begin, const Slot* end) noexcept
{
    const auto bytes = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(end) -
        reinterpret_cast<std::uintptr_t>(begin));
    return std::bit_cast<std::int32_t>(bytes) / 12;
}

Slot* Allocate(std::uint32_t capacity, IEnvironmentRootMemory& memory)
{
    const auto bytes = capacity <= MaximumReferences ? capacity * 12u : 0xFFFFFFFFu;
    if (bytes < 0x1000u)
        return bytes ? static_cast<Slot*>(memory.AllocateBody(bytes)) : nullptr;
    const auto request = bytes > 0xFFFFFFFFu - 0x23u ? 0xFFFFFFFFu : bytes + 0x23u;
    void* raw = memory.AllocateBody(request);
    if (!raw) _invalid_parameter_noinfo_noreturn();
    const auto aligned = (reinterpret_cast<std::uintptr_t>(raw) + 0x23u) & ~std::uintptr_t{31};
    std::memcpy(reinterpret_cast<void*>(aligned - 4u), &raw, sizeof(raw));
    return reinterpret_cast<Slot*>(aligned);
}

void Deallocate(Slot* begin, std::uint32_t capacity) noexcept
{
    auto bytes = capacity * 12u;
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

void CopySlot(Slot& destination, const Slot& source) noexcept
{
    destination.baseAt00 = source.baseAt00;
    destination.biasAt04 = source.biasAt04;
    destination.controlAt08 = source.controlAt08;
    if (destination.controlAt08) ++destination.controlAt08->referenceCountAt04;
}

void CopyRange(const Slot* begin, const Slot* end, Slot* destination) noexcept
{
    for (; begin != end; ++begin, ++destination) CopySlot(*destination, *begin);
}

class LayerControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit LayerControl(LayerItemNativeBody14h* layer) noexcept
        : EnvironmentObjectReferenceControl(layer, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        // 492E60：必须经过正文当前 +08 删除槽，stride14h 不等于派生正文大小。
        auto* body = static_cast<LayerItemNativeBody14h*>(payloadAt14);
        if (body) body->vptrAt00->deletingDestructorAt08(body, 1);
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
        { RetireEnvironmentObjectControl(this, flags); }
};
static_assert(sizeof(LayerControl) == 0x18);
}

std::int32_t DrawLayerReferenceCount(const DrawLayerReferenceArray& array) noexcept
    { return Distance(array.begin, array.end); }
std::int32_t DrawLayerReferenceCapacity(const DrawLayerReferenceArray& array) noexcept
    { return Distance(array.begin, array.capacity); }
LayerItemNativeBody14h* ResolveDrawLayerReference(const Slot& reference) noexcept
    { return ResolveEnvironmentObjectReference<LayerItemNativeBody14h>(reference); }

Slot* GrowDrawLayerReferencesAt454580(DrawLayerReferenceArray& array,
    Slot* position, const Slot& source, IEnvironmentRootMemory& memory)
{
    const auto index = static_cast<std::uint32_t>(Distance(array.begin, position));
    const auto size = static_cast<std::uint32_t>(Distance(array.begin, array.end));
    if (size == MaximumReferences) throw std::length_error("vector<T> too long");
    const auto required = size + 1;
    const auto capacity = static_cast<std::uint32_t>(Distance(array.begin, array.capacity));
    const auto grown = capacity > MaximumReferences - capacity / 2 ? required : capacity + capacity / 2;
    const auto nextCapacity = (std::max)(required, grown);
    auto* next = Allocate(nextCapacity, memory);
    auto* inserted = next + index;
    auto* cleanupBegin = inserted;
    try
    {
        // 45469A在分配之后才读取source；source允许别名旧数组或被分配回调改写。
        CopySlot(*inserted, source);
        if (position == array.end) CopyRange(array.begin, array.end, next);
        else
        {
            CopyRange(array.begin, position, next);
            cleanupBegin = next;
            CopyRange(position, array.end, inserted + 1);
        }
    }
    catch (...)
    {
        ReleaseDrawLayerRangeAt452440(cleanupBegin, inserted + 1);
        Deallocate(next, nextCapacity);
        throw;
    }
    // 45472B撤销新存储守卫；释放旧范围后才重读旧头和发布新头。
    if (array.begin)
    {
        ReleaseDrawLayerRangeAt452440(array.begin, array.end);
        Deallocate(array.begin, static_cast<std::uint32_t>(Distance(array.begin, array.capacity)));
    }
    array.begin = next;
    array.end = next + required;
    array.capacity = next + nextCapacity;
    return array.begin + index;
}

void AppendDrawLayerReference(DrawLayerReferenceArray& array, const Slot& source,
    IEnvironmentRootMemory& memory)
{
    auto* position = array.end;
    if (position != array.capacity)
    {
        CopySlot(*position, source);
        ++array.end;
    }
    else static_cast<void>(GrowDrawLayerReferencesAt454580(array, position, source, memory));
}

void AppendOwnedDrawLayerAt46A880(DrawLayerReferenceArray& array,
    LayerItemNativeBody14h* layer, IEnvironmentRootMemory& memory)
{
    Slot local;
    LayerControl* control = nullptr;
    if (layer)
    {
        local.baseAt00 = reinterpret_cast<std::uintptr_t>(layer);
        void* storage = memory.AllocateBody(0x18);
        control = storage ? ::new(storage) LayerControl(layer) : nullptr;
        if (!control) _invalid_parameter_noinfo_noreturn();
        control->referenceCountAt04 = 1;
        control->owningAt08 = 1;
        control->strideAt0C = 0x14;
        local.controlAt08 = control;
        control->countAt10 = 1;
    }
    try { AppendDrawLayerReference(array, local, memory); }
    catch (...)
    {
        // 5989C0 -> 460570：EH成员清理经当前Ref；释放异常终止。
        ReleaseDrawLayerRangeAt452440(&local, &local + 1);
        throw;
    }
    // 46A946正常路径保留ESI，不重读局部Ref；不能套用成员Ref的重取control语义。
    // FuncInfo 5E9CA4：state1的action=541E55，正常尾部的动态释放异常也必须终止。
    [&]() noexcept
    {
        if (control && --control->referenceCountAt04 == 0)
        {
            if (control->owningAt08) control->DestroyPayloadAtVslot00();
            control->DeleteControlAtVslot04(1);
        }
    }();
}

void ReleaseDrawLayerRangeAt452440(Slot* begin, Slot* end) noexcept
{
    for (; begin != end; ++begin) ReleaseYtlReference(*begin);
}

void DestroyDrawLayerReferencesAt461750(DrawLayerReferenceArray& array) noexcept
{
    if (!array.begin) return;
    ReleaseDrawLayerRangeAt452440(array.begin, array.end);
    Deallocate(array.begin, static_cast<std::uint32_t>(Distance(array.begin, array.capacity)));
    array = {};
}
}
