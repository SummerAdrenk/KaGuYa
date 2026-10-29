#include <Runtime/GuiMultiCheckButtonNative.h>
#include <Runtime/GuiDialogNative.h>
#include <Runtime/GraphicsSurfaceNativeBody.h>
#include <memory>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Base = GuiControlNativeBody50h;
using Body = GuiMultiCheckButtonNativeBody78h;
struct LocalReference final
{
    YtlReferenceSlotRuntime value{};
    ~LocalReference() noexcept { ReleaseYtlReference(value); }
};
Body& Multi(Base* body) noexcept { return *reinterpret_cast<Body*>(body); }
GuiDialogNativeBody5Ch& Parent(Base* body) noexcept
{ return *static_cast<GuiDialogNativeBody5Ch*>(body->parentAt4C); }
void Invoke(Base* body)
{
    LocalReference event;
    CopyGuiClickedEventReference(event.value, body->eventAt3C);
    if (auto* value = ResolveGuiClickedEvent(event.value)) value->vptrAt00->invokeAt00(value);
}
std::int32_t __fastcall HitSlot(Base* body, void*)
{
    const auto state = static_cast<std::uint32_t>(body->vptrAt00->resourceStateAt38(body));
    if (state >= 7) throw std::out_of_range("MultiCheckButton visual state outside seven slots");
    return Multi(body).hitSlots[state];
}
std::int32_t __fastcall HasHit(Base* body, void*) { return HitSlot(body, nullptr) != 7; }
Base* __fastcall Delete(Base* body, void*, std::uint32_t flags) noexcept
{
    // 资源/事件仍由同一Button基类释放，最后按扩展正文大小释放存储。
    GuiButtonVtable().common.deletingDestructorAt00(body, 0);
    if (flags & 1u) ::operator delete(reinterpret_cast<Body*>(body), sizeof(Body));
    return body;
}
std::int32_t __fastcall Keyboard(Base* body, void*)
{
    if (body->childIdAt0C == -1 || !body->vptrAt00->enabledAt1C(body)) return 0;
    auto& button = Multi(body).button;
    auto& parent = Parent(body);
    // 原07 4DF2F0使用held查询；不将这个独立旧控件入口改成pressed。
    if (button.keyboardLatchAt58)
    {
        if (parent.vptrAt00->heldAt6C(&parent, body->childIdAt0C)) return 2;
        button.pressedAt50 = 0;
        button.keyboardLatchAt58 = 0;
        Invoke(body);
        return 1;
    }
    if (!parent.vptrAt00->heldAt6C(&parent, body->childIdAt0C)) return 0;
    button.pressedAt50 = 1;
    button.keyboardLatchAt58 = 1;
    return 2;
}
std::int32_t __fastcall Mouse(Base* body, void*)
{
    if (!body->vptrAt00->enabledAt1C(body)) return 0;
    const auto previousHover = body->hoverAt1C;
    auto& parent = Parent(body);
    const auto* hitMethods = body->vptrAt00;
    POINT storage;
    const auto* point = parent.vptrAt00->pointerAt74(&parent, &storage);
    const auto hover = hitMethods->hitAt40(body, point->x, point->y) != 0 ? 1 : 0;
    if (hover && !body->focusAt20)
    {
        auto& current = Parent(body);
        current.vptrAt00->setActiveAt30(&current, body);
    }
    // 4E1800在命中查询期间保留旧hover，以当前状态选mask；不能先清零后查询ON图。
    if (previousHover != hover) body->hoverAt1C = hover;
    auto& button = Multi(body).button;
    if (!body->hoverAt1C) { button.pressedAt50 = 0; return 0; }
    auto& current = Parent(body);
    if (button.pressedAt50)
    {
        if (current.vptrAt00->mouseReleaseAt84(&current))
        {
            button.pressedAt50 = 0;
            Invoke(body);
        }
    }
    else if (current.vptrAt00->mousePressAt80(&current)) button.pressedAt50 = 1;
    return 0;
}
}

const GuiButtonNativeVtable29& GuiMultiCheckButtonMethods() noexcept
{
    static const auto table = [] {
        auto value = GuiButtonVtable();
        value.common.deletingDestructorAt00 = reinterpret_cast<decltype(value.common.deletingDestructorAt00)>(&Delete);
        value.common.keyboardAt14 = reinterpret_cast<decltype(value.common.keyboardAt14)>(&Keyboard);
        value.common.mouseAt18 = reinterpret_cast<decltype(value.common.mouseAt18)>(&Mouse);
        value.hasHitSlotAt64 = reinterpret_cast<decltype(value.hasHitSlotAt64)>(&HasHit);
        value.hitSlotAt68 = reinterpret_cast<decltype(value.hitSlotAt68)>(&HitSlot);
        return value;
    }();
    return table;
}

YtlReferenceSlotRuntime* CreateGuiMultiCheckButtonNative(YtlReferenceSlotRuntime& output,
    void* parent, const GuiMultiCheckButtonDescriptorNative44h& descriptor,
    IGuiControlResourceLoadServices& loader, IGraphicsSurfaceApi& surfaces, IRuntimeStringManagerSource& strings)
{
    const GraphicsSurfaceNativeScope graphics(surfaces);
    std::array<const char*, 7> names;
    for (unsigned i = 0; i < names.size(); ++i) names[i] = descriptor.states[i].name;
    LocalReference group;
    LoadGuiControlResourceAt5047A0(group.value, names.data(), loader, surfaces, strings);
    if (!group.value.baseAt00) throw 1; // 原4E3AD0检查工厂的空资源组，不继续构造CElement消费者。
    if (descriptor.position)
    {
        YtlReferenceSlotRuntime moved;
        CopyGuiControlResourceReference(moved, group.value);
        MoveGuiControlResourceAt5086C0(moved, descriptor.position->x, descriptor.position->y);
    }
    auto body = std::make_unique<Body>();
    YtlReferenceSlotRuntime ownedGroup;
    CopyGuiControlResourceReference(ownedGroup, group.value);
    ConstructGuiButtonAt4DA700(&body->button, parent, descriptor.event, ownedGroup);
    body->button.common.vptrAt00 = &GuiMultiCheckButtonMethods().common;
    body->button.common.childIdAt0C = descriptor.childId;
    for (unsigned i = 0; i < body->hitSlots.size(); ++i) body->hitSlots[i] = descriptor.states[i].hitSlot;
    try
    {
        // 共用单对象Ref控制块/动态删除；stride明确改为实际78h，不能留成基类5Ch。
        ConstructGuiButtonReferenceAt504298(output, &body->button);
        static_cast<EnvironmentObjectReferenceControl*>(output.controlAt08)->strideAt0C = sizeof(Body);
    }
    catch (...)
    {
        GuiButtonVtable().common.deletingDestructorAt00(&body->button.common, 0);
        output = {};
        throw;
    }
    body.release();
    return &output;
}
}
