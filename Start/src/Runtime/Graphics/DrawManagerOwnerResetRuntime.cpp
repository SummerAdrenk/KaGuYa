#include <Runtime/DrawManagerOwnerResetRuntime.h>

namespace KaGuYa::Start::Runtime
{
DrawManagerOwnerResetRuntime::DrawManagerOwnerResetRuntime(
    DrawManagerLayerRuntime& layers, IGraphicsWindowSurfaceApi& graphics) noexcept
    : layers_(layers), graphics_(graphics) {}

void DrawManagerOwnerResetRuntime::SynchronizeAtVslot18()
{
    layers_.ResetOwnedLayersAt47E940(graphics_);
}
}
