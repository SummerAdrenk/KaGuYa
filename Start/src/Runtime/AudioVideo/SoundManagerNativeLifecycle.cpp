#include <Runtime/SoundManagerNativeLifecycle.h>
#include <Runtime/SoundLowerReferenceNative.h>
#include <Runtime/SoundSlotVectorNative.h>
#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <array>
#include <cstring>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
thread_local IEnvironmentRootMemory* activeMemory = nullptr;
IEnvironmentRootMemory& Memory()
{
    if (!activeMemory) throw std::logic_error("SoundManagerNativeScope memory is not installed");
    return *activeMemory;
}
class RetiredControl final : public YtlReferenceControlRuntime
{
public:
    RetiredControl() noexcept : YtlReferenceControlRuntime(UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override { _purecall(); }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    { if (flags & 1U) Memory().DeleteBody(this, 0x14); }
    std::uint32_t stride, count;
};
static_assert(sizeof(RetiredControl) == 0x14);
void Retire(EnvironmentObjectReferenceControl* control, std::uint32_t flags)
{
    std::array<std::byte, 20> fields;
    auto* raw = reinterpret_cast<std::byte*>(control);
    std::memcpy(fields.data(), raw + 4, fields.size());
    ::new(raw) RetiredControl;
    std::memcpy(raw + 4, fields.data(), fields.size());
    if (flags & 1U) Memory().DeleteBody(raw, 0x18);
}
class LowerControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit LowerControl(SoundLowerVolumeNative10h* p) noexcept : EnvironmentObjectReferenceControl(p, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        auto* const body = static_cast<SoundLowerVolumeNative10h*>(payloadAt14);
        if (!body) return;
        DestroyRuntimeCStringAt40CD40(&body->relationAt0C);
        Memory().DeleteBody(body, 0x10); // 493230，Free失败不能继续删除正文。
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override { Retire(this, flags); }
};
class ManagerControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit ManagerControl(SoundManagerNativeBody50h* p) noexcept : EnvironmentObjectReferenceControl(p, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        auto* const body = static_cast<SoundManagerNativeBody50h*>(payloadAt14);
        if (body) static_cast<const SoundManagerNativeVtable*>(body->vptrAt00)->deleteAt00(body, 1);
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override { Retire(this, flags); }
};
static_assert(sizeof(LowerControl) == 0x18 && sizeof(ManagerControl) == 0x18);
SoundManagerNativeBody50h* __fastcall DeleteEntry(SoundManagerNativeBody50h* body, void*, std::uint32_t flags)
{ return DeleteSoundManagerAt469480(*body, flags, Memory()); }
// 原生x86虚槽桥接：对象走ECX，额外void*只占EDX且不消费，其余参数/返回与原槽对应。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const SoundManagerNativeVtable methods{
    reinterpret_cast<decltype(SoundManagerNativeVtable::deleteAt00)>(&DeleteEntry)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
}
SoundManagerNativeScope::SoundManagerNativeScope(IEnvironmentRootMemory& memory) noexcept : previous_(activeMemory)
{ activeMemory = &memory; }
SoundManagerNativeScope::~SoundManagerNativeScope() noexcept { activeMemory = previous_; }
const SoundManagerNativeVtable& SoundManagerNativeMethods() noexcept { return methods; }

SoundLowerVolumeNative10h* CreateSoundLowerAt45D394(SoundManagerNativeBody50h& manager, std::uint32_t iteration,
    IRuntimeStringManagerSource& strings, IEnvironmentRootMemory& memory)
{
    auto* raw = memory.AllocateBody(0x10);
    auto* body = raw ? ::new(raw) SoundLowerVolumeNative10h : nullptr;
    if (!body) return nullptr;
    const auto index = static_cast<volatile SoundManagerNativeBody50h&>(manager).firstEffectsBeginAt18 + iteration;
    // 原GetNil失败没有body回收资格；+08及其它未写字节不清零。
    InitializeEmptyRuntimeCString(body->relationAt0C, strings);
    body->slotAt00 = index;
    body->alternateAt04 = 0;
    return body;
}
void BuildSoundLowerReferenceAt45D3E7(YtlReferenceSlotRuntime& slot, SoundLowerVolumeNative10h* body,
    IEnvironmentRootMemory& memory)
{
    slot = {};
    if (!body) return;
    slot.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    auto* raw = memory.AllocateBody(0x18);
    auto* control = raw ? ::new(raw) LowerControl(body) : nullptr;
    // 原45D412先直接写已分配control，45D424才发布temp.control。
    control->referenceCountAt04 = 1;
    control->owningAt08 = 1;
    control->strideAt0C = 0x10;
    slot.controlAt08 = control;
    control->countAt10 = 1;
}
void ReleaseSoundLowerCapturedAt45D469(YtlReferenceSlotRuntime& slot, YtlReferenceControlRuntime* control) noexcept
{
    if (!control) return;
    if (--control->referenceCountAt04 == 0)
    {
        if (control->owningAt08) control->DestroyPayloadAtVslot00();
        control->DeleteControlAtVslot04(1); // state6终止，固定ESI，不重取temp.control。
    }
    slot.controlAt08 = nullptr; slot.baseAt00 = 0;
}
void DestroySoundManagerAt463ED0(SoundManagerNativeBody50h& body, IEnvironmentRootMemory& memory) noexcept
{
    const SoundManagerNativeScope scope(memory);
    body.vptrAt00 = &methods;
    auto& current = static_cast<volatile SoundManagerNativeBody50h&>(body);
    auto* const end = current.slotsEndAt08;
    auto* begin = current.slotsBeginAt04;
    if (begin != end)
    {
        DestroySoundSlotRangeAt48FC60(begin, end);
        begin = current.slotsBeginAt04;
    }
    current.slotsEndAt08 = begin;
    DestroySoundLowerAt462240(body, memory);
    ReleaseYtlReference(body.paramsAt24);
    DestroySoundSlotsAt4627C0(body, memory);
}
SoundManagerNativeBody50h* DeleteSoundManagerAt469480(SoundManagerNativeBody50h& body,
    std::uint32_t flags, IEnvironmentRootMemory& memory) noexcept
{
    DestroySoundManagerAt463ED0(body, memory);
    if (flags & 1U) memory.DeleteBody(&body, 0x50);
    return &body;
}
const YtlReferenceSlotRuntime* SoundManagerSavedParamsSource::BorrowSoundParamsAt48AB24()
{
    auto* const save = static_cast<SaveDataEnvironmentArchive*>(
        ResolveEnvironmentArchiveAt417990(*root_.CaptureAt4180C0(), 1));
    return reinterpret_cast<const YtlReferenceSlotRuntime*>(reinterpret_cast<std::uintptr_t>(save) + 0x1CU);
}
SoundManagerNativeBody50h* CreateSoundManagerAt48AAFF(ISoundManagerSavedParamsSource& saved,
    ParamsRuntimeRootOwner& params, SoundModuleNativeApi& sound, IRuntimeStringManagerSource& strings,
    IEnvironmentRootMemory& memory)
{
    void* raw = memory.AllocateBody(0x50);
    if (!raw) return nullptr;
    struct RawUnwind final
    {
        void* raw; IEnvironmentRootMemory& memory; bool active = true;
        ~RawUnwind() noexcept { if (active) memory.DeleteBody(raw, 0x50); }
    } cleanup{raw, memory}; // 48AB0E state15 -> 59AC5F，取saved前已经生效。
    auto* const body = ::new(raw) SoundManagerNativeBody50h;
    const auto* const source = saved.BorrowSoundParamsAt48AB24();
    ConstructSoundManagerAt45D120(*body, *source, params, sound, strings, memory);
    cleanup.active = false;
    return body;
}
void InstallSoundManagerReferenceAt48AB35(YtlReferenceSlotRuntime& slot, SoundManagerNativeBody50h* body,
    IEnvironmentRootMemory& memory)
{
    const SoundManagerNativeScope scope(memory);
    ReleaseYtlReference(slot);
    if (!body) return;
    // 原旧释放末尾ECX=0，不增补一次对回调晚填控制块的检查。
    slot.biasAt04 = 0; slot.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    auto* raw = memory.AllocateBody(0x18);
    auto* control = raw ? ::new(raw) ManagerControl(body) : nullptr;
    slot.controlAt08 = control;
    control->referenceCountAt04 = 1;
    slot.controlAt08->owningAt08 = 1;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->strideAt0C = 0x50;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->countAt10 = 1;
}
void ReleaseSoundManagerReference(YtlReferenceSlotRuntime& slot, IEnvironmentRootMemory& memory)
{
    const SoundManagerNativeScope scope(memory);
    ReleaseYtlReference(slot);
}
}
