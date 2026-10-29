#pragma once
#include <Runtime/EditWindowNativeCreation.h>

namespace KaGuYa::Start::Runtime
{
class RootRuntimePayload;
class EnvironmentEditWindowNativeParentSource final:public IEditWindowNativeParentSource
{
public:
    explicit EnvironmentEditWindowNativeParentSource(EnvironmentRootOwner& env) noexcept:environment_(env){}
    HWND CaptureWindowAt4180C0() override;
private:
    EnvironmentRootOwner& environment_;
};
// 固定caller已经选择的opaque根，不额外Ensure/捕获，不按原21Ch偏移读取现代payload。
class RootEditWindowNativeSource final
{
public:
    RootEditWindowNativeSource(EditWindowNativeCreationRuntime& creation,EditWindowNativeMapServices& maps,
        WindowThunkNativeServices& thunks) noexcept:creation_(creation),maps_(maps),thunks_(thunks){}
    EditWindowNativeBody98h* CreateAt46FA30(RootRuntimePayload&,DWORD,DWORD,std::uintptr_t);
    YtlReferenceSlotRuntime* FindAt479DF0(RootRuntimePayload&,YtlReferenceSlotRuntime*,const void*);
    void RemoveAt486B60(RootRuntimePayload&,const void*);
    EditWindowNativeMapServices& MapServices() const noexcept{return maps_;}
    WindowThunkNativeServices& ThunkServices() const noexcept{return thunks_;}
private:
    EditWindowNativeCreationRuntime& creation_;
    EditWindowNativeMapServices& maps_;
    WindowThunkNativeServices& thunks_;
};
}
