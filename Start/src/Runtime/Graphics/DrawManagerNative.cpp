#include <Runtime/DrawManagerNative.h>
#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/LayerListNative.h>
#include <Runtime/TransitRuntime.h>
#include <Runtime/TransitNative.h>
#include <Runtime/GameBuildProfile.h>

#include <cstdlib>
#include <bit>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Body = DrawManagerNativeBody2Ch;
using Table = DrawManagerNativeVtable;

Body* __fastcall DeleteBody(Body* self, void*, std::uint32_t flags)
{
    self->DestroyAt462BB0();
    if (flags & 1u) ::operator delete(self, std::size_t{0x2C});
    return self;
}

// 原生x86虚槽桥接：对象走ECX，额外void*只占EDX且不消费，其余参数/返回与原槽对应。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const Table methods{reinterpret_cast<decltype(Table::deletingDestructorAt00)>(&DeleteBody)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif

class DrawManagerControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit DrawManagerControl(Body* body) noexcept
        : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        auto* body = static_cast<Body*>(payloadAt14);
        if (body) body->vptrAt00->deletingDestructorAt00(body, 1);
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
        { RetireEnvironmentObjectControl(this, flags); }
};
static_assert(sizeof(DrawManagerControl) == 0x18);

void UnwindMembers(Body& body) noexcept
{
    // 596356 -> 59634B -> 596340，各成员清理自身为终止边界，不补删裸base。
    ReleaseYtlReference(body.transitAt20);
    DestroyDrawLayerReferencesAt461750(body.ownedLayersAt14);
    ReleaseYtlReference(body.registeredLayersAt08);
}

const LayerItemNativeFrameVtable& FrameMethods(LayerItemNativeBody14h* body)
{
    if (!body) throw std::logic_error("DrawManager native owning layer has an empty base");
    // 46B270/47E970的接收者必须具备扩展表；不为ILayerItem基类伪造这些槽。
    return *reinterpret_cast<const LayerItemNativeFrameVtable*>(body->vptrAt00);
}

std::uint8_t RequestLayer(IDrawManagerNativeRootSource& source,
    std::uint32_t index, std::uint8_t requested, std::int32_t argument = 0)
{
    auto* const manager = source.CaptureDrawManagerAfter413BE0();
    if (!manager) throw std::logic_error("DrawManager prepare has an empty root reference");
    auto* const layer = DrawLayerAt418C10(*manager, index);
    return FrameMethods(layer).requestAt10(layer, requested, argument);
}
}

DrawManagerNativeBody2Ch::DrawManagerNativeBody2Ch(ICrtRandomSource& random,
    IEnvironmentRootMemory& memory) : vptrAt00(&methods), ownedLayersAt14{}
{
    try
    {
        auto* list = CreateLayerListNativeAt440E30(memory);
        InstallLayerListReferenceAt4516C0(registeredLayersAt08, list, memory);
        auto* transit = NewTransitNativeAt458819(random, memory);
        InstallTransitReferenceAt45883E(transitAt20, transit, memory);
    }
    catch (...)
    {
        UnwindMembers(*this);
        throw;
    }
}

void DrawManagerNativeBody2Ch::DestroyAt462BB0() noexcept
{
    vptrAt00 = &methods;
    ReleaseYtlReference(transitAt20);
    ReleaseDrawLayerRangeAt452440(ownedLayersAt14.begin, ownedLayersAt14.end);
    ownedLayersAt14.end = ownedLayersAt14.begin;
    ReleaseYtlReference(registeredLayersAt08);
    // 462C66后的成员尾部并非冗余：前面的回调允许重新填充这两槽及数组。
    ReleaseYtlReference(transitAt20);
    DestroyDrawLayerReferencesAt461750(ownedLayersAt14);
    ReleaseYtlReference(registeredLayersAt08);
}

Body* NewDrawManagerNativeAt48A593(ICrtRandomSource& random, IEnvironmentRootMemory& memory)
{
    void* storage = memory.AllocateBody(0x2C);
    if (!storage) return nullptr;
    try { return ::new(storage) Body(random, memory); }
    catch (...)
    {
        // 59ABC6只删未完成的2Ch裸块，成员前缀由458770自己的EH清理。
        [&]() noexcept { memory.DeleteBody(storage, 0x2C); }();
        throw;
    }
}

void InstallDrawManagerReferenceAt48A5B3(YtlReferenceSlotRuntime& slot, Body* created,
    IEnvironmentRootMemory& memory)
{
    ReleaseYtlReference(slot);
    if (!created) return;
    // 48A5F0检查的是已清零的ECX，不是再读slot并追加一次control删除。
    slot.biasAt04 = 0;
    slot.baseAt00 = reinterpret_cast<std::uintptr_t>(created);
    void* storage = memory.AllocateBody(0x18);
    auto* control = storage ? ::new(storage) DrawManagerControl(created) : nullptr;
    slot.controlAt08 = control;
    // 48A626直接写control+04；null返回保留原访问故障，不转换成invalid_parameter。
    control->referenceCountAt04 = 1;
    slot.controlAt08->owningAt08 = 1;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->strideAt0C = 0x2C;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->countAt10 = 1;
}

Body* ResolveDrawManagerReferenceAt4175A0(const YtlReferenceSlotRuntime& slot) noexcept
    { return ResolveEnvironmentObjectReference<Body>(slot); }
LayerListNativeBody18h* ResolveDrawLayerListAt47AE80(const Body& manager) noexcept
    { return ResolveLayerListReference(manager.registeredLayersAt08); }
TransitRuntime* ResolveDrawTransit(const Body& manager) noexcept
    { return ResolveTransitReference(manager.transitAt20); }

std::uint8_t RenderDrawTransitionAt4C39E0(const Body& manager,
    const TransitOperation& operation, IGraphicsTransitApi& graphics)
{
    auto* const transit = ResolveDrawTransit(manager);
    // 4C39E3先读manager+20；空Ref仍以ECX=0尾跳449DD0，不增设空Transit保护。
    TransitGraphicsScope scope(graphics);
    return transit->DispatchRawAt449DD0(operation);
}

LayerItemNativeBody14h* DrawLayerAt418C10(const Body& manager, std::uint32_t index)
{
    // 418C14/418C1C（47E1F4内联同序）先固定begin，再读end；空receiver必须先read14。
    const auto* first = static_cast<const volatile DrawLayerReferenceArray&>(manager.ownedLayersAt14).begin;
    const auto* last = static_cast<const volatile DrawLayerReferenceArray&>(manager.ownedLayersAt14).end;
    const auto bytes = std::uint32_t(reinterpret_cast<std::uintptr_t>(last)-reinterpret_cast<std::uintptr_t>(first));
    const auto count = std::uint32_t(std::bit_cast<std::int32_t>(bytes)/12);
    if (index > count) return nullptr;
    // 418C2F..418C3D的unsigned jnb包括index==count；继续读取物理槽。
    return ResolveDrawLayerReference(*reinterpret_cast<const YtlReferenceSlotRuntime*>(
        reinterpret_cast<std::uintptr_t>(first)+index*12u));
}

void AdvanceDrawLayersAt46B240(Body& manager)
{
    // 46B244固定游标；回调以后重读end，但不通过index重新抓begin适配重分配。
    auto* cursor = manager.ownedLayersAt14.begin;
    while (cursor != manager.ownedLayersAt14.end)
    {
        auto* const layer = ResolveDrawLayerReference(*cursor);
        FrameMethods(layer).advanceAt14(layer);
        ++cursor;
    }
}

void ResetDrawLayersAt47E940(Body& manager)
{
    auto* cursor = manager.ownedLayersAt14.begin;
    while (cursor != manager.ownedLayersAt14.end)
    {
        auto* const layer = ResolveDrawLayerReference(*cursor);
        FrameMethods(layer).resetAt18(layer);
        ++cursor;
    }
}

std::uint8_t PrepareDrawLayersAt46A980(IDrawManagerNativeRootSource& source, std::int32_t mode)
{
    auto result = RequestLayer(source, 1, 0);
    for (std::uint32_t index = 2; index <= LastStartupDrawLayerIndex(); ++index)
    {
        const auto request = StartupPrepareRequest(index, false, mode);
        result = static_cast<std::uint8_t>(result | RequestLayer(source, index, request.requested, request.argument));
    }
    return result;
}

std::uint8_t PrepareDrawLayersAt46AD20(IDrawManagerNativeRootSource& source, std::int32_t mode)
{
    auto result = RequestLayer(source, 1, 1);
    for (std::uint32_t index = 2; index <= LastStartupDrawLayerIndex(); ++index)
    {
        const auto request = StartupPrepareRequest(index, true, mode);
        result = static_cast<std::uint8_t>(result | RequestLayer(source, index, request.requested, request.argument));
    }
    return result;
}

void ReleaseDrawManagerReferenceAt477736(YtlReferenceSlotRuntime& slot) { ReleaseYtlReference(slot); }
void ReleaseDrawManagerReferenceAt40D050(YtlReferenceSlotRuntime& slot) noexcept { ReleaseYtlReference(slot); }
void ReleaseDrawManagerReferenceAt4643DB(YtlReferenceSlotRuntime& slot) noexcept { ReleaseYtlReference(slot); }
}
