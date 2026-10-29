#pragma once
#include <Runtime/RootEditWindowNativeSource.h>
#include <Runtime/EditWindowNativeViewport.h>

namespace KaGuYa::Start::Runtime
{
// 同进程38h/2Ch原模块由CRT持有，本集合仅拥有创建/Map/thunk/viewport服务和新鲜类信息。
// 构造不登记类/创建窗口/取根；全部98h/28h窗口、thunk和Root+54末Ref须先退役。
class RootEditWindowNativeServicesRuntime final
{
public:
    RootEditWindowNativeServicesRuntime(RootRuntimeOwner&,EnvironmentRootOwner&,
        IRuntimeStringManagerSource& strings=DefaultRuntimeStringManagerSource())noexcept;
    RootEditWindowNativeServicesRuntime(const RootEditWindowNativeServicesRuntime&)=delete;
    RootEditWindowNativeServicesRuntime& operator=(const RootEditWindowNativeServicesRuntime&)=delete;
    RootEditWindowNativeSource& Source()noexcept{return source_;}
    EditWindowNativeMapServices& Maps()noexcept{return maps_;}
    WindowThunkNativeServices& Thunks()noexcept{return thunks_;}
    EditWindowNativeViewportServices& Viewport()noexcept{return viewport_;}
    EditWindowNativeCreationRuntime& Creation()noexcept{return creation_;}
private:
    WindowClassNativeInfo68h editClass_{};
    WindowClassNativeInfo68h controlClass_{};
    RECT rectangle_{CW_USEDEFAULT,CW_USEDEFAULT,0,0};
    EnvironmentEditWindowNativeParentSource parent_;
    EditWindowNativeCreationRuntime creation_;
    EditWindowNativeMapServices maps_;
    WindowThunkNativeServices thunks_;
    RootEditWindowNativeSource source_;
    RootEditWindowNativeViewportSource modes_;
    EnvironmentEditWindowNativeViewportSource dimensions_;
    EditWindowNativeViewportServices viewport_;
};
}
