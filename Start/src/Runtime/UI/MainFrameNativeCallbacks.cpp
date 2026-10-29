#include <Runtime/MainFrameNativeCallbacks.h>
#include <Runtime/RootFrameRuntime.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/RootLegacyDisplaySettingsRuntime.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
template<class T>T At(void* object,DWORD offset)noexcept
{return *reinterpret_cast<const volatile T*>(reinterpret_cast<DWORD>(object)+offset);}
class Api final:public IMainFrameNativeCallbackApi
{
public:
    int TranslateAcceleratorA(HWND window,HACCEL accelerator,MSG* message)override
    {return ::TranslateAcceleratorA(window,accelerator,message);}
    BOOL PostMessageA(HWND window,UINT message,WPARAM w,LPARAM l)override
    {return ::PostMessageA(window,message,w,l);}
};
constinit Api api;
}
IMainFrameNativeCallbackApi& DefaultMainFrameNativeCallbackApi()noexcept{return api;}
RootMainFrameNativeIdleSource::RootMainFrameNativeIdleSource(RootRuntimeOwner& root,RootFrameRuntime& frame)noexcept
    :root_(root),frame_(frame){}
void RootMainFrameNativeIdleSource::EnsureRootAt413BE0(){root_.EnsureRootAt413BE0();}
void* RootMainFrameNativeIdleSource::RootAt6198C4(){return root_.RootAt6198C4();}
DWORD RootMainFrameNativeIdleSource::ExitFlagAt1BC(void* selected){return root_.ExitFlagAt1BC(selected);}
DWORD RootMainFrameNativeIdleSource::RunFrameAt4834F0(void* selected)
{return frame_.TickFrameAt4834F0(*static_cast<RootRuntimePayload*>(selected));}
BOOL TranslateMainFrameNativeAt42D1A0(void* filter,MSG* message,IMainFrameNativeCallbackApi& windows)
{
    if (auto* legacy=BoundRootLegacyDisplaySettings(); legacy && legacy->PreTranslate(*message)) return TRUE;
    const auto accelerator=At<HACCEL>(filter,0xFFFFFFFCu);if(!accelerator)return FALSE;
    return windows.TranslateAcceleratorA(At<HWND>(filter,0xFFFFFFD0u),accelerator,message)?TRUE:FALSE;
}
BOOL IdleMainFrameNativeAt424E10(void* idle,IMainFrameNativeIdleSource& source,IMainFrameNativeCallbackApi& windows)
{
    // 01:40B580；关闭设置窗的这一帧仍跳过普通Root tick。
    if (auto* legacy=BoundRootLegacyDisplaySettings(); legacy && legacy->Poll()) return FALSE;
    source.EnsureRootAt413BE0();auto* selected=source.RootAt6198C4();
    if(!static_cast<BYTE>(source.RunFrameAt4834F0(selected)))
    {
        source.EnsureRootAt413BE0();selected=source.RootAt6198C4();
        if(!source.ExitFlagAt1BC(selected))
            static_cast<void>(windows.PostMessageA(At<HWND>(idle,0xFFFFFFCCu),WM_CLOSE,0,0));
    }
    return FALSE;
}
LRESULT DispatchMainFrameNativeMessageAt43A080(EditWindowEndpointNativeBody24h& endpoint,UINT message,WPARAM w,LPARAM l,
    IEditWindowNativeMessageApi& windows)
{return DispatchEditWindowNativeMessageAt439F60(endpoint,message,w,l,windows);}
}
