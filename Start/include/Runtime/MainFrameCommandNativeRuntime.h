#pragma once
#include <Runtime/MainFrameTerminalNativeRuntime.h>
#include <Runtime/MainFrameSoundDeviceNative.h>

namespace KaGuYa::Start::Runtime
{
class IMainFrameNativePopupHandlers
{
public:
    virtual ~IMainFrameNativePopupHandlers()=default;
    virtual LRESULT ContextMenuAt428C10(MainFrameNativeBody90h&,DWORD,DWORD,LPARAM,BOOL*)=0;
    virtual LRESULT ChevronAt423550(MainFrameNativeBody90h&,WPARAM,LPARAM,BOOL*)=0;
};
// 设备外层和重建拥有同一声音服务；剩余两弹出业务显式依赖，不给空处理。
class MainFrameCommandNativeRuntime final:public IMainFrameNativeCommandHandlers
{
public:
    MainFrameCommandNativeRuntime(IMainFrameSoundDeviceNativeSource&,IMainFrameNativeWindowApi&,
        ISoundDeviceRebuildNative&,IMainFrameNativePopupHandlers&)noexcept;
    LRESULT DeviceChangeAt424070(MainFrameNativeBody90h&,UINT,WPARAM,LPARAM,BOOL*)override;
    LRESULT CommandAt428C10(MainFrameNativeBody90h&,DWORD,DWORD,LPARAM,BOOL*)override;
    LRESULT ChevronAt423550(MainFrameNativeBody90h&,WPARAM,LPARAM,BOOL*)override;
private:
    IMainFrameSoundDeviceNativeSource& source_;IMainFrameNativeWindowApi& windows_;
    ISoundDeviceRebuildNative& rebuilder_;IMainFrameNativePopupHandlers& popup_;
};
}
