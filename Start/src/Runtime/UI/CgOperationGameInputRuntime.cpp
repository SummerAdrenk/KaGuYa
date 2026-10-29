#include <Runtime/CgOperationGameInputRuntime.h>

#include <Runtime/GuiInputStateRuntime.h>

namespace KaGuYa::Start::Runtime
{
CgOperationGameInputRuntime::CgOperationGameInputRuntime(
    const GameGuiInputStateRuntime& input,
    IGraphicsSurfacePixelApi& pixels) noexcept
    : input_(input), pixels_(pixels)
{
}

bool CgOperationGameInputRuntime::ReadInputAtVslot70(std::int32_t eventValue)
{
    // 5100E0 的根输入分支调用 +04，读取新按下，而非 +00 的持续按住。
    return input_.ReadPressedAtVslot04(eventValue);
}

POINT CgOperationGameInputRuntime::ReadPointerAtVslot74()
{
    return input_.ReadPointerAtVslot08();
}

std::int32_t CgOperationGameInputRuntime::ReadStepDirectionAtVslot78()
{
    // 50B920 -> 50B8E0 原样返回有符号 wheel；+2C 门控由真实输入实现。
    // 521A20..521A38 在 Slider 内正值减 step、负值加 step，此处不反号或归一化。
    return input_.ReadWheelAtVslot0C();
}

bool CgOperationGameInputRuntime::ReadInputAtVslot7C()
{
    return input_.ReadPointerMovedAtVslot10();
}

bool CgOperationGameInputRuntime::ReadPressAtVslot80()
{
    return input_.ReadMousePressAtVslot14();
}

bool CgOperationGameInputRuntime::ReadReleaseAtVslot84()
{
    // +18 是松开或抑制条件，不是仅在松开边沿成立。
    return input_.ReadMouseReleaseAtVslot18();
}

std::uint32_t CgOperationGameInputRuntime::ReadSurfacePixelAtGetSurfacePixel(
    GraphicsSurfaceHandle handle,
    std::int32_t x,
    std::int32_t y)
{
    return pixels_.GetSurfacePixel(handle, x, y);
}
}
