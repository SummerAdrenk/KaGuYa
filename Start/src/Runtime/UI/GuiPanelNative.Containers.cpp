#include "GuiPanelNative.Internal.h"
#include <bit>
#include <cstdlib>
#include <cstring>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
std::uint32_t Address(const void* p) noexcept { return reinterpret_cast<std::uintptr_t>(p); }
std::uint32_t Count(const void* end, const void* begin) noexcept
{ return static_cast<std::uint32_t>(std::bit_cast<std::int32_t>(Address(end) - Address(begin)) >> 2); }
std::int32_t* At(std::uint32_t base, std::uint32_t bytes) noexcept
{ return reinterpret_cast<std::int32_t*>(base + bytes); }
void FreeIds(GuiPanelIdStorage& ids) noexcept
{
    auto* raw = ids.begin;
    auto bytes = Count(ids.capacity, raw) * 4u;
    if (bytes >= 0x1000u)
    {
        void* allocation;
        std::memcpy(&allocation, reinterpret_cast<const void*>(Address(raw) - 4u), sizeof(allocation));
        bytes += 0x23u;
        if (Address(raw) - Address(allocation) - 4u > 0x1Fu) _invalid_parameter_noinfo_noreturn();
        raw = static_cast<std::int32_t*>(allocation);
    }
    ::operator delete(raw, bytes);
}
void GrowIdsAt4D82F0(GuiPanelIdStorage& ids, std::int32_t* position, const std::int32_t* value)
{
    const auto offset = Count(position, ids.begin);
    const auto size = Count(ids.end, ids.begin);
    if (size == 0x3FFFFFFFu) throw std::length_error("vector<T> too long");
    const auto nextSize = size + 1u;
    const auto capacity = Count(ids.capacity, ids.begin);
    const auto half = capacity >> 1;
    auto nextCapacity = nextSize;
    if (capacity <= 0x3FFFFFFFu - half && capacity + half >= nextSize) nextCapacity = capacity + half;
    const auto bytes = nextCapacity * 4u;
    auto* created = AllocateGuiPanelIdsAt491040(nextCapacity);
    *At(Address(created), offset * 4u) = *value;
    // 4D8355..4D83A7：分配之后重读旧范围及存储释放接收者。
    if (position == ids.end)
        std::memmove(created, ids.begin, Address(ids.end) - Address(ids.begin));
    else
    {
        std::memmove(created, ids.begin, Address(position) - Address(ids.begin));
        std::memmove(At(Address(created), offset * 4u + 4u), position, Address(ids.end) - Address(position));
    }
    if (ids.begin) FreeIds(ids);
    ids.begin = created;
    ids.end = At(Address(created), nextSize * 4u);
    ids.capacity = At(Address(created), bytes);
}
}
std::int32_t* AllocateGuiPanelIdsAt491040(std::uint32_t capacity)
{
    // 49104D..57：越过3FFFFFFF时请求FFFFFFFF，不允许乘法回绕成小块。
    const auto bytes=capacity<=0x3FFFFFFFu?capacity*4u:0xFFFFFFFFu;
    if(bytes<0x1000u)return bytes?static_cast<std::int32_t*>(::operator new(bytes)):nullptr;
    const auto expanded=bytes+0x23u;
    void* raw=::operator new(expanded<=bytes?0xFFFFFFFFu:expanded);
    if(!raw)_invalid_parameter_noinfo_noreturn();
    auto* created=At((Address(raw)+0x23u)&~0x1Fu,0);
    std::memcpy(At(Address(created),0xFFFFFFFCu),&raw,sizeof(raw));
    return created;
}
namespace GuiPanelNativeDetail
{
void DestroyIds(GuiPanelIdStorage& ids) noexcept
{
    if (!ids.begin) return;
    FreeIds(ids);
    ids = {};
}
}
void AddGuiPanelIdAt4FF0E0(GuiPanelNativeBody6Ch& body, std::int32_t id)
{
    auto& ids = body.idsAt50;
    for (auto* p = ids.begin; p != ids.end; ++p) if (*p == id) return;
    auto* position = ids.end;
    if (position == ids.capacity) GrowIdsAt4D82F0(ids, position, &id);
    else { *position = id; ids.end = At(Address(ids.end), 4); }
}
void AddGuiPanelNavigationAt4FF290(GuiPanelNativeBody6Ch& body, YtlReferenceSlotRuntime ownedNavigation)
{
    GuiPanelNativeDetail::LocalReference argument{ownedNavigation};
    // 4D86D0与4D9D60的Ref12增长/复制/旧释放/发布相同，上限15555555。
    AppendGuiNavigationReferenceAt4D9D60(body.navigationAt5C, ownedNavigation);
    argument.Current(false); // state1终止边界；正常消费不清参数位型。
}
namespace GuiPanelNativeDetail::Entries
{
void __fastcall AddId(Common* p, void*, std::int32_t id) { AddGuiPanelIdAt4FF0E0(Panel(p), id); }
void __fastcall AddNavigation(Common* p, void*, Slot owned) { AddGuiPanelNavigationAt4FF290(Panel(p), owned); }
}
}
