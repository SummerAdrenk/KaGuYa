#pragma once

namespace KaGuYa::Start::Runtime
{
// Start.exe:4347C0/432B40。窗口模式只改变目标视口，不修改逻辑画布尺寸。
class IGraphicsWindowModeApi
{
public:
    virtual ~IGraphicsWindowModeApi() = default;
    // 指针由DLL持有；调用者按原顺序立即复制四个DWORD，不延长其有效期。
    [[nodiscard]] virtual const int* ResizeScreenIfLoaded(int width, int height, int mode) = 0;
    virtual void ResetRendererIfLoaded() = 0;
};
}
