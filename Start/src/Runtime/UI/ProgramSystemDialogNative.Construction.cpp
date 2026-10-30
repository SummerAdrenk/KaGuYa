#include "ProgramSystemDialogNative.Internal.h"
#include <Runtime/GuiPanelNative.h>
#include <Runtime/GuiSliderNative.h>

namespace KaGuYa::Start::Runtime
{
namespace ProgramSystemNativeDetail
{
namespace
{
// 5FAF04/5FB8EC：只拥有原局部Ref资格，不给raw事件/完成child增添补偿guard。
struct ConstructorFrame final
{
    Body& body;
    Fresh child, factory, event, binding, eventAppend, childAppend;
    int state = 0;
    bool childProtected = false;
    ~ConstructorFrame() noexcept
    {
        switch (state)
        {
        case -1: return;
        case 3: case 5: case 7: case 9: case 11: case 13: case 15: case 17:
            ReleaseGuiControlReferenceAt4F6460(factory.slot); break;
        case 20:
            ReleaseGuiClickedEventReferenceAt4F64D0(binding.slot);
            [[fallthrough]];
        case 19:
            ReleaseGuiClickedEventReferenceAt4F64D0(event.slot); break;
        case 22:
            ReleaseGuiClickedEventReferenceAt4F64D0(eventAppend.slot);
            ReleaseGuiClickedEventReferenceAt4F64D0(event.slot); break;
        case 25:
            ReleaseGuiControlReferenceAt4F6460(childAppend.slot); break;
        case 4: case 6: case 8: case 10: case 12: case 14: case 16: case 18:
        case 21: case 23: case 24: case 26: case 27: std::terminate();
        default: break;
        }
        if (state >= 2 && childProtected) ReleaseGuiControlReferenceAt4F6460(child.slot);
        if (state >= 1) ReleaseCurrent(body.clickedManagerAt5C, true);
        DestroyGuiDialogAt4F7AC0(&body.common);
    }
};
constexpr std::uint32_t strides[]{0x24, 0x2C, 0x48, 0x6C, 0x44, 0x2C, 0x2C, 0x24};
Slot* CreateChild(Slot& output, Common* parent, int kind, const void* descriptor,
    ProgramSystemDialogNativeServices& s)
{
    auto& p = s.parent;
    SaveLoadMessageBoxNativeChildFactoriesRuntime factory({s.loader, p.surfaces, p.strings});
    switch (kind)
    {
    case 0: return CreateGuiStaticAt505800(output, parent,
        *static_cast<const GuiStaticDescriptorNative24h*>(descriptor), s.loader, p.surfaces, p.strings);
    case 1: return CreateGuiButtonAt5041D0(output, parent,
        *static_cast<const GuiButtonDescriptorNative2Ch*>(descriptor), s.loader, p.surfaces, p.strings);
    case 2: return factory.CreateAt5043B0(&output, parent, descriptor);
    case 3: return CreateGuiSliderAt5052C0(output, parent,
        *static_cast<const GuiSliderDescriptorNative6Ch*>(descriptor), s.loader, p.surfaces, p.strings);
    case 4: return factory.CreateAt503BB0(&output, parent, descriptor);
    case 5: return factory.CreateAt503E60(&output, parent, descriptor);
    case 6: return factory.CreateAt505090(&output, parent, descriptor);
    case 7: return CreateGuiPanelAt504E90(output, parent,
        *static_cast<const GuiStaticDescriptorNative24h*>(descriptor), s.loader, p.surfaces, p.strings);
    default: throw 1; // 4DFC5F/4E04DF，不把未知kind变成null成功。
    }
}

// 复用真实ClickedEvent工厂；仅桥接原内联4DF959的捕获差异：
// base/control固定在寄存器，bias必须在new0C成功后从原child栈槽读取。
class EventAllocationCapture final : public IEnvironmentRootMemory
{
public:
    EventAllocationCapture(Slot& input, const Slot& child) noexcept : input_(input), child_(child) {}
    void* AllocateBody(std::size_t bytes) override
    {
        auto* raw = DefaultEnvironmentRootMemory().AllocateBody(bytes);
        if (raw && bytes == 0x0C) input_.biasAt04 = Read(child_.biasAt04);
        return raw;
    }
    void DeleteBody(void* p, std::size_t bytes) override { DefaultEnvironmentRootMemory().DeleteBody(p, bytes); }
    void DeleteVector(void* p, std::size_t bytes) override { DefaultEnvironmentRootMemory().DeleteVector(p, bytes); }
private:
    Slot& input_;
    const Slot& child_;
};
void Populate(ConstructorFrame& f, const ProgramSystemDialogNativeControlGroup& group,
    ProgramSystemDialogNativeServices& s)
{
    for (std::int32_t index = 0; index < Read(group.countAt04); ++index)
    {
        ::new(&f.child.slot) Slot;
        f.childProtected = true;
        f.state = 2;
        const auto kind = Read(group.kindAt00);
        if (std::uint32_t(kind) > 7u) throw 1;
        const auto* descriptor = reinterpret_cast<const void*>(
            reinterpret_cast<std::uintptr_t>(Read(group.descriptorsAt08)) + std::uint32_t(index) * strides[kind]);
        auto* returned = CreateChild(f.factory.slot, &f.body.common, kind, descriptor, s);
        f.state = 3 + 2 * kind;
        if (&f.child.slot != returned)
        {
            f.child.slot.controlAt08 = Read(returned->controlAt08);
            f.child.slot.baseAt00 = Read(returned->baseAt00);
            f.child.slot.biasAt04 = Read(returned->biasAt04);
            if (f.child.slot.controlAt08) ++f.child.slot.controlAt08->referenceCountAt04;
        }
        const Slot cached{Read(f.child.slot.baseAt00), Read(f.child.slot.biasAt04), Read(f.child.slot.controlAt08)};
        f.state = 4 + 2 * kind;
        ReleaseCurrent(f.factory.slot, true);
        f.state = 2;
        if (Read(Resolve<GuiControlNativeBody50h>(cached)->typeAt08) != 1)
        {
            Slot input{cached.baseAt00, cached.biasAt04, cached.controlAt08};
            EventAllocationCapture allocation{input, f.child.slot};
            CreateToolClickedEventAt4E2F5A(f.event.slot, input, allocation);
            f.state = 19;
            auto* const eventControl = Read(f.event.slot.controlAt08);
            f.binding.slot = CopyParts(Read(f.event.slot.baseAt00), 0, eventControl);
            const Slot currentChild{Read(f.child.slot.baseAt00), Read(f.child.slot.biasAt04), cached.controlAt08};
            auto* selected = Resolve<GuiControlNativeBody50h>(currentChild);
            f.state = 20;
            auto& target = selected->eventAt3C;
            if (&target != &f.binding.slot)
            {
                ReleaseCurrent(target, false);
                target.baseAt00 = Read(f.event.slot.baseAt00); // 4DFA40：旧release后才取源base。
                target.biasAt04 = 0;
                target.controlAt08 = eventControl;
                if (eventControl) ++eventControl->referenceCountAt04;
            }
            selected->eventEnabledAt48 = 1;
            f.state = 21;
            ReleaseCaptured(f.binding.slot, eventControl);
            f.eventAppend.slot = CopyParts(Read(f.event.slot.baseAt00), 0, eventControl);
            auto* capturedAppend = eventControl;
            auto* manager = Resolve<GuiClickedEventReferences>(f.body.clickedManagerAt5C);
            auto* position = Read(manager->end);
            f.state = 22;
            if (position == Read(manager->capacity))
            {
                GrowGuiClickedEventReferencesAt4D9620(*manager, position, f.eventAppend.slot);
                capturedAppend = Read(f.eventAppend.slot.controlAt08);
            }
            else
            {
                const Slot fast{Read(f.event.slot.baseAt00), 0, eventControl};
                AppendGuiClickedEventReference(*manager, fast);
            }
            f.state = 23;
            ReleaseCaptured(f.eventAppend.slot, capturedAppend);
            f.state = 24;
            ReleaseCaptured(f.event.slot, eventControl);
        }
        f.childAppend.slot = CopyParts(Read(f.child.slot.baseAt00), Read(f.child.slot.biasAt04), cached.controlAt08);
        auto* capturedAppend = cached.controlAt08;
        auto& children = f.body.common.childrenAt0C;
        auto* position = Read(children.end);
        f.state = 25;
        if (position == Read(children.capacity))
        {
            GrowGuiControlChildrenAt4D93B0(children, position, f.childAppend.slot);
            capturedAppend = Read(f.childAppend.slot.controlAt08);
        }
        else
        {
            const Slot fast{Read(f.child.slot.baseAt00), Read(f.child.slot.biasAt04), cached.controlAt08};
            AppendGuiControlChildReference(children, fast);
        }
        f.state = 26;
        ReleaseCaptured(f.childAppend.slot, capturedAppend);
        f.childProtected = false;
        f.state = 27;
        ReleaseCaptured(f.child.slot, cached.controlAt08);
        f.state = 1;
    }
}
}
Body* Construct(void* raw, Common* parent, std::int32_t id, bool mode01,
    std::span<const ProgramSystemDialogNativeControlGroup, 4> groups)
{
    Leaves leaves;
    auto* body = ::new(raw) Body;
    ConstructGuiDialogAt4E0BE0(&body->common, parent, id);
    body->common.vptrAt00 = mode01 ? &ProgramSystem01DialogNativeVtable() : &ProgramSystem00DialogNativeVtable();
    ConstructorFrame frame{.body=*body,.child={},.factory={},.event={},
        .binding={},.eventAppend={},.childAppend={}};
    ::new(&body->clickedManagerAt5C) Slot;
    frame.state = 1;
    void* managerRaw = ::operator new(0x0C);
    auto* manager = managerRaw ? ::new(managerRaw) GuiClickedEventReferences{} : nullptr;
    AdoptGuiClickedEventManagerAt4D7B90(body->clickedManagerAt5C, manager);
    for (const auto& group : groups) Populate(frame, group, leaves.services);
    // 原ctor没有GameInput、navigation、bounds、自动Begin或额外尺寸计算。
    frame.state = -1;
    return body;
}
}
ProgramSystemDialogNativeBody68h* ConstructProgramSystem00DialogNativeAt4DF420(
    void* raw, GuiDialogNativeBody5Ch* parent, std::int32_t id)
{ return ProgramSystemNativeDetail::Construct(raw, parent, id, false, ProgramSystem00DialogNativeGroupsSample18()); }
ProgramSystemDialogNativeBody68h* ConstructProgramSystem01DialogNativeAt4DFCA0(
    void* raw, GuiDialogNativeBody5Ch* parent, std::int32_t id)
{ return ProgramSystemNativeDetail::Construct(raw, parent, id, true, ProgramSystem01DialogNativeGroupsSample18()); }
ProgramSystemDialogNativeBody68h* ConstructProgramSystem00DialogNativeAt4DF420(
    void* raw, GuiDialogNativeBody5Ch* parent, std::int32_t id, ProgramSystemDialogNativeServices& s)
{ ProgramSystemDialogNativeScope scope{s}; return ConstructProgramSystem00DialogNativeAt4DF420(raw, parent, id); }
ProgramSystemDialogNativeBody68h* ConstructProgramSystem01DialogNativeAt4DFCA0(
    void* raw, GuiDialogNativeBody5Ch* parent, std::int32_t id, ProgramSystemDialogNativeServices& s)
{ ProgramSystemDialogNativeScope scope{s}; return ConstructProgramSystem01DialogNativeAt4DFCA0(raw, parent, id); }
ProgramSystemDialogNativeBody68h* ConstructProgramSystemDialogNativeWithGroups(
    void* raw, GuiDialogNativeBody5Ch* parent, std::int32_t id, bool mode01,
    std::span<const ProgramSystemDialogNativeControlGroup, 4> groups, ProgramSystemDialogNativeServices& s)
{ ProgramSystemDialogNativeScope scope{s}; return ProgramSystemNativeDetail::Construct(raw, parent, id, mode01, groups); }
}
