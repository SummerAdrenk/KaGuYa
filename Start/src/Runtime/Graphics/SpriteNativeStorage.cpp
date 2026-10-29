#include <Runtime/SpriteNativeStorage.h>

#include <bit>
#include <cstdlib>
#include <cstring>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
std::uint32_t ArrayBytes(std::uint32_t count, std::uint32_t stride, bool cookie) noexcept
{
    const auto bytes = static_cast<std::uint64_t>(count) * stride + (cookie ? 4u : 0u);
    return bytes > 0xFFFFFFFFu ? 0xFFFFFFFFu : static_cast<std::uint32_t>(bytes);
}
enum class ArrayKind { Integers, Branches, Commands, Surfaces };
template<ArrayKind Kind> class ArrayControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit ArrayControl(void* body) noexcept : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        if constexpr (Kind == ArrayKind::Integers || Kind == ArrayKind::Branches)
        {
            // 43BAA0/4487E0：无cookie，无count乘法，无元素析构，只有unsized CRT释放。
            ::operator delete[](payloadAt14);
        }
        else if constexpr (Kind == ArrayKind::Commands)
            DeleteSpriteCommandArrayAt4487F0(static_cast<SpriteCommandNativeBody10h*>(payloadAt14));
        else
        {
            // 43BC00：count以cookie为准，非0时调用首元素当前vptr，不能直调默认删除槽。
            auto* const body = static_cast<GraphicsSurfaceNativeBody*>(payloadAt14);
            if (!body) return;
            auto* const cookie = reinterpret_cast<std::uint32_t*>(body) - 1;
            if (*cookie) body->vptrAt00->deletingDestructor(body, 3);
            else ::operator delete[](cookie);
        }
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override { RetireEnvironmentObjectControl(this, flags); }
};
static_assert(sizeof(ArrayControl<ArrayKind::Commands>) == 0x18);

template<ArrayKind Kind> void Publish(YtlReferenceSlotRuntime& slot, void* body,
    std::uint32_t stride, std::uint32_t count)
{
    // 原数组工厂在元素构造完成/分配回调后重查control；只删control，不再次减持payload。
    if (slot.controlAt08) slot.controlAt08->DeleteControlAtVslot04(1);
    slot.biasAt04 = 0;
    slot.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    auto* const raw = ::operator new(std::size_t{0x18});
    auto* const control = raw ? ::new (raw) ArrayControl<Kind>(body) : nullptr;
    slot.controlAt08 = control;
    // 保留原null+4非法写入，不添加分配失败回滚或伪成功。
    control->referenceCountAt04 = 1;
    slot.controlAt08->owningAt08 = 1;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->strideAt0C = stride;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->countAt10 = count;
}
}

void ClearSpriteNameVectorAt43F240(EnvironmentVectorStorage<RuntimeCString>& names) noexcept
{
    // PE5E3568 flags5；正序固定范围，Free可重入改头，末尾才重读begin。
    auto* const end = names.end;
    for (auto* current = names.begin; current != end; ++current)
        DestroyRuntimeCStringAt40CD40(current);
    names.end = names.begin;
}
void ClearSpriteAnimationAt43EF10(SpriteAnimationNativeBody54h& body)
{
    body.flagsAt00 = 0; body.waitAt04 = 0; body.indexAt08 = 0;
    body.mappingCountAt14 = 0; body.repeatAt10 = 0;
    ReleaseYtlReference(body.currentAt18);
    ReleaseYtlReference(body.finalAt24);
    ReleaseYtlReference(body.countersAt30);
    ReleaseYtlReference(body.commandsAt3C);
    ReleaseYtlReference(body.branchesAt48);
    body.resultAt0C = -1;
}
void DestroySpriteAnimationAt43EB50(SpriteAnimationNativeBody54h& body) noexcept
{
    ReleaseYtlReference(body.branchesAt48);
    ReleaseYtlReference(body.commandsAt3C);
    ReleaseYtlReference(body.countersAt30);
    ReleaseYtlReference(body.finalAt24);
    ReleaseYtlReference(body.currentAt18);
}
void ConstructSpriteCommandAt444920(SpriteCommandNativeBody10h& body) noexcept
{ body.opcodeAt00 = 0; body.argumentsAt04 = {}; }
void DestroySpriteCommandAt4449C0(SpriteCommandNativeBody10h& body) noexcept
{
    auto& arguments = body.argumentsAt04;
    if (!arguments.begin) return;
    void* raw = arguments.begin;
    auto bytes = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(arguments.capacity) -
        reinterpret_cast<std::uintptr_t>(arguments.begin)) & ~3u;
    if (bytes >= 0x1000u)
    {
        const auto address = reinterpret_cast<std::uintptr_t>(raw);
        std::memcpy(&raw, reinterpret_cast<void*>(address - 4u), sizeof(raw));
        bytes += 0x23u;
        if (address - reinterpret_cast<std::uintptr_t>(raw) - 4u > 0x1Fu)
            _invalid_parameter_noinfo_noreturn();
    }
    ::operator delete(raw, bytes);
    arguments = {};
}
void DeleteSpriteCommandArrayAt4487F0(SpriteCommandNativeBody10h* body) noexcept
{
    if (!body) return;
    auto* const cookie = reinterpret_cast<std::uint32_t*>(body) - 1;
    const auto count = *cookie;
    for (auto index = count; index; --index) DestroySpriteCommandAt4449C0(body[index - 1]);
    ::operator delete[](cookie, std::uint32_t{4} + *cookie * 0x10u);
}
void ReplaceSpriteIntArrayAt4127B0(YtlReferenceSlotRuntime& slot, std::int32_t count)
{
    ReleaseYtlReference(slot);
    if (count <= 0) return;
    auto* const body = ::operator new[](ArrayBytes(static_cast<std::uint32_t>(count), 4, false));
    // 原整数正文不初始化；映射填-1属于后续47E980，不提前放进数组工厂。
    Publish<ArrayKind::Integers>(slot, body, 4, static_cast<std::uint32_t>(count));
}
void ReplaceSpriteCommandArrayAt444EB0(YtlReferenceSlotRuntime& slot, std::int32_t count)
{
    ReleaseYtlReference(slot);
    if (count <= 0) return;
    auto* const raw = ::operator new[](ArrayBytes(static_cast<std::uint32_t>(count), 0x10, true));
    SpriteCommandNativeBody10h* body = nullptr;
    if (raw)
    {
        *static_cast<std::uint32_t*>(raw) = static_cast<std::uint32_t>(count);
        body = reinterpret_cast<SpriteCommandNativeBody10h*>(static_cast<std::byte*>(raw) + 4);
        for (std::int32_t i = 0; i < count; ++i)
            ConstructSpriteCommandAt444920(*::new (static_cast<void*>(body + i)) SpriteCommandNativeBody10h);
    }
    // 444920无可抛调用；完成后撤raw保护，后续发布失败不删数组。
    Publish<ArrayKind::Commands>(slot, body, 0x10, static_cast<std::uint32_t>(count));
}
void ReplaceSpriteSurfaceArrayAt444FE0(YtlReferenceSlotRuntime& slot, std::int32_t count,
    IGraphicsSurfaceApi& surfaces, IRuntimeStringManagerSource& strings)
{
    GraphicsSurfaceNativeScope scope(surfaces);
    ReleaseYtlReference(slot);
    if (count <= 0) return;
    const auto bytes = ArrayBytes(static_cast<std::uint32_t>(count), 0x24, true);
    auto* const raw = ::operator new[](bytes);
    GraphicsSurfaceNativeBody* body = nullptr;
    if (raw)
    {
        *static_cast<std::uint32_t*>(raw) = static_cast<std::uint32_t>(count);
        body = reinterpret_cast<GraphicsSurfaceNativeBody*>(static_cast<std::byte*>(raw) + 4);
        std::int32_t constructed = 0;
        try
        {
            for (; constructed < count; ++constructed)
                ::new (static_cast<void*>(body + constructed)) GraphicsSurfaceNativeBody(surfaces, strings);
        }
        catch (...)
        {
            // 原vector ctor只销毁此前完整元素，随后595400删原申请尺寸的裸cookie块。
            struct Unwind final
            {
                void* raw;
                GraphicsSurfaceNativeBody* body;
                std::int32_t constructed;
                std::uint32_t bytes;
                ~Unwind() noexcept
                {
                    while (constructed) body[--constructed].~GraphicsSurfaceNativeBody();
                    ::operator delete[](raw, bytes);
                }
            } unwind{raw, body, constructed, bytes};
            throw;
        }
    }
    Publish<ArrayKind::Surfaces>(slot, body, 0x24, static_cast<std::uint32_t>(count));
}
void ReplaceSpriteBranchArrayAt446327(YtlReferenceSlotRuntime& slot, std::uint16_t count)
{
    ReleaseYtlReference(slot);
    if (!count) return;
    auto* const body = static_cast<SpriteBranchNativeBody8h*>(::operator new[](static_cast<std::uint32_t>(count) * 8u));
    if (body) for (std::uint32_t i = 0; i < count; ++i)
        ::new (static_cast<void*>(body + i)) SpriteBranchNativeBody8h{-1, -1};
    Publish<ArrayKind::Branches>(slot, body, 8, count);
}
}
