#include <Runtime/RuntimeCStringVector.h>

#include <cstdlib>
#include <cstring>
#include <limits>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Storage = EnvironmentVectorStorage<RuntimeCString>;
static_assert(sizeof(void*) == 4 && sizeof(RuntimeCString) == 4);
static_assert(sizeof(ATL::CStringData) == 16);
static_assert(offsetof(ATL::CStringData, pStringMgr) == 0);
static_assert(offsetof(ATL::CStringData, nDataLength) == 4);
static_assert(offsetof(ATL::CStringData, nRefs) == 12);
constexpr std::uint32_t MaximumCount = 0x3FFFFFFFu;

std::uintptr_t Address(const void* pointer) noexcept
{
    return reinterpret_cast<std::uintptr_t>(pointer);
}

RuntimeCString* Advance(RuntimeCString* pointer, std::uint32_t count) noexcept
{
    return reinterpret_cast<RuntimeCString*>(Address(pointer) + count * 4u);
}

std::uint32_t Count(RuntimeCString* begin, RuntimeCString* end) noexcept
{
    const std::uint32_t bytes = Address(end) - Address(begin);
    // 456EC1/456EC4：DWORD 相减后 SAR，不使用 C++ 指针相减。
    return (bytes >> 2u) | ((bytes & 0x80000000u) ? 0xC0000000u : 0u);
}

ATL::CStringData* Header(const RuntimeCString* element) noexcept
{
    const char* data;
    std::memcpy(&data, element, sizeof(data));
    return reinterpret_cast<ATL::CStringData*>(Address(data) - 16u);
}

char* Data(ATL::CStringData* header) noexcept
{
    return reinterpret_cast<char*>(Address(header) + 16u);
}

void Publish(RuntimeCString* element, ATL::CStringData* header) noexcept
{
    char* const data = Data(header);
    // 直接发布原始字符串槽；v143 构造函数会经过带 throw() 声明的管理器槽。
    std::memcpy(static_cast<void*>(element), &data, sizeof(data));
}

template<class Function>
Function ManagerSlot(ATL::IAtlStringMgr* manager, std::size_t offset) noexcept
{
    const std::byte* table;
    // 读取原生vptr，保持管理器虚槽及异常传播边界。
    std::memcpy(&table, static_cast<const void*>(manager), sizeof(table));
    Function function;
    static_assert(sizeof(function) == sizeof(void*));
    std::memcpy(&function, table + offset, sizeof(function));
    return function;
}

RuntimeCString* ConstructAt466830(RuntimeCString* next, std::uint32_t count,
    IRuntimeStringManagerSource& strings)
{
    // 43A7E0 与 466830 的逐串 Capture/GetNil/发布/递增次序相同，均无局部 EH。
    using GetNil = ATL::CStringData* (__thiscall*)(ATL::IAtlStringMgr*);
    for (; count; --count, next = Advance(next, 1))
    {
        auto* manager = strings.CaptureAt417360();
        if (!manager) throw ATL::CAtlException(E_FAIL);
        Publish(next, ManagerSlot<GetNil>(manager, 12)(manager));
    }
    return next; // 无局部前缀清理；只有正常返回后，调用者才更新完成指针。
}

void DestroyRangeAt43A440(RuntimeCString* begin, RuntimeCString* end)
{
    for (; begin != end; begin = Advance(begin, 1))
        DestroyRuntimeCStringAt40CD40(begin);
}

RuntimeCString* CopyAt40AE60(RuntimeCString* begin, RuntimeCString* end, RuntimeCString* destination)
{
    struct PrefixCleanup
    {
        RuntimeCString* begin;
        RuntimeCString* completed;
        bool active = true;
        ~PrefixCleanup() noexcept
        {
            // 590E00 -> 40CE20；5E2668 EHFlags=5，清理失败先终止，不进入外层 catch。
            if (active) DestroyRangeAt43A440(begin, completed);
        }
    } cleanup{destination, destination};
    for (; begin != end; begin = Advance(begin, 1))
    {
        Publish(cleanup.completed, CloneRuntimeCStringDataAt414540(Header(begin)));
        cleanup.completed = Advance(cleanup.completed, 1);
    }
    cleanup.active = false;
    return cleanup.completed;
}

RuntimeCString* AllocateAt43B1D0(std::uint32_t count, IEnvironmentRootMemory& memory)
{
    constexpr auto maximum = (std::numeric_limits<std::uint32_t>::max)();
    const std::uint32_t bytes = count > MaximumCount ? maximum : count * 4u;
    if (bytes < 0x1000u)
        return bytes ? static_cast<RuntimeCString*>(memory.AllocateBody(bytes)) : nullptr;
    const std::uint32_t extended = bytes + 0x23u;
    void* raw = memory.AllocateBody(extended <= bytes ? maximum : extended);
    if (!raw) _invalid_parameter_noinfo_noreturn();
    const auto aligned = (Address(raw) + 0x23u) & 0xFFFFFFE0u;
    std::memcpy(reinterpret_cast<void*>(aligned - 4u), &raw, sizeof(raw));
    return reinterpret_cast<RuntimeCString*>(aligned);
}

void DeleteStorage(RuntimeCString* begin, std::uint32_t bytes, IEnvironmentRootMemory& memory)
{
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

void ReleaseCurrentStorage(Storage& storage, IEnvironmentRootMemory& memory)
{
    if (!storage.begin) return;
    DestroyRangeAt43A440(storage.begin, storage.end);
    // 43A3C5/43A712：元素回调可能改写字段；释放存储时重读，不沿用初始范围。
    const std::uint32_t bytes = (Address(storage.capacity) - Address(storage.begin)) & 0xFFFFFFFCu;
    DeleteStorage(storage.begin, bytes, memory);
}
}

Storage* ConstructRuntimeCStringVectorAt40B7A0(Storage& destination, std::uint32_t count,
    IRuntimeStringManagerSource& strings, IEnvironmentRootMemory& memory)
{
    destination.begin = nullptr;
    destination.end = nullptr;
    destination.capacity = nullptr;
    if (!count) return &destination;
    if (count > MaximumCount) throw std::length_error("vector<T> too long");
    auto* const next = AllocateAt43B1D0(count, memory);
    destination.begin = next;
    destination.end = next;
    try
    {
        // 40B803 取得 state0 后才写 capacity；43A7E0 正常返回才更新 end。
        destination.capacity = Advance(destination.begin, count);
        destination.end = ConstructAt466830(destination.begin, count, strings);
    }
    catch (...)
    {
        // 40B830 的普通 catch：end 尚未含部分 nil 串；清理异常可替换原异常。
        DestroyRuntimeCStringVectorAt43A700(destination, memory);
        throw;
    }
    return &destination;
}

void ResizeRuntimeCStringVectorAt456E80(Storage& storage, std::uint32_t count,
    IRuntimeStringManagerSource& strings, IEnvironmentRootMemory& memory)
{
    auto* const oldEnd = storage.end;
    auto* const oldBegin = storage.begin;
    const auto size = Count(oldBegin, oldEnd);
    const auto capacity = Count(oldBegin, storage.capacity);
    if (count <= capacity)
    {
        if (count > size)
            storage.end = ConstructAt466830(oldEnd, count - size, strings);
        else if (count < size)
        {
            auto* const end = Advance(oldBegin, count);
            DestroyRangeAt43A440(end, oldEnd);
            storage.end = end;
        }
        return;
    }

    if (count > MaximumCount) throw std::length_error("vector<T> too long");
    const auto half = capacity >> 1u;
    const auto grown = capacity > MaximumCount - half ? count : capacity + half;
    const auto nextCapacity = grown < count ? count : grown;
    auto* const next = AllocateAt43B1D0(nextCapacity, memory);
    auto* const tail = Advance(next, size);
    auto* completedTail = tail;
    try
    {
        completedTail = ConstructAt466830(tail, count - size, strings);
        // 456F32/456F35：全部尾段回调返回后，才捕获当前旧范围用于复制。
        CopyAt40AE60(storage.begin, storage.end, next);
    }
    catch (...)
    {
        // 456FB7 是普通 catch，不是 noexcept 清理；清理失败以新异常替换原异常。
        DestroyRangeAt43A440(tail, completedTail);
        // 48FDD0 仅返回 ECX；43B390 按保存的新容量释放新块。
        DeleteStorage(next, nextCapacity * 4u, memory);
        throw;
    }
    // 456F3F 在 43A3B0 前撤销新块清理资格；旧 Free 失败不能回滚新块。
    ReleaseCurrentStorage(storage, memory);
    storage.begin = next;
    storage.end = Advance(next, count);
    storage.capacity = Advance(next, nextCapacity);
}

void AppendRuntimeCStringAt41D5E9(Storage& storage, const RuntimeCString& source,
    IEnvironmentRootMemory& memory)
{
    auto* const position = storage.end;
    if (position != storage.capacity)
    {
        // 41D5FD缓存目标槽；41D608却对回调后的当前end加4，不发布position+1。
        Publish(position, CloneRuntimeCStringDataAt414540(Header(&source)));
        storage.end = Advance(storage.end, 1);
        return;
    }

    auto* const oldBegin = storage.begin;
    const auto size = Count(oldBegin, storage.end);
    const auto index = Count(oldBegin, position);
    // 408602仅判相等；不能改成resize的count > MaximumCount。
    if (size == MaximumCount) throw std::length_error("vector<T> too long");
    const auto nextSize = size + 1u;
    const auto capacity = Count(oldBegin, storage.capacity);
    const auto half = capacity >> 1u;
    const auto grown = capacity > MaximumCount - half ? nextSize : capacity + half;
    const auto nextCapacity = grown < nextSize ? nextSize : grown;
    auto* const next = AllocateAt43B1D0(nextCapacity, memory);
    auto* const inserted = Advance(next, index);
    auto* const afterInserted = Advance(inserted, 1);
    auto* cleanupBegin = afterInserted;
    try
    {
        // 408661在存储分配后才读取source槽；414540内部固定header跨Clone/Allocate。
        auto* const cloned = CloneRuntimeCStringDataAt414540(Header(&source));
        cleanupBegin = inserted;
        Publish(inserted, cloned);
        auto* const currentEnd = storage.end;
        if (position == currentEnd)
            CopyAt40AE60(storage.begin, currentEnd, next);
        else
        {
            // 43A8E0逐指令核实也是Clone复制，且同经40CE20清已完成前缀，不是move。
            // 插入位置保持入口值；首段回调返回后，第二段才重读end。
            CopyAt40AE60(storage.begin, position, next);
            cleanupBegin = next;
            CopyAt40AE60(position, storage.end, afterInserted);
        }
    }
    catch (...)
    {
        // 4086E2普通catch：尾值/首段清理或Delete抛出会替换原异常，不得加noexcept。
        DestroyRangeAt43A440(cleanupBegin, afterInserted);
        DeleteStorage(next, nextCapacity * 4u, memory);
        throw;
    }
    // 4086BC已撤销新块清理资格；旧Free失败不回滚。43A3B0在释放后才发布三指针。
    ReleaseCurrentStorage(storage, memory);
    storage.begin = next;
    storage.end = Advance(next, nextSize);
    storage.capacity = Advance(next, nextCapacity);
}

void CopyRuntimeCStringVectorAt40B6E0(Storage& destination, const Storage& source,
    IEnvironmentRootMemory& memory)
{
    // 原构造先写空目的，再读源数量；不提前保留源快照或对自别名增设特殊分支。
    destination.begin = nullptr;
    destination.end = nullptr;
    destination.capacity = nullptr;
    const auto count = Count(source.begin, source.end);
    if (!count) return;
    if (count > MaximumCount) throw std::length_error("vector<T> too long");
    auto* const next = AllocateAt43B1D0(count, memory);
    destination.begin = next;
    destination.end = next;
    try
    {
        // 40B749 的 state0 已生效；容量用缓存数量，复制范围却在分配回调后重新读取。
        destination.capacity = Advance(destination.begin, count);
        destination.end = CopyAt40AE60(source.begin, source.end, destination.begin);
    }
    catch (...)
    {
        // 5DF06C try[0,0] -> 40B77E：前缀已由 40CE20 清理，这里释放目的当前存储。
        // 43A700 不带 noexcept；不能让 40DD20 吞掉本应外传的清理异常。
        DestroyRuntimeCStringVectorAt43A700(destination, memory);
        throw;
    }
}

void DestroyRuntimeCStringVectorAt43A700(Storage& storage, IEnvironmentRootMemory& memory)
{
    if (!storage.begin) return;
    ReleaseCurrentStorage(storage, memory);
    storage.begin = nullptr;
    storage.end = nullptr;
    storage.capacity = nullptr;
}

void DestroyRuntimeCStringVectorAt40DD20(Storage& storage, IEnvironmentRootMemory& memory) noexcept
{
    // 5DF090 flags5 的既有边界保持；raw 43A700 本身不增加终止语义。
    DestroyRuntimeCStringVectorAt43A700(storage, memory);
}
}
