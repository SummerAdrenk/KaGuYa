#include <Runtime/MainFrameCommandNativeRuntime.h>

namespace KaGuYa::Start::Runtime
{
MainFrameCommandNativeRuntime::MainFrameCommandNativeRuntime(IMainFrameSoundDeviceNativeSource& source,
    IMainFrameNativeWindowApi& windows,ISoundDeviceRebuildNative& rebuilder,IMainFrameNativePopupHandlers& popup)noexcept
    :source_(source),windows_(windows),rebuilder_(rebuilder),popup_(popup){}
LRESULT MainFrameCommandNativeRuntime::DeviceChangeAt424070(MainFrameNativeBody90h& frame,UINT message,WPARAM w,LPARAM l,BOOL*)
{return DeviceChangeMainFrameNativeAt424070(frame,message,w,l,source_,windows_,rebuilder_);}
LRESULT MainFrameCommandNativeRuntime::CommandAt428C10(MainFrameNativeBody90h& frame,DWORD code,DWORD id,LPARAM l,BOOL* handled)
{return popup_.ContextMenuAt428C10(frame,code,id,l,handled);}
LRESULT MainFrameCommandNativeRuntime::ChevronAt423550(MainFrameNativeBody90h& frame,WPARAM w,LPARAM l,BOOL* handled)
{return popup_.ChevronAt423550(frame,w,l,handled);}
}
