#pragma once
#include <Runtime/MainFrameNativeDispatch.h>
#include <Runtime/GraphicsScreenNativeApi.h>
#include <Runtime/NativeSoundRuntimeLibrary.h>

namespace KaGuYa::Start::Runtime
{
class IMainFrameNativeModules
{
public:
    virtual ~IMainFrameNativeModules()=default;
    virtual void ActivateSoundAt42103E(WPARAM)=0;
    virtual DWORD CheckGraphicsDeviceAt42426B()=0;
    virtual void ResetGraphicsAt432B40()=0;
};
class MainFrameNativeModules final:public IMainFrameNativeModules
{
public:
    MainFrameNativeModules(NativeSoundRuntimeLibrary&,GraphicsRootOwner&,IRuntimeStringManagerSource&,
        IGraphicsModuleNativeWin32Api&)noexcept;
    void ActivateSoundAt42103E(WPARAM)override;
    DWORD CheckGraphicsDeviceAt42426B()override;
    void ResetGraphicsAt432B40()override;
private:
    NativeSoundRuntimeLibrary& sound_;GraphicsRootOwner& graphics_;IRuntimeStringManagerSource& strings_;
    IGraphicsModuleNativeWin32Api& windows_;GraphicsScreenNativeApi screen_;
};
LRESULT ActivateMainFrameNativeAt420F90(MainFrameNativeBody90h&,UINT,WPARAM,LPARAM,
    IMainFrameNativeRootSource&,IMainFrameNativeWindowApi&,IMainFrameNativeModules&);
LRESULT DisplayChangeMainFrameNativeAt424230(MainFrameNativeBody90h&,UINT,WPARAM,LPARAM,
    IMainFrameNativeRootSource&,IMainFrameNativeWindowApi&,IMainFrameNativeModules&);
}
