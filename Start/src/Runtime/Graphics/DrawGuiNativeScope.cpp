#include <Runtime/DrawGuiNativeScope.h>

namespace KaGuYa::Start::Runtime
{
DrawGuiNativeScope::DrawGuiNativeScope(DrawGuiNativeServices* services) noexcept
{
    if (!services) return;
    graphics_.emplace(services->graphics);
    input_.emplace(services->input);
    opacity_.emplace(services->opacityReset, services->opacityUpdate);
}
DrawGuiNativeScope::~DrawGuiNativeScope() noexcept = default;
}
