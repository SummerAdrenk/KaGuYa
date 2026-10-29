#include "ProgramMain01DialogNative.Internal.h"
#include <Runtime/SaveLoadMessageBoxNativeChildFactoriesRuntime.h>
#include <Runtime/GuiSliderNative.h>
#include <Runtime/GuiPanelNative.h>

namespace KaGuYa::Start::Runtime::ProgramMain01NativeDetail
{
namespace
{
Slot* CreateControl(Slot& out, Common* parent, std::int32_t kind, const void* descriptor)
{
    auto& s = RequireProgramMain01DialogNativeServices();
    auto& shared = s.shared;
    SaveLoadMessageBoxNativeChildFactoriesRuntime factories({s.loader, shared.surfaces, shared.strings});
    switch (kind)
    {
    case 0: return CreateGuiStaticAt505800(out, parent,
        *static_cast<const GuiStaticDescriptorNative24h*>(descriptor), s.loader, shared.surfaces, shared.strings);
    case 1: return CreateGuiButtonAt5041D0(out, parent,
        *static_cast<const GuiButtonDescriptorNative2Ch*>(descriptor), s.loader, shared.surfaces, shared.strings);
    case 2: return factories.CreateAt5043B0(&out, parent, descriptor);
    case 3: return CreateGuiSliderAt5052C0(out, parent,
        *static_cast<const GuiSliderDescriptorNative6Ch*>(descriptor), s.loader, shared.surfaces, shared.strings);
    case 4: return factories.CreateAt503BB0(&out, parent, descriptor);
    case 5: return factories.CreateAt503E60(&out, parent, descriptor);
    case 6: return factories.CreateAt505090(&out, parent, descriptor);
    case 7: return CreateGuiPanelAt504E90(out, parent,
        *static_cast<const GuiStaticDescriptorNative24h*>(descriptor), s.loader, shared.surfaces, shared.strings);
    default: throw 1; // 4DF09D，未知类型不返回空控件或假成功。
    }
}
class Main01EventControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit Main01EventControl(GuiClickedEventNativeBody* p) noexcept
        : EnvironmentObjectReferenceControl(p, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        auto* p = static_cast<GuiClickedEventNativeBody*>(payloadAt14);
        if (p) Read(p->vptrAt00)->deletingDestructorAt08(p, 1);
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override { RetireEnvironmentObjectControl(this, flags); }
};
static_assert(sizeof(Main01EventControl) == 0x18);
void CreateEvent(Fresh& event, const Slot& child, std::uintptr_t base, Control* control)
{
    // 4DECA1..4DECF4：先new0C，再用缓存EDI/ESI及晚读bias解析child。
    // 既有CreateTool口不接收缓存字段，不能把bias提前复制以冒充该时序。
    auto* raw = static_cast<GuiClickedEventNativeBody*>(::operator new(0x0C));
    if (raw)
    {
        auto* selected = ResolveCached<Child>(child, base, control);
        ::new(raw) GuiClickedEventNativeBody{&GuiClickedEventVtable(), 0, Read(selected->eventAt04)};
    }
    event.slot = {};
    if (raw)
    {
        event.slot.baseAt00 = reinterpret_cast<std::uintptr_t>(raw);
        void* storage = ::operator new(0x18);
        auto* created = storage ? ::new(storage) Main01EventControl(raw) : nullptr;
        created->referenceCountAt04 = 1;
        created->owningAt08 = 1;
        created->strideAt0C = 0x0C;
        event.slot.controlAt08 = created;
        created->countAt10 = 1;
    }
}
constexpr std::uint32_t strides[]{0x24, 0x2C, 0x48, 0x6C, 0x44, 0x2C, 0x2C, 0x24};
}
void Populate(Body& b, const ProgramMain01DialogNativeControlGroup& group)
{
    for (std::int32_t index = 0; index < Read(group.countAt04); ++index)
    {
        // state0A先保护零child；工厂成功返回之后才有typed临时Ref的展开资格。
        Slot child{}; RefLife childLife{child};
        const auto* first = Read(group.descriptorsAt08);
        const auto kind = Read(group.typeAt00);
        if (std::uint32_t(kind) > 7u) throw 1;
        const auto* descriptor = reinterpret_cast<const void*>(reinterpret_cast<std::uintptr_t>(first) +
            std::uint32_t(index) * strides[kind]);
        Fresh typed;
        auto* returned = CreateControl(typed.slot, &b.common, kind, descriptor);
        RefLife typedLife{typed.slot};
        if (&child != returned)
        {
            child.controlAt08 = Read(returned->controlAt08);
            child.baseAt00 = Read(returned->baseAt00);
            child.biasAt04 = Read(returned->biasAt04);
            if (child.controlAt08) ++child.controlAt08->referenceCountAt04;
        }
        const auto capturedBase = Read(child.baseAt00);
        auto* capturedControl = Read(child.controlAt08);
        typedLife.Current(true); // 0C/0E/10/12/14/16/18/1A均为terminate态。
        auto* selected = ResolveCached<Child>(child, capturedBase, capturedControl);
        if (Read(selected->typeAt08) != 1)
        {
            Fresh event;
            CreateEvent(event, child, capturedBase, capturedControl);
            RefLife eventLife{event.slot}; // 1B
            auto* capturedEvent = Read(event.slot.controlAt08);
            Slot binding{Read(event.slot.baseAt00), 0, capturedEvent};
            if (capturedEvent) ++capturedEvent->referenceCountAt04;
            {
                RefLife bindingLife{binding}; // 1C
                selected = ResolveCached<Child>(child, Read(child.baseAt00), capturedControl);
                auto& target = selected->eventAt3C;
                if (&target != &binding)
                {
                    ReleaseOld(target);
                    target.baseAt00 = Read(event.slot.baseAt00);
                    target.biasAt04 = 0; target.controlAt08 = capturedEvent;
                    if (capturedEvent) ++capturedEvent->referenceCountAt04;
                }
                selected->eventEnabledAt48 = 1;
                bindingLife.Captured(capturedEvent, true); // 1D：不重读binding.control。
            }
            Slot argument{Read(event.slot.baseAt00), 0, capturedEvent};
            if (capturedEvent) ++capturedEvent->referenceCountAt04;
            {
                RefLife argumentLife{argument}; // 1E：先manager追加，后child追加。
                auto* capturedArgument = capturedEvent;
                auto* manager = Resolve<GuiClickedEventReferences>(b.managerAt5C);
                AppendPopupClickedEventReferenceAt4EA97F(*manager, argument, capturedArgument);
                argumentLife.Captured(capturedArgument, true, true); // 1F：payload后重读argument.control。
            }
            eventLife.Captured(capturedEvent, true); // 20
        }
        Slot upcast{Read(child.baseAt00), Read(child.biasAt04), capturedControl};
        if (capturedControl) ++capturedControl->referenceCountAt04;
        {
            RefLife upcastLife{upcast}; // 21
            auto* capturedUpcast = capturedControl;
            auto& children = b.common.childrenAt0C;
            if (Read(children.end) != Read(children.capacity))
            {
                const Slot fast{Read(child.baseAt00), Read(child.biasAt04), capturedControl};
                AppendGuiControlChildReference(children, fast);
            }
            else
            {
                GrowGuiControlChildrenAt4D93B0(children, Read(children.end), upcast);
                capturedUpcast = Read(upcast.controlAt08);
            }
            upcastLife.Captured(capturedUpcast, true); // 22
        }
        childLife.Captured(capturedControl, true); // 23，释放故障不继续成员展开。
    }
}
void PopulateNavigation(Body& b, const ProgramMain01DialogNativeNavigationGroup& group)
{
    Fresh navigation;
    CreateGuiNavigationAt504CE0(navigation.slot, Read(group.inputAt00), Read(group.pairsAt04));
    RefLife life{navigation.slot}; // 24，原ex=0。
    AppendGuiNavigationReferenceAt4D9D60(b.common.navigationAt24, navigation.slot);
    life.Current(true); // 25
}
}
