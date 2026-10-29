#include <Runtime/ParamsRegistSceneTable.h>
#include <Runtime/EnvironmentArchiveVectorRuntime.h>
#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/RuntimeCStringVector.h>

#include <array>
#include <bit>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <new>
#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Slot = YtlReferenceSlotRuntime;
using Storage = EnvironmentVectorStorage<Slot>;
using CStringStorage = EnvironmentVectorStorage<RuntimeCString>;

class RetiredRegistSceneControl final : public EnvironmentObjectReferenceControl
{
public:
    RetiredRegistSceneControl() noexcept : EnvironmentObjectReferenceControl(nullptr) {}
    void DestroyPayloadAtVslot00() override { _purecall(); }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        // 5ADD18 -> 412460：退役后的基类删除尺寸为 14h，不再是 18h。
        if (flags & 1u) ::operator delete(this, std::size_t{0x14});
    }
};
static_assert(sizeof(RetiredRegistSceneControl) == 0x18);

void RetireRegistSceneControl(EnvironmentObjectReferenceControl* control, std::uint32_t flags)
{
    // 411980/411A70：flags=0 也退化虚表；其余二十字节（含 padding）保持。
    std::array<std::byte, 20> fields;
    std::memcpy(fields.data(), reinterpret_cast<const std::byte*>(control) + 4, fields.size());
    auto* retired = ::new (static_cast<void*>(control)) RetiredRegistSceneControl;
    std::memcpy(reinterpret_cast<std::byte*>(retired) + 4, fields.data(), fields.size());
    if (flags & 1u) ::operator delete(retired, std::size_t{0x18});
}

class RegistSceneGroupControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit RegistSceneGroupControl(ParamsRegistSceneGroupRuntime* body) noexcept
        : EnvironmentObjectReferenceControl(body) {}

    void DestroyPayloadAtVslot00() override
    {
        auto* const body = static_cast<ParamsRegistSceneGroupRuntime*>(payloadAt14);
        if (!body) return;
        // 5AE498 -> 43CD00：40E490 整体 noexcept，之后的 name Free 可外传。
        // 不调用 40FD90 式整体 noexcept 析构，也不让 C++ 再析构一次成员。
        DestroyEnvironmentArchiveReferencesAt497080(body->itemsAt04.storage);
        DestroyRuntimeCStringAt40CD40(&body->nameAt00);
        ::operator delete(body, std::size_t{0x10});
    }

    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        RetireRegistSceneControl(this, flags);
    }
};
static_assert(sizeof(RegistSceneGroupControl) == 0x18);

void DestroyRegistSceneItemTailAt43CE75(ParamsRegistSceneItemRuntime* body) noexcept
{
    // 43CE77 state0 -> 541E55；边界包括 vector 释放及其后的 body delete。
    DestroyRuntimeCStringVectorAt40DD20(body->scenesAt00);
    ::operator delete(body, std::size_t{0x10});
}

class RegistSceneItemControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit RegistSceneItemControl(ParamsRegistSceneItemRuntime* body) noexcept
        : EnvironmentObjectReferenceControl(body) {}

    void DestroyPayloadAtVslot00() override
    {
        auto* const body = static_cast<ParamsRegistSceneItemRuntime*>(payloadAt14);
        if (!body) return;
        // 5AE4B0 -> 43CE30：name Free 在 state=-1，可抛且不补清 scenes/body。
        DestroyRuntimeCStringAt40CD40(&body->nameAt0C);
        DestroyRegistSceneItemTailAt43CE75(body);
    }

    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        RetireRegistSceneControl(this, flags);
    }
};
static_assert(sizeof(RegistSceneItemControl) == 0x18);

void ConstructRegistSceneGroupAt40C990(ParamsRegistSceneGroupRuntime& body,
    IRuntimeStringManagerSource& strings) noexcept
{
    // 5DFE18：无 states、EHFlags=5；GetNil 失败终止，不向工厂外传或回收 body。
    InitializeEmptyRuntimeCString(body.nameAt00, strings);
    // 仅在 name 成功后开始 items 生命周期，只写三个空指针，不构造整个 group。
    ::new (static_cast<void*>(&body.itemsAt04)) ParamsRegistSceneItemTable;
}

void ConstructRegistSceneItemAt40CAC0(ParamsRegistSceneItemRuntime& body,
    IRuntimeStringManagerSource& strings) noexcept
{
    // 5DFD78 同为 flags=5；Capture/GetNil 之前已清三指针，name 尚未发布。
    ::new (static_cast<void*>(&body.scenesAt00)) CStringStorage{nullptr, nullptr, nullptr};
    InitializeEmptyRuntimeCString(body.nameAt0C, strings);
}

template<class Control, class Body>
Control* ConstructUnpublishedControl(void* raw, Body* body) noexcept
{
    if (!raw) return nullptr;
    // 共享基类有 refs/owning 默认值；先保存并恢复未发布字段，避免把它们提前提交。
    // 本段无回调；最终仅改变 vptr/payload，+04..13 仍是分配器给出的原字节。
    std::array<std::byte, 16> fields;
    std::memcpy(fields.data(), static_cast<const std::byte*>(raw) + 4, fields.size());
    auto* control = ::new (raw) Control(body);
    std::memcpy(reinterpret_cast<std::byte*>(control) + 4, fields.data(), fields.size());
    return control;
}

template<class Control, class Body>
void CreateSceneReference(Slot& outRef, Body* body, IEnvironmentRootMemory& memory)
{
    // 407930/4079A0 只在正文分配/构造完成之后清输出；不释放槽原内容。
    outRef.baseAt00 = 0;
    outRef.biasAt04 = 0;
    outRef.controlAt08 = nullptr;
    if (!body) return;
    outRef.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    void* raw = memory.AllocateBody(0x18);
    // EDI 保留创建时 body；分配回调即使改 out.base，也不改变 payload。
    auto* control = ConstructUnpublishedControl<Control>(raw, body);
    // 40796F/4079DF 先发布，再写 refs；null control 仍须在原来的写入点故障。
    outRef.controlAt08 = control;
    control->referenceCountAt04 = 1;
    // 三次重读槽，不沿用局部 control；+09..0B 从不补零。
    outRef.controlAt08->owningAt08 = 1;
    static_cast<EnvironmentObjectReferenceControl*>(outRef.controlAt08)->strideAt0C = 0x10;
    static_cast<EnvironmentObjectReferenceControl*>(outRef.controlAt08)->countAt10 = 1;
}

template<void (*Release)(Slot&) noexcept>
class ReferenceCleanup final
{
public:
    explicit ReferenceCleanup(Slot& slot) noexcept : slot_(slot) {}
    ~ReferenceCleanup() noexcept { if (active_) Release(slot_); }
    void Deactivate() noexcept { active_ = false; }
private:
    Slot& slot_;
    bool active_ = true;
};

void ReleaseCapturedAppendControl(YtlReferenceControlRuntime* control) noexcept
{
    // 40B1E6/40B2B6 state1 -> terminate；payload 后仍删除缓存 ESI，不重读局部槽。
    if (control && --control->referenceCountAt04 == 0)
    {
        if (control->owningAt08) control->DestroyPayloadAtVslot00();
        control->DeleteControlAtVslot04(1);
    }
}

template<void (*Release)(Slot&) noexcept>
void AppendSceneReference(Storage& storage, const Slot& source, IEnvironmentRootMemory& memory)
{
    // 40B150/40B220：局部 Ref 三字段显式为零，state0 的展开清理重读此槽。
    Slot temporary{};
    ReferenceCleanup<Release> cleanup(temporary);
    YtlReferenceControlRuntime* capturedControl = nullptr;
    std::uintptr_t base = 0;
    std::uint32_t bias = 0;
    if (&temporary != &source)
    {
        capturedControl = source.controlAt08;
        base = source.baseAt00;
        bias = source.biasAt04;
        temporary.baseAt00 = base;
        temporary.biasAt04 = bias;
        temporary.controlAt08 = capturedControl;
        if (capturedControl) ++capturedControl->referenceCountAt04;
    }

    auto* const end = storage.end;
    if (end != storage.capacity)
    {
        // 快路径使用入口缓存的三字段，并为目的槽再增持一次；ESI 不从临时槽重取。
        end->baseAt00 = base;
        end->biasAt04 = bias;
        end->controlAt08 = capturedControl;
        if (capturedControl) ++capturedControl->referenceCountAt04;
        storage.end = reinterpret_cast<Slot*>(reinterpret_cast<std::uintptr_t>(storage.end) + 12u);
    }
    else
    {
        // 409A10/40A160 与 454310 及范围/copy/delete/EH 已逐 helper 核对。
        // 源为额外持有的局部 Ref，不把调用者的可变 source 直接传给增长函数。
        AppendEnvironmentArchiveReferenceAt49852D(storage, temporary, memory);
        capturedControl = temporary.controlAt08; // 40B1E3/40B2B3：仅慢路径重读。
    }
    cleanup.Deactivate();
    ReleaseCapturedAppendControl(capturedControl);
}

void SnapshotString(RuntimeCString& destination, const char* chars, std::size_t length)
{
    if (length > static_cast<std::size_t>((std::numeric_limits<int>::max)()))
        throw std::length_error("Params registScene snapshot exceeds CString length");
    SetRuntimeCStringAt437680(destination, chars, static_cast<int>(length));
}

// 仅供现代 fixture/profile 深快照；不代表 reader 的 2B/2C/2D/2F 清理资格。
class SnapshotCString final
{
public:
    SnapshotCString() { InitializeEmptyRuntimeCString(value); }
    ~SnapshotCString() noexcept { DestroyRuntimeCStringAt40CD40(&value); }
    SnapshotCString(const SnapshotCString&) = delete;
    SnapshotCString& operator=(const SnapshotCString&) = delete;
    union { RuntimeCString value; };
};

void AppendSnapshotString(CStringStorage& storage, const char* chars, std::size_t length)
{
    SnapshotCString copy;
    SnapshotString(copy.value, chars, length);
    // 40B010 -> 408470 与已审 4085C0 相同；复制追加，不以 resize 或移动代替。
    AppendRuntimeCStringAt41D5E9(storage, copy.value);
}

bool EqualStrings(const CStringStorage& left, const CStringStorage& right) noexcept
{
    auto* first = left.begin;
    auto* second = right.begin;
    for (; first != left.end && second != right.end; ++first, ++second)
        if (RuntimeStringView(*first) != RuntimeStringView(*second)) return false;
    return first == left.end && second == right.end;
}

std::size_t TableSize(const Storage& storage) noexcept
{
    const auto bytes = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(storage.end) -
        reinterpret_cast<std::uintptr_t>(storage.begin));
    return static_cast<std::uint32_t>(std::bit_cast<std::int32_t>(bytes) / 12);
}

const Slot& ReferenceAt(const Storage& storage, std::uint32_t index) noexcept
{
    // DWORD 索引回绕后取 Ref，不添加 vector/count 范围校正。
    const auto address = reinterpret_cast<std::uintptr_t>(storage.begin) + index * 12u;
    return *reinterpret_cast<const Slot*>(address);
}
}

void CreateParamsRegistSceneGroupAt41DC40(Slot& outRef, IRuntimeStringManagerSource& strings,
    IEnvironmentRootMemory& memory)
{
    auto* body = static_cast<ParamsRegistSceneGroupRuntime*>(memory.AllocateBody(0x10));
    if (body) ConstructRegistSceneGroupAt40C990(*body, strings);
    // 41DC6C 才建立 reader 的 group 资格；本工厂不回收分配失败留下的正文。
    CreateSceneReference<RegistSceneGroupControl>(outRef, body, memory);
}

void CreateParamsRegistSceneItemAt41DCB0(Slot& outRef, IRuntimeStringManagerSource& strings,
    IEnvironmentRootMemory& memory)
{
    auto* body = static_cast<ParamsRegistSceneItemRuntime*>(memory.AllocateBody(0x10));
    if (body) ConstructRegistSceneItemAt40CAC0(*body, strings);
    // 旧 reader 到 41DE76 才跳入2F；不得在工厂内提前给 item 加 RAII 清理资格。
    CreateSceneReference<RegistSceneItemControl>(outRef, body, memory);
}

ParamsRegistSceneGroupRuntime* ResolveParamsRegistSceneGroupAt4105C0(const Slot& reference) noexcept
{
    return ResolveEnvironmentObjectReference<ParamsRegistSceneGroupRuntime>(reference);
}

ParamsRegistSceneItemRuntime* ResolveParamsRegistSceneItemAt4105F0(const Slot& reference) noexcept
{
    return ResolveEnvironmentObjectReference<ParamsRegistSceneItemRuntime>(reference);
}

void ReleaseParamsRegistSceneGroupAt40DA60(Slot& reference) noexcept
{
    // 5E025C flags=5：正常/展开均为整入口终止边界，不是 CG 的可抛 raw release。
    ReleaseYtlReference(reference);
}

void ReleaseParamsRegistSceneItemAt40DBB0(Slot& reference) noexcept
{
    // 5E0280 flags=5；payload 后由共享 helper 重读槽，保留 bias。
    ReleaseYtlReference(reference);
}

ParamsRegistSceneItemTable::~ParamsRegistSceneItemTable() noexcept
{
    // 40E490 / 5DFDC8 flags=5；begin 为空不改 end/capacity。
    DestroyEnvironmentArchiveReferencesAt497080(storage);
}

ParamsRegistSceneItemTable::ParamsRegistSceneItemTable(ParamsRegistSceneItemTable&& other) noexcept
    : storage(std::exchange(other.storage, Storage{})) {}

ParamsRegistSceneItemTable& ParamsRegistSceneItemTable::operator=(ParamsRegistSceneItemTable&& other) noexcept
{
    if (this != &other)
    {
        DestroyEnvironmentArchiveReferencesAt497080(storage);
        storage = std::exchange(other.storage, Storage{});
    }
    return *this;
}

ParamsRegistSceneItemTable::ParamsRegistSceneItemTable(const ParamsRegistSceneItemTable& other)
{
    ParamsRegistSceneItemTable snapshot;
    for (const auto& source : other)
    {
        Slot fresh;
        CreateParamsRegistSceneItemAt41DCB0(fresh);
        ReferenceCleanup<ReleaseParamsRegistSceneItemAt40DBB0> cleanup(fresh);
        auto& body = Resolve(fresh);
        SnapshotString(body.nameAt0C, source.nameAt0C.GetString(), source.nameAt0C.GetLength());
        for (auto* scene = source.scenesAt00.begin; scene != source.scenesAt00.end; ++scene)
            AppendSnapshotString(body.scenesAt00, scene->GetString(), scene->GetLength());
        snapshot.AppendReference(fresh);
    }
    storage = std::exchange(snapshot.storage, Storage{});
}

ParamsRegistSceneItemTable& ParamsRegistSceneItemTable::operator=(const ParamsRegistSceneItemTable& other)
{
    if (this != &other)
    {
        ParamsRegistSceneItemTable snapshot(other);
        *this = std::move(snapshot);
    }
    return *this;
}

void ParamsRegistSceneItemTable::push_back(const ParamsRegistSceneItem& fixture)
{
    Slot fresh;
    CreateParamsRegistSceneItemAt41DCB0(fresh);
    ReferenceCleanup<ReleaseParamsRegistSceneItemAt40DBB0> cleanup(fresh);
    auto& body = Resolve(fresh);
    SnapshotString(body.nameAt0C, fixture.name.cp932.data(), fixture.name.cp932.size());
    for (const auto& scene : fixture.scenes)
        AppendSnapshotString(body.scenesAt00, scene.cp932.data(), scene.cp932.size());
    AppendReference(fresh);
}

std::size_t ParamsRegistSceneItemTable::size() const noexcept { return TableSize(storage); }

ParamsRegistSceneItemRuntime& ParamsRegistSceneItemTable::Resolve(const Slot& reference)
{
    auto* body = ResolveParamsRegistSceneItemAt4105F0(reference);
    if (!body) throw std::logic_error("Params registScene item reference resolves to null");
    return *body;
}

ParamsRegistSceneItemRuntime& ParamsRegistSceneItemTable::At(std::uint32_t index)
{
    return const_cast<ParamsRegistSceneItemRuntime&>(std::as_const(*this).At(index));
}

const ParamsRegistSceneItemRuntime& ParamsRegistSceneItemTable::At(std::uint32_t index) const
{
    return Resolve(ReferenceAt(storage, index));
}

void ParamsRegistSceneItemTable::clear() noexcept
{
    // 408350 对应已审 4523B0；清元素后重读 begin 设置 end，保留 capacity。
    ClearEnvironmentArchiveReferencesAt498312(storage);
}

void ParamsRegistSceneItemTable::AppendReference(const Slot& source, IEnvironmentRootMemory& memory)
{
    AppendSceneReference<ReleaseParamsRegistSceneItemAt40DBB0>(storage, source, memory);
}

bool ParamsRegistSceneItemTable::operator==(const ParamsRegistSceneItemTable& other) const
{
    if (size() != other.size()) return false;
    for (std::uint32_t index = 0; index < size(); ++index)
    {
        const auto& left = At(index);
        const auto& right = other.At(index);
        if (RuntimeStringView(left.nameAt0C) != RuntimeStringView(right.nameAt0C) ||
            !EqualStrings(left.scenesAt00, right.scenesAt00)) return false;
    }
    return true;
}

ParamsRegistSceneTable::~ParamsRegistSceneTable() noexcept
{
    // 40E280 / 5DFE60 flags=5，逐项释放后重读 begin/capacity 并删除存储。
    DestroyEnvironmentArchiveReferencesAt497080(storage);
}

ParamsRegistSceneTable::ParamsRegistSceneTable(ParamsRegistSceneTable&& other) noexcept
    : storage(std::exchange(other.storage, Storage{})) {}

ParamsRegistSceneTable& ParamsRegistSceneTable::operator=(ParamsRegistSceneTable&& other) noexcept
{
    if (this != &other)
    {
        DestroyEnvironmentArchiveReferencesAt497080(storage);
        storage = std::exchange(other.storage, Storage{});
    }
    return *this;
}

ParamsRegistSceneTable::ParamsRegistSceneTable(const ParamsRegistSceneTable& other)
{
    ParamsRegistSceneTable snapshot;
    for (const auto& source : other)
    {
        Slot fresh;
        CreateParamsRegistSceneGroupAt41DC40(fresh);
        ReferenceCleanup<ReleaseParamsRegistSceneGroupAt40DA60> cleanup(fresh);
        auto& body = Resolve(fresh);
        SnapshotString(body.nameAt00, source.nameAt00.GetString(), source.nameAt00.GetLength());
        body.itemsAt04 = source.itemsAt04;
        snapshot.AppendReference(fresh);
    }
    storage = std::exchange(snapshot.storage, Storage{});
}

ParamsRegistSceneTable& ParamsRegistSceneTable::operator=(const ParamsRegistSceneTable& other)
{
    if (this != &other)
    {
        ParamsRegistSceneTable snapshot(other);
        *this = std::move(snapshot);
    }
    return *this;
}

void ParamsRegistSceneTable::push_back(const ParamsRegistSceneGroup& fixture)
{
    Slot fresh;
    CreateParamsRegistSceneGroupAt41DC40(fresh);
    ReferenceCleanup<ReleaseParamsRegistSceneGroupAt40DA60> cleanup(fresh);
    auto& body = Resolve(fresh);
    SnapshotString(body.nameAt00, fixture.name.cp932.data(), fixture.name.cp932.size());
    for (const auto& item : fixture.items) body.itemsAt04.push_back(item);
    AppendReference(fresh);
}

std::size_t ParamsRegistSceneTable::size() const noexcept { return TableSize(storage); }

ParamsRegistSceneGroupRuntime& ParamsRegistSceneTable::Resolve(const Slot& reference)
{
    auto* body = ResolveParamsRegistSceneGroupAt4105C0(reference);
    if (!body) throw std::logic_error("Params registScene group reference resolves to null");
    return *body;
}

ParamsRegistSceneGroupRuntime& ParamsRegistSceneTable::At(std::uint32_t index)
{
    return const_cast<ParamsRegistSceneGroupRuntime&>(std::as_const(*this).At(index));
}

const ParamsRegistSceneGroupRuntime& ParamsRegistSceneTable::At(std::uint32_t index) const
{
    return Resolve(ReferenceAt(storage, index));
}

void ParamsRegistSceneTable::clear() noexcept
{
    // 43B300 / 5DFE84 flags=5，回调后重读 begin；不释放容量。
    ClearEnvironmentArchiveReferencesAt498312(storage);
}

void ParamsRegistSceneTable::AppendReference(const Slot& source, IEnvironmentRootMemory& memory)
{
    AppendSceneReference<ReleaseParamsRegistSceneGroupAt40DA60>(storage, source, memory);
}

bool ParamsRegistSceneTable::operator==(const ParamsRegistSceneTable& other) const
{
    if (size() != other.size()) return false;
    for (std::uint32_t index = 0; index < size(); ++index)
    {
        const auto& left = At(index);
        const auto& right = other.At(index);
        if (RuntimeStringView(left.nameAt00) != RuntimeStringView(right.nameAt00) ||
            !(left.itemsAt04 == right.itemsAt04)) return false;
    }
    return true;
}
}
