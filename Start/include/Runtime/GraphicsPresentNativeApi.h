#pragma once

#include <Runtime/GraphicsRootNative.h>

namespace KaGuYa::Start::Runtime
{
// 4836FF/507251：只有呈现命令；门控和取根属于命令内部，不提供分离的Peek。
class IGraphicsPresentNativeApi
{
public:
    virtual ~IGraphicsPresentNativeApi() = default;
    virtual void RenderScreenIfLoaded() = 0;
};

// 正文外的稳定借用服务，不拥有根、模块或导出地址。
class GraphicsPresentNativeApi final : public IGraphicsPresentNativeApi
{
public:
    GraphicsPresentNativeApi(GraphicsRootOwner& owner, IRuntimeStringManagerSource& strings,
        IGraphicsModuleNativeWin32Api& win32) noexcept;
    void RenderScreenIfLoaded() override;

private:
    GraphicsRootOwner& owner_;
    IRuntimeStringManagerSource& strings_;
    IGraphicsModuleNativeWin32Api& win32_;
};
}
