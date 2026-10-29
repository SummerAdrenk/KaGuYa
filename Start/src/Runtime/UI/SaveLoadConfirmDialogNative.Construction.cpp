#include "SaveLoadConfirmDialogNative.Internal.h"
#include <Runtime/SaveLoadResourceProfile.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace SaveLoadConfirmNativeDetail;
template<class Descriptor>
void CreateControlAt5045C0(Slot& output, Common* parent, const Descriptor& descriptor)
{
    auto& deps = RequireSaveLoadConfirmDialogNativeServices();
    output = {};
    RefLifetime outputLifetime{output}; // 原var10 bit0，先保护fresh返回槽。
    Slot temporary;
    if constexpr (std::is_same_v<Descriptor, GuiStaticDescriptorNative24h>)
        CreateGuiStaticAt505800(temporary, parent, descriptor, deps.source, deps.surfaces, deps.strings);
    else
        CreateGuiButtonAt5041D0(temporary, parent, descriptor, deps.source, deps.surfaces, deps.strings);
    RefLifetime temporaryLifetime{temporary};
    // 504740 -> 4FA010：不能省掉通用工厂返回槽的额外复制/增持/正常退役。
    CopyAssignGraphicsSurfaceReferenceAt465870(output, temporary);
    temporaryLifetime.armed = false;
    ReleaseGuiControlReferenceAt4F6460(temporary);
    outputLifetime.armed = false;
}
void AppendControl(Body& body, Slot& child, RECT& bounds)
{
    RefLifetime childLifetime{child}; // 4EADF9，成功返回后才登记。
    RECT temporary;
    auto* selected = ResolveGuiControl(child);
    ::UnionRect(&bounds, &bounds, selected->vptrAt00->rectAt3C(selected, &temporary));
    Slot event;
    CreateToolClickedEventAt4E2F5A(event, child); // 先new0C再从当前child读+04。
    RefLifetime eventLifetime{event};
    auto* cachedEventControl = event.controlAt08;
    Slot argument;
    Copy(argument, event);
    RefLifetime argumentLifetime{argument};
    selected = ResolveGuiControl(child);
    auto& destination = selected->eventAt3C;
    if (&destination != &argument)
    {
        ReleaseUncleared(destination);
        destination = {event.baseAt00, 0, cachedEventControl};
        if (cachedEventControl) ++cachedEventControl->referenceCountAt04;
    }
    selected->eventEnabledAt48 = 1;
    argumentLifetime.FinishCaptured(cachedEventControl);
    Slot copiedChild;
    Copy(copiedChild, child);
    RefLifetime copyLifetime{copiedChild};
    AppendGuiControlChildReference(body.common.childrenAt0C, copiedChild);
    copyLifetime.Finish(true);
    // 4D8550与4D9620共享53BF60/53BB70/53B540和allocator；快路固定bias=0。
    AppendPopupClickedEventReferenceAt4EA97F(body.eventsAt74, event, cachedEventControl);
    eventLifetime.FinishCaptured(cachedEventControl);
    childLifetime.Finish(true);
}
void AppendSurface(GuiControlSurfaceReferences& output, Slot& local,
    const Slot& fixed, YtlReferenceControlRuntime*& captured)
{
    if (output.end == output.capacity)
    {
        (void)GrowGuiControlSurfaceReferencesAt4540A0(output, output.end, local);
        captured = local.controlAt08;
    }
    else
    {
        auto* end = output.end;
        *end = fixed;
        if (captured) ++captured->referenceCountAt04;
        ++output.end;
    }
}
void LoadDigits(GuiControlSurfaceReferences& output, std::span<const char* const> names)
{
    auto& deps = RequireSaveLoadConfirmDialogNativeServices();
    for (const auto* name : names)
    {
        Slot local{};
        RefLifetime lifetime{local};
        Slot fixed{};
        YtlReferenceControlRuntime* captured = nullptr;
        if (name)
        {
            auto* receiver = deps.source.CaptureLoadReceiverAt413BE0();
            AdoptGraphicsSurfaceAt407A80(local, NewGraphicsSurfaceAt40C2E0(deps.surfaces, deps.strings));
            // 4EB1FD/4EB33F/4EB47A/4EB5BF：装载前缓存EBX/ESI/bias，快路不重读。
            fixed = local;
            captured = fixed.controlAt08;
            if (!deps.source.LoadIntoAt4800A0(receiver, *Surface(fixed), name)) throw 1;
        }
        AppendSurface(output, local, fixed, captured);
        lifetime.FinishCaptured(captured);
    }
}
void LoadSpecial(GuiControlSurfaceReferences& output, const char* name)
{
    auto& deps = RequireSaveLoadConfirmDialogNativeServices();
    Slot local{};
    RefLifetime lifetime{local};
    auto* receiver = deps.source.CaptureLoadReceiverAt413BE0();
    if (!deps.source.LoadSurfaceAt480000(receiver, local, name)) throw 1;
    auto* captured = local.controlAt08;
    const Slot fixed = local;
    AppendSurface(output, local, fixed, captured);
    lifetime.FinishCaptured(captured, false); // 4EB703/4EB7CC不清局部槽。
}
}
GuiDialogNativeBody5Ch* ConstructSaveLoadConfirmDialogNativeAt4EACA0(void* storage)
{
    const auto& profile = CurrentSaveLoadResourceProfile().confirmation;
    // 四种显示策略均来自各自原表；未登记策略不能隐式当作18号数字框。
    switch (profile.kind)
    {
    case SaveLoadConfirmationKind::Text:
    case SaveLoadConfirmationKind::SingleThemeDigits:
    case SaveLoadConfirmationKind::PairedDigits:
    case SaveLoadConfirmationKind::PairedDigitsWithSeparators:
        break;
    default:
        throw std::logic_error("save confirmation kind is not supported");
    }
    auto& deps = RequireSaveLoadConfirmDialogNativeServices();
    auto* body = ::new(storage)SaveLoadConfirmDialogNativeBodyE4h;
    ConstructGuiDialogAt4E0BE0(&body->common, nullptr, -1);
    body->capturedAt5C = {};
    body->eventsAt74 = {};
    body->common.vptrAt00 = &SaveLoadConfirmDialogNativeVtable().common;
    body->maskAt84 = {}; body->labelsAt90 = {}; body->previewAt9C = {};
    for (auto& group : body->digitsAtA8) group = {};
    body->specialPagesAtD8 = {};
    // 68/6C/70/80原ctor不写；初始化成员均不抛，state5后才进入业务。
    struct Unwind
    {
        Body& body;
        bool armed = true;
        ~Unwind() noexcept { if (armed) DestroyMembers(body); }
    } unwind{*body};
    RECT bounds{};
    for (const auto& descriptor : profile.controls.statics)
    {
        Slot child;
        CreateControlAt5045C0(child, &body->common, descriptor);
        AppendControl(*body, child, bounds);
    }
    for (const auto& descriptor : profile.controls.buttons)
    {
        Slot child;
        CreateControlAt5045C0(child, &body->common, descriptor);
        AppendControl(*body, child, bounds);
    }
    AdoptGraphicsSurfaceAt407A80(body->labelsAt90, NewGraphicsSurfaceAt40C2E0(deps.surfaces, deps.strings));
    if (!Surface(body->labelsAt90)->CreateAt415010(deps.surfaces,
        Sub(bounds.right, bounds.left), Sub(bounds.bottom, bounds.top), 1, 1)) throw 1;
    Surface(body->labelsAt90)->originYAt14 = bounds.top;
    Surface(body->labelsAt90)->originXAt10 = bounds.left;
    for (std::uint32_t group = 0; group != 4; ++group)
        LoadDigits(body->digitsAtA8[group], profile.digits[group]);
    for (const auto* name : profile.specialPages) LoadSpecial(body->specialPagesAtD8, name);
    // 4EB7EB先捕获配置chars，再取root加载receiver；配置与正文服务均不额外拥有。
    if (profile.mask)
    {
        const auto* maskName = deps.source.MaskNameAt61A608().GetString();
        auto* receiver = deps.source.CaptureLoadReceiverAt413BE0();
        if (!deps.source.LoadSurfaceAt480000(receiver, body->maskAt84, maskName)) throw 1;
    }
    AdoptGuiInputAt536EA0(body->common.inputAt18, NewGameGuiInputAt4EAAED());
    for (const auto& navigation : profile.controls.navigation)
        CreateAndAppendGuiNavigation(body->common.navigationAt24, navigation.input, navigation.pairs.data());
    unwind.armed = false;
    return &body->common;
}
}
