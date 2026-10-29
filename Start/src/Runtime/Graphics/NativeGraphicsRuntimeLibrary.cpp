#include <Runtime/NativeGraphicsRuntimeLibrary.h>
#include <Runtime/ProcessGraphicsRuntime.h>

namespace KaGuYa::Start::Runtime
{
NativeGraphicsRuntimeLibrary::NativeGraphicsRuntimeLibrary() noexcept
    : NativeGraphicsRuntimeLibrary(ProcessGraphicsRootOwner(), DefaultEnvironmentRootMemory(),
        DefaultRuntimeStringManagerSource(), DefaultGraphicsModuleNativeWin32Api(),
        DefaultGraphicsInitializationPathsNativeSource()) {}
NativeGraphicsRuntimeLibrary::NativeGraphicsRuntimeLibrary(GraphicsRootOwner& root, IEnvironmentRootMemory& memory,
    IRuntimeStringManagerSource& strings, IGraphicsModuleNativeWin32Api& win32,
    IGraphicsInitializationPathsNativeSource& paths) noexcept
    : root_(root), memory_(memory), strings_(strings), win32_(win32), paths_(paths),
      surfaces_(root,strings,win32), gui_(root,strings,win32,surfaces_), text_(root,strings,win32),
      screen_(root,strings,win32), transit_(root,strings,win32), present_(root,strings,win32),
      filters_(root,strings,win32) {}
GraphicsRootNativeBody1Ch* NativeGraphicsRuntimeLibrary::CaptureAt4182F0() { return root_.CaptureAt4182F0(); }
bool NativeGraphicsRuntimeLibrary::InitializeCapturedAt440770(GraphicsRootNativeBody1Ch& root,
    std::uint32_t mode, HWND window, int width, int height, const std::uint8_t* option)
{ return InitializeGraphicsRootNativeAt440770(root,mode,window,width,height,option,memory_,strings_,win32_,paths_); }
void NativeGraphicsRuntimeLibrary::Shutdown()
{ ShutdownGraphicsRootAt440660(*CaptureAt4182F0(),memory_,strings_,win32_); }
const char* NativeGraphicsRuntimeLibrary::ReadErrorAt479FE0()
{ return ReadGraphicsErrorAt479FE0(*CaptureAt4182F0()); }
bool NativeGraphicsRuntimeLibrary::ModuleLoaded() const
{ return root_.CaptureAt4182F0()->moduleAt00.baseAt00!=0; }

void NativeGraphicsRuntimeLibrary::RenderScreenIfLoaded() { present_.RenderScreenIfLoaded(); }
SIZE NativeGraphicsRuntimeLibrary::ReadScreenSizeAt47CC00() { return screen_.ReadScreenSizeAt47CC00(); }
const int* NativeGraphicsRuntimeLibrary::ResizeScreenIfLoaded(int w,int h,int mode)
{ return screen_.ResizeScreenIfLoaded(w,h,mode); }
void NativeGraphicsRuntimeLibrary::ResetRendererIfLoaded() { screen_.ResetRendererIfLoaded(); }
void NativeGraphicsRuntimeLibrary::ClearScreenIfLoaded(const RECT* rect) { screen_.ClearScreenIfLoaded(rect); }
void NativeGraphicsRuntimeLibrary::InvalidatedRectIfLoaded(const RECT* rect) { screen_.InvalidatedRectIfLoaded(rect); }
void NativeGraphicsRuntimeLibrary::ClearDirtyRectIfLoaded() { screen_.ClearDirtyRectIfLoaded(); }
bool NativeGraphicsRuntimeLibrary::CreateTextFont(const GraphicsTextFontParameters& p) { return text_.CreateTextFont(p); }
bool NativeGraphicsRuntimeLibrary::InitializeText(const char* p,int x,int y) { return text_.InitializeText(p,x,y); }
RECT NativeGraphicsRuntimeLibrary::ReadTextDrawRect() { return text_.ReadTextDrawRect(); }
int NativeGraphicsRuntimeLibrary::GetTextLength() { return text_.GetTextLength(); }
void NativeGraphicsRuntimeLibrary::SetTextBasePosition(int x,int y) { text_.SetTextBasePosition(x,y); }
bool NativeGraphicsRuntimeLibrary::BlitAllTextToSurface(int target,RECT* rect) { return text_.BlitAllTextToSurface(target,rect); }
bool NativeGraphicsRuntimeLibrary::BlitTextGlyphToSurface(int target,std::uint32_t index,RECT* rect)
{ return text_.BlitTextGlyphToSurface(target,index,rect); }
int NativeGraphicsRuntimeLibrary::CreateSurface(int w,int h,int alpha,int clear) { return surfaces_.CreateSurface(w,h,alpha,clear); }
void NativeGraphicsRuntimeLibrary::ReleaseSurface(int handle) { surfaces_.ReleaseSurface(handle); }
bool NativeGraphicsRuntimeLibrary::LoadBitmap(int handle,const void* file,const void* info,const void* pixels,std::uint32_t flags)
{ return surfaces_.LoadBitmap(handle,file,info,pixels,flags); }
bool NativeGraphicsRuntimeLibrary::LoadAlp(int handle,std::uint8_t mode,int w,int h,const void* pixels)
{ return surfaces_.LoadAlp(handle,mode,w,h,pixels); }
void NativeGraphicsRuntimeLibrary::LoadAnimationPixels(int handle,int w,int h,int channels,const void* pixels)
{ surfaces_.LoadAnimationPixels(handle,w,h,channels,pixels); }
std::byte* NativeGraphicsRuntimeLibrary::GetSurfacePointerRaw(int handle) { return surfaces_.GetSurfacePointerRaw(handle); }
int NativeGraphicsRuntimeLibrary::GetSurfacePitchRaw(int handle) { return surfaces_.GetSurfacePitchRaw(handle); }
std::uint32_t NativeGraphicsRuntimeLibrary::GetSurfacePixel(int handle,int x,int y) { return gui_.GetSurfacePixel(handle,x,y); }
void NativeGraphicsRuntimeLibrary::SurfaceClearRaw(int handle,std::uint32_t color,const RECT* clip)
{ surfaces_.SurfaceClearRaw(handle,color,clip); }
int NativeGraphicsRuntimeLibrary::SurfaceDrawRaw(int src,int x,int y,const SIZE* size,const RECT* rect,const RECT* clip,int anchor)
{ return surfaces_.SurfaceDrawRaw(src,x,y,size,rect,clip,anchor); }
int NativeGraphicsRuntimeLibrary::SurfaceBlitRaw(int dst,int src,int x,int y,const SIZE* size,const RECT* rect,const RECT* clip,int anchor)
{ return surfaces_.SurfaceBlitRaw(dst,src,x,y,size,rect,clip,anchor); }
void NativeGraphicsRuntimeLibrary::ClearSurface(int handle,std::uint32_t color,const RECT* rect) { gui_.ClearSurface(handle,color,rect); }
bool NativeGraphicsRuntimeLibrary::DrawSurface(int src,int x,int y,const SIZE* size,const RECT* rect,const RECT* clip,int anchor)
{ return gui_.DrawSurface(src,x,y,size,rect,clip,anchor); }
bool NativeGraphicsRuntimeLibrary::BlitSurface(int dst,int src,int x,int y,const SIZE* size,const RECT* rect,const RECT* clip,int anchor)
{ return gui_.BlitSurface(dst,src,x,y,size,rect,clip,anchor); }
bool NativeGraphicsRuntimeLibrary::CopyBlitSurface(int dst,int src,int x,int y,const SIZE* size,const RECT* rect,const RECT* clip,int anchor)
{ return transit_.TransitCopyBlit(dst,src,x,y,size,rect,clip,anchor)!=0; }
std::uint8_t NativeGraphicsRuntimeLibrary::TransitCopyBlit(int dst,int src,int x,int y,const SIZE* size,const RECT* rect,const RECT* clip,int anchor)
{ return transit_.TransitCopyBlit(dst,src,x,y,size,rect,clip,anchor); }
std::uint8_t NativeGraphicsRuntimeLibrary::TransitCopyDraw(int src,int x,int y,const SIZE* size,const RECT* rect,const RECT* clip,int anchor)
{ return transit_.TransitCopyDraw(src,x,y,size,rect,clip,anchor); }
std::uint8_t NativeGraphicsRuntimeLibrary::TransitBlendBlit(int dst,int src,int x,int y,int alpha,const SIZE* size,const RECT* rect,const RECT* clip,int anchor)
{ return transit_.TransitBlendBlit(dst,src,x,y,alpha,size,rect,clip,anchor); }
std::uint8_t NativeGraphicsRuntimeLibrary::TransitBlendDraw(int src,int x,int y,int alpha,const SIZE* size,const RECT* rect,const RECT* clip,int anchor)
{ return transit_.TransitBlendDraw(src,x,y,alpha,size,rect,clip,anchor); }
std::uint8_t NativeGraphicsRuntimeLibrary::TransitBlit(int dst,int src,int x,int y,const SIZE* size,const RECT* rect,const RECT* clip,int anchor)
{ return transit_.TransitBlit(dst,src,x,y,size,rect,clip,anchor); }
std::uint8_t NativeGraphicsRuntimeLibrary::TransitDraw(int src,int x,int y,const SIZE* size,const RECT* rect,const RECT* clip,int anchor)
{ return transit_.TransitDraw(src,x,y,size,rect,clip,anchor); }
void NativeGraphicsRuntimeLibrary::TransitUniversalBlit(int dst,int mask,const std::int32_t* values,int x,int y,const SIZE* size,const RECT* rect,const RECT* clip,int anchor)
{ transit_.TransitUniversalBlit(dst,mask,values,x,y,size,rect,clip,anchor); }
void NativeGraphicsRuntimeLibrary::TransitMosaic(int dst,std::uint32_t size,const RECT* clip) { transit_.TransitMosaic(dst,size,clip); }
void NativeGraphicsRuntimeLibrary::CopyDrawSurfaceIfLoaded(int src,int x,int y,const RECT* rect)
{ (void)transit_.TransitCopyDraw(src,x,y,nullptr,rect,nullptr,0); }
void NativeGraphicsRuntimeLibrary::BlendDrawSurfaceIfLoaded(int src,int x,int y,int alpha)
{ (void)transit_.TransitBlendDraw(src,x,y,alpha,nullptr,nullptr,nullptr,0); }
void NativeGraphicsRuntimeLibrary::UniversalBlitSurfaceIfLoaded(int dst,int mask,const std::int32_t* values)
{ transit_.TransitUniversalBlit(dst,mask,values,0,0,nullptr,nullptr,nullptr,0); }
void NativeGraphicsRuntimeLibrary::SurfaceFilterRaw(int h,std::uint32_t c,const RECT* r) { filters_.SurfaceFilterIfLoaded(h,c,r); }
void NativeGraphicsRuntimeLibrary::SurfaceFilter2Raw(int h,std::uint32_t c,const RECT* r) { filters_.SurfaceFilter2IfLoaded(h,c,r); }
void NativeGraphicsRuntimeLibrary::SurfaceFlushRaw(int h,const RECT* r) { filters_.SurfaceFlushIfLoaded(h,r); }
void NativeGraphicsRuntimeLibrary::SurfaceBlurRaw(int d,int s,int n,const RECT* r) { filters_.SurfaceBlurIfLoaded(d,s,n,r); }
void NativeGraphicsRuntimeLibrary::SurfaceGaussBlurRaw(int d,int s,int n,const RECT* r) { filters_.SurfaceGaussBlurIfLoaded(d,s,n,r); }
void NativeGraphicsRuntimeLibrary::SurfaceAddColorRaw(int h,std::uint32_t c,const RECT* r) { filters_.SurfaceAddColorIfLoaded(h,c,r); }
void NativeGraphicsRuntimeLibrary::SurfaceSubColorRaw(int h,std::uint32_t c,const RECT* r) { filters_.SurfaceSubColorIfLoaded(h,c,r); }
void NativeGraphicsRuntimeLibrary::SurfaceCopyDrawRaw(int h,int x,int y,const SIZE* s,const RECT* r,const RECT* c,int a)
{ filters_.SurfaceCopyDrawIfLoaded(h,x,y,s,r,c,a); }
void NativeGraphicsRuntimeLibrary::SurfaceCopyBlitRaw(int d,int h,int x,int y,const SIZE* s,const RECT* r,const RECT* c,int a)
{ filters_.SurfaceCopyBlitRaw(d,h,x,y,s,r,c,a); }
void NativeGraphicsRuntimeLibrary::SurfaceMulAlphaRaw(int h,std::uint32_t c,const RECT* r) { filters_.SurfaceMulAlphaIfLoaded(h,c,r); }
void NativeGraphicsRuntimeLibrary::SurfaceAddSubSurfaceRaw(int h,std::uintptr_t s,const RECT* r) { filters_.SurfaceAddSubSurfaceIfLoaded(h,s,r); }
void NativeGraphicsRuntimeLibrary::SurfaceMosaicRaw(int h,int n,const RECT* r) { filters_.SurfaceMosaicIfLoaded(h,n,r); }
void NativeGraphicsRuntimeLibrary::SurfaceMulColorRaw(int h,std::uint32_t c,const RECT* r) { filters_.SurfaceMulColorIfLoaded(h,c,r); }
void NativeGraphicsRuntimeLibrary::SurfaceFlipRaw(int h,int n,const RECT* r) { filters_.SurfaceFlipIfLoaded(h,n,r); }
}
