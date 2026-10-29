#include <Runtime/MessageReaderVectors.h>

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
constexpr std::uint32_t MaximumOffsets = 0x3FFFFFFFu;
constexpr std::uint32_t MaximumReferences = 0x15555555u;
using Slot = YtlReferenceSlotRuntime;

std::uint32_t ByteDistance(const void* begin, const void* end) noexcept
{
    return static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(end) -
        reinterpret_cast<std::uintptr_t>(begin));
}

std::uint32_t OffsetDistance(const std::uint32_t* begin, const std::uint32_t* end) noexcept
{
    // 453A87..453A8A：地址差按有符号 DWORD 算术右移，不能改成 size_t 除法。
    return static_cast<std::uint32_t>(std::bit_cast<std::int32_t>(ByteDistance(begin, end)) >> 2);
}

template<class Ref>
std::uint32_t ReferenceDistance(const Ref* begin, const Ref* end) noexcept
{
    static_assert(sizeof(Ref) == 12);
    // 454D0F..454D28 / 45656F..456588：有符号差除十二，向零取整。
    return static_cast<std::uint32_t>(std::bit_cast<std::int32_t>(ByteDistance(begin, end)) / 12);
}

std::uint32_t NextCapacity(std::uint32_t oldCapacity, std::uint32_t required,
    std::uint32_t maximum) noexcept
{
    // 453AA6..453ABD / 454D44..454D6B / 4565A4..4565CB：溢出退到所需量。
    const auto grown = oldCapacity > maximum - oldCapacity / 2u ? required : oldCapacity + oldCapacity / 2u;
    return (std::max)(required, grown);
}

// MessageFormatVectors 的 helper 为内部链接；此处局部复用其分配协议，不改公共源码。
template<class T>
T* AllocateStorage(std::uint32_t capacity, std::uint32_t maximum)
{
    const auto bytes = capacity <= maximum ? capacity * static_cast<std::uint32_t>(sizeof(T)) : 0xFFFFFFFFu;
    if (bytes < 0x1000u)
    {
        if (!bytes) return nullptr; // 491179 / 454DC9 / 456629
        auto* raw = static_cast<T*>(::operator new(bytes));
        // 与既有 helper 一致的安全差异：原小块 null 后非法写入；这里显式终止。
        if (!raw) _invalid_parameter_noinfo_noreturn();
        return raw;
    }
    // 491141..491161 / 454D8E..454DB0 / 4565EE..456610：饱和请求、32字节对齐。
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
        // 48F6DC..48F6EA / 491697..4916A5 / 4919D7..4919E5：无符号距离校验。
        if (aligned - allocation - 4u > 0x1Fu) _invalid_parameter_noinfo_noreturn();
        raw = reinterpret_cast<void*>(allocation);
    }
    // 53FBFB 实际只转交地址给 free；第二个字节数实参未被叶函数使用。
    ::operator delete(raw);
}

void MoveOffsets(std::uint32_t* destination, const std::uint32_t* begin,
    const std::uint32_t* end) noexcept
{
    const auto bytes = ByteDistance(begin, end);
    // 原零长也调 memmove；这里避免把空指针交给标准库，不改变存储或分配事件。
    if (bytes) std::memmove(destination, begin, bytes);
}

template<class Ref>
void CopyReferences(const Ref* begin, const Ref* end, Ref* destination) noexcept
{
    // 490520..490542 / 490C70..490C92：三字段原样复制、逐项增持，备用槽不写。
    for (; begin != end; ++begin, ++destination) std::construct_at(destination, *begin);
}

template<class Ref>
void ReleaseReferences(Ref* begin, Ref* end) noexcept
{
    // 4525F0 / 452D40：正序，边界入栈捕获；复用 typed Ref 析构的 control 重读。
    for (; begin != end; ++begin) std::destroy_at(begin);
}

void ReleaseTemporaryOnUnwind(Slot& temporary) noexcept
{
    // 5961F0 -> 40D980；59A620 -> 461300：正文回调后重读临时槽，二次异常终止。
    ReleaseYtlReference(temporary);
}

void ReleaseCapturedTemporary(YtlReferenceControlRuntime* control) noexcept
{
    // 457E21..457E39 / 48489C..4848B4：正常释放不重读槽。
    // 5E64B8 / 5E6458 的展开项指向 541E55 terminate，不能传播到 Load 的 catch。
    if (control && --control->referenceCountAt04 == 0)
    {
        if (control->owningAt08) control->DestroyPayloadAtVslot00();
        control->DeleteControlAtVslot04(1);
    }
}

template<class Storage, class Ref>
void GrowReferences(Storage& storage, Ref* position, const Slot& source)
{
    const auto index = ReferenceDistance(storage.begin, position);
    const auto oldSize = ReferenceDistance(storage.begin, storage.end);
    if (oldSize == MaximumReferences) throw std::length_error("vector<T> too long"); // 490EE0 / 490FB0
    const auto required = oldSize + 1u;
    const auto nextCapacity = NextCapacity(ReferenceDistance(storage.begin, storage.capacity), required, MaximumReferences);
    // 分配在构造保护区之外；失败只由外层释放临时 Ref，旧容器不发布、不销毁。
    auto* next = AllocateStorage<Ref>(nextCapacity, MaximumReferences);
    auto* inserted = next + index;
    auto* cleanupBegin = inserted;
    auto* cleanupEnd = inserted + 1;
    try
    {
        std::construct_at(inserted, source); // 454DEA..454DFE / 45664A..45665E：插入先于搬运。
        if (position == storage.end)
            CopyReferences(storage.begin, storage.end, next);
        else
        {
            CopyReferences(storage.begin, position, next);
            cleanupBegin = next; // 454E6B / 4566CB：首段完成后才扩展 EH 范围。
            CopyReferences(position, storage.end, cleanupEnd);
        }
    }
    catch (...)
    {
        // 454F03..454F23 / 456763..456783：前向清理新范围、释放新块、重抛。
        // 原复制只有读写和 inc，无可抛回调；不虚构生产故障注入点。
        ReleaseReferences(cleanupBegin, cleanupEnd);
        DeleteStorage(next, nextCapacity * 12u);
        throw;
    }
    // 454E7B / 4566DB：退出保护区后释放旧范围，失败不能回滚新块。
    if (storage.begin)
    {
        ReleaseReferences(storage.begin, storage.end);
        // 454E90..454E98 / 4566F0..4566F8：元素回调结束后重读 begin/capacity。
        DeleteStorage(storage.begin, ReferenceDistance(storage.begin, storage.capacity) * 12u);
    }
    // 454ED7..454EE7 / 456737..456747：旧存储释放后逐槽发布，size 使用分配前捕获值。
    storage.begin = next;
    storage.end = next + required;
    storage.capacity = next + nextCapacity;
}

template<class Storage, class Ref>
void PushReference(Storage& storage, const Ref& source)
{
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
            GrowReferences(storage, position, temporary);
    }
    catch (...)
    {
        ReleaseTemporaryOnUnwind(temporary);
        throw;
    }
    // 457E13 / 484891：扩容后捕获临时 control，正常释放不受上面的 catch 保护。
    auto* captured = temporary.controlAt08;
    ReleaseCapturedTemporary(captured);
    if constexpr (std::is_same_v<Ref, MessageRawFormatRef>)
    {
        // 4848B7..4848BE：raw 正常尾声清 base/control，bias 不写；TextCmd 没有这两次写入。
        if (captured)
        {
            temporary.controlAt08 = nullptr;
            temporary.baseAt00 = 0;
        }
    }
}

template<class Storage>
void ClearReferences(Storage& storage) noexcept
{
    ReleaseReferences(storage.begin, storage.end);
    storage.end = storage.begin; // 46D396..46D398 / 46D3F9..46D3FE：回调后重读。
}

template<class Storage>
void DestroyReferences(Storage& storage) noexcept
{
    // 461A38 / 462688：begin 为空不改写另外两槽。
    if (!storage.begin) return;
    ReleaseReferences(storage.begin, storage.end);
    DeleteStorage(storage.begin, ReferenceDistance(storage.begin, storage.capacity) * 12u);
    storage.begin = nullptr;
    storage.end = nullptr;
    storage.capacity = nullptr;
}
}

MessageOffsetVector::~MessageOffsetVector() noexcept { DestroyStorage(); }

void MessageOffsetVector::PushBack(const std::uint32_t& source)
{
    auto* position = storage.end;
    if (position != storage.capacity)
    {
        *position = source; // 49462B..494632：DWORD 位模式不作符号转换。
        ++storage.end;
    }
    else
        GrowAndInsert(position, source);
}

void MessageOffsetVector::GrowAndInsert(std::uint32_t* position, const std::uint32_t& source)
{
    const auto index = OffsetDistance(storage.begin, position);
    const auto oldSize = OffsetDistance(storage.begin, storage.end);
    if (oldSize == MaximumOffsets) throw std::length_error("vector<T> too long"); // 490E20
    const auto required = oldSize + 1u;
    const auto nextCapacity = NextCapacity(OffsetDistance(storage.begin, storage.capacity), required, MaximumOffsets);
    auto* next = AllocateStorage<std::uint32_t>(nextCapacity, MaximumOffsets);
    next[index] = source; // 453ADB..453AE3：分配之后读源，旧存储释放之前捕获别名值。
    if (position == storage.end)
        MoveOffsets(next, storage.begin, storage.end);
    else
    {
        MoveOffsets(next, storage.begin, position);
        MoveOffsets(next + index + 1u, position, storage.end); // 453B0B：重读尾界。
    }
    if (storage.begin)
        DeleteStorage(storage.begin, ByteDistance(storage.begin, storage.capacity) & ~3u);
    // 48F6F0 先删除旧块；48F6FE..48F70C 才发布，不能提前交换头。
    storage.begin = next;
    storage.end = next + required;
    storage.capacity = next + nextCapacity;
}

std::size_t MessageOffsetVector::size() const noexcept { return OffsetDistance(storage.begin, storage.end); }
std::size_t MessageOffsetVector::capacity() const noexcept { return OffsetDistance(storage.begin, storage.capacity); }
void MessageOffsetVector::clear() noexcept { storage.end = storage.begin; }
void MessageOffsetVector::DestroyStorage() noexcept
{
    if (!storage.begin) return; // 464CD8..464CDA：空 begin 保留其余槽。
    DeleteStorage(storage.begin, ByteDistance(storage.begin, storage.capacity) & ~3u);
    storage.begin = nullptr;
    storage.end = nullptr;
    storage.capacity = nullptr;
}

MessageTextCommandReferenceVector::~MessageTextCommandReferenceVector() noexcept { DestroyStorage(); }
void MessageTextCommandReferenceVector::PushBack(const MessageTextCommandReference& source) { PushReference(storage, source); }
std::size_t MessageTextCommandReferenceVector::size() const noexcept { return ReferenceDistance(storage.begin, storage.end); }
std::size_t MessageTextCommandReferenceVector::capacity() const noexcept { return ReferenceDistance(storage.begin, storage.capacity); }
void MessageTextCommandReferenceVector::clear() noexcept { ClearReferences(storage); }
void MessageTextCommandReferenceVector::DestroyStorage() noexcept { DestroyReferences(storage); }

MessageRawFormatReferenceVector::~MessageRawFormatReferenceVector() noexcept { DestroyStorage(); }
void MessageRawFormatReferenceVector::PushBack(const MessageRawFormatRef& source) { PushReference(storage, source); }
std::size_t MessageRawFormatReferenceVector::size() const noexcept { return ReferenceDistance(storage.begin, storage.end); }
std::size_t MessageRawFormatReferenceVector::capacity() const noexcept { return ReferenceDistance(storage.begin, storage.capacity); }
void MessageRawFormatReferenceVector::clear() noexcept { ClearReferences(storage); }
void MessageRawFormatReferenceVector::DestroyStorage() noexcept { DestroyReferences(storage); }
}
