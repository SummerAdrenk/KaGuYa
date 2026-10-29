#include "MusicDialogNative.Internal.h"
#include <stdexcept>

namespace KaGuYa::Start::Runtime::MusicDialogNativeDetail
{
namespace
{
void Background(Common& owner)
{
    auto& s = RequireMusicDialogNativeServices();
    GuiControlSurfaceReferences surfaces{};
    struct VectorLife final
    {
        GuiControlSurfaceReferences& refs;
        bool active = true;
        ~VectorLife() noexcept { if (active) DestroyGuiControlSurfaceReferencesAt43EAA0(refs); }
        void Finish() noexcept { active = false; DestroyGuiControlSurfaceReferencesAt43EAA0(refs); }
    } vectorLife{surfaces};
    for (std::size_t i = 0; i < s.resources.backgrounds.size(); ++i)
    {
        Slot surface{};
        RefLife surfaceLife{surface}; // state3在取根与申请Surface之前就取得展开资格。
        void* receiver = s.pictures.CaptureLoadReceiverAt413BE0();
        const auto* name = BackgroundName(static_cast<std::uint32_t>(i));
        AdoptGraphicsSurfaceAt407A80(surface, NewGraphicsSurfaceAt40C2E0(s.surfaces, s.strings));
        auto* captured = Read(surface.controlAt08);
        if (!s.pictures.LoadIntoAt4800A0(receiver, *Resolve<GraphicsSurfaceNativeBody>(surface), name)) throw 1;
        auto* end = Read(surfaces.end);
        if (end != Read(surfaces.capacity))
        {
            end->baseAt00 = Read(surface.baseAt00);
            end->biasAt04 = Read(surface.biasAt04);
            end->controlAt08 = captured;
            if (captured) ++captured->referenceCountAt04;
            surfaces.end = end + 1;
        }
        else
        {
            GrowGuiControlSurfaceReferencesAt4540A0(surfaces, end, surface);
            captured = Read(surface.controlAt08);
        }
        surfaceLife.Captured(captured);
    }
    auto* const refs = Read(surfaces.begin);
    if (s.resources.presentation==MusicResourceProfile::Presentation::TiledCheckboxes)
    {
        for (std::int32_t i = 0; i < ItemCount(); ++i)
        {
            auto* frame = Resolve<GraphicsSurfaceNativeBody>(refs[1]);
            const auto destination = Read(Resolve<GraphicsSurfaceNativeBody>(refs[0])->handleAt04);
            const auto y = Positions()[i].y, x = Positions()[i].x;
            if (!destination) continue;
            const auto source = Read(frame->handleAt04);
            if (!source) continue;
            (void)s.drawing.SurfaceBlitRaw(destination, source, x, y, nullptr, nullptr, nullptr, 0);
        }
        auto* title = Resolve<GraphicsSurfaceNativeBody>(refs[2]);
        const auto x = Read(title->originXAt10), y = Read(title->originYAt14);
        title = Resolve<GraphicsSurfaceNativeBody>(refs[2]);
        auto* base = Resolve<GraphicsSurfaceNativeBody>(refs[0]);
        const auto destination = Read(base->handleAt04);
        if (destination)
        {
            const auto source = Read(title->handleAt04);
            if (source) (void)s.drawing.SurfaceBlitRaw(destination, source, x, y, nullptr, nullptr, nullptr, 0);
        }
    }
    Fresh group;
    auto* rawGroup = static_cast<GuiControlResourceNativeBody38h*>(s.memory.AllocateBody(0x38));
    if (rawGroup) ConstructGuiControlResourceBodyAt4DABB0(*rawGroup);
    ConstructGuiControlResourceReferenceAt4D76A0(group.slot, rawGroup, 1);
    RefLife groupLife{group.slot};
    const auto capturedGroupBase = Read(group.slot.baseAt00);
    auto* capturedGroup = Read(group.slot.controlAt08);
    AppendGuiControlSurfaceArgumentAt538350(*Resolve<GuiControlResourceNativeBody38h>(group.slot), 0, Copy(refs[0]));
    void* raw = s.memory.AllocateBody(0x50);
    RawLife allocation{raw, 0x50};
    Control* child = nullptr;
    if (raw)
    {
        Slot argument{capturedGroupBase, Read(group.slot.biasAt04), capturedGroup};
        if (capturedGroup) ++capturedGroup->referenceCountAt04;
        child = ConstructGuiStaticAt4F4050(static_cast<Control*>(raw), &owner, 1, argument);
    }
    allocation.active = false;
    Fresh control;
    ConstructGuiStaticReferenceAt4D7940(control.slot, child, 1);
    RefLife controlLife{control.slot};
    auto* capturedChild = Read(control.slot.controlAt08);
    GuiDialogBaseVtable().appendChildAt10(&owner, Copy(control.slot));
    controlLife.Captured(capturedChild, false);
    groupLife.Captured(capturedGroup, false);
    vectorLife.Finish();
}
void CheckBoxes(Body& body)
{
    auto& s = RequireMusicDialogNativeServices();
    Fresh descriptor;
    CreateDescriptorReference(descriptor.slot);
    RefLife descriptorLife{descriptor.slot};
    auto* capturedDescriptor = Read(descriptor.slot.controlAt08);
    auto* data = Resolve<Descriptor>(descriptor.slot);
    *data = CheckTemplate();
    SaveLoadMessageBoxNativeChildFactoriesRuntime factory({s.pictures, s.surfaces, s.strings});
    for (std::int32_t i = 0; i < ItemCount(); ++i)
    {
        if (s.resources.presentation==MusicResourceProfile::Presentation::IndividualStateImages)
        {
            // 旧表逐曲七状态显式投影；position=null保留图片原点及两组的矩形并集。
            *data=s.resources.trackButtons[static_cast<std::size_t>(i)];
            if(data->eventAt00!=i+CheckTemplate().eventAt00)
                throw std::invalid_argument("Music track control ID does not match its song index");
        }
        else
        {
            data->eventAt00 = i + CheckTemplate().eventAt00;
            data->positionAt3C = Positions() + i;
        }
        Fresh child;
        factory.CreateAt5043B0(&child.slot, &body.common, data);
        RefLife childLife{child.slot};
        Fresh event;
        CreateToolClickedEventAt4E2F5A(event.slot, child.slot, s.memory);
        RefLife eventLife{event.slot};
        auto* capturedEvent = Read(event.slot.controlAt08);
        Slot binding{Read(event.slot.baseAt00), 0, capturedEvent};
        if (capturedEvent) ++capturedEvent->referenceCountAt04;
        {
            auto* control = Resolve<Control>(child.slot);
            RefLife bindingLife{binding};
            auto& target = control->eventAt3C;
            if (&target != &binding)
            {
                auto* old = Read(target.controlAt08);
                if (old && --old->referenceCountAt04 == 0)
                {
                    if (old->owningAt08) old->DestroyPayloadAtVslot00();
                    if (auto* now = Read(target.controlAt08)) now->DeleteControlAtVslot04(1);
                }
                target.baseAt00 = Read(event.slot.baseAt00);
                target.biasAt04 = 0;
                target.controlAt08 = capturedEvent;
                if (capturedEvent) ++capturedEvent->referenceCountAt04;
            }
            control->eventEnabledAt48 = 1;
            bindingLife.Captured(capturedEvent);
        }
        {
            Slot childCopy = Copy(child.slot);
            RefLife copyLife{childCopy};
            auto* captured = Read(childCopy.controlAt08);
            auto& children = body.common.childrenAt0C;
            auto* end = Read(children.end);
            if (end != Read(children.capacity))
            {
                end->baseAt00 = Read(childCopy.baseAt00);
                end->biasAt04 = Read(childCopy.biasAt04);
                end->controlAt08 = captured;
                if (captured) ++captured->referenceCountAt04;
                children.end = end + 1;
            }
            else
            {
                GrowGuiControlChildrenAt4D93B0(children, end, childCopy);
                captured = Read(childCopy.controlAt08);
            }
            copyLife.Captured(captured);
        }
        {
            Slot eventCopy{Read(event.slot.baseAt00), 0, capturedEvent};
            if (capturedEvent) ++capturedEvent->referenceCountAt04;
            RefLife copyLife{eventCopy};
            auto* captured = capturedEvent;
            AppendPopupClickedEventReferenceAt4EA97F(*Resolve<GuiClickedEventReferences>(body.eventsAt74),
                eventCopy, captured, s.memory);
            copyLife.Captured(captured);
        }
        eventLife.Captured(capturedEvent);
        childLife.Reload();
    }
    descriptorLife.Captured(capturedDescriptor, false);
}
}
}
namespace KaGuYa::Start::Runtime
{
GuiDialogNativeBody5Ch* ConstructMusicDialogNativeAt4E3900(void* storage)
{
    using namespace MusicDialogNativeDetail;
    auto& s = RequireMusicDialogNativeServices();
    (void)ItemCount();
    auto* body = ::new(storage) Body;
    ConstructGuiDialogAt4E0BE0(&body->common, nullptr, -1);
    body->common.vptrAt00 = &MusicDialogNativeVtable();
    body->boundsAt64 = {};
    body->eventsAt74 = {};
    struct Prefix final
    {
        Body* body;
        bool active = true;
        ~Prefix() noexcept
        {
            if (!active) return;
            ReleaseYtlReference(body->eventsAt74);
            DestroyGuiDialogAt4F7AC0(&body->common);
        }
    } prefix{body};
    auto* manager = static_cast<GuiClickedEventReferences*>(s.memory.AllocateBody(0x0C));
    if (manager) *manager = {};
    AdoptGuiClickedEventManagerAt4D7B90(body->eventsAt74, manager);
    Background(body->common);
    CheckBoxes(*body);
    AdoptGuiInputAt536EA0(body->common.inputAt18, NewGameGuiInputAt4EAAED());
    for (const auto& record : Operations())
    {
        Fresh operation;
        CreateNavigationExAt504CE0(operation.slot, record.input, record.pairs);
        Resolve<NavigationEx24h>(operation.slot)->firstDelayAt1C = record.first;
        Resolve<NavigationEx24h>(operation.slot)->repeatDelayAt20 = record.repeat;
        RefLife operationLife{operation.slot};
        AppendGuiNavigationReferenceAt4D9D60(body->common.navigationAt24, operation.slot);
        operationLife.Reload();
    }
    prefix.active = false;
    return &body->common;
}
}
