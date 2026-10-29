#pragma once
#include <Runtime/ModalDialogServicesRuntime.h>
#include <Runtime/WindowInputUpdateRuntime.h>

namespace KaGuYa::Start::Runtime
{
class NativeInputRuntime;
class NativeGraphicsRuntimeLibrary;
// 同一Root的模态输入与5071D0服务；Win32叶与显式测试叶使用同一业务实现。
// 构造不取根/帧/窗口，不泵消息，也不创建第二个Input或Graphics状态。
class RootModalDialogNativeServicesRuntime final
{
public:
    RootModalDialogNativeServicesRuntime(RootRuntimeOwner&,EnvironmentStartupRuntime&,NativeInputRuntime&,
        NativeGraphicsRuntimeLibrary&)noexcept;
    RootModalDialogNativeServicesRuntime(RootRuntimeOwner&,EnvironmentStartupRuntime&,NativeInputRuntime&,
        NativeGraphicsRuntimeLibrary&,const Platform::IMousePositionApi&,IModalDialogMessageApi&)noexcept;
    RootModalDialogNativeServicesRuntime(const RootModalDialogNativeServicesRuntime&)=delete;
    RootModalDialogNativeServicesRuntime& operator=(const RootModalDialogNativeServicesRuntime&)=delete;
    ModalDialogServicesRuntime& Services()noexcept{return services_;}
    WindowInputUpdateRuntime& InputUpdate()noexcept{return input_;}
private:
    Platform::Win32MousePositionApi mouse_;
    Win32ModalDialogMessageApi messages_;
    WindowInputUpdateRuntime input_;
    ModalDialogServicesRuntime services_;
};
}
