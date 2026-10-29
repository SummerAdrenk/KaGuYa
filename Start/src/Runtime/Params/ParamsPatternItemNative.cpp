#include <Runtime/ParamsPatternItemNative.h>
#include <Runtime/ParamsLegacyTables.h>

#include <array>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <memory>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Slot = YtlReferenceSlotRuntime;
using Control = EnvironmentObjectReferenceControl;

class RetiredPatternControl final : public Control
{
public:
    RetiredPatternControl() noexcept : Control(nullptr, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override { _purecall(); }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        if (flags & 1u) ::operator delete(this, std::size_t{0x14});
    }
};
static_assert(sizeof(RetiredPatternControl) == 0x18);

void Retire(Control* control, std::uint32_t flags)
{
    // 专属 slot04 先退役再 delete18；退役表 412460 后续删除尺寸为14h，无额外 noexcept。
    std::array<std::byte, 20> fields;
    std::memcpy(fields.data(), reinterpret_cast<const std::byte*>(control) + 4, fields.size());
    auto* const retired = ::new (static_cast<void*>(control)) RetiredPatternControl;
    std::memcpy(reinterpret_cast<std::byte*>(retired) + 4, fields.data(), fields.size());
    if (flags & 1u) ::operator delete(retired, std::size_t{0x18});
}

template<class Body>
class PatternObjectControl final : public Control
{
public:
    explicit PatternObjectControl(Body* body) noexcept : Control(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        auto* const body = static_cast<Body*>(payloadAt14);
        if (!body) return;
        // 不用 delete 表达式：字段析构抛出时，原叶不继续回收正文。
        // v143的std::destroy_at无条件noexcept；原Name/Exc/Convert的Free异常须外传。
        body->~Body();
        ::operator delete(body, sizeof(Body));
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override { Retire(this, flags); }
};
static_assert(sizeof(PatternObjectControl<ParamsPatternFileNameNative>) == 0x18);
static_assert(sizeof(PatternObjectControl<ParamsPatternFileGroupNative>) == 0x18);
static_assert(sizeof(PatternObjectControl<ParamsPatternExcPositionNative>) == 0x18);
static_assert(sizeof(PatternObjectControl<ParamsPatternFileConvertNative>) == 0x18);
static_assert(sizeof(PatternObjectControl<ParamsPatternGroupNative>) == 0x18);

struct ConstructorRawCleanup final
{
    void* raw;
    std::size_t bytes;
    IEnvironmentRootMemory& memory;
    bool active = true;
    ~ConstructorRawCleanup() noexcept
    {
        if (active) memory.DeleteBody(raw, bytes);
    }
};

template<class Body, bool cleanFailedConstructor>
Body* AllocateBody(IRuntimeStringManagerSource& strings, IEnvironmentRootMemory& memory)
{
    void* const raw = memory.AllocateBody(sizeof(Body));
    if (!raw) return nullptr;
    if constexpr (cleanFailedConstructor)
    {
        ConstructorRawCleanup cleanup{raw, sizeof(Body), memory};
        auto* const body = ::new (raw) Body(strings);
        cleanup.active = false;
        return body;
    }
    else
    {
        return ::new (raw) Body(strings);
    }
}

template<class Body>
Control* AllocateControl(Body* body, IEnvironmentRootMemory& memory)
{
    void* const raw = memory.AllocateBody(sizeof(PatternObjectControl<Body>));
    return raw ? ::new (raw) PatternObjectControl<Body>(body) : nullptr;
}

template<class Body>
void PublishRereadControl(Slot& out, Body* body, IEnvironmentRootMemory& memory)
{
    auto* const control = AllocateControl(body, memory);
    out.controlAt08 = control;
    // 原先发布 null 再访问 null+4；现代明确终止，不补正文回收或空对象成功。
    if (!control) _invalid_parameter_noinfo_noreturn();
    control->referenceCountAt04 = 1;
    out.controlAt08->owningAt08 = 1;
    static_cast<Control*>(out.controlAt08)->strideAt0C = static_cast<std::uint32_t>(sizeof(Body));
    static_cast<Control*>(out.controlAt08)->countAt10 = 1;
}

template<class Body>
void PublishCachedControl(Slot& out, Body* body, IEnvironmentRootMemory& memory)
{
    auto* const control = AllocateControl(body, memory);
    // inline typed 局部在 refs/owning/stride 写完后才发布 control；null 故障发生在发布前。
    if (!control) _invalid_parameter_noinfo_noreturn();
    control->referenceCountAt04 = 1;
    control->owningAt08 = 1;
    control->strideAt0C = static_cast<std::uint32_t>(sizeof(Body));
    out.controlAt08 = control;
    control->countAt10 = 1;
}

template<class Body, bool cleanFailedConstructor>
Body* CreateTypedItem(Slot& out, IRuntimeStringManagerSource& strings, IEnvironmentRootMemory& memory)
{
    auto* const body = AllocateBody<Body, cleanFailedConstructor>(strings, memory);
    // 原 typed Ref 的初始化在正文构造之后；此处不释放调用者旧 out。
    out.baseAt00 = 0;
    out.biasAt04 = 0;
    out.controlAt08 = nullptr;
    if (body)
    {
        out.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
        PublishCachedControl(out, body, memory);
    }
    return body;
}

template<bool rereadControl>
ParamsPatternGroupNative* RebuildGroup(Slot& out, IRuntimeStringManagerSource& strings,
    IEnvironmentRootMemory& memory)
{
    ReleaseYtlReference(out);
    auto* const body = AllocateBody<ParamsPatternGroupNative, false>(strings, memory);
    // 构造回调可能改槽：这里仅 delete control，不再减持或调用 payload。
    if (out.controlAt08) out.controlAt08->DeleteControlAtVslot04(1);
    out.biasAt04 = 0;
    out.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    if constexpr (rereadControl) PublishRereadControl(out, body, memory);
    else PublishCachedControl(out, body, memory);
    return body;
}

template<bool cleanFailedConstructor>
ParamsPatternFileNameNative* CreateGenericFileName(Slot& out,
    IRuntimeStringManagerSource& strings, IEnvironmentRootMemory& memory)
{
    auto* const body = AllocateBody<ParamsPatternFileNameNative, cleanFailedConstructor>(strings, memory);
    // 41E4B3/41E92C均在构造完成后进入407B20；差别只在构造阶段是否有raw-delete资格。
    ReleaseYtlReference(out);
    if (!body) return nullptr;
    if (out.controlAt08) out.controlAt08->DeleteControlAtVslot04(1);
    out.biasAt04 = 0;
    out.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    PublishRereadControl(out, body, memory);
    return body;
}
}

ParamsPatternFileNameNative::ParamsPatternFileNameNative(IRuntimeStringManagerSource& strings)
{
    kindAt00 = 0;
    InitializeEmptyRuntimeCString(nameAt04, strings);
}
ParamsPatternFileNameNative::~ParamsPatternFileNameNative() noexcept(false)
{
    DestroyRuntimeCStringAt40CD40(&nameAt04);
}

ParamsPatternFileGroupNative::ParamsPatternFileGroupNative(IRuntimeStringManagerSource& strings)
{
    kindAt00 = 1;
    InitializeEmptyRuntimeCString(nameAt04, strings);
    std::construct_at(&stringsAt08);
}
ParamsPatternFileGroupNative::~ParamsPatternFileGroupNative() noexcept
{
    std::destroy_at(&stringsAt08);
    DestroyRuntimeCStringAt40CD40(&nameAt04);
}

ParamsPatternExcPositionNative::ParamsPatternExcPositionNative(IRuntimeStringManagerSource& strings)
{
    kindAt00 = 2;
    InitializeEmptyRuntimeCString(nameAt04, strings);
    InitializeEmptyRuntimeCString(subNameAt08, strings);
    xAt0C = 0;
    yAt10 = 0;
}
ParamsPatternExcPositionNative::~ParamsPatternExcPositionNative() noexcept(false)
{
    DestroyRuntimeCStringAt40CD40(&subNameAt08);
    // subName 回调完成后才从正文读当前 name，不能预缓存两个 header。
    DestroyRuntimeCStringAt40CD40(&nameAt04);
}

ParamsPatternFileConvertNative::ParamsPatternFileConvertNative(IRuntimeStringManagerSource& strings)
{
    kindAt00 = 3;
    InitializeEmptyRuntimeCString(nameAt04, strings);
    InitializeEmptyRuntimeCString(subNameAt08, strings);
    // 原41ED32后不写 flagsAt0C。
}
ParamsPatternFileConvertNative::~ParamsPatternFileConvertNative() noexcept(false)
{
    DestroyRuntimeCStringAt40CD40(&subNameAt08);
    DestroyRuntimeCStringAt40CD40(&nameAt04);
}

ParamsPatternGroupNative::ParamsPatternGroupNative(IRuntimeStringManagerSource& strings)
{
    InitializeEmptyRuntimeCString(nameAt00, strings);
    std::construct_at(&indicesAt04);
}
ParamsPatternGroupNative::~ParamsPatternGroupNative() noexcept
{
    ReleaseYtlReference(indicesAt04);
    DestroyRuntimeCStringAt40CD40(&nameAt00);
}

ParamsPatternFileNameNative* ConstructParamsPatternFileNameAt40C3D0(
    ParamsPatternFileNameNative* storage, IRuntimeStringManagerSource& strings)
{ return std::construct_at(storage, strings); }
ParamsPatternFileGroupNative* ConstructParamsPatternFileGroupAt40C380(
    ParamsPatternFileGroupNative* storage, IRuntimeStringManagerSource& strings)
{ return std::construct_at(storage, strings); }
ParamsPatternExcPositionNative* ConstructParamsPatternExcPositionAt41EB58(
    ParamsPatternExcPositionNative* storage, IRuntimeStringManagerSource& strings)
{ return std::construct_at(storage, strings); }
ParamsPatternFileConvertNative* ConstructParamsPatternFileConvertAt41ECFB(
    ParamsPatternFileConvertNative* storage, IRuntimeStringManagerSource& strings)
{ return std::construct_at(storage, strings); }
ParamsPatternGroupNative* ConstructParamsPatternGroupAt4124DD(
    ParamsPatternGroupNative* storage, IRuntimeStringManagerSource& strings)
{ return std::construct_at(storage, strings); }

ParamsPatternFileNameNative* CreateParamsPatternFileNameAt41E904(
    Slot& out, IRuntimeStringManagerSource& strings, IEnvironmentRootMemory& memory)
{ return CreateGenericFileName<true>(out, strings, memory); }
ParamsPatternFileNameNative* CreateParamsPatternFileNameAt41E080(
    Slot& out, IRuntimeStringManagerSource& strings, IEnvironmentRootMemory& memory)
{ return CreateTypedItem<ParamsPatternFileNameNative, false>(out, strings, memory); }
ParamsPatternFileNameNative* CreateParamsPatternFileNameAt41E47C(
    Slot& out, IRuntimeStringManagerSource& strings, IEnvironmentRootMemory& memory)
{ return CreateGenericFileName<false>(out, strings, memory); }
ParamsPatternFileGroupNative* CreateParamsPatternFileGroupAt41E972(
    Slot& out, IRuntimeStringManagerSource& strings, IEnvironmentRootMemory& memory)
{ return CreateTypedItem<ParamsPatternFileGroupNative, true>(out, strings, memory); }
ParamsPatternExcPositionNative* CreateParamsPatternExcPositionAt41EB42(
    Slot& out, IRuntimeStringManagerSource& strings, IEnvironmentRootMemory& memory)
{ return CreateTypedItem<ParamsPatternExcPositionNative, false>(out, strings, memory); }
ParamsPatternFileConvertNative* CreateParamsPatternFileConvertAt41ECE8(
    Slot& out, IRuntimeStringManagerSource& strings, IEnvironmentRootMemory& memory)
{ return CreateTypedItem<ParamsPatternFileConvertNative, false>(out, strings, memory); }
ParamsPatternGroupNative* CreateParamsPatternFirstGroupAt41F163(
    Slot& out, IRuntimeStringManagerSource& strings, IEnvironmentRootMemory& memory)
{ return RebuildGroup<false>(out, strings, memory); }
ParamsPatternGroupNative* RebuildParamsPatternGroupAt412490(
    Slot& out, IRuntimeStringManagerSource& strings, IEnvironmentRootMemory& memory)
{ return RebuildGroup<true>(out, strings, memory); }

std::int32_t* RebuildParamsPatternGroupIndices(Slot& out, std::uint32_t count,
    IEnvironmentRootMemory& memory)
{
    // 41C2DE..360 与41F331..3B9/41F68E..727的释放、分配、重读、发布顺序同构。
    return RebuildParamsLegacyIntegersAt41C2DE(out, count, memory);
}

std::int32_t* CreateParamsPatternOuterIntegersAt41EF45(Slot& out, std::uint32_t count,
    IEnvironmentRootMemory& memory)
{
    out.controlAt08 = nullptr;
    out.baseAt00 = 0;
    // bias 的初始零在41E3D0，由循环外局部槽承担；41EF45不重写它。
    if (!count) return nullptr;
    constexpr auto maximum = (std::numeric_limits<std::uint32_t>::max)();
    const auto bytes = count > maximum / 4u ? maximum : count * 4u;
    auto* const body = static_cast<std::int32_t*>(::operator new[](bytes));
    out.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    auto* const control = AllocateParamsLegacyIntegerControl(body, memory);
    if (!control) _invalid_parameter_noinfo_noreturn();
    control->referenceCountAt04 = 1;
    control->owningAt08 = 1;
    control->strideAt0C = 4;
    out.controlAt08 = control;
    control->countAt10 = count;
    return body;
}

void DestroyParamsPatternFileName(ParamsPatternFileNameNative& body) { body.~ParamsPatternFileNameNative(); }
void DestroyParamsPatternFileGroupAt40F280(ParamsPatternFileGroupNative& body) noexcept { std::destroy_at(&body); }
void DestroyParamsPatternExcPosition(ParamsPatternExcPositionNative& body) { body.~ParamsPatternExcPositionNative(); }
void DestroyParamsPatternFileConvert(ParamsPatternFileConvertNative& body) { body.~ParamsPatternFileConvertNative(); }
void DestroyParamsPatternGroupAt40F7D0(ParamsPatternGroupNative& body) noexcept { std::destroy_at(&body); }
void ReleaseParamsPatternItemAt43B810(Slot& reference) { ReleaseYtlReference(reference); }
void ReleaseParamsPatternGroupAt43B850(Slot& reference) { ReleaseYtlReference(reference); }
void DestroyParamsPatternItemReferenceAt40D590(Slot& reference) noexcept { ReleaseYtlReference(reference); }
void DestroyParamsPatternGroupReferenceAt40D670(Slot& reference) noexcept { ReleaseYtlReference(reference); }

ParamsPatternFileNameNative* ResolveParamsPatternFileName(Slot& reference) noexcept
{ return ResolveEnvironmentObjectReference<ParamsPatternFileNameNative>(reference); }
const ParamsPatternFileNameNative* ResolveParamsPatternFileName(const Slot& reference) noexcept
{ return ResolveEnvironmentObjectReference<const ParamsPatternFileNameNative>(reference); }
ParamsPatternFileGroupNative* ResolveParamsPatternFileGroup(Slot& reference) noexcept
{ return ResolveEnvironmentObjectReference<ParamsPatternFileGroupNative>(reference); }
const ParamsPatternFileGroupNative* ResolveParamsPatternFileGroup(const Slot& reference) noexcept
{ return ResolveEnvironmentObjectReference<const ParamsPatternFileGroupNative>(reference); }
ParamsPatternExcPositionNative* ResolveParamsPatternExcPosition(Slot& reference) noexcept
{ return ResolveEnvironmentObjectReference<ParamsPatternExcPositionNative>(reference); }
const ParamsPatternExcPositionNative* ResolveParamsPatternExcPosition(const Slot& reference) noexcept
{ return ResolveEnvironmentObjectReference<const ParamsPatternExcPositionNative>(reference); }
ParamsPatternFileConvertNative* ResolveParamsPatternFileConvert(Slot& reference) noexcept
{ return ResolveEnvironmentObjectReference<ParamsPatternFileConvertNative>(reference); }
const ParamsPatternFileConvertNative* ResolveParamsPatternFileConvert(const Slot& reference) noexcept
{ return ResolveEnvironmentObjectReference<const ParamsPatternFileConvertNative>(reference); }
ParamsPatternGroupNative* ResolveParamsPatternGroup(Slot& reference) noexcept
{ return ResolveEnvironmentObjectReference<ParamsPatternGroupNative>(reference); }
const ParamsPatternGroupNative* ResolveParamsPatternGroup(const Slot& reference) noexcept
{ return ResolveEnvironmentObjectReference<const ParamsPatternGroupNative>(reference); }
}
