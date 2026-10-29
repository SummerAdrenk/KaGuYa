#include <Runtime/GuiControlNative.h>

#include <cstring>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Body = GuiControlNativeBody50h;
using Reference = YtlReferenceSlotRuntime;

// parentAt4C是原正文，不是现代C++接口；只按已核的原槽读取函数指针。
template<class Function> Function ParentSlot(void* parent, std::size_t offset) noexcept
{
    const std::byte* table;
    std::memcpy(&table, parent, sizeof(table));
    Function function;
    std::memcpy(&function, table + offset, sizeof(function));
    return function;
}
using ParentAction = std::int32_t (__thiscall*)(void*, std::int32_t);
using ParentPoint = POINT* (__thiscall*)(void*, POINT*);
using ParentScalar = std::int32_t (__thiscall*)(void*);
using ParentFocus = void (__thiscall*)(void*, Body*);

class EventUnwind final
{
public:
    explicit EventUnwind(Reference& reference) noexcept : reference_(&reference) {}
    ~EventUnwind() noexcept
    {
        if (reference_) ReleaseGuiClickedEventReferenceAt4F64D0(*reference_);
    }
    void Dismiss() noexcept { reference_ = nullptr; }
private:
    Reference* reference_;
};

void InvokeEvent(Body& body)
{
    // 50A4B0先复制增持；state0的5A7520/5A7880展开项随后才生效。
    // 正常和展开均走4F64D0，其5F3250 EHFlags=5，释放失败终止。
    Reference temporary;
    CopyGuiClickedEventReference(temporary, body.eventAt3C);
    EventUnwind cleanup(temporary);
    if (temporary.baseAt00)
    {
        auto* event = ResolveGuiClickedEvent(temporary);
        event->vptrAt00->invokeAt00(event);
    }
}
}

namespace GuiControlNativeEntries
{
// 表槽仍为thiscall；fastcall第二个寄存器参数只用于适配原ECX正文入口。
std::int32_t __fastcall Keyboard(Body* body, void*)
{
    if (body->childIdAt0C == -1) return 0;
    if (!body->vptrAt00->enabledAt1C(body)) return 0;

    // 51E68F：enabled回调之后才读取latch、parent、childId。
    const bool latched = ButtonField<0x58>(*body) != 0;
    auto* parent = body->parentAt4C;
    const auto childId = body->childIdAt0C;
    if (latched)
    {
        if (ParentSlot<ParentAction>(parent, 0x6C)(parent, childId)) return 2;
        ButtonField<0x50>(*body) = 0;
        ButtonField<0x58>(*body) = 0;
        if (body->eventEnabledAt48) InvokeEvent(*body);
        return 1;
    }
    if (!ParentSlot<ParentAction>(parent, 0x70)(parent, childId)) return 0;
    ButtonField<0x50>(*body) = 1;
    ButtonField<0x58>(*body) = 1;
    return 2;
}

std::int32_t __fastcall Mouse(Body* body, void*)
{
    if (!body->vptrAt00->enabledAt1C(body)) return 0;

    auto* parent = body->parentAt4C;
    // 5214EC缓存表地址，不缓存hit函数；+74回调改body.vptr不会改本次选表。
    const auto* hitTable = body->vptrAt00;
    body->hoverAt1C = 0;
    POINT storage;
    auto* point = ParentSlot<ParentPoint>(parent, 0x74)(parent, &storage);
    if (hitTable->hitAt40(body, point->x, point->y))
    {
        const bool focused = body->focusAt20 != 0;
        body->hoverAt1C = 1;
        if (!focused)
        {
            parent = body->parentAt4C;
            ParentSlot<ParentFocus>(parent, 0x30)(parent, body);
        }
    }
    // 52151F每轮重读hover；focus回调可以清hover或更换parent/pressed。
    if (!body->hoverAt1C)
    {
        ButtonField<0x50>(*body) = 0;
        return 0;
    }
    const bool pressed = ButtonField<0x50>(*body) != 0;
    parent = body->parentAt4C;
    if (pressed)
    {
        if (ParentSlot<ParentScalar>(parent, 0x84)(parent))
        {
            const bool eventEnabled = body->eventEnabledAt48 != 0;
            ButtonField<0x50>(*body) = 0;
            if (eventEnabled) InvokeEvent(*body);
        }
    }
    else if (ParentSlot<ParentScalar>(parent, 0x80)(parent))
        ButtonField<0x50>(*body) = 1;
    return 0;
}
}

void BindPopupGuiControlEventAt4EA856(Body& body, Reference ownedEvent, const Reference& lateSource)
{
    auto* const capturedControl = ownedEvent.controlAt08;
    auto base = lateSource.baseAt00;
    EventUnwind cleanup(ownedEvent);
    auto& target = body.eventAt3C;
    // 4EA869..891：旧Ref不预清；payload之后从旧槽重读control，再flags1删除。
    if (auto* control = target.controlAt08)
    {
        if (--control->referenceCountAt04 == 0)
        {
            if (control->owningAt08)
            {
                control->DestroyPayloadAtVslot00();
                base = lateSource.baseAt00; // 4EA880从外层var_50重读，不从按值临时Ref读。
            }
            if (auto* current = target.controlAt08)
            {
                current->DeleteControlAtVslot04(1);
                base = lateSource.baseAt00; // 4EA891；control来源仍保留入口ESI。
            }
        }
    }
    target.baseAt00 = base;
    target.biasAt04 = 0;
    target.controlAt08 = capturedControl;
    if (capturedControl) ++capturedControl->referenceCountAt04;
    body.eventEnabledAt48 = 1;

    // 4EA8B1为state20终止区；成功尾使用缓存ESI，异常展开才走4F64D0。
    cleanup.Dismiss();
    ReleasePopupClickedEventTemporaryAt4EA9BC(ownedEvent, capturedControl);
}
}
