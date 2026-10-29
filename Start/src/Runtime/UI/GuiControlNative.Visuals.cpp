#include <Runtime/GuiControlNative.h>
#include <Runtime/GraphicsSurfaceReference.h>
#include <Runtime/GraphicsSurfaceRuntime.h>
#include <bit>
#include <cstring>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
thread_local IGuiControlGraphicsSource* graphicsSource = nullptr;
using Body = GuiControlNativeBody50h;
std::int32_t Add(std::int32_t a, std::int32_t b) noexcept
{ return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(a) + static_cast<std::uint32_t>(b)); }
std::int32_t Sub(std::int32_t a, std::int32_t b) noexcept
{ return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(a) - static_cast<std::uint32_t>(b)); }
void FinishCaptured(YtlReferenceControlRuntime* control) noexcept
{
    if (control && --control->referenceCountAt04 == 0)
    {
        if (control->owningAt08) control->DestroyPayloadAtVslot00();
        control->DeleteControlAtVslot04(1);
    }
}
struct Temporary final
{
    YtlReferenceSlotRuntime& value;
    bool active = true;
    ~Temporary() noexcept { if (active) ReleaseYtlReference(value); }
    void Finish(YtlReferenceControlRuntime* captured) noexcept
    { active = false; FinishCaptured(captured); }
    void FinishFromSlot(bool clear = false) noexcept
    {
        active = false;
        auto* control = value.controlAt08;
        if (!control) return;
        if (--control->referenceCountAt04 == 0)
        {
            if (control->owningAt08) control->DestroyPayloadAtVslot00();
            if (value.controlAt08) value.controlAt08->DeleteControlAtVslot04(1);
        }
        if (clear) { value.controlAt08 = nullptr; value.baseAt00 = 0; }
    }
    void ReleaseNow() noexcept { active = false; ReleaseYtlReference(value); }
};
const YtlReferenceSlotRuntime& Group(Body& body, std::size_t index)
{
    // 原越界解引用改为明确失败，不将缺资源归一为空图。
    auto& storage = body.resourcesAt28;
    if (!storage.begin || !storage.end || index >= static_cast<std::size_t>(storage.end - storage.begin))
        throw std::out_of_range("GuiControl group index is outside native storage");
    return storage.begin[index];
}
RECT GroupZeroRect(Body& body)
{
    YtlReferenceSlotRuntime temporary;
    CopyGuiControlResourceReference(temporary, Group(body, 0));
    Temporary lifetime{temporary};
    auto* const captured = temporary.controlAt08;
    const RECT result = RequireGuiControlResource(temporary).combinedRectAt28;
    lifetime.Finish(captured);
    return result;
}
GraphicsSurfaceNativeBody& Surface(const YtlReferenceSlotRuntime& reference)
{
    auto* body = ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(reference);
    if (!body) throw std::invalid_argument("GuiControl selected Surface is null");
    return *body;
}
void SelectSurface(YtlReferenceSlotRuntime& out, GuiControlResourceNativeBody38h* group, std::int32_t slot)
{
    out = {}; // 原先解析group、后清返回槽；不释放旧out。
    if (slot >= 7) return;
    if (slot < 0) throw std::out_of_range("GuiControl negative resource slot dereferences before CElement");
    if (!group) throw std::invalid_argument("GuiControl resource group is null");
    const auto index = group->slotIndicesAt00[slot];
    if (index < 0) return;
    if (index >= GuiControlSurfaceReferenceCount(group->surfacesAt1C))
        throw std::out_of_range("GuiControl resource mapping is outside Surface storage");
    CopyAssignGraphicsSurfaceReferenceAt465870(out, group->surfacesAt1C.begin[index]);
}
void StaticSurface(Body& body, std::int32_t slot, YtlReferenceSlotRuntime& out,
    YtlReferenceControlRuntime*& capturedSurfaceControl)
{
    YtlReferenceSlotRuntime group;
    CopyGuiControlResourceReference(group, Group(body, 0));
    Temporary groupLifetime{group};
    auto* const captured = group.controlAt08;
    auto* const selected = ResolveGuiControlResource(group);
    out = {};
    Temporary resultLifetime{out};
    SelectSurface(out, selected, slot);
    capturedSurfaceControl = out.controlAt08;
    groupLifetime.Finish(captured);
    resultLifetime.active = false;
}
POINT StaticPosition(Body& body, const YtlReferenceSlotRuntime& source)
{
    // Static先释放当前组0的RECT临时，随后才读取Surface原点；CheckBox相反。
    const auto rect = GroupZeroRect(body);
    const auto& surface = Surface(source);
    return {Add(Sub(surface.originXAt10, rect.left), body.xAt34),
        Add(Sub(surface.originYAt14, rect.top), body.yAt38)};
}
POINT CheckPosition(Body& body, const YtlReferenceSlotRuntime& source)
{
    const auto& surface = Surface(source);
    const auto x = surface.originXAt10;
    const auto y = surface.originYAt14;
    POINT point;
    GuiControlNativeEntries::Position(&body, nullptr, &point, x, y);
    return point;
}
void DrawLeaf(const YtlReferenceSlotRuntime& source, POINT point, const RECT* clip)
{
    const auto handle = Surface(source).handleAt04;
    if (!handle) return;
    auto& api = RequireGuiControlGraphicsSource().CompositionAt4182F0();
    static_cast<void>(api.DrawSurface(handle, point.x, point.y, nullptr, nullptr, clip, 0));
}
void CompositeLeaf(const YtlReferenceSlotRuntime& target, const YtlReferenceSlotRuntime& source,
    POINT point, const RECT* clip)
{
    const auto targetHandle = Surface(target).handleAt04;
    if (!targetHandle) return;
    const auto sourceHandle = Surface(source).handleAt04;
    if (!sourceHandle) return;
    auto& api = RequireGuiControlGraphicsSource().CompositionAt4182F0();
    static_cast<void>(api.BlitSurface(targetHandle, sourceHandle, point.x, point.y, nullptr, nullptr, clip, 0));
}
template<class Function> Function ButtonSlot(const GuiControlNativeVtable23* table,
    std::size_t offset) noexcept
{
    // 只读当前槽。原版缓存表地址但不缓存整表，前一回调可以改写后一个槽。
    Function result;
    std::memcpy(&result, reinterpret_cast<const std::byte*>(table) + offset, sizeof(result));
    return result;
}
using ResourceSlot = decltype(GuiButtonNativeVtable29::resourceAt5C);
using ScalarSlot = decltype(GuiButtonNativeVtable29::hitSlotAt68);
}
GuiControlGraphicsScope::GuiControlGraphicsScope(IGuiControlGraphicsSource& source) noexcept
    : GuiControlGraphicsScope(&source) {}
GuiControlGraphicsScope::GuiControlGraphicsScope(IGuiControlGraphicsSource* source) noexcept
    : previous_(graphicsSource) { graphicsSource = source; }
GuiControlGraphicsScope::~GuiControlGraphicsScope() noexcept { graphicsSource = previous_; }
IGuiControlGraphicsSource& RequireGuiControlGraphicsSource()
{
    if (!graphicsSource) throw std::logic_error("GuiControl graphics source is not bound");
    return *graphicsSource;
}
namespace GuiControlNativeEntries
{
RECT* __fastcall Rect(Body* body, void*, RECT* output)
{
    auto rect = GroupZeroRect(*body);
    const auto x = body->xAt34;
    rect.right = Add(rect.right, Sub(x, rect.left));
    const auto y = body->yAt38;
    rect.bottom = Add(rect.bottom, Sub(y, rect.top));
    rect.left = x;
    rect.top = y;
    *output = rect;
    return output;
}
POINT* __fastcall Position(Body* body, void*, POINT* output, std::int32_t x, std::int32_t y)
{
    const auto rect = GroupZeroRect(*body);
    *output = {Add(Sub(body->xAt34, rect.left), x), Add(Sub(body->yAt38, rect.top), y)};
    return output;
}
YtlReferenceSlotRuntime* __fastcall Resource(Body* body, void*, YtlReferenceSlotRuntime* output, std::int32_t slot)
{
    const auto& box = *reinterpret_cast<GuiCheckBoxNativeBody60h*>(body);
    YtlReferenceSlotRuntime group;
    CopyGuiControlResourceReference(group, Group(*body, box.checkedAt5C != 0 ? 1 : 0));
    Temporary groupLifetime{group};
    auto* const captured = group.controlAt08;
    auto* const selected = ResolveGuiControlResource(group);
    *output = {};
    Temporary resultLifetime{*output};
    SelectSurface(*output, selected, slot);
    groupLifetime.Finish(captured);
    resultLifetime.active = false;
    return output;
}
YtlReferenceSlotRuntime* __fastcall ButtonResource(Body* body, void*, YtlReferenceSlotRuntime* output, std::int32_t slot)
{
    // 50B100始终取组0；不能读不存在的+5C，更不能按checked选择组1。
    YtlReferenceSlotRuntime group;
    CopyGuiControlResourceReference(group, Group(*body, 0));
    Temporary groupLifetime{group};
    auto* const captured = group.controlAt08;
    auto* const selected = ResolveGuiControlResource(group);
    *output = {};
    Temporary resultLifetime{*output};
    SelectSurface(*output, selected, slot);
    groupLifetime.Finish(captured);
    resultLifetime.active = false;
    return output;
}
void __fastcall SetHitSlot(Body* body, void*, std::int32_t slot)
{
    YtlReferenceSlotRuntime selected;
    ButtonSlot<ResourceSlot>(body->vptrAt00, 0x5C)(body, &selected, slot);
    Temporary temporary{selected};
    ButtonField<0x54>(*body) = selected.baseAt00 ? slot : 7;
    temporary.FinishFromSlot();
}
std::int32_t __fastcall HitStatic(Body* body, void*, std::int32_t x, std::int32_t y)
{
    RECT local;
    const auto* result = body->vptrAt00->rectAt3C(body, &local);
    RECT rect;
    ::CopyRect(&rect, result);
    return ::PtInRect(&rect, {x, y});
}
std::int32_t __fastcall HitCheckBox(Body* body, void*, std::int32_t x, std::int32_t y)
{
    if (!HitStatic(body, nullptr, x, y)) return 0;
    if (!ButtonSlot<ScalarSlot>(body->vptrAt00, 0x64)(body)) return 1;
    const auto* table = body->vptrAt00; // 502F1B固定同一表的68/5C。
    const auto slot = ButtonSlot<ScalarSlot>(table, 0x68)(body);
    YtlReferenceSlotRuntime selected;
    ButtonSlot<ResourceSlot>(table, 0x5C)(body, &selected, slot);
    Temporary temporary{selected};
    if (Surface(selected).alphaAt18 != 0)
    {
        const auto point = CheckPosition(*body, selected);
        const auto pixelX = Sub(x, point.x);
        const auto pixelY = Sub(y, point.y);
        const auto handle = Surface(selected).handleAt04;
        auto& api = RequireGuiControlGraphicsSource().PixelsAt4182F0();
        if ((api.GetSurfacePixel(handle, pixelX, pixelY) & 0xFF000000u) == 0)
        {
            temporary.ReleaseNow(); // 502FCB显式40D2F0，不走成功尾的非清槽释放。
            return 0;
        }
    }
    temporary.FinishFromSlot();
    return 1;
}
void __fastcall DrawStatic(Body* body, void*, const RECT* clip)
{
    const auto slot = body->vptrAt00->resourceStateAt38(body);
    YtlReferenceSlotRuntime source;
    YtlReferenceControlRuntime* captured;
    StaticSurface(*body, slot, source, captured);
    Temporary temporary{source};
    if (source.baseAt00) DrawLeaf(source, StaticPosition(*body, source), clip);
    temporary.Finish(captured);
}
void __fastcall DrawCheckBox(Body* body, void*, const RECT* clip)
{
    const auto* table = body->vptrAt00;
    const auto slot = table->resourceStateAt38(body);
    YtlReferenceSlotRuntime source;
    ButtonSlot<ResourceSlot>(table, 0x5C)(body, &source, slot);
    Temporary temporary{source};
    if (source.baseAt00) DrawLeaf(source, CheckPosition(*body, source), clip);
    temporary.FinishFromSlot();
}
void __fastcall CompositeStatic(Body* body, void*, YtlReferenceSlotRuntime target, const RECT* clip)
{
    Temporary parameter{target};
    const auto slot = body->vptrAt00->resourceStateAt38(body);
    YtlReferenceSlotRuntime source;
    YtlReferenceControlRuntime* captured;
    StaticSurface(*body, slot, source, captured);
    Temporary temporary{source};
    if (source.baseAt00) CompositeLeaf(target, source, StaticPosition(*body, source), clip);
    temporary.Finish(captured);
    parameter.FinishFromSlot();
}
void __fastcall CompositeCheckBox(Body* body, void*, YtlReferenceSlotRuntime target, const RECT* clip)
{
    Temporary parameter{target};
    const auto* table = body->vptrAt00;
    const auto slot = table->resourceStateAt38(body);
    YtlReferenceSlotRuntime source;
    ButtonSlot<ResourceSlot>(table, 0x5C)(body, &source, slot);
    Temporary temporary{source};
    if (source.baseAt00) CompositeLeaf(target, source, CheckPosition(*body, source), clip);
    temporary.FinishFromSlot(true);
    parameter.FinishFromSlot();
}
}
}
