#include <Runtime/ParamsPlaceArray.h>
#include <Runtime/EnvironmentObjectReference.h>

#include <array>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <new>
#include <stdexcept>
#include <string_view>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
static_assert(sizeof(std::uintptr_t) == 4);
static_assert(alignof(ParamsPlaceEntry) <= alignof(std::uint32_t));

std::uint32_t AllocationBytes(std::uint32_t count) noexcept
{
    // 41D0C8..41D0DB：乘八或加四溢出均饱和到 UINT_MAX，不预先拒绝大数量。
    constexpr auto maximum = (std::numeric_limits<std::uint32_t>::max)();
    return count > (maximum - 4u) / 8u ? maximum : count * 8u + 4u;
}

ParamsPlaceEntry* Advance(ParamsPlaceEntry* elements, std::uint32_t index) noexcept
{
    return reinterpret_cast<ParamsPlaceEntry*>(
        reinterpret_cast<std::uintptr_t>(elements) + index * 8u);
}

void ConstructPlaceAt40C930(ParamsPlaceEntry* element, IRuntimeStringManagerSource& strings) noexcept
{
    // PE FuncInfo 5DFB50：maxState=0、EHFlags=5，整个构造入口 noexcept。
    // Capture/GetNil 失败在此终止，不外传给数组 iterator，也不回滚已构造前缀。
    // 原 40C968 仅写 name 四字节；不构造整个聚合，更不写 valueAt04。
    InitializeEmptyRuntimeCString(element->nameAt00, strings);
}

void DestroyPlaceAt40FCE0(ParamsPlaceEntry* element) noexcept
{
    // PE FuncInfo 5DFB74 同为 maxState=0、EHFlags=5；原始 Free 抛出在此终止。
    DestroyRuntimeCStringAt40CD40(&element->nameAt00);
}

void DestroyPlacePayloadAt43C405(ParamsPlaceEntry* elements, std::uint32_t* cookie,
    std::uint32_t count) noexcept
{
    // 43C405 的 state0 -> 541E55，覆盖逆序 iterator 及后续数组删除。
    // 初始数量已缓存；元素回调不能改变本次析构范围。
    while (count) DestroyPlaceAt40FCE0(Advance(elements, --count));
    // 43C414 重新读取 cookie；释放大小按 DWORD 回绕，不复用分配时的饱和计算。
    const std::uint32_t bytes = *cookie * 8u + 4u;
    // 43C41F -> 53FC17 是数组 sized delete；不得换成普通 delete 或再次跑元素析构。
    ::operator delete[](cookie, static_cast<std::size_t>(bytes));
}

class RetiredPlaceControl final : public EnvironmentObjectReferenceControl
{
public:
    RetiredPlaceControl() noexcept
        : EnvironmentObjectReferenceControl(nullptr, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override { _purecall(); }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        // 退休表 5ADD18 的 slot04=412460，后续删除传 14h，而不是原派生 control 的 18h。
        if (flags & 1u) ::operator delete(this, std::size_t{0x14});
    }
};
static_assert(sizeof(RetiredPlaceControl) == 0x18);

void RetirePlaceControlAt411140(EnvironmentObjectReferenceControl* control, std::uint32_t flags)
{
    // 原 41114A：flags=0 也退化虚表，其余二十字节（含 padding/payload）原样保留。
    std::array<std::byte, 20> fields;
    std::memcpy(fields.data(), reinterpret_cast<const std::byte*>(control) + 4, fields.size());
    auto* retired = ::new (static_cast<void*>(control)) RetiredPlaceControl;
    std::memcpy(reinterpret_cast<std::byte*>(retired) + 4, fields.data(), fields.size());
    // 411155 -> 53FBFB：本次派生删除专属尺寸为 18h，不借用退休后的 14h 入口。
    if (flags & 1u) ::operator delete(retired, std::size_t{0x18});
}

class PlaceArrayControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit PlaceArrayControl(ParamsPlaceEntry* elements) noexcept
        : EnvironmentObjectReferenceControl(elements, UninitializedFieldsTag{}) {}

    void DestroyPayloadAtVslot00() override
    {
        // 原真实表 5AE2F8={43C3D0,411140}；payload 从 control+14 取，不依赖 Ref 的 base/bias。
        auto* const elements = static_cast<ParamsPlaceEntry*>(payloadAt14);
        if (!elements) return;
        auto* const cookie = reinterpret_cast<std::uint32_t*>(elements) - 1;
        const auto count = *cookie;
        // FuncInfo 5E1E54：maxState=1、unwind=5E1E4C、flags=1，不是整体 flags5。
        // 5E1E4C={-1,541E55}；只将已经进入 state0 的 iterator/delete 放进终止块。
        DestroyPlacePayloadAt43C405(elements, cookie, count);
    }

    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        RetirePlaceControlAt411140(this, flags);
    }
};
static_assert(sizeof(PlaceArrayControl) == 0x18);

std::int32_t SnapshotCount(std::size_t count)
{
    // 仅保护现代快照：不得截断为负数后把非空输入误当成清表请求。
    if (count > static_cast<std::size_t>((std::numeric_limits<std::int32_t>::max)()))
        throw std::length_error("Params place snapshot exceeds signed array count");
    return static_cast<std::int32_t>(count);
}

void SetSnapshotString(RuntimeCString& destination, std::string_view bytes)
{
    if (bytes.size() > static_cast<std::size_t>((std::numeric_limits<int>::max)()))
        throw std::length_error("Params place snapshot exceeds CString length");
    // 重新写入完整 cp932 字节，保留嵌入 NUL，不共享源的非空 header 或保存 utf16 镜像。
    SetRuntimeCStringAt437680(destination, bytes.data(), static_cast<int>(bytes.size()));
}
}

ParamsPlaceArray::~ParamsPlaceArray() noexcept
{
    // GS 40F3FB..40F437：退出所属析构的异常终止；显式 Rebuild 的释放则允许外传。
    ReleaseYtlReference(storage);
}

void ParamsPlaceArray::RebuildAt41D0B2(std::int32_t count, IRuntimeStringManagerSource& strings)
{
    // 43B910 与此 helper 逐指令一致：回调后重读 control，保留 bias，无 control 不清 base。
    ReleaseYtlReference(storage);
    if (count <= 0) return;
    const auto extent = static_cast<std::uint32_t>(count);
    auto* const cookie = static_cast<std::uint32_t*>(::operator new[](AllocationBytes(extent)));
    ParamsPlaceEntry* elements = nullptr;
    if (cookie)
    {
        *cookie = extent;
        elements = reinterpret_cast<ParamsPlaceEntry*>(cookie + 1);
        for (std::uint32_t index = 0; index < extent; ++index)
            ConstructPlaceAt40C930(Advance(elements, index), strings);
    }

    // 41D10E：构造回调后只删除槽内当前 control，不再减持或释放其 payload。
    if (storage.controlAt08) storage.controlAt08->DeleteControlAtVslot04(1);
    storage.biasAt04 = 0;
    storage.baseAt00 = reinterpret_cast<std::uintptr_t>(elements);
    // 41D12A 才分配 control；此后失败保留已发布正文，不添加数组回收或槽回滚。
    void* const raw = ::operator new(sizeof(PlaceArrayControl));
    auto* const control = raw ? ::new (raw) PlaceArrayControl(elements) : nullptr;
    // uninit tag 只写 vptr/payload；先发布 control，再初始化 refs/owning/stride/count。
    storage.controlAt08 = control;
    // 原 41D14C 对 null control 硬故障；在相同发布点后明确终止，不伪造空表成功。
    if (!control) _invalid_parameter_noinfo_noreturn();
    control->referenceCountAt04 = 1;
    storage.controlAt08->owningAt08 = 1;
    static_cast<EnvironmentObjectReferenceControl*>(storage.controlAt08)->strideAt0C = 8;
    static_cast<EnvironmentObjectReferenceControl*>(storage.controlAt08)->countAt10 = extent;
}

std::size_t ParamsPlaceArray::size() const noexcept
{
    // 只取当前 control.count，不扣 bias，不读 cookie，也不由 base 推测数量。
    return storage.controlAt08 ?
        static_cast<const EnvironmentObjectReferenceControl*>(storage.controlAt08)->countAt10 : 0u;
}

ParamsPlaceEntry* ParamsPlaceArray::Resolve(std::uint32_t index) noexcept
{
    return const_cast<ParamsPlaceEntry*>(std::as_const(*this).Resolve(index));
}

const ParamsPlaceEntry* ParamsPlaceArray::Resolve(std::uint32_t index) const noexcept
{
    // 41D175/41D1A9：两次独立解析；空 base 不读 control，无 control 时仍可借用孤立正文。
    const auto base = storage.baseAt00;
    if (!base) return nullptr;
    const auto stride = storage.controlAt08 ?
        static_cast<const EnvironmentObjectReferenceControl*>(storage.controlAt08)->strideAt0C : 0u;
    const std::uint32_t offset = (storage.biasAt04 + index) * stride;
    return reinterpret_cast<const ParamsPlaceEntry*>(base + offset);
}

ParamsPlaceEntry& ParamsPlaceArray::operator[](std::size_t index)
{
    return const_cast<ParamsPlaceEntry&>(std::as_const(*this)[index]);
}

const ParamsPlaceEntry& ParamsPlaceArray::operator[](std::size_t index) const
{
    const auto* const element = Resolve(static_cast<std::uint32_t>(index));
    if (!element) throw std::out_of_range("Params place reference resolves to null");
    return *element;
}

ParamsPlaceEntry& ParamsPlaceArray::at(std::size_t index)
{
    return const_cast<ParamsPlaceEntry&>(std::as_const(*this).at(index));
}

const ParamsPlaceEntry& ParamsPlaceArray::at(std::size_t index) const
{
    if (index >= size()) throw std::out_of_range("Params place index is out of range");
    return (*this)[index];
}

ParamsPlaceArray::ParamsPlaceArray(std::initializer_list<ParamsNamedValue> values)
{
    AssignSnapshot(std::span<const ParamsNamedValue>(values.begin(), values.size()));
}

ParamsPlaceArray& ParamsPlaceArray::operator=(std::initializer_list<ParamsNamedValue> values)
{
    AssignSnapshot(std::span<const ParamsNamedValue>(values.begin(), values.size()));
    return *this;
}

void ParamsPlaceArray::AssignSnapshot(std::span<const ParamsNamedValue> values)
{
    const auto count = SnapshotCount(values.size());
    ParamsPlaceArray snapshot;
    snapshot.RebuildAt41D0B2(count);
    for (std::size_t index = 0; index < values.size(); ++index)
    {
        auto& destination = snapshot[index];
        SetSnapshotString(destination.nameAt00, values[index].name.cp932);
        destination.valueAt04 = values[index].value;
    }
    // 仅现代快照使用局部拥有者；空快照也替换旧表，不改变生产 reader 的零计数留旧语义。
    *this = std::move(snapshot);
}

ParamsPlaceArray::ParamsPlaceArray(const ParamsPlaceArray& other)
{
    const auto count = other.size();
    ParamsPlaceArray snapshot;
    snapshot.RebuildAt41D0B2(SnapshotCount(count));
    for (std::size_t index = 0; index < count; ++index)
    {
        const auto& source = other[index];
        auto& destination = snapshot[index];
        SetSnapshotString(destination.nameAt00, RuntimeStringView(source.nameAt00));
        // 对 value 复制对象字节，不给尚未填入的槽补零或强制读取一个未初始化的整数值。
        std::memcpy(&destination.valueAt04, &source.valueAt04, sizeof(source.valueAt04));
    }
    storage = std::exchange(snapshot.storage, YtlReferenceSlotRuntime{});
}

ParamsPlaceArray& ParamsPlaceArray::operator=(const ParamsPlaceArray& other)
{
    if (this != &other)
    {
        ParamsPlaceArray snapshot(other);
        *this = std::move(snapshot);
    }
    return *this;
}

ParamsPlaceArray::ParamsPlaceArray(ParamsPlaceArray&& other) noexcept
    : storage(std::exchange(other.storage, YtlReferenceSlotRuntime{})) {}

ParamsPlaceArray& ParamsPlaceArray::operator=(ParamsPlaceArray&& other)
{
    if (this != &other)
    {
        // 释放成功后才读取并转移源槽；回调失败不提前清空源，也不吞掉异常。
        ReleaseYtlReference(storage);
        storage = std::exchange(other.storage, YtlReferenceSlotRuntime{});
    }
    return *this;
}

bool ParamsPlaceArray::operator==(const ParamsPlaceArray& other) const
{
    const auto count = size();
    if (count != other.size()) return false;
    for (std::size_t index = 0; index < count; ++index)
    {
        const auto& left = (*this)[index];
        const auto& right = other[index];
        if (RuntimeStringView(left.nameAt00) != RuntimeStringView(right.nameAt00) ||
            left.valueAt04 != right.valueAt04) return false;
    }
    return true;
}
}
