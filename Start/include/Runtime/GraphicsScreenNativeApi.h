#pragma once

#include <Runtime/GraphicsRootNative.h>
#include <Runtime/GraphicsSurfaceRuntime.h>
#include <Runtime/GraphicsDirtyRectApi.h>
#include <Runtime/GraphicsWindowModeApi.h>

namespace KaGuYa::Start::Runtime
{
// 4B02E0消费caller已4182F0取得的根，绝不再次取singleton或当前发布槽。
void InvalidateCapturedGraphicsRectAt4B02E0(GraphicsRootNativeBody1Ch&,const RECT*,
    IRuntimeStringManagerSource&,IGraphicsModuleNativeWin32Api&);
POINT* ReadCapturedGraphicsScreenSizeAt47CC00(GraphicsRootNativeBody1Ch&,POINT*,
    IRuntimeStringManagerSource&,IGraphicsModuleNativeWin32Api&);
// 单个叶内固定模块正文，下一叶重新取根；不缓存尺寸、视口或DLL返回指针。
class GraphicsScreenNativeApi final : public IGraphicsScreenSizeApi,
    public IGraphicsScreenCalls, public IGraphicsDirtyRectApi, public IGraphicsWindowModeApi
{
public:
    GraphicsScreenNativeApi(GraphicsRootOwner& owner, IRuntimeStringManagerSource& strings,
        IGraphicsModuleNativeWin32Api& win32) noexcept;
    [[nodiscard]] SIZE ReadScreenSizeAt47CC00() override;
    POINT* ReadScreenSizeCapturedAt47CC00(GraphicsRootNativeBody1Ch& root,POINT* out)
    {return ReadCapturedGraphicsScreenSizeAt47CC00(root,out,strings_,win32_);}
    [[nodiscard]] const int* ResizeScreenIfLoaded(int width, int height, int mode) override;
    void ResetRendererIfLoaded() override;
    void ClearScreenIfLoaded(const RECT* rect) override;
    void InvalidatedRectIfLoaded(const RECT* rect) override;
    void ClearDirtyRectIfLoaded() override;
    void InvalidatedRectCapturedAt4B02E0(GraphicsRootNativeBody1Ch& root,const RECT* rect)
    {InvalidateCapturedGraphicsRectAt4B02E0(root,rect,strings_,win32_);}
private:
    [[nodiscard]] bool CaptureModule(GraphicsModuleNativeBody8h*& module);
    GraphicsRootOwner& owner_;
    IRuntimeStringManagerSource& strings_;
    IGraphicsModuleNativeWin32Api& win32_;
};
}
