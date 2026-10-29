#include "CgButtonNative.Internal.h"
#include <algorithm>
#include <stdexcept>

namespace KaGuYa::Start::Runtime::CgButtonNativeDetail
{
namespace
{
Slot* AllocateVector(unsigned count)
{
    const auto bytes = count * 12u;
    if (bytes < 0x1000u) return bytes ? static_cast<Slot*>(::operator new(bytes)) : nullptr;
    auto* raw = ::operator new(std::size_t(bytes) + 0x23u);
    if (!raw) _invalid_parameter_noinfo_noreturn();
    auto address = (reinterpret_cast<std::uintptr_t>(raw) + 0x23u) & ~std::uintptr_t(31);
    reinterpret_cast<void**>(address)[-1] = raw;
    return reinterpret_cast<Slot*>(address);
}
void DeleteVector(Slot* first, unsigned count)
{
    auto bytes = std::size_t(count) * 12u;
    void* raw = first;
    if (bytes >= 0x1000u)
    {
        raw = reinterpret_cast<void**>(first)[-1];
        if (reinterpret_cast<std::uintptr_t>(first) - reinterpret_cast<std::uintptr_t>(raw) - 4u > 0x1Fu)
            _invalid_parameter_noinfo_noreturn();
        bytes += 0x23u;
    }
    ::operator delete(raw, bytes);
}
}
void AppendChild(ModeButtonReferenceVector& v, const Slot& source)
{
    auto* position = Read(v.end);
    if (position != Read(v.capacity))
    { *position = Copy(source); v.end = Advance(position, 1); return; }
    // 4992A0只在尾增处使用；与4540A0区别是旧区498C10释放可抛，不能借noexcept surface范围。
    const auto size = unsigned(Count(v));
    if (size == 0x15555555u) throw std::length_error("vector<T> too long");
    const auto oldCapacity = unsigned(Distance(Read(v.begin), Read(v.capacity)));
    const auto required = size + 1u;
    const auto grown = oldCapacity > 0x15555555u - oldCapacity / 2u ? required : oldCapacity + oldCapacity / 2u;
    const auto capacity = (std::max)(required, grown);
    auto* next = AllocateVector(capacity);
    auto* inserted = Advance(next, int(size));
    *inserted = Copy(source);
    auto* out = next;
    for (auto* at = Read(v.begin); at != Read(v.end); at = Advance(at, 1), out = Advance(out, 1)) *out = Copy(*at);
    // 49941A调用空范围498C10，构造保护已撤销；旧释放异常不回滚next。
    if (Read(v.begin))
    {
        ReleaseModeButtonReferenceRangeNativeAt498C10(Read(v.begin), Read(v.end));
        DeleteVector(Read(v.begin), unsigned(Distance(Read(v.begin), Read(v.capacity))));
    }
    v.begin = next;
    v.end = Advance(next, int(required));
    v.capacity = Advance(next, int(capacity));
}
}
