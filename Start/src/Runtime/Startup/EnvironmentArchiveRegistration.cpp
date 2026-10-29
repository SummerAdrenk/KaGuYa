#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/EnvironmentArchiveVectorRuntime.h>
#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/EnvironmentStateArchives.h>
#include <Runtime/SystemDataEnvironmentArchive.h>
#include <Runtime/UnSaveDataRuntime.h>

#include <array>
#include <bit>
#include <cstdlib>
#include <cstring>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
class RetiredArchiveControl final : public YtlReferenceControlRuntime
{
public:
    RetiredArchiveControl() noexcept : YtlReferenceControlRuntime(UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override { _purecall(); }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        // 5ADD18的第二槽412460：退役后再删传14h，不沿用派生IArchive的18h。
        if (flags & 1u) ::operator delete(this, std::size_t{0x14});
    }
    std::uint32_t strideAt0C;
    std::uint32_t countAt10;
};
static_assert(sizeof(RetiredArchiveControl) == 0x14);

void RetireArchiveControlAt467E90(EnvironmentObjectReferenceControl* control, std::uint32_t flags)
{
    // flags=0也退化虚表；+04..+17（包括原payload尾字）保持，不构造第二个拥有者。
    std::array<std::byte, 20> fields;
    auto* raw = reinterpret_cast<std::byte*>(control);
    std::memcpy(fields.data(), raw + 4, fields.size());
    ::new (static_cast<void*>(control)) RetiredArchiveControl;
    std::memcpy(raw + 4, fields.data(), fields.size());
    // 467EA2的派生入口第一次删除传18h；v143实际delete不可抛，不虚构分配器回调。
    if (flags & 1u) ::operator delete(static_cast<void*>(raw), std::size_t{0x18});
}

class ArchiveControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit ArchiveControl(IEnvironmentArchiveRuntime* archive) noexcept
        : EnvironmentObjectReferenceControl(archive, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        // 492E50：取payload后经正文的+04(1)删除，不直接delete基类。
        if (auto* archive = static_cast<IEnvironmentArchiveRuntime*>(payloadAt14))
            archive->DeleteAtVslot04(1);
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        RetireArchiveControlAt467E90(this, flags);
    }
};
static_assert(sizeof(ArchiveControl) == 0x18);

class TemporaryReference final
{
public:
    explicit TemporaryReference(YtlReferenceSlotRuntime& reference) noexcept : reference_(&reference) {}
    ~TemporaryReference() noexcept
    {
        // 5E6F18 state0 -> 598960 -> 460490；展开期间重读槽，二次异常终止。
        if (reference_) ReleaseYtlReference(*reference_);
    }
    void Dismiss() noexcept { reference_ = nullptr; }
private:
    YtlReferenceSlotRuntime* reference_;
};

void ReleaseCachedControlAt46A566(YtlReferenceControlRuntime* control) noexcept
{
    // state1 -> 541E55：此段任意C++异常终止。与临时Ref析构不同，这里不重取槽。
    if (!control || --control->referenceCountAt04 != 0) return;
    if (control->owningAt08) control->DestroyPayloadAtVslot00();
    control->DeleteControlAtVslot04(1);
}
}

IEnvironmentArchiveRuntime* ResolveEnvironmentArchiveAt417990(
    const EnvironmentRootStorage& root, std::uint32_t index) noexcept
{
    // 417994先begin、41799C再end；volatile保留空根/页边界时原首个读故障。
    const volatile auto& storage = root.archivesAt30;
    const auto begin = reinterpret_cast<std::uintptr_t>(storage.begin);
    const auto bytes = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(storage.end) - begin);
    // 417997..4179AF是有符号除12后作无符号index比较，不额外验证三指针或capacity。
    const auto count = static_cast<std::uint32_t>(std::bit_cast<std::int32_t>(bytes) / 12);
    if (index >= count) return nullptr;
    const auto* slot = reinterpret_cast<const volatile YtlReferenceSlotRuntime*>(begin + index * 12u);
    const auto base = slot->baseAt00;
    if (!base) return nullptr;
    const auto* control = static_cast<const volatile EnvironmentObjectReferenceControl*>(slot->controlAt08);
    // 4179C7先bias、4179CA再stride；control为空也保留4179D7的bias读取。
    const auto bias = slot->biasAt04;
    const auto stride = control ? control->strideAt0C : 0u;
    return reinterpret_cast<IEnvironmentArchiveRuntime*>(base + bias * stride);
}

void InitializeEnvironmentArchiveReferenceAt46A4E5(
    YtlReferenceSlotRuntime& reference, IEnvironmentArchiveRuntime* archive)
{
    if (!archive) return;
    reference.biasAt04 = 0;
    reference.baseAt00 = reinterpret_cast<std::uintptr_t>(archive);
    // 尚未建立临时Ref清理资格；raw控制块分配抛出不能补销毁正文。
    void* raw = ::operator new(sizeof(ArchiveControl));
    auto* control = raw ? ::new (raw) ArchiveControl(archive) : nullptr;
    control->referenceCountAt04 = 1;
    control->owningAt08 = 1;
    control->strideAt0C = 4; // 原IArchive大小，不是具体正文大小。
    reference.controlAt08 = control;
    control->countAt10 = 1;
}

void RegisterEnvironmentArchiveAt46A4A0(EnvironmentRootStorage& root,
    IEnvironmentArchiveRuntime* archive, IEnvironmentRootMemory& memory)
{
    auto& storage = root.archivesAt30;
    YtlReferenceSlotRuntime temporary;
    InitializeEnvironmentArchiveReferenceAt46A4E5(temporary, archive);
    TemporaryReference cleanup(temporary);
    AppendEnvironmentArchiveReferenceAt49852D(storage, temporary, memory);
    cleanup.Dismiss();
    ReleaseCachedControlAt46A566(temporary.controlAt08);
}

void RegisterClickInfoArchiveAt47E3B7(EnvironmentRootOwner& owner, IEnvironmentRootMemory& memory)
{
    void* raw = ::operator new(ClickInfoArchive::OriginalObjectSize);
    auto* archive = raw ? ::new (raw) ClickInfoArchive : nullptr;
    // 47E3A4已撤去前一正文的EH资格；这里没有unique_ptr式失败回滚。
    auto* root = owner.CaptureAt4180C0();
    RegisterEnvironmentArchiveAt46A4A0(*root, archive, memory);
}

void RegisterSaveDataArchiveAt47E2D8(EnvironmentRootOwner& owner,
    ISaveDataConstructionSource& source, ISoundParamsSettingsSource& sounds,
    ISettingTagErrorSource& errors, IRuntimeStringManagerSource& strings,
    IEnvironmentRootMemory& memory)
{
    auto* archive = CreateSaveDataAt47E2B0(source, sounds, errors, strings, memory);
    auto* root = owner.CaptureAt4180C0();
    RegisterEnvironmentArchiveAt46A4A0(*root, archive, memory);
}

void RegisterGlobalParamsArchiveAt47E4A8(EnvironmentRootOwner& owner, IEnvironmentRootMemory& memory)
{
    void* raw = ::operator new(GlobalParamsArchive::OriginalObjectSize);
    auto* archive = raw ? ::new (raw) GlobalParamsArchive : nullptr;
    auto* root = owner.CaptureAt4180C0();
    RegisterEnvironmentArchiveAt46A4A0(*root, archive, memory);
}

void RegisterUnSaveDataArchiveAt47E312(EnvironmentRootOwner& owner,
    IUnSaveDataGameSystemSource& source, IRuntimeStringManagerSource& strings,
    IEnvironmentRootMemory& memory)
{
    void* raw = ::operator new(UnSaveDataRuntime::OriginalObjectSize);
    UnSaveDataRuntime* archive = nullptr;
    try { if (raw) archive = ::new (raw) UnSaveDataRuntime(source, strings, memory); }
    catch (...)
    {
        // 599E52：ctor已完成成员展开，工厂只sized-delete正文。
        ::operator delete(raw, UnSaveDataRuntime::OriginalObjectSize);
        throw;
    }
    auto* root = owner.CaptureAt4180C0();
    RegisterEnvironmentArchiveAt46A4A0(*root, archive, memory);
}

void RegisterSecretScArchiveAt47E380(EnvironmentRootOwner& owner,
    ISecretScCountSource& source, IEnvironmentRootMemory& memory)
{
    void* raw = ::operator new(SecretScArchive::OriginalObjectSize);
    SecretScArchive* archive = nullptr;
    try { if (raw) archive = ::new (raw) SecretScArchive(source, memory); }
    catch (...)
    {
        ::operator delete(raw, SecretScArchive::OriginalObjectSize);
        throw;
    }
    auto* root = owner.CaptureAt4180C0();
    RegisterEnvironmentArchiveAt46A4A0(*root, archive, memory);
}
}
