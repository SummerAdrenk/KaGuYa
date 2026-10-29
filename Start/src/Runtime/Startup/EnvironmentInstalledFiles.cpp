#include <Runtime/EnvironmentInstalledFiles.h>

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
using Slot = YtlReferenceSlotRuntime;
constexpr std::uint32_t MaximumReferences = 0x15555555u;

std::uint32_t Distance(const Slot* begin, const Slot* end) noexcept
{
    const auto bytes = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(end) -
        reinterpret_cast<std::uintptr_t>(begin));
    return static_cast<std::uint32_t>(std::bit_cast<std::int32_t>(bytes) / 12);
}

Slot* AllocateStorage(std::uint32_t capacity, IEnvironmentRootMemory& memory)
{
    const auto bytes = capacity <= MaximumReferences ? capacity * 12u : 0xFFFFFFFFu;
    if (bytes < 0x1000u)
        return bytes ? static_cast<Slot*>(memory.AllocateBody(bytes)) : nullptr;
    const auto request = bytes > 0xFFFFFFFFu - 0x23u ? 0xFFFFFFFFu : bytes + 0x23u;
    auto* raw = memory.AllocateBody(request);
    // 453D82..453D90：仅大块分配分支检查返回空指针。
    if (!raw) _invalid_parameter_noinfo_noreturn();
    const auto address = (reinterpret_cast<std::uintptr_t>(raw) + 0x23u) & ~std::uintptr_t{31};
    std::memcpy(reinterpret_cast<void*>(address - 4u), &raw, sizeof(raw));
    return reinterpret_cast<Slot*>(address);
}

void DeleteStorage(Slot* begin, std::uint32_t capacity, IEnvironmentRootMemory& memory)
{
    auto bytes = capacity * 12u;
    void* allocation = begin;
    if (bytes >= 0x1000u)
    {
        const auto address = reinterpret_cast<std::uintptr_t>(begin);
        std::uintptr_t raw;
        std::memcpy(&raw, reinterpret_cast<const void*>(address - 4u), sizeof(raw));
        bytes += 0x23u;
        if (address - raw - 4u > 0x1Fu) _invalid_parameter_noinfo_noreturn();
        allocation = reinterpret_cast<void*>(raw);
    }
    memory.DeleteVector(allocation, bytes); // 491480 / 53FBFB：传入调整后的大小。
}

void ReleaseRange(Slot* begin, Slot* end) noexcept
{
    // 452200：正序释放；活动释放状态的异常展开进入 terminate。
    for (auto* cursor = begin; cursor != end; ++cursor) ReleaseYtlReference(*cursor);
}

void CopyRange(const Slot* begin, const Slot* end, Slot* destination) noexcept
{
    // 453E00..453E28 / 4900A0：不移动、不复制 CString、不调用回调、不填充备用槽。
    for (; begin != end; ++begin, ++destination)
        CopyEnvironmentInstalledFileReference(*destination, *begin);
}

void GrowAndInsert(EnvironmentVectorStorage<Slot>& storage, Slot* position,
    const Slot& source, IEnvironmentRootMemory& memory)
{
    const auto index = Distance(storage.begin, position);
    const auto size = Distance(storage.begin, storage.end);
    if (size == MaximumReferences) throw std::length_error("vector<T> too long");
    const auto required = size + 1u;
    const auto capacity = Distance(storage.begin, storage.capacity);
    // 453D3D：增长溢出时选择 required，不选择 MaximumReferences。
    const auto grown = capacity > MaximumReferences - capacity / 2u ? required : capacity + capacity / 2u;
    const auto nextCapacity = (std::max)(required, grown);
    auto* next = AllocateStorage(nextCapacity, memory);
    auto* inserted = next + index;
    auto* cleanupBegin = inserted;
    try
    {
        CopyEnvironmentInstalledFileReference(*inserted, source);
        if (position == storage.end)
            CopyRange(storage.begin, storage.end, next);
        else
        {
            // 453E3B..453E54：保留共享辅助函数的非尾部插入分支，
            // 包括先取得 position、随后分配回调改变 end 的情况。
            CopyRange(storage.begin, position, next);
            cleanupBegin = next;
            CopyRange(position, storage.end, inserted + 1);
        }
    }
    catch (...)
    {
        // 453EE3..453F03 / 5EC138：仅捕获构造阶段异常。DWORD 复制循环
        // 本身不会抛出 C++ 异常，不额外虚构可抛异常的钩子。
        ReleaseRange(cleanupBegin, inserted + 1);
        DeleteStorage(next, nextCapacity, memory);
        throw;
    }
    // 453E5B 在旧元素及存储释放前置 state=-1；此阶段失败不回滚。
    if (storage.begin)
    {
        ReleaseRange(storage.begin, storage.end);
        DeleteStorage(storage.begin, Distance(storage.begin, storage.capacity), memory);
    }
    storage.begin = next;
    storage.end = next + required;
    storage.capacity = next + nextCapacity;
}
}

EnvironmentInstalledFile* CreateEnvironmentInstalledFileAt46A16C(IRuntimeStringManagerSource& strings)
{
    auto* payload = static_cast<EnvironmentInstalledFile*>(::operator new(sizeof(EnvironmentInstalledFile)));
    if (!payload) return nullptr;
    auto* manager = strings.CaptureAt417360();
    if (!manager) throw ATL::CAtlException(E_FAIL); // 46A30D
    std::construct_at(&payload->stringAt00, manager);
    manager = strings.CaptureAt417360();
    if (!manager) throw ATL::CAtlException(E_FAIL); // 46A317：不清理第一个字符串。
    std::construct_at(&payload->stringAt04, manager);
    return payload;
}

void InitializeEnvironmentInstalledFileReference(Slot& reference, EnvironmentInstalledFile* payload)
{
    reference = {};
    if (!payload) return;
    reference.baseAt00 = reinterpret_cast<std::uintptr_t>(payload);
    // 46A1D7..46A20C：保留未写区和control先于count的发布顺序。
    void* storage = ::operator new(sizeof(EnvironmentInstalledFileControl));
    auto* control = storage ? ::new (storage) EnvironmentInstalledFileControl(payload) : nullptr;
    control->referenceCountAt04 = 1;
    control->owningAt08 = 1;
    control->strideAt0C = 8;
    reference.controlAt08 = control;
    control->countAt10 = 1;
}

void CopyEnvironmentInstalledFileReference(Slot& destination, const Slot& source) noexcept
{
    destination.baseAt00 = source.baseAt00;
    destination.biasAt04 = source.biasAt04;
    destination.controlAt08 = source.controlAt08;
    if (destination.controlAt08) ++destination.controlAt08->referenceCountAt04;
}

EnvironmentInstalledFile* ResolveEnvironmentInstalledFile(const Slot& reference) noexcept
{
    // 458F26..458F42 / 45901B..45904D：control 为空时 stride 为零，并非正文为空。
    return ResolveEnvironmentObjectReference<EnvironmentInstalledFile>(reference);
}

EnvironmentInstalledFiles::~EnvironmentInstalledFiles() noexcept { DestroyStorage(); }

void EnvironmentInstalledFiles::AppendCopy(const Slot& source, IEnvironmentRootMemory& memory)
{
    auto* end = storage.end;
    if (end != storage.capacity)
    {
        CopyEnvironmentInstalledFileReference(*end, source);
        ++storage.end;
        return;
    }
    GrowAndInsert(storage, end, source, memory);
}

void EnvironmentInstalledFiles::clear() noexcept
{
    ReleaseRange(storage.begin, storage.end);
    storage.end = storage.begin;
}

void EnvironmentInstalledFiles::DestroyStorage(IEnvironmentRootMemory& memory) noexcept
{
    // 496FAB：begin 为空时不改写另两个字段。
    if (!storage.begin) return;
    ReleaseRange(storage.begin, storage.end);
    // 497007：正文回调结束后重新读取 begin/capacity。
    DeleteStorage(storage.begin, Distance(storage.begin, storage.capacity), memory);
    storage = {nullptr, nullptr, nullptr};
}

std::size_t EnvironmentInstalledFiles::size() const noexcept { return Distance(storage.begin, storage.end); }
std::span<Slot> EnvironmentInstalledFiles::References() noexcept { return {storage.begin, size()}; }
std::span<const Slot> EnvironmentInstalledFiles::References() const noexcept { return {storage.begin, size()}; }
}
