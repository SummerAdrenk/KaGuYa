#include <Runtime/OpacityDialogNative.h>
#include <Runtime/GuiControlFactoryNative.h>
#include <Runtime/GuiSliderNative.h>
#include <Runtime/OpacityDialogObjectGraphRuntime.h>
#include <Runtime/PopupNativeLifetime.h>

#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Body = OpacityDialogNativeBody6Ch;
using Base = GuiDialogNativeBody5Ch;
using Slot = YtlReferenceSlotRuntime;
thread_local const IOpacityDialogResetStateServices* resetSource = nullptr;
thread_local IOpacityDialogUpdateServices* updateSource = nullptr;

const IOpacityDialogResetStateServices& ResetSource()
{
    if (!resetSource) throw std::logic_error("native OpacityDialog reset source is not installed");
    return *resetSource;
}
IOpacityDialogUpdateServices& UpdateSource()
{
    if (!updateSource) throw std::logic_error("native OpacityDialog update source is not installed");
    return *updateSource;
}
const GuiSliderNativeVtable31& SliderMethods(GuiControlNativeBody50h* body) noexcept
{
    return *reinterpret_cast<const GuiSliderNativeVtable31*>(body->vptrAt00);
}

// 正常尾不清局部Ref；payload返回后重读该局部control，与4E49D6的ESI版本分开。
void FinishChild(Slot& slot) noexcept
{
    auto* control = slot.controlAt08;
    if (!control || --control->referenceCountAt04 != 0) return;
    if (control->owningAt08) control->DestroyPayloadAtVslot00();
    if (slot.controlAt08) slot.controlAt08->DeleteControlAtVslot04(1);
}
struct ChildUnwind final
{
    Slot& slot;
    bool active = true;
    ~ChildUnwind() noexcept { if (active) ReleaseGuiControlReferenceAt4F6460(slot); }
    void Finish() noexcept { active = false; FinishChild(slot); }
};
struct BaseUnwind final
{
    Base& body;
    bool active = true;
    ~BaseUnwind() noexcept { if (active) DestroyGuiDialogAt4F7AC0(&body); }
};
struct FactoryUnwind final
{
    Slot& slot;
    YtlReferenceControlRuntime* captured;
    bool active = true;
    ~FactoryUnwind() noexcept { if (active) ReleaseGuiFactoryReferenceAt4F6540(slot); }
    void Finish() noexcept
    {
        active = false;
        // 4E47E3捕获ESI；正常尾不从slot重取，也不清局部三DWORD。
        if (!captured || --captured->referenceCountAt04 != 0) return;
        if (captured->owningAt08) captured->DestroyPayloadAtVslot00();
        captured->DeleteControlAtVslot04(1);
    }
};

GuiDirtyRectNativeBody80h* __fastcall Dirty(Base* body, void*) noexcept
{ return ResolveGuiDirtyRectReference(body->dirtyAt50); }
Base* __fastcall Delete(Base* body, void*, std::uint32_t flags) noexcept
{
    return reinterpret_cast<Base*>(DeleteOpacityDialogNativeAt4FCE80(
        reinterpret_cast<Body*>(body), flags));
}
void __fastcall Reset(Base* body, void*)
{
    body->vptrAt00->writeEnabledAt24(body, 1);
    Slot background;
    body->vptrAt00->findChildAt14(body, &background, 0);
    {
        ChildUnwind lifetime{background};
        auto* child = ResolveGuiControl(background);
        child->vptrAt00->resetAt08(child);
        lifetime.Finish();
    }
    Slot slider;
    body->vptrAt00->findChildAt14(body, &slider, 1);
    ChildUnwind lifetime{slider};
    auto* child = ResolveGuiControl(slider);
    child->vptrAt00->resetAt08(child);
    const bool nonZero = ResetSource().SelectMessageWindowOpacitySelectorNonZeroAt50D730();
    const auto value = ResetSource().ReadSaveSlot1OpacitySelectorZeroOrNonZero(nonZero);
    child = ResolveGuiControl(slider);
    SliderMethods(child).setValueAt64(child, value);
    lifetime.Finish();
    body->activeAt4C = nullptr;
}
std::int32_t __fastcall Update(Base* body, void*)
{
    Slot slider;
    body->vptrAt00->findChildAt14(body, &slider, 1);
    ChildUnwind lifetime{slider};
    auto* child = ResolveGuiControl(slider);
    const auto before = SliderMethods(child).valueAt60(child);
    body->vptrAt00->processInputAt54(body);
    child = ResolveGuiControl(slider);
    const auto after = SliderMethods(child).valueAt60(child);
    if (before != after)
    {
        const bool nonZero = UpdateSource().SelectMessageWindowOpacitySelectorNonZeroAt52DF20();
        UpdateSource().WriteSaveSlot1OpacitySelectorZeroOrNonZero(nonZero, after);
        // 52E039/52E078是另一次Ensure/CMesWin读取，来源实现不能复用写Save前receiver。
        UpdateSource().RefreshMessageWindowStyleAt5325F0(nonZero ? 1 : 0);
    }
    lifetime.Finish();
    return 1;
}
std::int32_t __fastcall End(Base*, void*) noexcept { return 1; }

class DialogControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit DialogControl(Body* body) noexcept
        : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        auto* body = static_cast<Body*>(payloadAt14);
        if (body) body->common.vptrAt00->deletingDestructorAt04(&body->common, 1);
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    { RetireEnvironmentObjectControl(this, flags); }
};
static_assert(sizeof(DialogControl) == 0x18);

// 606FE4/607008与5BCB34/50/68：保持原七槽null身份及CP932字节，不创建现代描述符。
constexpr char backgroundName[] =
    "\x83\x52\x83\x7D\x83\x93\x83\x68\x83\x76\x83\x8C\x83\x43"
    "\x93\xA7\x96\xBE\x93\x78\x2E\x76\x72\x6D";
constexpr char cursorOn[] =
    "\x8B\xA4\x92\xCA\x83\x4A\x81\x5B\x83\x5C\x83\x8B\x83\x49"
    "\x83\x93\x2E\x76\x72\x6D";
constexpr char cursorOff[] =
    "\x8B\xA4\x92\xCA\x83\x4A\x81\x5B\x83\x5C\x83\x8B\x83\x49"
    "\x83\x74\x2E\x76\x72\x6D";
const GuiStaticDescriptorNative24h backgroundDescriptor{
    0, {backgroundName, nullptr, nullptr, backgroundName, backgroundName, nullptr, nullptr}, nullptr};
const GuiSliderDescriptorNative6Ch sliderDescriptor{
    1, {cursorOn, cursorOff, nullptr, cursorOn, cursorOn, cursorOn, nullptr},
    {}, nullptr, 0, 0, 0, {845, 661, 1009, 680}, {0, 0, -12, 0}};
const OpacityDialogNativeProfile defaultProfile{sliderDescriptor.interactionRectAt4C};

void AppendChild(Base& body, const Slot& source)
{
    Slot copied = source;
    if (copied.controlAt08) ++copied.controlAt08->referenceCountAt04;
    // 4E4871/4E4927直接4FF020，不使用可被替换的派生append槽。
    GuiDialogBaseVtable().appendChildAt10(&body, copied);
}
}

const OpacityDialogNativeProfile& MamatomoOpacityDialogNativeProfile() noexcept { return defaultProfile; }
const GuiDialogNativeVtable35& OpacityDialogNativeVtable() noexcept
{
    static const GuiDialogNativeVtable35 table = [] {
        auto result = GuiDialogBaseVtable();
#define OPACITY_SLOT(slot, entry) result.slot = reinterpret_cast<decltype(result.slot)>(&entry)
        OPACITY_SLOT(dirtyAt00, Dirty);
        OPACITY_SLOT(deletingDestructorAt04, Delete);
        OPACITY_SLOT(beginAt44, Reset);
        OPACITY_SLOT(pollAt48, Update);
        OPACITY_SLOT(endAt4C, End);
#undef OPACITY_SLOT
        return result;
    }();
    return table;
}

OpacityDialogNativeBody6Ch* ConstructOpacityDialogNativeAt4E4780(
    Body& body, IGuiControlResourceLoadServices& loader, IGraphicsSurfaceApi& surfaces,
    IRuntimeStringManagerSource& strings)
{
    return ConstructOpacityDialogNativeAt4E4780(body, loader, surfaces, strings, defaultProfile);
}
OpacityDialogNativeBody6Ch* ConstructOpacityDialogNativeAt4E4780(
    Body& body, IGuiControlResourceLoadServices& loader, IGraphicsSurfaceApi& surfaces,
    IRuntimeStringManagerSource& strings, const OpacityDialogNativeProfile& profile)
{
    GraphicsSurfaceNativeScope surfacesScope(surfaces);
    ConstructGuiDialogAt4E0BE0(&body.common, nullptr, -1);
    BaseUnwind baseLifetime{body.common};
    body.common.vptrAt00 = &OpacityDialogNativeVtable();
    void* factoryBody = ::operator new(1);
    Slot factory;
    ConstructGuiFactoryReferenceAt4D7860(factory, factoryBody, 1);
    FactoryUnwind factoryLifetime{factory, factory.controlAt08};
    RECT bounds{};
    Slot child;
    auto background = backgroundDescriptor;
    background.positionAt20 = profile.backgroundPosition ? &*profile.backgroundPosition : nullptr;
    CreateGuiStaticAt505800(child, &body.common, background, loader, surfaces, strings);
    {
        ChildUnwind childLifetime{child};
        auto* receiver = ResolveGuiControl(child);
        RECT output;
        const auto* rect = receiver->vptrAt00->rectAt3C(receiver, &output);
        ::UnionRect(&bounds, &bounds, rect);
        AppendChild(body.common, child);
        childLifetime.Finish();
    }
    auto slider = sliderDescriptor;
    slider.interactionRectAt4C = profile.track;
    CreateGuiSliderAt5052C0(child, &body.common, slider, loader, surfaces, strings);
    {
        ChildUnwind childLifetime{child};
        auto* receiver = ResolveGuiControl(child);
        RECT output;
        const auto* rect = receiver->vptrAt00->rectAt3C(receiver, &output);
        ::UnionRect(&bounds, &bounds, rect);
        AppendChild(body.common, child);
        receiver = ResolveGuiControl(child);
        SliderMethods(receiver).setRangeAt5C(receiver, 0, 255);
        receiver = ResolveGuiControl(child);
        SliderMethods(receiver).setStepAt68(receiver, 1);
        childLifetime.Finish();
    }
    body.boundsAt5C = bounds;
    AdoptGuiInputAt536EA0(body.common.inputAt18, NewGameGuiInputAt4EAAED());
    factoryLifetime.Finish();
    baseLifetime.active = false;
    return &body;
}

OpacityDialogNativeBody6Ch* DeleteOpacityDialogNativeAt4FCE80(Body* body,
    std::uint32_t flags) noexcept
{
    body->common.vptrAt00 = &OpacityDialogNativeVtable();
    DestroyGuiDialogAt4F7AC0(&body->common);
    if ((flags & 1) != 0) ::operator delete(body, std::size_t{0x6C});
    return body;
}
void InstallOpacityDialogReferenceAt45AF17(Slot& slot, Body* completedBody)
{
    ReleaseYtlReference(slot);
    if (!completedBody) return;
    // 45AF47将ECX清零，45AF54不重取成员control；没有第二次control释放。
    slot.biasAt04 = 0;
    slot.baseAt00 = reinterpret_cast<std::uintptr_t>(completedBody);
    void* raw = ::operator new(0x18);
    auto* control = raw ? ::new (raw) DialogControl(completedBody) : nullptr;
    slot.controlAt08 = control;
    control->referenceCountAt04 = 1;
    slot.controlAt08->owningAt08 = 1;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->strideAt0C = 0x6C;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->countAt10 = 1;
}
OpacityDialogNativeBody6Ch* ResolveOpacityDialogReference(const Slot& slot) noexcept
{ return ResolveEnvironmentObjectReference<Body>(slot); }

OpacityDialogNativeScope::OpacityDialogNativeScope(const IOpacityDialogResetStateServices& reset,
    IOpacityDialogUpdateServices& update) noexcept
    : previousReset_(resetSource), previousUpdate_(updateSource)
{ resetSource = &reset; updateSource = &update; }
OpacityDialogNativeScope::~OpacityDialogNativeScope() noexcept
{ resetSource = previousReset_; updateSource = previousUpdate_; }
}
