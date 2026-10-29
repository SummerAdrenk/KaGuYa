#pragma once

#include <Runtime/PostWindowNativeLayers.h>
#include <Runtime/DrawGuiNativeScope.h>

namespace KaGuYa::Start::Runtime
{
// 只借用本次调用的服务，不取根、不分配、不拥有任何资源。
// 默认装配同时提供gui来源，覆盖Opacity重置/更新及GUI绘制/输入；
// 显式独立入口可借既有外层gui scope，不增加原数据对象或Capture。
class PostWindowNativeDrawScope final
{
public:
    PostWindowNativeDrawScope(PostWindowNativeLayerServices& layers,
        LayerWindowNativeServices& layerRuntime, IGraphicsScreenCalls* graphics,
        DrawGuiNativeServices* gui = nullptr) noexcept;
    ~PostWindowNativeDrawScope() noexcept;
    PostWindowNativeDrawScope(const PostWindowNativeDrawScope&) = delete;
    PostWindowNativeDrawScope& operator=(const PostWindowNativeDrawScope&) = delete;

private:
    DrawGuiNativeScope gui_;
    LayerWindowNativeScope layer_;
    MessageWindowLayerNativeScope message_;
    ToolWindowNativeScope tool_;
    TextWindowLayerNativeScope text_;
    WaitMarkNativeScope wait_;
    SelectWindowNativeScope select_;
    CountDownWindowNativeScope countdown_;
    MessageWindowNativeConfigScope configuration_;
    // 子scope也会安装Surface/LayerList上下文，且可能将screen置空。
    // 公共调用上下文最后安装，保证列表动态虚槽使用传入的screen服务。
    GraphicsSurfaceNativeScope surfaces_;
    LayerListNativeScope registration_;
};
}
