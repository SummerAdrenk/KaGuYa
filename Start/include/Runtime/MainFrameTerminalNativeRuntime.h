#pragma once
#include <Runtime/MainFrameCloseNative.h>
#include <Runtime/MainFrameDisplayNative.h>
#include <Runtime/MainFrameUiNative.h>

namespace KaGuYa::Start::Runtime
{
// 独立命令/设备/Chevron业务必须由caller提供完整实现，不以空返回伪装未接分支。
class IMainFrameNativeCommandHandlers
{
public:
    virtual ~IMainFrameNativeCommandHandlers()=default;
    virtual LRESULT DeviceChangeAt424070(MainFrameNativeBody90h&,UINT,WPARAM,LPARAM,BOOL*)=0;
    virtual LRESULT CommandAt428C10(MainFrameNativeBody90h&,DWORD,DWORD,LPARAM,BOOL*)=0;
    virtual LRESULT ChevronAt423550(MainFrameNativeBody90h&,WPARAM,LPARAM,BOOL*)=0;
};
// 全部依赖须属于同一Process；只冷借用，不构造正文、取根或预建GUI。
class MainFrameTerminalNativeRuntime final:public IMainFrameNativeTerminalHandlers
{
public:
    MainFrameTerminalNativeRuntime(IMainFrameNativeRootSource&,IMainFrameNativeWindowApi&,IMainFrameNativeModules&,
        IMainFrameCloseNativeDialogs&,MainFrameUiNativeServices,IMainFrameNativeCommandHandlers&)noexcept;
    LRESULT CloseAt423750(MainFrameNativeBody90h&,UINT,WPARAM,LPARAM,BOOL*)override;
    LRESULT DisplayChangeAt424230(MainFrameNativeBody90h&,UINT,WPARAM,LPARAM,BOOL*)override;
    LRESULT ActivateAt420F90(MainFrameNativeBody90h&,UINT,WPARAM,LPARAM,BOOL*)override;
    LRESULT DeviceChangeAt424070(MainFrameNativeBody90h&,UINT,WPARAM,LPARAM,BOOL*)override;
    LRESULT CommandAt428C10(MainFrameNativeBody90h&,DWORD,DWORD,LPARAM,BOOL*)override;
    LRESULT UpdateMenuAt428430(UpdateUiNativeBody18h&,UINT,HMENU,LPARAM,BOOL*)override;
    LRESULT NotifyAt423550(MainFrameNativeBody90h&,WPARAM,LPARAM,BOOL*)override;
    void UpdateUiAt439100(MainFrameNativeBody90h&,BOOL)override;
    BOOL FrameBaseAt42DC50(MainFrameNativeBody90h&,HWND,UINT,WPARAM,LPARAM,LRESULT*,DWORD)override;
private:
    IMainFrameNativeRootSource& roots_;IMainFrameNativeWindowApi& windows_;IMainFrameNativeModules& modules_;
    IMainFrameCloseNativeDialogs& dialogs_;MainFrameUiNativeServices ui_;IMainFrameNativeCommandHandlers& commands_;
};
}
