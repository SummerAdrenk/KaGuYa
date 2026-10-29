#include <Runtime/GuiControlFactoryNative.h>
#include <bit>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
std::int32_t Add(std::int32_t a, std::int32_t b) noexcept
{ return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(a) + static_cast<std::uint32_t>(b)); }
std::int32_t Sub(std::int32_t a, std::int32_t b) noexcept
{ return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(a) - static_cast<std::uint32_t>(b)); }
struct GroupUnwind final
{
    YtlReferenceSlotRuntime& slot;
    bool active = true;
    ~GroupUnwind() noexcept { if (active) ReleaseGuiControlResourceReferenceAt4F6380(slot); }
    void FinishCaptured(YtlReferenceControlRuntime* control) noexcept
    {
        active = false;
        if (control && --control->referenceCountAt04 == 0)
        {
            if (control->owningAt08) control->DestroyPayloadAtVslot00();
            control->DeleteControlAtVslot04(1);
        }
    }
};
struct RawStaticUnwind final
{
    void* body;
    bool active = true;
    ~RawStaticUnwind() noexcept { if (active) ::operator delete(body, std::size_t{0x50}); }
};
struct RawButtonUnwind final
{
    void* body;
    bool active = true;
    ~RawButtonUnwind() noexcept { if (active) ::operator delete(body, std::size_t{0x5C}); }
};
struct ButtonResultUnwind final
{
    YtlReferenceSlotRuntime& slot;
    bool active = false;
    ~ButtonResultUnwind() noexcept { if (active) ReleaseGuiControlReferenceAt4F6460(slot); }
};
void FinishMoveArgument(YtlReferenceSlotRuntime& slot) noexcept
{
    auto* control = slot.controlAt08;
    if (control && --control->referenceCountAt04 == 0)
    {
        if (control->owningAt08) control->DestroyPayloadAtVslot00();
        if (slot.controlAt08) slot.controlAt08->DeleteControlAtVslot04(1);
    }
}
}
YtlReferenceSlotRuntime* CreateGuiButtonAt5041D0(YtlReferenceSlotRuntime& output,
    void* parent, const GuiButtonDescriptorNative2Ch& descriptor,
    IGuiControlResourceLoadServices& loader, IGraphicsSurfaceApi& surfaces,
    IRuntimeStringManagerSource& strings)
{
    GraphicsSurfaceNativeScope scope(surfaces);
    // 5F463C state1先释放group，再进入state0的fresh返回Ref守卫。
    ButtonResultUnwind resultLifetime{output};
    YtlReferenceSlotRuntime group;
    LoadGuiControlResourceAt5047A0(group, descriptor.common.namesAt04.data(), loader, surfaces, strings);
    GroupUnwind groupLifetime{group};
    const auto capturedBase = group.baseAt00;
    if (!capturedBase) throw 1;
    auto* const capturedControl = group.controlAt08;
    if (const auto* point = descriptor.common.positionAt20)
    {
        const auto y = point->y;
        const auto x = point->x;
        YtlReferenceSlotRuntime copied{capturedBase, group.biasAt04, capturedControl};
        if (capturedControl) ++capturedControl->referenceCountAt04;
        MoveGuiControlResourceAt5086C0(copied, x, y);
    }
    void* raw = ::operator new(0x5C);
    RawButtonUnwind rawLifetime{raw};
    GuiButtonNativeBody5Ch* body = nullptr;
    if (raw)
    {
        YtlReferenceSlotRuntime copied{capturedBase, group.biasAt04, capturedControl};
        if (capturedControl) ++capturedControl->referenceCountAt04;
        body = ConstructGuiButtonAt4DA700(static_cast<GuiButtonNativeBody5Ch*>(raw),
            parent, descriptor.common.eventAt00, copied);
    }
    rawLifetime.active = false;
    ConstructGuiButtonReferenceAt504298(output, body);
    // 504303之后返回Ref才有展开资格；control分配失败保留完成正文。
    resultLifetime.active = true;
    auto* child = ResolveGuiControl(output);
    child->childIdAt0C = descriptor.childIdAt28;
    child = ResolveGuiControl(output);
    reinterpret_cast<const GuiButtonNativeVtable29*>(child->vptrAt00)->setHitSlotAt60(child, descriptor.hitSlotAt24);
    groupLifetime.FinishCaptured(capturedControl);
    resultLifetime.active = false;
    return &output;
}
void MoveGuiControlResourceAt5086C0(YtlReferenceSlotRuntime ownedGroup,
    std::int32_t x, std::int32_t y)
{
    auto rect = RequireGuiControlResource(ownedGroup).combinedRectAt28;
    auto* current = RequireGuiControlResource(ownedGroup).surfacesAt1C.begin;
    while (current != RequireGuiControlResource(ownedGroup).surfacesAt1C.end)
    {
        auto* surface = ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(*current);
        if (!surface) throw std::logic_error("5086C0 dereferenced a null Surface");
        const auto movedX = Add(Sub(x, rect.left), surface->originXAt10);
        const auto movedY = Add(Sub(y, rect.top), surface->originYAt14);
        surface = ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(*current);
        surface->originXAt10 = movedX;
        surface->originYAt14 = movedY;
        ++current;
    }
    rect.right = Add(rect.right, Sub(x, rect.left));
    rect.left = x;
    rect.bottom = Add(rect.bottom, Sub(y, rect.top));
    rect.top = y;
    ::CopyRect(&RequireGuiControlResource(ownedGroup).combinedRectAt28, &rect);
    FinishMoveArgument(ownedGroup);
}
YtlReferenceSlotRuntime* CreateGuiStaticAt505800(YtlReferenceSlotRuntime& output,
    void* parent, const GuiStaticDescriptorNative24h& descriptor,
    IGuiControlResourceLoadServices& loader, IGraphicsSurfaceApi& surfaces,
    IRuntimeStringManagerSource& strings)
{
    GraphicsSurfaceNativeScope scope(surfaces);
    YtlReferenceSlotRuntime group;
    LoadGuiControlResourceAt5047A0(group, descriptor.namesAt04.data(), loader, surfaces, strings);
    GroupUnwind groupLifetime{group};
    const auto capturedBase = group.baseAt00;
    if (!capturedBase) throw 1;
    auto* const capturedControl = group.controlAt08;
    if (const auto* point = descriptor.positionAt20)
    {
        const auto y = point->y;
        const auto x = point->x;
        YtlReferenceSlotRuntime copied{capturedBase, group.biasAt04, capturedControl};
        if (capturedControl) ++capturedControl->referenceCountAt04;
        MoveGuiControlResourceAt5086C0(copied, x, y);
    }
    void* raw = ::operator new(0x50);
    RawStaticUnwind rawLifetime{raw};
    GuiControlNativeBody50h* body = nullptr;
    if (raw)
    {
        // 505898继续用EDI/ESI，只有bias从原局部槽重读。
        YtlReferenceSlotRuntime copied{capturedBase, group.biasAt04, capturedControl};
        if (capturedControl) ++capturedControl->referenceCountAt04;
        body = ConstructGuiStaticAt4F4050(static_cast<GuiControlNativeBody50h*>(raw),
            parent, descriptor.eventAt00, copied);
    }
    rawLifetime.active = false; // 5058BF撤销raw保护，control失败留下完整正文。
    ConstructGuiStaticReferenceAt4D7940(output, body, 1);
    groupLifetime.FinishCaptured(capturedControl);
    return &output;
}
}
