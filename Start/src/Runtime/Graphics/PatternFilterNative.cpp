#include <Runtime/PatternFilterNative.h>

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
using Body = PatternFilterNativeBody20h;
using Storage = EnvironmentVectorStorage<std::uint32_t>;
constexpr std::uint32_t MaximumCount = 0x3FFFFFFFu;
static_assert(sizeof(void*) == 4);

std::uintptr_t Address(const void* pointer) noexcept
{ return reinterpret_cast<std::uintptr_t>(pointer); }
std::uint32_t ByteDistance(const void* begin, const void* end) noexcept
{ return static_cast<std::uint32_t>(Address(end) - Address(begin)); }
std::uint32_t Distance(const void* begin, const void* end) noexcept
{ return static_cast<std::uint32_t>(std::bit_cast<std::int32_t>(ByteDistance(begin, end)) >> 2); }
std::uint32_t* Advance(std::uint32_t* begin, std::uint32_t count) noexcept
{ return reinterpret_cast<std::uint32_t*>(Address(begin) + count * 4u); }

std::uint32_t* AllocateAt491040(std::uint32_t count, IEnvironmentRootMemory& memory)
{
    const auto bytes = count <= MaximumCount ? count * 4u : 0xFFFFFFFFu;
    if (bytes < 0x1000u)
        return bytes ? static_cast<std::uint32_t*>(memory.AllocateBody(bytes)) : nullptr;
    const auto extended = bytes + 0x23u;
    auto* raw = memory.AllocateBody(extended <= bytes ? 0xFFFFFFFFu : extended);
    if (!raw) _invalid_parameter_noinfo_noreturn();
    const auto aligned = (Address(raw) + 0x23u) & ~std::uintptr_t{31};
    std::memcpy(reinterpret_cast<void*>(aligned - 4u), &raw, sizeof(raw));
    return reinterpret_cast<std::uint32_t*>(aligned);
}

void DeleteVectorStorage(const Storage& storage, IEnvironmentRootMemory& memory)
{
    auto* begin = storage.begin;
    if (!begin) return;
    auto bytes = ByteDistance(begin, storage.capacity) & ~3u;
    void* raw = begin;
    if (bytes >= 0x1000u)
    {
        std::memcpy(&raw, reinterpret_cast<const void*>(Address(begin) - 4u), sizeof(raw));
        bytes += 0x23u;
        if (Address(begin) - Address(raw) - 4u > 0x1Fu)
            _invalid_parameter_noinfo_noreturn();
    }
    memory.DeleteVector(raw, bytes);
}

void CopyVectorAt457F50(Storage& target, const Storage& source, IEnvironmentRootMemory& memory)
{
    target.begin = nullptr;
    target.end = nullptr;
    target.capacity = nullptr;
    const auto count = Distance(source.begin, source.end);
    if (!count) return;
    if (count > MaximumCount) throw std::length_error("vector<T> too long");
    target.begin = AllocateAt491040(count, memory);
    target.end = target.begin;
    target.capacity = Advance(target.begin, count);
    // 457F99..457FB0：capacity用分配前count，复制字节数与source指针都在分配后读取。
    const auto* begin = source.begin;
    auto* destination = target.begin;
    const auto bytes = ByteDistance(begin, source.end);
    if (bytes) std::memmove(destination, begin, bytes);
    target.end = reinterpret_cast<std::uint32_t*>(Address(destination) + bytes);
}

void AssignVectorAt4CA057(Storage& target, const Storage& source, IEnvironmentRootMemory& memory)
{
    if (&target == &source) return;
    const auto* sourceEnd = source.end;
    const auto* sourceBegin = source.begin;
    const auto bytes = ByteDistance(sourceBegin, sourceEnd);
    auto* const oldBegin = target.begin;
    auto* const oldCapacity = target.capacity;
    auto* const oldEnd = target.end;
    const auto count = Distance(sourceBegin, sourceEnd);
    const auto capacity = Distance(oldBegin, oldCapacity);
    const auto size = Distance(oldBegin, oldEnd);
    if (count > capacity)
    {
        if (count > MaximumCount) throw std::length_error("vector<T> too long");
        const auto half = capacity >> 1;
        const auto nextCapacity = capacity > MaximumCount - half ? count : (std::max)(count, capacity + half);
        // 4CA0BF -> 4D7110只消费缓存base/capacity；回调不能改删当前header指向的新块。
        const Storage previous{oldBegin, oldEnd, oldCapacity};
        DeleteVectorStorage(previous, memory);
        target.begin = nullptr;
        target.end = nullptr;
        target.capacity = nullptr;
        target.begin = AllocateAt491040(nextCapacity, memory);
        target.end = target.begin;
        target.capacity = Advance(target.begin, nextCapacity);
        auto* const destination = target.begin;
        // 4CA0FD沿用分配前的source指针和字节数，不重读被回调换过的源header。
        std::memmove(destination, sourceBegin, bytes);
        target.end = reinterpret_cast<std::uint32_t*>(Address(destination) + bytes);
    }
    else if (count > size)
    {
        const auto prefixBytes = size * 4u;
        std::memmove(oldBegin, sourceBegin, prefixBytes);
        const auto* remaining = reinterpret_cast<const std::uint32_t*>(Address(sourceBegin) + prefixBytes);
        const auto remainingBytes = ByteDistance(remaining, sourceEnd);
        // 4CA12E：首段memmove可以别名改写目标end，尾段必须在首段完成后才取。
        auto* const destination = target.end;
        std::memmove(destination, remaining, remainingBytes);
        target.end = reinterpret_cast<std::uint32_t*>(Address(destination) + remainingBytes);
    }
    else
    {
        std::memmove(oldBegin, sourceBegin, bytes);
        target.end = Advance(oldBegin, count);
    }
}

void RetireArchive(Body& body) noexcept
{
    // 共用真实两槽IArchive的退表叶，不调用派生C++析构以免提前换表/追加cleanup。
    body.IEnvironmentArchiveRuntime::DeleteAtVslot04(0);
}
}

PatternFilterNativeBody20h* ConstructPatternFilterNativeAt45B1CB(void* raw) noexcept
{
    auto* body = ::new (raw) Body;
    body->argumentsAt08.begin = nullptr;
    body->argumentsAt08.end = nullptr;
    body->argumentsAt08.capacity = nullptr;
    ::new (&body->temporaryAt14) YtlReferenceSlotRuntime{0, 0, nullptr};
    body->modeAt04 = 0;
    body->argumentsAt08.end = body->argumentsAt08.begin;
    return body;
}

PatternFilterNativeBody20h* CopyConstructPatternFilterNativeAt46F245(void* raw,
    const PatternFilterNativeBody20h& source, IEnvironmentRootMemory& memory)
{
    const auto mode = source.modeAt04;
    std::memcpy(static_cast<unsigned char*>(raw) + 4, &mode, sizeof(mode));
    auto* body = ::new (raw) Body;
    try
    {
        CopyVectorAt457F50(body->argumentsAt08, source.argumentsAt08, memory);
    }
    catch (...)
    {
        // 5ECBB8 state13 -> 598D43 -> 40F860；没有vector/temp析构或raw delete。
        RetireArchive(*body);
        throw;
    }
    ::new (&body->temporaryAt14) YtlReferenceSlotRuntime{source.temporaryAt14.baseAt00,
        source.temporaryAt14.biasAt04, source.temporaryAt14.controlAt08};
    auto* control = body->temporaryAt14.controlAt08;
    if (control) ++control->referenceCountAt04;
    return body;
}

PatternFilterNativeBody20h* AssignPatternFilterNativeAt4CA040(Body& body,
    const Body& source, IEnvironmentRootMemory& memory)
{
    body.modeAt04 = source.modeAt04;
    AssignVectorAt4CA057(body.argumentsAt08, source.argumentsAt08, memory);
    if (&body.temporaryAt14 != &source.temporaryAt14)
    {
        // 4CA17D释放回调后重取旧槽control；三项源Ref仅在旧退域完全结束后读取。
        ReleaseYtlReference(body.temporaryAt14);
        body.temporaryAt14.baseAt00 = source.temporaryAt14.baseAt00;
        body.temporaryAt14.biasAt04 = source.temporaryAt14.biasAt04;
        body.temporaryAt14.controlAt08 = source.temporaryAt14.controlAt08;
        if (body.temporaryAt14.controlAt08) ++body.temporaryAt14.controlAt08->referenceCountAt04;
    }
    return &body;
}

void DestroyPatternFilterNativeAt462FC0(PatternFilterNativeBody20h& body,
    IEnvironmentRootMemory& memory) noexcept
{
    ReleaseYtlReference(body.temporaryAt14);
    if (body.argumentsAt08.begin)
    {
        DeleteVectorStorage(body.argumentsAt08, memory);
        body.argumentsAt08.begin = nullptr;
        body.argumentsAt08.end = nullptr;
        body.argumentsAt08.capacity = nullptr;
    }
    RetireArchive(body);
}

void* DeletePatternFilterNativeAt468AB0(PatternFilterNativeBody20h* body, std::uint32_t flags,
    IEnvironmentRootMemory& memory) noexcept
{
    DestroyPatternFilterNativeAt462FC0(*body, memory);
    if (flags & 1u) memory.DeleteBody(body, 0x20);
    return body;
}

void ResizePatternFilterArgumentsAt4573E0(PatternFilterNativeBody20h& body, std::uint32_t count,
    IEnvironmentRootMemory& memory)
{
    auto& storage = body.argumentsAt08;
    auto* oldEnd = storage.end;
    const auto size = Distance(storage.begin, oldEnd);
    const auto capacity = Distance(storage.begin, storage.capacity);
    if (count > capacity)
    {
        if (count > MaximumCount) throw std::length_error("vector<T> too long");
        const auto grown = capacity > MaximumCount - capacity / 2u ? count : capacity + capacity / 2u;
        const auto nextCapacity = (std::max)(count, grown);
        auto* next = AllocateAt491040(nextCapacity, memory);
        std::memset(Advance(next, size), 0, (count - size) * 4u);
        const auto* begin = storage.begin;
        const auto bytes = ByteDistance(begin, storage.end);
        if (bytes) std::memmove(next, begin, bytes);
        // 48F600：删除的是当前三指针；异常不回收未发布的新块，不替换旧状态。
        DeleteVectorStorage(storage, memory);
        storage.begin = next;
        storage.end = Advance(next, count);
        storage.capacity = Advance(next, nextCapacity);
    }
    else if (count > size)
    {
        const auto bytes = (count - size) * 4u;
        std::memset(oldEnd, 0, bytes);
        storage.end = reinterpret_cast<std::uint32_t*>(Address(oldEnd) + bytes);
    }
    else if (count < size) storage.end = Advance(storage.begin, count);
}

void SerializePatternFilterNativeAt48AFF0(PatternFilterNativeBody20h& body,
    SerializeBuffer& serializer, IEnvironmentRootMemory& memory)
{
    serializer.SerializeSlot58Dword(body.modeAt04);
    // 4660A0：现代adapter只承接scalar，原三指针始终是唯一参数拥有存储。
    const bool writing = serializer.IsWriting();
    std::uint32_t count;
    if (writing) count = Distance(body.argumentsAt08.begin, body.argumentsAt08.end);
    serializer.SerializeSlot58Dword(count);
    if (!writing) ResizePatternFilterArgumentsAt4573E0(body, count, memory);
    const auto signedCount = std::bit_cast<std::int32_t>(count);
    for (std::int32_t index = 0; index < signedCount; ++index)
        serializer.SerializeSlot58Dword(body.argumentsAt08.begin[index]);
}

void PatternFilterNativeBody20h::Serialize(SerializeBuffer& serializer)
{ SerializePatternFilterNativeAt48AFF0(*this, serializer); }
void* PatternFilterNativeBody20h::DeleteAtVslot04(std::uint32_t flags) noexcept
{ return DeletePatternFilterNativeAt468AB0(this, flags); }
}
