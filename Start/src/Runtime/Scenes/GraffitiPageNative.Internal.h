#pragma once
#include <Runtime/GraffitiPageNative.h>
#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/LayerListNative.h>
#include <bit>

namespace KaGuYa::Start::Runtime::GraffitiPageNativeDetail
{
template<class T> T Read(const T& value) noexcept
{ return *static_cast<const volatile T*>(&value); }
inline GuiDialogNativeBody5Ch* Dialog(GraffitiPageNativeBody20h& body) noexcept
{
    const auto base = Read(body.dialogAt08.baseAt00);
    if (!base) return nullptr;
    auto* control = static_cast<EnvironmentObjectReferenceControl*>(Read(body.dialogAt08.controlAt08));
    const auto stride = control ? Read(control->strideAt0C) : 0u;
    return reinterpret_cast<GuiDialogNativeBody5Ch*>(base + Read(body.dialogAt08.biasAt04) * stride);
}
inline LayerListNativeBody18h* List(GraffitiPageNativeServices& services)
{
    auto* manager = services.source.CaptureDrawManagerAfter413BE0();
    return ResolveDrawLayerListAt47AE80(*manager);
}
GraffitiPageNativeServices& RequireServices();
void RetireTask(GraffitiPageNativeBody20h&) noexcept;
void PublishDialog(YtlReferenceSlotRuntime&, GuiDialogNativeBody5Ch*, bool weight,
    IEnvironmentRootMemory&);
}
