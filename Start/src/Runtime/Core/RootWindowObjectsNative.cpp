#include <Runtime/RootWindowObjectsNative.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/WindowModeNative.h>
#include <Runtime/FontManagerNative.h>

namespace KaGuYa::Start::Runtime
{
namespace{template<class T>T Read(const T& value)noexcept{return *static_cast<const volatile T*>(&value);}}
void ReleaseRootWindowObjectsNativeAt4826A0(RootRuntimePayload& root)
{
    ReleaseWindowModeReferenceAt47798B(root.WindowModeSlotAt0C());
    root.ReleaseDirectMovie();
    {
        // 48272C..746的范围清理沿452290终止边界，全部元素完成后才晚读begin写end。
        EditWindowNativeScope services(root.BorrowEditWindowNativeMapServices(),root.BorrowEditWindowNativeThunkServices());
        RemoveEditWindowNativeAt486B60(root.EditWindowsAt54(),nullptr);
    }
    ReleaseFontManagerReferenceAt482749(root.FontManagerSlotAt60());
}
void PaintRootWindowNativeAt483B70(RootRuntimePayload& root,PAINTSTRUCT paint,GraphicsRootOwner& graphics,
    IRuntimeStringManagerSource& strings,IGraphicsModuleNativeWin32Api& windows)
{
    static_assert(sizeof(PAINTSTRUCT)==0x40);
    if(!Read(graphics.CaptureAt4182F0()->moduleAt00.baseAt00))return;
    if(!Read(root.Frame().Lifecycle().Native().ByteAt1AC()))return;
    if(root.MovieBasePresentAt48())return;
    auto* current=graphics.CaptureAt4182F0();if(!Read(current->moduleAt00.baseAt00))return;
    auto* selected=ResolveGraphicsModuleReference(current->moduleAt00);
    using Function=void(__cdecl*)(PAINTSTRUCT);
    const auto function=reinterpret_cast<Function>(ResolveGraphicsModuleExportWithCString(
        selected,"RenderSceenHDC",14,strings,windows));
    function(paint);
}
}
