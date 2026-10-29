#include <Runtime/SpriteNative.h>

#include <bit>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Body = SpriteNativeBodyA0h;
Body* __fastcall Delete(Body* body, void*, std::uint32_t flags) noexcept
{ return DeleteSpriteNativeAt43EDB0(body, flags); }
const SpriteNativeVtable methods{
    reinterpret_cast<decltype(SpriteNativeVtable::deletingDestructorAt00)>(&Delete)};

struct ConstructionUnwind final
{
    Body& body;
    bool firstString = false;
    bool completeMembers = false;
    ~ConstructionUnwind() noexcept
    {
        if (completeMembers)
        {
            DestroyGuiControlSurfaceReferencesAt43EAA0(body.auxiliarySurfacesAt94);
            ReleaseYtlReference(body.primarySurfacesAt88);
            DestroySpriteAnimationAt43EB50(body.animationAt34);
            DestroyRuntimeCStringVectorAt40DD20(body.memberNamesAt18);
            DestroyRuntimeCStringAt40CD40(&body.primaryNameAt14);
        }
        if (firstString) DestroyRuntimeCStringAt40CD40(&body.logicalNameAt0C);
    }
};
template<bool Array> class SpriteControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit SpriteControl(Body* body) noexcept : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        auto* const body = static_cast<Body*>(payloadAt14);
        if (!body) return;
        if constexpr (Array)
        {
            auto* const cookie = reinterpret_cast<std::uint32_t*>(body) - 1;
            if (*cookie) body->vptrAt00->deletingDestructorAt00(body, 3);
            else ::operator delete[](cookie); // 491FC7：零cookie不读取任何元素vptr。
        }
        else body->vptrAt00->deletingDestructorAt00(body, 1);
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override { RetireEnvironmentObjectControl(this, flags); }
};
static_assert(sizeof(SpriteControl<false>) == 0x18 && sizeof(SpriteControl<true>) == 0x18);

template<bool Array> void Publish(YtlReferenceSlotRuntime& slot, Body* body, std::uint32_t count)
{
    slot.biasAt04 = 0;
    slot.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    auto* const raw = ::operator new(std::size_t{0x18});
    auto* const control = raw ? ::new (raw) SpriteControl<Array>(body) : nullptr;
    slot.controlAt08 = control;
    control->referenceCountAt04 = 1;
    slot.controlAt08->owningAt08 = 1;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->strideAt0C = 0xA0;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->countAt10 = count;
}
}

const SpriteNativeVtable& SpriteNativeMethods() noexcept { return methods; }
SpriteNativeBodyA0h* ConstructSpriteNativeAt45D7D0(Body& body, IRuntimeStringManagerSource& strings)
{
    body.vptrAt00 = &methods;
    ConstructionUnwind unwind{body};
    InitializeEmptyRuntimeCString(body.logicalNameAt0C, strings);
    unwind.firstString = true;
    InitializeEmptyRuntimeCString(body.primaryNameAt14, strings);
    // 第二次GetNil后到state5之间无外部调用；不能在前面提前清这些正文槽。
    body.memberNamesAt18 = {};
    body.widthAt24 = 0; body.heightAt28 = 0; body.xAt2C = 0; body.yAt30 = 0;
    // union只延迟默认零写；此处才显式开始Ref成员生命期，按原顺序清各12-byte槽。
    ::new (static_cast<void*>(&body.animationAt34.currentAt18)) YtlReferenceSlotRuntime{};
    ::new (static_cast<void*>(&body.animationAt34.finalAt24)) YtlReferenceSlotRuntime{};
    ::new (static_cast<void*>(&body.animationAt34.countersAt30)) YtlReferenceSlotRuntime{};
    ::new (static_cast<void*>(&body.animationAt34.commandsAt3C)) YtlReferenceSlotRuntime{};
    ::new (static_cast<void*>(&body.animationAt34.branchesAt48)) YtlReferenceSlotRuntime{};
    ::new (static_cast<void*>(&body.primarySurfacesAt88)) YtlReferenceSlotRuntime{};
    body.auxiliarySurfacesAt94 = {};
    unwind.completeMembers = true;
    ClearSpriteNativeAt43F140(body);
    unwind.completeMembers = false; unwind.firstString = false;
    return &body;
}
void ClearSpriteNativeAt43F140(Body& body)
{
    body.modeAt04 = 0; body.activeAt08 = 0;
    body.widthAt24 = 0; body.heightAt28 = 0; body.xAt2C = 0; body.yAt30 = 0;
    SetRuntimeCStringAt437680(body.logicalNameAt0C, "", 0);
    SetRuntimeCStringAt437680(body.primaryNameAt14, "", 0);
    ClearSpriteNameVectorAt43F240(body.memberNamesAt18);
    body.argumentAt10 = 0;
    ReleaseYtlReference(body.primarySurfacesAt88);
    ReleaseGuiControlSurfaceRangeAt43EA10(body.auxiliarySurfacesAt94.begin, body.auxiliarySurfacesAt94.end);
    body.auxiliarySurfacesAt94.end = body.auxiliarySurfacesAt94.begin;
    ClearSpriteAnimationAt43EF10(body.animationAt34);
}
void DestroySpriteNativeAt43ECB0(Body& body) noexcept
{
    body.vptrAt00 = &methods;
    ClearSpriteNativeAt43F140(body);
    DestroyGuiControlSurfaceReferencesAt43EAA0(body.auxiliarySurfacesAt94);
    ReleaseYtlReference(body.primarySurfacesAt88);
    DestroySpriteAnimationAt43EB50(body.animationAt34);
    DestroyRuntimeCStringVectorAt43A700(body.memberNamesAt18);
    DestroyRuntimeCStringAt40CD40(&body.primaryNameAt14);
    DestroyRuntimeCStringAt40CD40(&body.logicalNameAt0C);
}
SpriteNativeBodyA0h* DeleteSpriteNativeAt43EDB0(Body* body, std::uint32_t flags) noexcept
{
    if (flags & 2u)
    {
        auto* const cookie = reinterpret_cast<std::uint32_t*>(body) - 1;
        const auto count = *cookie;
        for (auto index = count; index; --index) DestroySpriteNativeAt43ECB0(body[index - 1]);
        if (flags & 1u) ::operator delete[](cookie, std::uint32_t{4} + *cookie * 0xA0u);
        return reinterpret_cast<Body*>(cookie);
    }
    DestroySpriteNativeAt43ECB0(*body);
    if (flags & 1u) ::operator delete(body, std::size_t{0xA0});
    return body;
}
SpriteNativeBodyA0h* NewSpriteNativeAt4813E4(IRuntimeStringManagerSource& strings)
{
    void* const raw = ::operator new(std::size_t{0xA0});
    if (!raw) return nullptr;
    try { return ConstructSpriteNativeAt45D7D0(*::new (raw) Body, strings); }
    catch (...)
    {
        // 原EH动作处仍在展开；不能在catch正文先delete再throw，改变重入可见的异常状态。
        struct Unwind final
        {
            void* raw;
            ~Unwind() noexcept { ::operator delete(raw, std::size_t{0xA0}); }
        } unwind{raw};
        throw;
    }
}
void InstallSpriteReferenceAt48140F(YtlReferenceSlotRuntime& slot, Body* completed)
{
    ReleaseYtlReference(slot);
    if (!completed) return;
    // 48143F已清ECX；不重新读取slot，不额外delete重入晚填control。
    Publish<false>(slot, completed, 1);
}
SpriteNativeBodyA0h* NewSpriteArrayAt45B327(std::uint32_t count, IRuntimeStringManagerSource& strings)
{
    const auto wide = static_cast<std::uint64_t>(count) * 0xA0u + 4u;
    const auto bytes = wide > 0xFFFFFFFFu ? 0xFFFFFFFFu : static_cast<std::uint32_t>(wide);
    auto* const raw = ::operator new[](bytes);
    if (!raw) return nullptr;
    *static_cast<std::uint32_t*>(raw) = count;
    auto* const body = reinterpret_cast<Body*>(static_cast<std::byte*>(raw) + 4);
    std::uint32_t constructed = 0;
    try
    {
        for (; constructed < count; ++constructed)
            ConstructSpriteNativeAt45D7D0(*::new (static_cast<void*>(body + constructed)) Body, strings);
    }
    catch (...)
    {
        struct Unwind final
        {
            void* raw;
            Body* body;
            std::uint32_t constructed, bytes;
            ~Unwind() noexcept
            {
                while (constructed) DestroySpriteNativeAt43ECB0(body[--constructed]);
                ::operator delete[](raw, bytes);
            }
        } unwind{raw, body, constructed, bytes};
        throw;
    }
    return body;
}
void InstallSpriteArrayReferenceAt45B361(YtlReferenceSlotRuntime& slot, Body* completed, std::int32_t count)
{
    ReleaseYtlReference(slot);
    if (count <= 0) return;
    // 45B391已清ECX；与444EB0的分配后重新取control不是同一种installer。
    Publish<true>(slot, completed, static_cast<std::uint32_t>(count));
}
SpriteNativeBodyA0h* ResolveSpriteReference(const YtlReferenceSlotRuntime& slot) noexcept
{ return ResolveEnvironmentObjectReference<Body>(slot); }
void ReleaseSpriteReference(YtlReferenceSlotRuntime& slot) { ReleaseYtlReference(slot); }
void ReleaseSpriteReferenceAt460260(YtlReferenceSlotRuntime& slot) noexcept { ReleaseYtlReference(slot); }
}
