#include <Runtime/MessageFormatVectors.h>

#include <algorithm>
#include <bit>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Ref = MessageVoiceAndTextRef;
using Slot = YtlReferenceSlotRuntime;
constexpr std::uint32_t MaximumIndices = 0x3FFFFFFFu;
constexpr std::uint32_t MaximumReferences = 0x15555555u;

std::uint32_t ByteDistance(const void* begin, const void* end) noexcept
{
    return static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(end) -
        reinterpret_cast<std::uintptr_t>(begin));
}

std::uint32_t IndexDistance(const std::int32_t* begin, const std::int32_t* end) noexcept
{
    // 4447C7..4447CA：保留有符号算术右移；全空状态不作空指针减法。
    return static_cast<std::uint32_t>(std::bit_cast<std::int32_t>(ByteDistance(begin, end)) >> 2);
}

std::uint32_t ReferenceDistance(const Ref* begin, const Ref* end) noexcept
{
    // 454F7F..454F98 / 461BB5..461BBE：有符号地址差除以十二，向零取整。
    return static_cast<std::uint32_t>(std::bit_cast<std::int32_t>(ByteDistance(begin, end)) / 12);
}

template<class T>
T* AllocateStorage(std::uint32_t capacity, std::uint32_t maximum)
{
    const auto bytes = capacity <= maximum ? capacity * static_cast<std::uint32_t>(sizeof(T)) : 0xFFFFFFFFu;
    if (bytes < 0x1000u)
    {
        // 444857 / 455039：零字节不调用分配器。
        if (!bytes) return nullptr;
        auto* raw = static_cast<T*>(::operator new(bytes));
        // 明确安全差异：原小块空返回随后非法写入；这里显式终止，不伪装为空容器成功。
        if (!raw) _invalid_parameter_noinfo_noreturn();
        return raw;
    }
    // 44481F..444841 / 454FFE..455020：请求饱和，额外 23h，按三十二字节对齐。
    const auto request = bytes > 0xFFFFFFFFu - 0x23u ? 0xFFFFFFFFu : bytes + 0x23u;
    void* raw = ::operator new(request);
    if (!raw) _invalid_parameter_noinfo_noreturn();
    const auto aligned = (reinterpret_cast<std::uintptr_t>(raw) + 0x23u) & ~std::uintptr_t{0x1F};
    std::memcpy(reinterpret_cast<void*>(aligned - 4u), &raw, sizeof(raw));
    return reinterpret_cast<T*>(aligned);
}

void DeleteStorage(void* begin, std::uint32_t bytes) noexcept
{
    void* raw = begin;
    if (bytes >= 0x1000u)
    {
        const auto aligned = reinterpret_cast<std::uintptr_t>(begin);
        std::uintptr_t allocation;
        std::memcpy(&allocation, reinterpret_cast<const void*>(aligned - 4u), sizeof(allocation));
        // 4448C3..4448D3 / 455128..455138 / 4916D7..4916E7：无符号差验证原指针。
        if (aligned - allocation - 4u > 0x1Fu) _invalid_parameter_noinfo_noreturn();
        raw = reinterpret_cast<void*>(allocation);
    }
    // 53FBFB..53FC08 只把地址交给删除叶；原第二实参为字节数，大块额外加 23h。
    ::operator delete(raw);
}

void MoveIndices(std::int32_t* destination, const std::int32_t* begin, const std::int32_t* end) noexcept
{
    const auto bytes = ByteDistance(begin, end);
    // 原零长度搬运仍调用运行库；这里跳过，避免向标准库传入空源指针。
    if (bytes) std::memmove(destination, begin, bytes);
}

void CopyReferences(const Ref* begin, const Ref* end, Ref* destination) noexcept
{
    // 455090..4550B8 / 4905B0..4905D2：正序复制三字段并增持，不移动、不触及备用槽。
    for (; begin != end; ++begin, ++destination) std::construct_at(destination, *begin);
}

void ReleaseReferences(Ref* begin, Ref* end) noexcept
{
    // 452680 / 5EBA08：正序析构，正文回调后重读槽控制块，释放异常立即终止。
    for (; begin != end; ++begin) std::destroy_at(begin);
}

void ReleaseTemporaryOnUnwind(Slot& temporary) noexcept
{
    // 596220 -> 460B20 / 5E5E84：异常展开重读槽控制块，释放失败终止。
    ReleaseYtlReference(temporary);
}

void ReleaseCapturedTemporary(YtlReferenceControlRuntime* control) noexcept
{
    // 457EE6..457F09 / 5E6004：正常退出使用捕获的控制块，不重读临时槽。
    if (control && --control->referenceCountAt04 == 0)
    {
        if (control->owningAt08) control->DestroyPayloadAtVslot00();
        control->DeleteControlAtVslot04(1);
    }
}
}

MessageIndexVector::~MessageIndexVector() noexcept { DestroyStorage(); }

void MessageIndexVector::PushBack(std::int32_t value)
{
    auto* position = storage.end;
    if (position != storage.capacity)
    {
        *position = value;
        ++storage.end;
        return;
    }
    GrowAndInsert(position, value);
}

void MessageIndexVector::GrowAndInsert(std::int32_t* position, const std::int32_t& source)
{
    const auto index = IndexDistance(storage.begin, position);
    const auto oldSize = IndexDistance(storage.begin, storage.end);
    if (oldSize == MaximumIndices) throw std::length_error("vector<T> too long"); // 4487C0
    const auto required = oldSize + 1u;
    const auto oldCapacity = IndexDistance(storage.begin, storage.capacity);
    // 4447E6..4447FE：增长溢出时退到所需数量，不饱和到最大容量。
    const auto grown = oldCapacity > MaximumIndices - oldCapacity / 2u ? required : oldCapacity + oldCapacity / 2u;
    const auto nextCapacity = (std::max)(required, grown);
    auto* next = AllocateStorage<std::int32_t>(nextCapacity, MaximumIndices);
    // 444859..44486B：分配返回后才读取局部源槽，并先写插入项。
    next[index] = source;
    if (position == storage.end)
        MoveIndices(next, storage.begin, storage.end);
    else
    {
        // 444889..4448A5：分配回调可能改变结束指针，不能删去非尾插入路径。
        MoveIndices(next, storage.begin, position);
        MoveIndices(next + index + 1u, position, storage.end);
    }
    if (storage.begin)
        DeleteStorage(storage.begin, ByteDistance(storage.begin, storage.capacity) & ~3u);
    // 4448D7 释放旧存储后，4448E2..4448EF 才逐项发布；结束位置使用分配前的数量。
    storage.begin = next;
    storage.end = next + required;
    storage.capacity = next + nextCapacity;
}

std::size_t MessageIndexVector::size() const noexcept { return IndexDistance(storage.begin, storage.end); }
std::size_t MessageIndexVector::capacity() const noexcept { return IndexDistance(storage.begin, storage.capacity); }

std::int32_t& MessageIndexVector::at(std::size_t index)
{
    if (index >= size()) throw std::out_of_range("MessageIndexVector index out of range");
    return storage.begin[index];
}

const std::int32_t& MessageIndexVector::at(std::size_t index) const
{
    if (index >= size()) throw std::out_of_range("MessageIndexVector index out of range");
    return storage.begin[index];
}

void MessageIndexVector::clear() noexcept { storage.end = storage.begin; }

void MessageIndexVector::DestroyStorage() noexcept
{
    // 493328..493369：起始指针为空时不改写另两槽；释放成功后才清三个指针。
    if (!storage.begin) return;
    DeleteStorage(storage.begin, ByteDistance(storage.begin, storage.capacity) & ~3u);
    storage.begin = nullptr;
    storage.end = nullptr;
    storage.capacity = nullptr;
}

MessageReferenceVector::~MessageReferenceVector() noexcept { DestroyStorage(); }

void MessageReferenceVector::PushBack(const MessageVoiceAndTextRef& source)
{
    // 457EA2..457EB9：使用十二字节临时槽，区分正常释放与异常展开的控制块读取方式。
    Slot temporary = source.RawSlot();
    if (temporary.controlAt08) ++temporary.controlAt08->referenceCountAt04;
    try
    {
        auto* position = storage.end;
        if (position != storage.capacity)
        {
            std::construct_at(position, temporary);
            ++storage.end;
        }
        else
            GrowAndInsert(position, temporary);
    }
    catch (...)
    {
        ReleaseTemporaryOnUnwind(temporary);
        throw;
    }
    // 457EE3：扩容返回后重新捕获临时槽控制块；正常释放不纳入上面的展开清理。
    ReleaseCapturedTemporary(temporary.controlAt08);
}

void MessageReferenceVector::GrowAndInsert(Ref* position, const Slot& source)
{
    const auto index = ReferenceDistance(storage.begin, position);
    const auto oldSize = ReferenceDistance(storage.begin, storage.end);
    if (oldSize == MaximumReferences) throw std::length_error("vector<T> too long"); // 490EF0
    const auto required = oldSize + 1u;
    const auto oldCapacity = ReferenceDistance(storage.begin, storage.capacity);
    // 454FC5..454FDE：溢出回到所需数量，正常取所需数量与一点五倍容量的较大值。
    const auto grown = oldCapacity > MaximumReferences - oldCapacity / 2u ? required : oldCapacity + oldCapacity / 2u;
    const auto nextCapacity = (std::max)(required, grown);
    // 45500A / 45502A 在受保护构造区之前；分配抛出不释放旧范围或发布新指针。
    auto* next = AllocateStorage<Ref>(nextCapacity, MaximumReferences);
    auto* inserted = next + index;
    auto* cleanupBegin = inserted;
    auto* cleanupEnd = inserted + 1;
    try
    {
        std::construct_at(inserted, source); // 45505A..45506E：插入项先增持。
        if (position == storage.end)
            CopyReferences(storage.begin, storage.end, next);
        else
        {
            // 4550CB..4550E4：首段成功后才扩大清理范围，随后重读结束指针复制尾段。
            CopyReferences(storage.begin, position, next);
            cleanupBegin = next;
            CopyReferences(position, storage.end, cleanupEnd);
        }
    }
    catch (...)
    {
        // 455173..455193 / 5EC2E4：正序清理新范围，再经 4916C0 释放新存储并重抛。
        // 三字段复制及增持不调用可抛回调；不为此清理路径虚构额外故障入口。
        ReleaseReferences(cleanupBegin, cleanupEnd);
        DeleteStorage(next, nextCapacity * 12u);
        throw;
    }
    // 4550EB 退出构造保护区：旧元素或旧存储释放失败不回滚新分配。
    if (storage.begin)
    {
        ReleaseReferences(storage.begin, storage.end);
        // 455100..45511D：元素回调之后重读起始指针和容量，不能提前捕获释放地址。
        DeleteStorage(storage.begin, ReferenceDistance(storage.begin, storage.capacity) * 12u);
    }
    // 455147..455157：正序释放旧范围和旧存储之后，才依次发布三个指针。
    storage.begin = next;
    storage.end = next + required;
    storage.capacity = next + nextCapacity;
}

std::size_t MessageReferenceVector::size() const noexcept { return ReferenceDistance(storage.begin, storage.end); }
std::size_t MessageReferenceVector::capacity() const noexcept { return ReferenceDistance(storage.begin, storage.capacity); }

MessageVoiceAndTextRef& MessageReferenceVector::at(std::size_t index)
{
    if (index >= size()) throw std::out_of_range("MessageReferenceVector index out of range");
    return storage.begin[index];
}

const MessageVoiceAndTextRef& MessageReferenceVector::at(std::size_t index) const
{
    if (index >= size()) throw std::out_of_range("MessageReferenceVector index out of range");
    return storage.begin[index];
}

void MessageReferenceVector::clear() noexcept
{
    ReleaseReferences(storage.begin, storage.end);
    storage.end = storage.begin;
}

void MessageReferenceVector::DestroyStorage() noexcept
{
    if (!storage.begin) return; // 461B98..461B9A：空起始指针不改写另两槽。
    ReleaseReferences(storage.begin, storage.end);
    // 461BA6..461BE2：回调返回后重读存储，释放成功后才清三个指针。
    DeleteStorage(storage.begin, ReferenceDistance(storage.begin, storage.capacity) * 12u);
    storage.begin = nullptr;
    storage.end = nullptr;
    storage.capacity = nullptr;
}
}
