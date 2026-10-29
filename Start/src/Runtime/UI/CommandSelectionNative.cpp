#include <Runtime/CommandSelectionNative.h>
#include <Runtime/LayerListNative.h>
#include <Runtime/SelectWindowNativeWorkflow.h>
#include <Runtime/UnSaveDataRuntime.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <bit>
#include <memory>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Body = CommandSelectionNativeBody;
using Base = LayerItemNativeBody14h;
void __fastcall Draw(Base* base, void*, const RECT* clip)
{
    auto& dialog = reinterpret_cast<Body*>(base)->dialog->common;
    dialog.vptrAt00->drawAt60(&dialog, clip);
}
void __fastcall Composite(Base* base, void*, GraphicsSurfaceNativeBody* target, const RECT* clip)
{
    auto& dialog = reinterpret_cast<Body*>(base)->dialog->common;
    YtlReferenceSlotRuntime borrowed{reinterpret_cast<std::uintptr_t>(target), 0, nullptr};
    dialog.vptrAt00->compositeAt5C(&dialog, borrowed, clip);
}
Base* __fastcall Delete(Base* base, void*, std::uint32_t flags)
{ return reinterpret_cast<Base*>(DeleteCommandSelectionNative(*reinterpret_cast<Body*>(base), flags)); }
RECT* __fastcall Bounds(Base* base, void*, RECT* result)
{ *result = reinterpret_cast<Body*>(base)->window.boundsAt14; return result; }
std::uint8_t __fastcall Request(Base* base, void*, std::uint8_t requested, std::int32_t)
{
    const auto value = static_cast<std::uint8_t>(requested != 0);
    if (base->enabledAt0C == value) return 0;
    base->enabledAt0C = value;
    base->parentAt10->vptrAt00->addDirtyAt20(base->parentAt10, &reinterpret_cast<Body*>(base)->window.boundsAt14);
    return 1;
}
void __fastcall Advance(Base*, void*) {} // 原460510为空；轮询由ReadScript命令选择状态驱动。
void __fastcall Reset(Base* base, void*) { base->enabledAt0C = 0; }
const LayerItemNativeFrameVtable methods{
    {reinterpret_cast<decltype(LayerItemNativeVtable::drawAt00)>(&Draw),
     reinterpret_cast<decltype(LayerItemNativeVtable::compositeAt04)>(&Composite),
     reinterpret_cast<decltype(LayerItemNativeVtable::deletingDestructorAt08)>(&Delete)},
    reinterpret_cast<decltype(LayerItemNativeFrameVtable::boundsAt0C)>(&Bounds),
    reinterpret_cast<decltype(LayerItemNativeFrameVtable::requestAt10)>(&Request),
    reinterpret_cast<decltype(LayerItemNativeFrameVtable::advanceAt14)>(&Advance),
    reinterpret_cast<decltype(LayerItemNativeFrameVtable::resetAt18)>(&Reset)};
}

CommandSelectionNativeBody* ConstructCommandSelectionNative(Body& body, LayerListNativeBody18h* parent,
    CommandSelectionNativeServices& services)
{
    body.services = &services;
    body.dialog = nullptr;
    ConstructLayerItemNativeAt441270(body.window.layer, parent);
    body.window.layer.vptrAt00 = methods.AsBase();
    body.window.layer.enabledAt0C = 0;
    body.window.boundsAt14 = {};
    const GraphicsSurfaceNativeScope graphics(services.surfaces);
    try
    {
        auto dialog = std::make_unique<CommandSelectionDialogNativeBody>();
        ConstructCommandSelectionDialog(*dialog, services);
        body.dialog = dialog.release();
        const auto size = services.source.CanvasSize();
        body.window.boundsAt14 = {0, 0, size.cx, size.cy};
    }
    catch (...)
    {
        if (body.dialog) { DestroyCommandSelectionDialog(*body.dialog); delete body.dialog; body.dialog = nullptr; }
        DestroyLayerWindowNativeAt464910(body.window);
        throw;
    }
    return &body;
}
CommandSelectionNativeBody* DeleteCommandSelectionNative(Body& body, std::uint32_t flags) noexcept
{
    const GraphicsSurfaceNativeScope graphics(body.services->surfaces);
    DestroyCommandSelectionDialog(*body.dialog);
    delete body.dialog;
    body.dialog = nullptr;
    DestroyLayerWindowNativeAt464910(body.window);
    if (flags & 1u) ::operator delete(&body, sizeof(Body));
    return &body;
}
void BeginCommandSelectionNative(Body& body)
{ body.dialog->common.vptrAt00->beginAt44(&body.dialog->common); }

std::int32_t TickCommandSelectionNative(Body& body, std::uint8_t blocked, SelectWindowNativeWorkflowServices& services)
{
    std::int32_t result = -1;
    if (!blocked)
    {
        auto* mouse = services.source.CaptureMouseAt418210();
        if (mouse->secondaryAt2C == 0 && mouse->wheelAt34 > 0)
        {
            auto* unsave = static_cast<UnSaveDataRuntime*>(
                ResolveEnvironmentArchiveAt417990(*services.environment.CaptureAt4180C0(), 2));
            unsave->FlagsAt1C() |= 2u;
        }
        else
        {
            auto& dialog = body.dialog->common;
            if (dialog.vptrAt00->pollAt48(&dialog) == 0) result = body.dialog->result;
            auto* dirty = dialog.vptrAt00->dirtyAt00(&dialog);
            dirty->vptrAt00->coalesceAt04(dirty);
            const auto count = std::bit_cast<std::int32_t>(dirty->vptrAt00->countAt18(dirty));
            if (count)
            {
                auto* rectangles = dirty->vptrAt00->rectsAt1C(dirty);
                for (std::int32_t i = 0; i < count; ++i)
                    body.window.layer.parentAt10->vptrAt00->addDirtyAt20(body.window.layer.parentAt10, rectangles + i);
                dirty->vptrAt00->clearAt00(dirty);
            }
        }
    }
    // 4A47A0：工具栏阻止轮询时仍刷新声音，且命令按钮不写普通选项已读标记。
    services.sound.CaptureRootAfter413BE0().Resolve().volume.Refresh();
    return result;
}
}
