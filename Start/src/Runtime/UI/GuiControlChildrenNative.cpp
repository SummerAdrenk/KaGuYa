#include <Runtime/GuiControlChildrenNative.h>

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
Slot* Allocate(std::uint32_t capacity)
{
    const auto bytes = capacity <= MaximumReferences ? capacity * 12u : 0xFFFFFFFFu;
    if (bytes < 0x1000u) return bytes ? static_cast<Slot*>(::operator new(bytes)) : nullptr;
    const auto request = bytes > 0xFFFFFFFFu - 0x23u ? 0xFFFFFFFFu : bytes + 0x23u;
    void* raw = ::operator new(request);
    if (!raw) _invalid_parameter_noinfo_noreturn();
    const auto aligned = (reinterpret_cast<std::uintptr_t>(raw) + 0x23u) & ~std::uintptr_t{31};
    std::memcpy(reinterpret_cast<void*>(aligned - 4u), &raw, sizeof(raw));
    return reinterpret_cast<Slot*>(aligned);
}
void Deallocate(Slot* begin, std::uint32_t capacity)
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
void CopyRange(const Slot* begin, const Slot* end, Slot* output) noexcept
{
    for (; begin != end; ++begin, ++output) CopySlot(*output, *begin);
}
}

std::int32_t GuiControlChildReferenceCount(const GuiControlChildrenStorage& children) noexcept
    { return Distance(children.begin, children.end); }
std::int32_t GuiControlChildReferenceCapacity(const GuiControlChildrenStorage& children) noexcept
    { return Distance(children.begin, children.capacity); }

Slot* GrowGuiControlChildrenAt4D93B0(GuiControlChildrenStorage& children,
    Slot* position, const Slot& source)
{
    const auto index = static_cast<std::uint32_t>(Distance(children.begin, position));
    const auto size = static_cast<std::uint32_t>(Distance(children.begin, children.end));
    if (size == MaximumReferences) throw std::length_error("vector<T> too long");
    const auto required = size + 1;
    const auto capacity = static_cast<std::uint32_t>(Distance(children.begin, children.capacity));
    const auto grown = capacity > MaximumReferences - capacity / 2 ? required : capacity + capacity / 2;
    const auto nextCapacity = (std::max)(required, grown);
    auto* next = Allocate(nextCapacity);
    auto* inserted = next + index;
    auto* cleanupBegin = inserted;
    try
    {
        CopySlot(*inserted, source);
        // 4D94E1才读旧end；插入值必须先于旧范围复制，借用source允许别名旧元素。
        if (position == children.end) CopyRange(children.begin, children.end, next);
        else
        {
            CopyRange(children.begin, position, next);
            cleanupBegin = next;
            CopyRange(position, children.end, inserted + 1);
        }
    }
    catch (...)
    {
        ReleaseGuiControlChildrenRangeAt4D7F00(cleanupBegin, inserted + 1);
        Deallocate(next, nextCapacity);
        throw;
    }
    // 4D955B已撤新存储保护；回调可观察旧头，释放存储前按4D9570重读头。
    if (children.begin)
    {
        ReleaseGuiControlChildrenRangeAt4D7F00(children.begin, children.end);
        Deallocate(children.begin, static_cast<std::uint32_t>(Distance(children.begin, children.capacity)));
    }
    children.begin = next;
    children.end = next + required;
    children.capacity = next + nextCapacity;
    return children.begin + index;
}

void AppendGuiControlChildReference(GuiControlChildrenStorage& children, const Slot& source)
{
    auto* position = children.end;
    if (position != children.capacity)
    {
        CopySlot(*position, source);
        ++children.end;
    }
    else static_cast<void>(GrowGuiControlChildrenAt4D93B0(children, position, source));
}

void ReleaseGuiControlChildrenRangeAt4D7F00(Slot* begin, Slot* end) noexcept
{
    // 5FD51C的state0清理指向terminate；与普通可抛Reset的政策不同。
    for (; begin != end; ++begin) ReleaseYtlReference(*begin);
}
void DestroyGuiControlChildrenAt4F6C60(GuiControlChildrenStorage& children) noexcept
{
    if (!children.begin) return;
    ReleaseGuiControlChildrenRangeAt4D7F00(children.begin, children.end);
    Deallocate(children.begin, static_cast<std::uint32_t>(Distance(children.begin, children.capacity)));
    children = {};
}
}
