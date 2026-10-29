#pragma once

#include <Runtime/GraphicsSurfaceRuntime.h>

namespace KaGuYa::Start::Runtime
{
class IGraphicsSurfaceSaveApi
{
public:
    virtual ~IGraphicsSurfaceSaveApi() = default;
    // 4895B3..4895C0：cdecl(handle, HANDLE, 0)，仅观察AL；模块为空返回0。
    [[nodiscard]] virtual std::uint8_t SaveSurfaceIfLoaded(
        GraphicsSurfaceHandle surface, HANDLE file) = 0;
};
}
