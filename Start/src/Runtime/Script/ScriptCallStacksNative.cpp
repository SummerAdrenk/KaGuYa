#include <Runtime/ScriptCallStacksNative.h>

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
constexpr std::uint32_t MaximumLocal = 0x3FFFFFFFu;
constexpr std::uint32_t MaximumFiles = 0x15555555u;

std::uint32_t ByteDistance(const void* begin, const void* end) noexcept
{
    return static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(end) -
        reinterpret_cast<std::uintptr_t>(begin));
}
template<class T> std::uint32_t Distance(const T* begin, const T* end) noexcept
{
    const auto bytes = std::bit_cast<std::int32_t>(ByteDistance(begin, end));
    if constexpr (sizeof(T) == 4) return static_cast<std::uint32_t>(bytes >> 2);
    else return static_cast<std::uint32_t>(bytes / 12);
}
template<class T> T* AllocateStorage(std::uint32_t count, std::uint32_t maximum,
    IEnvironmentRootMemory& memory)
{
    const auto bytes = count <= maximum ? count * static_cast<std::uint32_t>(sizeof(T)) : 0xFFFFFFFFu;
    if (bytes < 0x1000u)
    {
        if (!bytes) return nullptr;
        auto* result = static_cast<T*>(memory.AllocateBody(bytes));
        // 原小块null随后非法写入；显式终止，不产生空栈成功结果。
        if (!result) _invalid_parameter_noinfo_noreturn();
        return result;
    }
    const auto request = bytes > 0xFFFFFFFFu - 0x23u ? 0xFFFFFFFFu : bytes + 0x23u;
    void* raw = memory.AllocateBody(request);
    if (!raw) _invalid_parameter_noinfo_noreturn();
    const auto address = (reinterpret_cast<std::uintptr_t>(raw) + 0x23u) & ~std::uintptr_t{31};
    std::memcpy(reinterpret_cast<void*>(address - 4u), &raw, sizeof(raw));
    return reinterpret_cast<T*>(address);
}
void DeleteStorage(void* begin, std::uint32_t bytes, IEnvironmentRootMemory& memory)
{
    void* raw = begin;
    if (bytes >= 0x1000u)
    {
        const auto address = reinterpret_cast<std::uintptr_t>(begin);
        std::uintptr_t allocation;
        std::memcpy(&allocation, reinterpret_cast<const void*>(address - 4u), sizeof(allocation));
        if (address - allocation - 4u > 0x1Fu) _invalid_parameter_noinfo_noreturn();
        raw = reinterpret_cast<void*>(allocation);
        bytes += 0x23u;
    }
    memory.DeleteVector(raw, bytes);
}
std::uint32_t NextCapacity(std::uint32_t required, std::uint32_t capacity, std::uint32_t maximum)
{
    const auto grown = capacity > maximum - capacity / 2u ? required : capacity + capacity / 2u;
    return (std::max)(required, grown);
}
void CopyReference(Slot& destination, const Slot& source) noexcept
{
    destination.baseAt00 = source.baseAt00;
    destination.biasAt04 = source.biasAt04;
    destination.controlAt08 = source.controlAt08;
    if (destination.controlAt08) ++destination.controlAt08->referenceCountAt04;
}
void CopyReferences(const Slot* begin, const Slot* end, Slot* destination) noexcept
{
    // 4D6FC0：正序三字段复制，末尾4C8570只处理空区间。
    for (; begin != end; ++begin, ++destination) CopyReference(*destination, *begin);
}
void ReleaseReferences(Slot* begin, Slot* end) noexcept
{
    // 4C8570/48F830：每个Destroy回调之后重读当前槽control，异常进入terminate。
    for (; begin != end; ++begin) ReleaseYtlReference(*begin);
}
void MoveDwords(std::uint32_t* destination, const std::uint32_t* begin,
    const std::uint32_t* end) noexcept
{
    const auto bytes = ByteDistance(begin, end);
    if (bytes) std::memmove(destination, begin, bytes);
}
class StackControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit StackControl(ScriptStackFrameNative* body) noexcept
        : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        // 4D74F0：通过正文IArchive+04虚槽删除，不静态delete，也不把null交给虚调用。
        if (payloadAt14) static_cast<IEnvironmentArchiveRuntime*>(payloadAt14)->DeleteAtVslot04(1);
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    { RetireEnvironmentObjectControl(this, flags); }
};
static_assert(sizeof(StackControl) == 0x18);
}

ScriptStackFrameNative::ScriptStackFrameNative(std::int32_t scriptId, std::uint32_t returnPc) noexcept
    : scriptIdAt04(scriptId), returnPcAt08(returnPc) {}

void ScriptStackFrameNative::Serialize(SerializeBuffer& archive)
{
    // 4D5760：两次+3C，第一项失败不能触及第二项；CSerialize仍用既有现代适配器。
    archive.SerializeSlot3CDword(reinterpret_cast<std::uint32_t&>(scriptIdAt04));
    archive.SerializeSlot3CDword(returnPcAt08);
}
void* ScriptStackFrameNative::DeleteAtVslot04(std::uint32_t flags) noexcept
{
    this->~ScriptStackFrameNative();
    if (flags & 1u) ::operator delete(this, sizeof(ScriptStackFrameNative));
    return this;
}
void CreateScriptStackReference(Slot& slot, ScriptStackFrameNative* body, IEnvironmentRootMemory& memory)
{
    slot = {};
    if (!body) return;
    slot.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    void* allocation = memory.AllocateBody(0x18);
    auto* control = allocation ? ::new (allocation) StackControl(body) : nullptr;
    if (!control) throw std::bad_alloc{}; // 原空control写入的明确安全拒绝。
    control->referenceCountAt04 = 1;
    control->owningAt08 = 1;
    control->strideAt0C = 0x0C;
    slot.controlAt08 = control;
    control->countAt10 = 1;
}
void ReleaseScriptStackReferenceAt4C9330(Slot& slot) noexcept { ReleaseYtlReference(slot); }
void ReleaseCapturedScriptStackControl(YtlReferenceControlRuntime* control) noexcept
{
    if (control && --control->referenceCountAt04 == 0)
    {
        if (control->owningAt08) control->DestroyPayloadAtVslot00();
        control->DeleteControlAtVslot04(1);
    }
}

ScriptDwordVectorStorage::~ScriptDwordVectorStorage() noexcept { DestroyStorage(); }
void ScriptDwordVectorStorage::Push(std::uint32_t value, IEnvironmentRootMemory& memory)
{
    auto* position = storage.end;
    if (position != storage.capacity) { *position = value; ++storage.end; return; }
    GrowAndInsertAt453A70(position, value, memory);
}
void ScriptDwordVectorStorage::GrowAndInsertAt453A70(std::uint32_t* position,
    const std::uint32_t& source, IEnvironmentRootMemory& memory)
{
    const auto index = Distance(storage.begin, position);
    const auto size = Size();
    if (size == MaximumLocal) throw std::length_error("vector<T> too long");
    const auto required = size + 1u;
    const auto capacity = NextCapacity(required, Distance(storage.begin, storage.capacity), MaximumLocal);
    auto* next = AllocateStorage<std::uint32_t>(capacity, MaximumLocal, memory);
    // 453AE1：分配后读source；分配回调也可能改变end，不能只实现尾插入。
    next[index] = source;
    if (position == storage.end) MoveDwords(next, storage.begin, storage.end);
    else
    {
        MoveDwords(next, storage.begin, position);
        MoveDwords(next + index + 1, position, storage.end);
    }
    // 48F6C0无构造回滚：删除旧存储成功之后才发布新三指针。
    if (storage.begin) DeleteStorage(storage.begin, ByteDistance(storage.begin, storage.capacity) & ~3u, memory);
    storage.begin = next;
    storage.end = next + required;
    storage.capacity = next + capacity;
}
std::uint32_t ScriptDwordVectorStorage::Size() const noexcept { return Distance(storage.begin, storage.end); }
void ScriptDwordVectorStorage::clear() noexcept { storage.end = storage.begin; }
void ScriptDwordVectorStorage::DestroyStorage(IEnvironmentRootMemory& memory) noexcept
{
    if (!storage.begin) return;
    DeleteStorage(storage.begin, ByteDistance(storage.begin, storage.capacity) & ~3u, memory);
    storage = {};
}

ScriptFileCallStorage::~ScriptFileCallStorage() noexcept { DestroyStorage(); }
void ScriptFileCallStorage::Append(const Slot& source, IEnvironmentRootMemory& memory)
{
    auto* position = storage.end;
    if (position != storage.capacity) { CopyReference(*position, source); ++storage.end; return; }
    GrowAndInsertAt4C8800(position, source, memory);
}
void ScriptFileCallStorage::GrowAndInsertAt4C8800(Slot* position, const Slot& source,
    IEnvironmentRootMemory& memory)
{
    const auto index = Distance(storage.begin, position);
    const auto size = Size();
    if (size == MaximumFiles) throw std::length_error("vector<T> too long");
    const auto required = size + 1u;
    const auto capacity = NextCapacity(required, Distance(storage.begin, storage.capacity), MaximumFiles);
    auto* next = AllocateStorage<Slot>(capacity, MaximumFiles, memory);
    auto* inserted = next + index;
    auto* cleanupBegin = inserted;
    try
    {
        CopyReference(*inserted, source);
        if (position == storage.end) CopyReferences(storage.begin, storage.end, next);
        else
        {
            CopyReferences(storage.begin, position, next);
            cleanupBegin = next;
            CopyReferences(position, storage.end, inserted + 1);
        }
    }
    catch (...)
    {
        ReleaseReferences(cleanupBegin, inserted + 1);
        DeleteStorage(next, capacity * 12u, memory);
        throw;
    }
    // 4C897D已撤构造清理资格；旧存储删除抛出不回收尚未发布的新范围。
    if (storage.begin)
    {
        ReleaseReferences(storage.begin, storage.end);
        DeleteStorage(storage.begin, Distance(storage.begin, storage.capacity) * 12u, memory);
    }
    storage.begin = next;
    storage.end = next + required;
    storage.capacity = next + capacity;
}
void ScriptFileCallStorage::PopReferenceAt4D4B40(Slot& result) noexcept
{
    result = {};
    auto* end = storage.end;
    if (!Distance(storage.begin, end)) return;
    if (&result != end - 1) CopyReference(result, end[-1]);
    end = storage.end;
    // 4D4BFD..4D4C11：仅原control非空才在释放后重读end；bias不清零。
    if (end[-1].controlAt08)
    {
        ReleaseYtlReference(end[-1]);
        end = storage.end;
    }
    storage.end = end - 1;
}
std::uint32_t ScriptFileCallStorage::Size() const noexcept { return Distance(storage.begin, storage.end); }
void ScriptFileCallStorage::clear() noexcept
{
    ReleaseReferences(storage.begin, storage.end);
    storage.end = storage.begin;
}
void ScriptFileCallStorage::DestroyStorage(IEnvironmentRootMemory& memory) noexcept
{
    if (!storage.begin) return;
    ReleaseReferences(storage.begin, storage.end);
    DeleteStorage(storage.begin, Distance(storage.begin, storage.capacity) * 12u, memory);
    storage = {};
}
}
