#pragma once
#include <Runtime/GraphicsRuntimeApi.h>
#include <Runtime/GraphicsRootNativeInitialize.h>
#include <Runtime/GraphicsSurfaceNativeApi.h>
#include <Runtime/GraphicsGuiNativeApi.h>
#include <Runtime/GraphicsTextNativeApi.h>
#include <Runtime/GraphicsScreenNativeApi.h>
#include <Runtime/GraphicsTransitNativeApi.h>
#include <Runtime/GraphicsPresentNativeApi.h>
#include <Runtime/GraphicsPatternFilterNativeApi.h>

namespace KaGuYa::Start::Runtime
{
// 唯一Graphics根的非拥有外观；析构不卸载，也不保存HMODULE/Initialized/error快照。
class NativeGraphicsRuntimeLibrary final : public IGraphicsRuntimeCalls
{
public:
    NativeGraphicsRuntimeLibrary() noexcept;
    NativeGraphicsRuntimeLibrary(GraphicsRootOwner&, IEnvironmentRootMemory&,
        IRuntimeStringManagerSource&, IGraphicsModuleNativeWin32Api&,
        IGraphicsInitializationPathsNativeSource&) noexcept;
    GraphicsRootNativeBody1Ch* CaptureAt4182F0();
    IGraphicsPresentNativeApi& PresentCalls() noexcept { return present_; }
    IGuiControlGraphicsSource& GuiCalls() noexcept { return gui_; }
    GraphicsTextNativeApi& TextCalls() noexcept { return text_; }
    GraphicsSurfaceNativeApi& SurfaceCalls() noexcept { return surfaces_; }
    // 供原生资源服务借同一模块来源；不Capture、不增持，也不建立第二个Graphics根。
    GraphicsRootOwner& RootSource() noexcept { return root_; }
    IGraphicsModuleNativeWin32Api& Win32Source() noexcept { return win32_; }
    void InvalidatedRectCapturedAt4B02E0(GraphicsRootNativeBody1Ch& root,const RECT* rect)
    {screen_.InvalidatedRectCapturedAt4B02E0(root,rect);}
    POINT* ReadScreenSizeCapturedAt47CC00(GraphicsRootNativeBody1Ch& root,POINT* out)
    {return screen_.ReadScreenSizeCapturedAt47CC00(root,out);}
    bool InitializeCapturedAt440770(GraphicsRootNativeBody1Ch&, std::uint32_t mode,
        HWND, std::int32_t width, std::int32_t height, const std::uint8_t* option);
    void ApplyInitialFilteringAt48A3A5();
    void Shutdown();
    const char* ReadErrorAt479FE0();
    bool ModuleLoaded() const override;
    void SetTextFirstName(const char*) override;
    void SetTextSecondName(const char*) override;
    void RenderScreenIfLoaded() override;
    SIZE ReadScreenSizeAt47CC00() override;
    const int* ResizeScreenIfLoaded(int,int,int) override;
    void ResetRendererIfLoaded() override;
    void ClearScreenIfLoaded(const RECT*) override;
    void InvalidatedRectIfLoaded(const RECT*) override;
    void ClearDirtyRectIfLoaded() override;
    std::uint8_t SaveSurfaceIfLoaded(GraphicsSurfaceHandle,HANDLE) override;
    void BackupScreenToSurfaceIfLoaded(GraphicsSurfaceHandle) override;
    void CopyDrawSurfaceIfLoaded(GraphicsSurfaceHandle,int,int,const RECT*) override;
    void BlendDrawSurfaceIfLoaded(GraphicsSurfaceHandle,int,int,int) override;
    void UniversalBlitSurfaceIfLoaded(GraphicsSurfaceHandle,GraphicsSurfaceHandle,const std::int32_t*) override;
    bool CreateTextFont(const GraphicsTextFontParameters&) override;
    bool InitializeText(const char*,int,int) override;
    RECT ReadTextDrawRect() override;
    std::int32_t GetTextLength() override;
    void SetTextBasePosition(int,int) override;
    bool BlitAllTextToSurface(GraphicsSurfaceHandle,RECT*) override;
    bool BlitTextGlyphToSurface(GraphicsSurfaceHandle,std::uint32_t,RECT*) override;
    GraphicsSurfaceHandle CreateSurface(int,int,int,int) override;
    void ReleaseSurface(GraphicsSurfaceHandle) override;
    bool LoadBitmap(GraphicsSurfaceHandle,const void*,const void*,const void*,std::uint32_t) override;
    bool LoadAlp(GraphicsSurfaceHandle,std::uint8_t,int,int,const void*) override;
    void LoadAnimationPixels(GraphicsSurfaceHandle,int,int,int,const void*) override;
    std::byte* GetSurfacePointerRaw(GraphicsSurfaceHandle) override;
    int GetSurfacePitchRaw(GraphicsSurfaceHandle) override;
    std::uint32_t GetSurfacePixel(GraphicsSurfaceHandle,int,int) override;
    void SurfaceClearRaw(GraphicsSurfaceHandle,std::uint32_t,const RECT*) override;
    int SurfaceDrawRaw(GraphicsSurfaceHandle,int,int,const SIZE*,const RECT*,const RECT*,int) override;
    int SurfaceBlitRaw(GraphicsSurfaceHandle,GraphicsSurfaceHandle,int,int,const SIZE*,const RECT*,const RECT*,int) override;
    void ClearSurface(GraphicsSurfaceHandle,std::uint32_t,const RECT*) override;
    bool DrawSurface(GraphicsSurfaceHandle,int,int,const SIZE*,const RECT*,const RECT*,int) override;
    bool BlitSurface(GraphicsSurfaceHandle,GraphicsSurfaceHandle,int,int,const SIZE*,const RECT*,const RECT*,int) override;
    bool CopyBlitSurface(GraphicsSurfaceHandle,GraphicsSurfaceHandle,int,int,const SIZE*,const RECT*,const RECT*,int) override;
    std::uint8_t TransitCopyBlit(GraphicsSurfaceHandle,GraphicsSurfaceHandle,int,int,const SIZE*,const RECT*,const RECT*,int) override;
    std::uint8_t TransitCopyDraw(GraphicsSurfaceHandle,int,int,const SIZE*,const RECT*,const RECT*,int) override;
    std::uint8_t TransitBlendBlit(GraphicsSurfaceHandle,GraphicsSurfaceHandle,int,int,int,const SIZE*,const RECT*,const RECT*,int) override;
    std::uint8_t TransitBlendDraw(GraphicsSurfaceHandle,int,int,int,const SIZE*,const RECT*,const RECT*,int) override;
    std::uint8_t TransitBlit(GraphicsSurfaceHandle,GraphicsSurfaceHandle,int,int,const SIZE*,const RECT*,const RECT*,int) override;
    std::uint8_t TransitDraw(GraphicsSurfaceHandle,int,int,const SIZE*,const RECT*,const RECT*,int) override;
    void TransitUniversalBlit(GraphicsSurfaceHandle,GraphicsSurfaceHandle,const std::int32_t*,int,int,const SIZE*,const RECT*,const RECT*,int) override;
    void TransitMosaic(GraphicsSurfaceHandle,std::uint32_t,const RECT*) override;
    void SurfaceFilterRaw(int,std::uint32_t,const RECT*) override;
    void SurfaceFilter2Raw(int,std::uint32_t,const RECT*) override;
    void SurfaceFlushRaw(int,const RECT*) override;
    void SurfaceBlurRaw(int,int,int,const RECT*) override;
    void SurfaceGaussBlurRaw(int,int,int,const RECT*) override;
    void SurfaceAddColorRaw(int,std::uint32_t,const RECT*) override;
    void SurfaceSubColorRaw(int,std::uint32_t,const RECT*) override;
    void SurfaceCopyDrawRaw(int,int,int,const SIZE*,const RECT*,const RECT*,int) override;
    void SurfaceCopyBlitRaw(int,int,int,int,const SIZE*,const RECT*,const RECT*,int) override;
    void SurfaceMulAlphaRaw(int,std::uint32_t,const RECT*) override;
    void SurfaceAddSubSurfaceRaw(int,std::uintptr_t,const RECT*) override;
    void SurfaceMosaicRaw(int,int,const RECT*) override;
    void SurfaceMulColorRaw(int,std::uint32_t,const RECT*) override;
    void SurfaceFlipRaw(int,int,const RECT*) override;
    // 覆盖旧二段式门控：每次只由Native叶取一次根，不先调用ModuleLoaded再重取。
    void SurfaceFilterIfLoaded(int h,std::uint32_t c,const RECT* r) override { SurfaceFilterRaw(h,c,r); }
    void SurfaceFilter2IfLoaded(int h,std::uint32_t c,const RECT* r) override { SurfaceFilter2Raw(h,c,r); }
    void SurfaceFlushIfLoaded(int h,const RECT* r) override { SurfaceFlushRaw(h,r); }
    void SurfaceBlurIfLoaded(int d,int s,int n,const RECT* r) override { SurfaceBlurRaw(d,s,n,r); }
    void SurfaceGaussBlurIfLoaded(int d,int s,int n,const RECT* r) override { SurfaceGaussBlurRaw(d,s,n,r); }
    void SurfaceAddColorIfLoaded(int h,std::uint32_t c,const RECT* r) override { SurfaceAddColorRaw(h,c,r); }
    void SurfaceSubColorIfLoaded(int h,std::uint32_t c,const RECT* r) override { SurfaceSubColorRaw(h,c,r); }
    void SurfaceCopyDrawIfLoaded(int h,int x,int y,const SIZE* s,const RECT* r,const RECT* c,int a) override { SurfaceCopyDrawRaw(h,x,y,s,r,c,a); }
    void SurfaceClearIfLoaded(int h,std::uint32_t c,const RECT* r) override { SurfaceClearRaw(h,c,r); }
    void SurfaceMulAlphaIfLoaded(int h,std::uint32_t c,const RECT* r) override { SurfaceMulAlphaRaw(h,c,r); }
    void SurfaceAddSubSurfaceIfLoaded(int h,std::uintptr_t s,const RECT* r) override { SurfaceAddSubSurfaceRaw(h,s,r); }
    void SurfaceMosaicIfLoaded(int h,int n,const RECT* r) override { SurfaceMosaicRaw(h,n,r); }
    void SurfaceMulColorIfLoaded(int h,std::uint32_t c,const RECT* r) override { SurfaceMulColorRaw(h,c,r); }
    void SurfaceFlipIfLoaded(int h,int n,const RECT* r) override { SurfaceFlipRaw(h,n,r); }
private:
    GraphicsRootOwner& root_;
    IEnvironmentRootMemory& memory_;
    IRuntimeStringManagerSource& strings_;
    IGraphicsModuleNativeWin32Api& win32_;
    IGraphicsInitializationPathsNativeSource& paths_;
    GraphicsSurfaceNativeApi surfaces_;
    GraphicsGuiNativeApi gui_;
    GraphicsTextNativeApi text_;
    GraphicsScreenNativeApi screen_;
    GraphicsTransitNativeApi transit_;
    GraphicsPresentNativeApi present_;
    GraphicsPatternFilterNativeApi filters_;
};
}
