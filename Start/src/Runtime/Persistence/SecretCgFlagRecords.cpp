#include <Runtime/SecretCgFlagRecords.h>
#include <Runtime/EnvironmentArchiveVectorRuntime.h>

#include <array>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
class RetiredControl final : public SecretCgFlagControl
{
public:
    RetiredControl() noexcept : SecretCgFlagControl(nullptr) {}
    void DestroyPayloadAtVslot00() override { _purecall(); }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        // 5ADD18 -> 412460：退化后再次调用slot04的尺寸是14h，不是原派生类的18h。
        if (flags & 1u) ::operator delete(this, std::size_t{20});
    }
};
static_assert(sizeof(RetiredControl) == 24);

class FlagControl final : public SecretCgFlagControl
{
public:
    explicit FlagControl(SecretCgFlagRecord* body) noexcept : SecretCgFlagControl(body) {}
    void DestroyPayloadAtVslot00() override
    {
        // 492FD0：虚槽必须可被通用ReleaseYtlReference直接调用，不依赖外部memory上下文。
        ::operator delete(payloadAt14, std::size_t{8});
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
        { RetireEnvironmentObjectControl(this, flags); }
};
static_assert(sizeof(FlagControl) == 24);

class IndicesControl final : public SecretCgFlagControl
{
public:
    explicit IndicesControl(void* body) noexcept : SecretCgFlagControl(body) {}
    void DestroyPayloadAtVslot00() override
    {
        // 5AE3D0 -> 43BAA0：数组释放路径，与45C838的array-new hooks配对。
        ::operator delete[](payloadAt14);
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        // 410B40与468190同构，先退化虚表，再按bit0删除18h。
        RetireEnvironmentObjectControl(this, flags);
    }
};
static_assert(sizeof(IndicesControl) == 24);

void ReleaseCreatedReferenceAt45C187(YtlReferenceSlotRuntime& reference) noexcept
{
    auto* control = reference.controlAt08;
    if (!control) return;
    // 45C18F..45C1A7正常路径缓存esi；460C00异常清理路径则在payload之后重读槽。
    if (--control->referenceCountAt04 == 0)
    {
        if (control->owningAt08) control->DestroyPayloadAtVslot00();
        control->DeleteControlAtVslot04(1);
    }
    reference.controlAt08 = nullptr;
    reference.baseAt00 = 0;
}
}

void CreateRecordReference(YtlReferenceSlotRuntime& out,
    const YtlReferenceSlotRuntime& sourceRef, std::uint32_t index,
    const SecretCgFlagControl* cachedControl, IEnvironmentRootMemory& memory)
{
    void* raw = memory.AllocateBody(8);
    SecretCgFlagRecord* body = nullptr;
    if (raw)
    {
        // 45C0CF/45C0D3使用组循环缓存EDI；分配后读其stride，不重取sourceRef.control。
        const auto base = sourceRef.baseAt00;
        const auto stride = base && cachedControl ? cachedControl->strideAt0C : 0u;
        const auto address = base ? base + (sourceRef.biasAt04 + index) * stride : 0u;
        body = ::new (raw) SecretCgFlagRecord;
        body->idAt00 = *reinterpret_cast<const std::uint32_t*>(address);
        body->byteFlagAt04 = 0; // 45C0E6/45C0E8：+05..07保持未写。
    }
    // 45C0B9/45C0EE：正文null跳过source解引用，仍允许追加空Ref。
    AdoptSecretCgFlagAt45C0F0(out, body, memory);
}

void AdoptSecretCgFlagAt45C0F0(YtlReferenceSlotRuntime& fresh,
    SecretCgFlagRecord* body, IEnvironmentRootMemory& memory)
{
    fresh = {};
    if (!body) return;
    fresh.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    // 45C10F先发布base；45C112失败时尚无临时Ref清理state，不回收8-byte正文。
    void* raw = memory.AllocateBody(24);
    auto* control = raw ? ::new (raw) FlagControl(body) : nullptr;
    // 45C12B..45C14A：control返回null不是空Ref成功；原版随即写null+4而故障。
    control->referenceCountAt04 = 1;
    control->owningAt08 = 1;
    control->strideAt0C = 8;
    fresh.controlAt08 = control;
    control->countAt10 = 1;
}

SecretCgFlagRecord* ResolveSecretCgFlag(const YtlReferenceSlotRuntime& reference) noexcept
{
    // 45CA80..45CAA0：base=0才返回null；control=0时stride=0，保留DWORD回绕。
    return ResolveEnvironmentObjectReference<SecretCgFlagRecord>(reference);
}

void AllocateSecretCgIndicesAt45C825(YtlReferenceSlotRuntime& out,
    std::uint32_t count, IEnvironmentRootMemory& memory)
{
    out = {};
    if (!count || count > 0x7FFFFFFFu) return; // 45C81E..45C823：signed jle。
    const auto bytes = count > 0x3FFFFFFFu ? 0xFFFFFFFFu : count * 4u;
    void* payload = ::operator new[](bytes); // 45C838：必须是数组分配，不能用AllocateBody代替。
    out.biasAt04 = 0;
    out.baseAt00 = reinterpret_cast<std::uintptr_t>(payload);
    void* raw = memory.AllocateBody(24); // 45C846/45C84B：失败不回收已发布数组。
    auto* control = raw ? ::new (raw) IndicesControl(payload) : nullptr;
    control->referenceCountAt04 = 1;
    control->owningAt08 = 1;
    control->strideAt0C = 4;
    control->countAt10 = count;
    out.controlAt08 = control; // 45C881：字段全部完成后才发布control。
}

void DestroySecretCgFlagPayloadAt492FD0(SecretCgFlagControl& control, IEnvironmentRootMemory& memory)
{
    memory.DeleteBody(control.payloadAt14, 8); // 492FD0：不清payload，不使用Ref的bias。
}

void DeleteSecretCgFlagControlAt468190(SecretCgFlagControl& control,
    std::uint32_t flags, IEnvironmentRootMemory& memory)
{
    // 46819A先退化虚表；flags=0也执行，+04..17必须全部保留。
    std::array<std::byte, 20> fields;
    std::memcpy(fields.data(), reinterpret_cast<const std::byte*>(&control) + 4, fields.size());
    auto* retired = ::new (static_cast<void*>(&control)) RetiredControl;
    std::memcpy(reinterpret_cast<std::byte*>(retired) + 4, fields.data(), fields.size());
    if (flags & 1u) memory.DeleteBody(retired, 24);
}

void ReleaseSecretCgReferenceAt460C00(YtlReferenceSlotRuntime& reference,
    IEnvironmentRootMemory&) noexcept
{
    // 5E8600 flags=5：释放异常终止；回调重读和保留bias与公共primitive完全相同。
    ReleaseYtlReference(reference);
}

void ReleaseSecretCgReferenceRangeAt452710(YtlReferenceSlotRuntime* begin,
    YtlReferenceSlotRuntime* end, IEnvironmentRootMemory&) noexcept
{
    // 45273E..452787 / 5EB8F8：固定进入时的范围，正序；元素异常立即terminate。
    for (; begin != end; ++begin) ReleaseYtlReference(*begin);
}

void AppendSecretCgReferenceAt4551B0(SecretCgReferenceVector& storage,
    const YtlReferenceSlotRuntime& source, IEnvironmentRootMemory& memory)
{
    // 4551B0/454310逐指令同构；408710的43B240/40AEE0/407D20等helper语义也已核对。
    AppendEnvironmentArchiveReferenceAt49852D(storage, source, memory);
}

void ClearSecretCgReferencesAt45BD08(SecretCgReferenceVector& storage,
    IEnvironmentRootMemory&) noexcept
{
    // 45BD08释放后45BD0D重读begin再写end，与498312相同。
    ClearEnvironmentArchiveReferencesAt498312(storage);
}

void DestroySecretCgReferencesAt461440(SecretCgReferenceVector& storage,
    IEnvironmentRootMemory& memory) noexcept
{
    // 461440 -> 461CD0 与497080同构，5E8400/5ED2B0均为flags=5。
    DestroyEnvironmentArchiveReferencesAt497080(storage, memory);
}

void AppendNewSecretCgFlagAt45C0AD(SecretCgReferenceVector& storage,
    const YtlReferenceSlotRuntime& sourceRef, std::uint32_t index,
    const SecretCgFlagControl* cachedControl, IEnvironmentRootMemory& memory)
{
    YtlReferenceSlotRuntime temporary;
    CreateRecordReference(temporary, sourceRef, index, cachedControl, memory);
    try
    {
        // 45C158 / 5E84BC[state12]：到这里才保护临时Ref。
        AppendSecretCgReferenceAt4551B0(storage, temporary, memory);
    }
    catch (...)
    {
        ReleaseSecretCgReferenceAt460C00(temporary, memory); // 5969FF -> 460C00。
        throw; // 45CAC4属于外层CSecretCG构造；叶API不吞异常。
    }
    ReleaseCreatedReferenceAt45C187(temporary);
}
}
