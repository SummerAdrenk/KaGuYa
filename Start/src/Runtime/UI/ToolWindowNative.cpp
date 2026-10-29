#include <Runtime/ToolWindowNative.h>
#include <Runtime/GraphicsSurfaceReference.h>

#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Body = ToolWindowNativeBody40h;
using Base = LayerItemNativeBody14h;
using Slot = YtlReferenceSlotRuntime;
thread_local ToolWindowNativeServices* servicesSource;
ToolWindowNativeServices& Services()
{
    if (!servicesSource) throw std::logic_error("ToolWindowNativeScope is not installed");
    return *servicesSource;
}
struct RefLifetime final
{
    Slot& slot;
    bool active = true;
    ~RefLifetime() noexcept { if (active) ReleaseYtlReference(slot); }
    void Finish() noexcept
    {
        active = false;
        auto* control = slot.controlAt08;
        if (control && --control->referenceCountAt04 == 0)
        {
            if (control->owningAt08) control->DestroyPayloadAtVslot00();
            if (slot.controlAt08) slot.controlAt08->DeleteControlAtVslot04(1);
        }
    }
};
class DialogControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit DialogControl(ToolDialogNativeBody7Ch* body) noexcept
        : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        auto* body = static_cast<GuiDialogNativeBody5Ch*>(payloadAt14);
        if (body) body->vptrAt00->deletingDestructorAt04(body, 1);
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    { RetireEnvironmentObjectControl(this, flags); }
};
static_assert(sizeof(DialogControl) == 0x18);
void AppendDialogAt457C80(DrawLayerReferenceArray& array, ToolDialogNativeBody7Ch* body,
    IEnvironmentRootMemory& memory)
{
    Slot slot{};
    if (body)
    {
        slot.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
        void* raw = memory.AllocateBody(0x18);
        auto* control = raw ? ::new(raw) DialogControl(body) : nullptr;
        control->referenceCountAt04 = 1;
        control->owningAt08 = 1;
        control->strideAt0C = 0x7C;
        slot.controlAt08 = control;
        control->countAt10 = 1;
    }
    RefLifetime lifetime{slot};
    // 454A60/461960及452560、490450、48F940等子树与共享Ref数组逐指令对应。
    // control使用Dialog+04删除，不能调用图层专用+08工厂。
    AppendDrawLayerReference(array, slot, memory);
    auto* captured = slot.controlAt08;
    lifetime.active = false;
    if (captured && --captured->referenceCountAt04 == 0)
    {
        if (captured->owningAt08) captured->DestroyPayloadAtVslot00();
        captured->DeleteControlAtVslot04(1);
    }
}
GuiDialogNativeBody5Ch* Dialog(const Slot& slot) noexcept
{ return ResolveEnvironmentObjectReference<GuiDialogNativeBody5Ch>(slot); }
void __fastcall Draw(Base* base, void*, const RECT* clip)
{
    Slot selected;
    SelectToolDialogNativeAt417B50(*reinterpret_cast<Body*>(base), selected, Services().replay);
    RefLifetime lifetime{selected};
    auto* dialog = Dialog(selected);
    dialog->vptrAt00->drawAt60(dialog, clip);
    lifetime.Finish();
}
void __fastcall Composite(Base* base, void*, GraphicsSurfaceNativeBody* target, const RECT* clip)
{
    Slot selected;
    SelectToolDialogNativeAt417B50(*reinterpret_cast<Body*>(base), selected, Services().replay);
    RefLifetime lifetime{selected};
    auto* dialog = Dialog(selected); // 481B26前固定receiver，451500分配后才取该receiver的表。
    Slot argument;
    ConstructGraphicsSurfaceReferenceAt451500(argument, target, 0);
    dialog->vptrAt00->compositeAt5C(dialog, argument, clip);
    lifetime.Finish();
}
Base* __fastcall Delete(Base* base, void*, std::uint32_t flags) noexcept
{ return reinterpret_cast<Base*>(DeleteToolWindowNativeAt468DE0(*reinterpret_cast<Body*>(base), flags)); }
RECT* __fastcall Bounds(Base* base, void*, RECT* output) noexcept
{ *output = reinterpret_cast<Body*>(base)->window.boundsAt14; return output; }
std::uint8_t __fastcall Request(Base* base, void*, std::uint8_t requested, std::int32_t)
{
    if ((requested != 0) == (base->enabledAt0C != 0)) return 0;
    EnableToolWindowNativeAt418F70(*reinterpret_cast<Body*>(base), requested ? 1 : 0, Services().replay);
    return 1;
}
void __fastcall Advance(Base*, void*) noexcept {} // 46B230只有retn，工具输入在4BE200单独调度。
void __fastcall Reset(Base* base, void*)
{
    base->enabledAt0C = 0;
    Slot selected;
    SelectToolDialogNativeAt417B50(*reinterpret_cast<Body*>(base), selected, Services().replay);
    RefLifetime lifetime{selected};
    auto* dialog = Dialog(selected);
    dialog->vptrAt00->beginAt44(dialog);
    lifetime.Finish();
}
const LayerItemNativeFrameVtable methods{
    {reinterpret_cast<decltype(LayerItemNativeVtable::drawAt00)>(&Draw),
     reinterpret_cast<decltype(LayerItemNativeVtable::compositeAt04)>(&Composite),
     reinterpret_cast<decltype(LayerItemNativeVtable::deletingDestructorAt08)>(&Delete)},
    reinterpret_cast<decltype(LayerItemNativeFrameVtable::boundsAt0C)>(&Bounds),
    reinterpret_cast<decltype(LayerItemNativeFrameVtable::requestAt10)>(&Request),
    reinterpret_cast<decltype(LayerItemNativeFrameVtable::advanceAt14)>(&Advance),
    reinterpret_cast<decltype(LayerItemNativeFrameVtable::resetAt18)>(&Reset)};
}

ToolWindowNativeScope::ToolWindowNativeScope(ToolWindowNativeServices& services) noexcept
    : previous_(servicesSource), surfaces_(services.surfaces), layers_(services.memory), dialog_(services.relation)
{ servicesSource = &services; }
ToolWindowNativeScope::~ToolWindowNativeScope() noexcept { servicesSource = previous_; }
const LayerItemNativeFrameVtable& ToolWindowNativeMethods() noexcept { return methods; }
Body* ConstructToolWindowNativeAt45A4B0(Body& body, LayerListNativeBody18h* parent, ToolWindowNativeServices& services)
{
    const ToolWindowNativeScope scope(services);
    ConstructLayerItemNativeAt441270(body.window.layer, parent);
    body.window.boundsAt14 = {};
    body.window.layer.vptrAt00 = &methods.base;
    body.dialogsAt24 = {};
    body.window.layer.enabledAt0C = 0;
    try
    {
        for (int mode = 0; mode != 2; ++mode)
        {
            void* raw = services.memory.AllocateBody(0x7C);
            ToolDialogNativeBody7Ch* dialog = nullptr;
            try
            {
                if (raw) dialog = ConstructToolDialogNativeAt4E27F0(*::new(raw) ToolDialogNativeBody7Ch,
                    mode, services.profile, services.resources, services.surfaces, services.strings);
            }
            catch (...) { ::operator delete(raw, std::size_t{0x7C}); throw; }
            AppendDialogAt457C80(body.dialogsAt24, dialog, services.memory);
        }
        body.selectedAt3C = -1;
        body.valueAt30 = 0;
    }
    catch (...) { body.window.boundsAt14 = {}; }
    return &body;
}
Body* DeleteToolWindowNativeAt468DE0(Body& body, std::uint32_t flags) noexcept
{
    DestroyDrawLayerReferencesAt461750(body.dialogsAt24);
    DestroyLayerWindowNativeAt464910(body.window);
    if (flags & 1U) ::operator delete(&body, std::size_t{0x40});
    return &body;
}
Slot* SelectToolDialogNativeAt417B50(Body& body, Slot& output, IToolWindowReplaySource& source)
{
    const int selected = source.ReadReplayAt417B50() != 0 ? 1 : 0;
    if (body.selectedAt3C != selected)
    {
        auto* dialog = Dialog(body.dialogsAt24.begin[selected]);
        ResetToolDialogControlsAt532480(*dialog);
        body.selectedAt3C = selected;
    }
    // 417C09重新取vector起址；reset抛出时selector及fresh输出都尚未提交。
    CopyGuiClickedEventReference(output, body.dialogsAt24.begin[selected]);
    return &output;
}
void EnableToolWindowNativeAt418F70(Body& body, std::uint8_t value, IToolWindowReplaySource& source)
{
    body.window.layer.enabledAt0C = value;
    Slot first;
    SelectToolDialogNativeAt417B50(body, first, source);
    RefLifetime firstLifetime{first};
    auto* dialog = Dialog(first);
    dialog->vptrAt00->writeEnabledAt24(dialog, value);
    firstLifetime.Finish();
    Slot second;
    SelectToolDialogNativeAt417B50(body, second, source);
    RefLifetime secondLifetime{second};
    auto* selected = ResolveEnvironmentObjectReference<ToolDialogNativeBody7Ch>(second);
    auto* parent = body.window.layer.parentAt10;
    parent->vptrAt00->addDirtyAt20(parent, &selected->boundsAt60);
    secondLifetime.Finish();
}
}
