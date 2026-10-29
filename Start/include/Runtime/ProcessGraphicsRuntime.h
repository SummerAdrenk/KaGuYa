#pragma once

#include <Runtime/GraphicsRootNative.h>

namespace KaGuYa::Start::Runtime
{
class GraphicsSurfaceNativeApi;
class GraphicsPatternFilterNativeApi;
class GraphicsScreenNativeApi;
class GraphicsTransitNativeApi;
class GraphicsGuiNativeApi;
class GraphicsPresentNativeApi;
// 6198B8唯一发布槽；取得owner本身不创建1Ch正文，也不装载DLL。
GraphicsRootOwner& ProcessGraphicsRootOwner() noexcept;
[[nodiscard]] GraphicsRootNativeBody1Ch* CaptureProcessGraphicsRootAt4182F0();
// 仅取稳定的非拥有代理，业务方法才Capture；构造/释放Surface及滤镜共享唯一owner。
GraphicsSurfaceNativeApi& ProcessGraphicsSurfaceNativeApi() noexcept;
GraphicsPatternFilterNativeApi& ProcessGraphicsPatternFilterNativeApi() noexcept;
GraphicsScreenNativeApi& ProcessGraphicsScreenNativeApi() noexcept;
GraphicsTransitNativeApi& ProcessGraphicsTransitNativeApi() noexcept;
GraphicsGuiNativeApi& ProcessGraphicsGuiNativeApi() noexcept;
GraphicsPresentNativeApi& ProcessGraphicsPresentNativeApi() noexcept;
}
