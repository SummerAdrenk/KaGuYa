#include <Runtime/RootModalDialogNativeServicesRuntime.h>
#include <Runtime/NativeInputRuntime.h>
#include <Runtime/NativeGraphicsRuntimeLibrary.h>

namespace KaGuYa::Start::Runtime
{
RootModalDialogNativeServicesRuntime::RootModalDialogNativeServicesRuntime(RootRuntimeOwner& root,
    EnvironmentStartupRuntime& environment,NativeInputRuntime& input,NativeGraphicsRuntimeLibrary& graphics)noexcept
    :input_(input,environment,root,mouse_),services_(graphics.PresentCalls(),root,input_,messages_){}
RootModalDialogNativeServicesRuntime::RootModalDialogNativeServicesRuntime(RootRuntimeOwner& root,
    EnvironmentStartupRuntime& environment,NativeInputRuntime& input,NativeGraphicsRuntimeLibrary& graphics,
    const Platform::IMousePositionApi& mouse,IModalDialogMessageApi& messages)noexcept
    :input_(input,environment,root,mouse),services_(graphics.PresentCalls(),root,input_,messages){}
}
