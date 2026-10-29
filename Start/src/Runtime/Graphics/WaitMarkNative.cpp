#include <Runtime/WaitMarkNative.h>
#include <Runtime/SpriteAnimationNativeExecution.h>
#include <Runtime/SpriteNativeResourceState.h>
#include <Runtime/UnSaveDataRuntime.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <bit>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Body = WaitMarkNativeBody34h;
using Base = LayerItemNativeBody14h;
thread_local WaitMarkNativeServices* activeServices;
WaitMarkNativeServices& Current()
{ if (!activeServices) throw std::logic_error("WaitMarkNativeScope not installed"); return *activeServices; }
const WaitMarkNativeProfile descriptors{{
    {"\x83\x7D\x81\x5B\x83\x4E.vrm", 1, 1, 1100, 650},
    {"\x83\x49\x81\x5B\x83\x67\x83\x7D\x81\x5B\x83\x4E.vrm", 1, 1, 1095, 650}}};
void ReleaseCaptured(YtlReferenceControlRuntime* control) noexcept
{
    if (control && --control->referenceCountAt04 == 0)
    {
        if (control->owningAt08) control->DestroyPayloadAtVslot00();
        control->DeleteControlAtVslot04(1);
    }
}
void Invalidate(Body& body)
{
    auto* const parent = body.window.layer.parentAt10;
    parent->vptrAt00->addDirtyAt20(parent, &body.window.boundsAt14);
}
void __fastcall Draw(Base* self, void*, const RECT* clip)
{
    auto* sprite = SelectedWaitMarkSpriteNativeAt47B030(*reinterpret_cast<Body*>(self), Current().state);
    if (sprite) (void)DrawSpriteNativeAt4768D0(*sprite, clip, Current().drawing);
}
void __fastcall Composite(Base* self, void*, GraphicsSurfaceNativeBody* target, const RECT* clip)
{
    auto* sprite = SelectedWaitMarkSpriteNativeAt47B030(*reinterpret_cast<Body*>(self), Current().state);
    if (sprite) (void)CompositeSpriteNativeAt46B390(*sprite, target, clip, Current().drawing);
}
Base* __fastcall Delete(Base* self, void*, std::uint32_t flags)
{ return &DeleteWaitMarkNativeAt469680(*reinterpret_cast<Body*>(self), flags)->window.layer; }
RECT* __fastcall Bounds(Base* self, void*, RECT* output)
{ *output = reinterpret_cast<Body*>(self)->window.boundsAt14; return output; }
std::uint8_t __fastcall Request(Base* self, void*, std::int32_t value, std::int32_t)
{ return RequestWaitMarkNativeAt4873C0(*reinterpret_cast<Body*>(self), value, Current().state); }
void __fastcall Advance(Base* self, void*)
{ AdvanceWaitMarkNativeAt46B1F0(*reinterpret_cast<Body*>(self), Current().state); }
void __fastcall Reset(Base* self, void*) { self->enabledAt0C = 0; }
const WaitMarkNativeVtable methods{
    {reinterpret_cast<decltype(LayerItemNativeVtable::drawAt00)>(&Draw),
     reinterpret_cast<decltype(LayerItemNativeVtable::compositeAt04)>(&Composite),
     reinterpret_cast<decltype(LayerItemNativeVtable::deletingDestructorAt08)>(&Delete)},
    reinterpret_cast<decltype(WaitMarkNativeVtable::boundsAt0C)>(&Bounds),
    reinterpret_cast<decltype(WaitMarkNativeVtable::requestAt10)>(&Request),
    reinterpret_cast<decltype(WaitMarkNativeVtable::advanceAt14)>(&Advance),
    reinterpret_cast<decltype(WaitMarkNativeVtable::resetAt18)>(&Reset)};
}
WaitMarkNativeScope::WaitMarkNativeScope(WaitMarkNativeServices& services) noexcept
    : previous_(activeServices), surfaces_(services.surfaces), layers_(services.memory) { activeServices = &services; }
WaitMarkNativeScope::~WaitMarkNativeScope() noexcept { activeServices = previous_; }
const WaitMarkNativeVtable& WaitMarkNativeMethods() noexcept { return methods; }
const WaitMarkNativeProfile& MamatomoWaitMarkNativeProfile() noexcept { return descriptors; }
Body* ConstructWaitMarkNativeAt45E450(Body& body, LayerListNativeBody18h* parent, WaitMarkNativeServices& services)
{
    const WaitMarkNativeScope scope(services);
    ConstructLayerItemNativeAt441270(body.window.layer, parent);
    body.window.boundsAt14 = {};
    body.window.layer.vptrAt00 = &methods.base;
    body.spritesAt24 = {};
    body.window.layer.enabledAt0C = 0;
    try
    {
        for (const auto& descriptor : services.profile)
        {
            YtlReferenceSlotRuntime sprite;
            struct Unwind final { YtlReferenceSlotRuntime& slot; bool armed = true;
                ~Unwind() noexcept { if (armed) ReleaseSpriteReferenceAt460260(slot); } } unwind{sprite};
            auto* const receiver = services.resources.CaptureDrawReceiverAt413BE0();
            auto* const created = NewSpriteNativeAt4813E4(services.strings);
            InstallSpriteReferenceAt48140F(sprite, created);
            if (!services.resources.LoadAt4814F0(receiver, created, descriptor.name, descriptor.restart)) throw 1;
            if (descriptor.position) { created->xAt2C = descriptor.x; created->yAt30 = descriptor.y; }
            const auto drawable = ReadSpriteDrawableRectNativeAt4AD080(*created);
            RECT rect = drawable.state == PatternSpriteRectState::Valid ? drawable.rect : RECT{};
            (void)::UnionRect(&body.window.boundsAt14, &body.window.boundsAt14, &rect);
            YtlReferenceSlotRuntime argument = sprite;
            if (argument.controlAt08) ++argument.controlAt08->referenceCountAt04;
            Unwind argumentUnwind{argument};
            // 453F20与4530D0同一Ref12复制/增长/452320/48F720子树；不使用layer专用control工厂。
            AppendDrawLayerReference(body.spritesAt24, argument, services.memory);
            argumentUnwind.armed = false;
            ReleaseCaptured(argument.controlAt08);
            unwind.armed = false;
            ReleaseCaptured(sprite.controlAt08);
        }
        body.completedAt30 = 0;
    }
    catch (...) { body.window.boundsAt14 = {}; }
    return &body;
}
Body* DeleteWaitMarkNativeAt469680(Body& body, std::uint32_t flags) noexcept
{
    DestroyDrawLayerReferencesAt461750(body.spritesAt24);
    DestroyLayerWindowNativeAt464910(body.window);
    if (flags & 1U) ::operator delete(&body, std::size_t{0x34});
    return &body;
}
SpriteNativeBodyA0h* SelectedWaitMarkSpriteNativeAt47B030(Body& body, IWaitMarkNativeStateSource& source)
{
    const auto index = (source.CaptureUnSaveInlineAt47B034()->FlagsAt18() & 2U) ? 1U : 0U;
    return ResolveSpriteReference(body.spritesAt24.begin[index]);
}
std::uint8_t RequestWaitMarkNativeAt4873C0(Body& body, std::int32_t requested, IWaitMarkNativeStateSource& source)
{
    if (requested)
    {
        if (ReadKeyboardSixHeldAt4AA573(*source.CaptureKeyboardAt4873CC())) return 0;
        if (source.CaptureUnSaveVia417990At487410()->FlagsAt14() & 2U) return 0;
        if (body.window.layer.enabledAt0C) return 0;
        body.window.layer.enabledAt0C = 1;
        Invalidate(body);
    }
    else if (body.window.layer.enabledAt0C)
    {
        body.window.layer.enabledAt0C = 0;
        Invalidate(body);
    }
    return 0;
}
void AdvanceWaitMarkNativeAt46B1F0(Body& body, IWaitMarkNativeStateSource& source)
{
    if (!body.window.layer.enabledAt0C) return;
    auto* const sprite = SelectedWaitMarkSpriteNativeAt47B030(body, source);
    if (!sprite || !sprite->logicalNameAt0C.GetLength() || sprite->modeAt04 != 2U) return;
    if (AdvanceSpriteAnimationNativeAt443740(sprite->animationAt34) == 0) Invalidate(body);
}
EnvironmentWaitMarkNativeStateSource::EnvironmentWaitMarkNativeStateSource(EnvironmentRootOwner& environment,
    InputRootOwner& input) noexcept : environment_(environment), input_(input) {}
KeyboardManagerNativeBody438h* EnvironmentWaitMarkNativeStateSource::CaptureKeyboardAt4873CC()
{ return ResolveEnvironmentObjectReference<KeyboardManagerNativeBody438h>(input_.CaptureAt418210()->keyboardAt18); }
UnSaveDataRuntime* EnvironmentWaitMarkNativeStateSource::CaptureUnSaveInlineAt47B034()
{
    auto* const root = environment_.CaptureAt4180C0();
    const auto bytes = std::uint32_t(reinterpret_cast<std::uintptr_t>(root->archivesAt30.end) - reinterpret_cast<std::uintptr_t>(root->archivesAt30.begin));
    const auto count = std::bit_cast<std::int32_t>(bytes) / 12;
    if (static_cast<std::uint32_t>(count) <= 2U) return nullptr;
    return ResolveEnvironmentObjectReference<UnSaveDataRuntime>(root->archivesAt30.begin[2]);
}
UnSaveDataRuntime* EnvironmentWaitMarkNativeStateSource::CaptureUnSaveVia417990At487410()
{ return static_cast<UnSaveDataRuntime*>(ResolveEnvironmentArchiveAt417990(*environment_.CaptureAt4180C0(), 2)); }
}
