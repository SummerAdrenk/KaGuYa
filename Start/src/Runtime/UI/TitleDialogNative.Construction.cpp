#include "TitleDialogNative.Internal.h"

#include <new>
#include <stdexcept>
#include <Logging/Logger.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace TitleDialogNativeDetail;
struct ConstructionUnwind final
{
    Body& body;
    bool active = true;
    ~ConstructionUnwind() noexcept
    {
        if (!active) return;
        // 5FA2F8 state3 -> 2 -> 1 -> 0; no derived deleting destructor on ctor EH.
        ReleaseYtlReference(body.clickedManagerAtC8);
        ReleaseYtlReference(body.canvasAt84);
        ReleaseYtlReference(body.blackAt78);
        DestroyGuiDialogAt4F7AC0(&body.common);
    }
};
// 已核14..18现代标题的组均为type0/count1、type1/count6，见两份标题审查。
// 5045C0's fresh output is cleared BEFORE the typed factory, unlike its local Ref.
void CreateChild(Slot& output, Common* parent, std::int32_t type, std::int32_t index,
    TitleDialogNativeServices& services, std::span<const GuiButtonDescriptorNative2Ch, 6> buttons)
{
    output = {};
    LocalReference outputLifetime{output};
    UnwrittenReference temporary;
    if (type == 0)
        CreateGuiStaticAt505800(temporary.slot, parent, ModernTitleDialogStatic(services.profile),
            services.loader, services.surfaces, services.strings);
    else if (type == 1)
        CreateGuiButtonAt5041D0(temporary.slot, parent, buttons[index],
            services.loader, services.surfaces, services.strings);
    else throw 1;
    LocalReference temporaryLifetime{temporary.slot};
    // 4FA010: output is the already-cleared fresh upcast Ref.
    ReleaseYtlReference(output);
    CopyGuiClickedEventReference(output, temporary.slot);
    temporaryLifetime.Reload();
    outputLifetime.active = false;
}
void PopulateChild(Body& body, std::int32_t type, std::int32_t index,
    TitleDialogNativeServices& services, std::span<const GuiButtonDescriptorNative2Ch, 6> buttons)
{
    UnwrittenReference childStorage;
    auto& child = childStorage.slot;
    CreateChild(child, &body.common, type, index, services, buttons);
    LocalReference childLifetime{child};
    UnwrittenReference eventStorage;
    auto& event = eventStorage.slot;
    CreateToolClickedEventAt4E2F5A(event, child);
    LocalReference eventLifetime{event};
    auto* eventControl = event.controlAt08; // 4F4737..4F49B2 original ESI.
    Slot binding{event.baseAt00, 0, eventControl};
    if (eventControl) ++eventControl->referenceCountAt04;
    BindPopupGuiControlEventAt4EA856(*ResolveGuiControl(child), binding, event);
    Slot upcast = Copy(child);
    LocalReference upcastLifetime{upcast};
    auto* upcastControl = upcast.controlAt08;
    auto& children = body.common.childrenAt0C;
    if (children.end == children.capacity)
    {
        GrowGuiControlChildrenAt4D93B0(children, children.end, upcast);
        upcastControl = upcast.controlAt08;
    }
    else AppendGuiControlChildReference(children, upcast);
    upcastLifetime.Captured(upcastControl);
    Slot eventArgument{event.baseAt00, 0, eventControl};
    if (eventControl) ++eventControl->referenceCountAt04;
    LocalReference eventArgumentLifetime{eventArgument};
    auto* argumentControl = eventControl;
    AppendPopupClickedEventReferenceAt4EA97F(*Manager(body), eventArgument, argumentControl);
    eventArgumentLifetime.Captured(argumentControl);
    eventLifetime.Captured(eventControl);
    childLifetime.Reload();
}
}
TitleDialogNativeBodyD8h* ConstructTitleDialogNativeAt4F4530(void* storage)
{
    auto& services = RequireTitleDialogNativeServices();
    // 来源家族是审计信息；运行时只要求已配置的资源和必要功能策略。
    if (services.profile.namePromptGate == TitleNamePromptGate::Unreviewed)
        throw std::logic_error("Title name prompt policy is not configured");
    // Reject incomplete profiles before touching the body or loading any resource.
    if (services.profile.phase5Recovery != TitlePhase5Recovery::ResetChildrenOnly &&
        services.profile.phase5Recovery != TitlePhase5Recovery::ResetChildrenAndParentInput)
        throw std::logic_error("Title phase5 recovery is not implemented or audited");
    const auto buttons = ModernTitleDialogButtons(services.profile);
    auto* body = static_cast<Body*>(storage);
    ConstructGuiDialogAt4E0BE0(&body->common, nullptr, -1);
    body->common.vptrAt00 = &TitleDialogNativeVtable().common;
    body->boundsAt68 = {};
    body->blackAt78 = {};
    body->canvasAt84 = {};
    ResetOperation(body->operationAt90);
    body->clickedManagerAtC8 = {};
    ConstructionUnwind construction{*body};
    void* raw = ::operator new(0x0C);
    auto* manager = raw ? ::new(raw) GuiClickedEventReferences{} : nullptr;
    AdoptGuiClickedEventManagerAt4D7B90(body->clickedManagerAtC8, manager);
    PopulateChild(*body, 0, 0, services, buttons);
    for (std::int32_t index = 0; index != 6; ++index) PopulateChild(*body, 1, index, services, buttons);

    auto& source = services.source;
    auto* capturedRoot = Root(source);
    auto* graphics = Resolve(source.RootDrawSlotAt18(capturedRoot));
    if (!source.LoadSurfaceAt480000(graphics, &body->blackAt78,
        ModernTitleDialogBlackName(services.profile))) throw 1;
    // New24 has only its own construction guard; control failure must not roll it back.
    auto* canvas = NewGraphicsSurfaceAt40C2E0(services.surfaces, services.strings);
    AdoptGraphicsSurfaceAt407A80(body->canvasAt84, canvas);
    const auto height = Field<std::int32_t>(source.SystemAt4180C0(), 0x1C);
    const auto width = Field<std::int32_t>(source.SystemAt4180C0(), 0x18);
    if (!ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(body->canvasAt84)->CreateAt415010(
        services.surfaces, width, height, 0, 1)) throw 1;
    const auto boundsHeight = Field<std::int32_t>(source.SystemAt4180C0(), 0x1C);
    const auto boundsWidth = Field<std::int32_t>(source.SystemAt4180C0(), 0x18);
    body->boundsAt68 = {0, 0, boundsWidth, boundsHeight};
    AdoptGuiInputAt536EA0(body->common.inputAt18, NewGameGuiInputAt4EAAED());
    for (std::int32_t index = 0; index != 2; ++index)
    {
        UnwrittenReference navigation;
        CreateGuiNavigationAt504CE0(navigation.slot, index + 1, ModernTitleDialogNavigation(index));
        LocalReference lifetime{navigation.slot};
        AppendGuiNavigationReferenceAt4D9D60(body->common.navigationAt24, navigation.slot);
        lifetime.Reload();
    }
    construction.active = false;
    return body;
}
}
