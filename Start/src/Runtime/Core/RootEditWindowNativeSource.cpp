#include <Runtime/RootEditWindowNativeSource.h>
#include <Runtime/RootRuntimeOwner.h>

namespace KaGuYa::Start::Runtime
{
HWND EnvironmentEditWindowNativeParentSource::CaptureWindowAt4180C0()
{return reinterpret_cast<HWND>(environment_.CaptureAt4180C0()->windowAt2C);}
EditWindowNativeBody98h* RootEditWindowNativeSource::CreateAt46FA30(RootRuntimePayload& root,DWORD disable,DWORD alignment,std::uintptr_t border)
{
    root.BindEditWindowNativeServices(maps_,thunks_);
    EditWindowNativeScope scope(maps_,thunks_);
    return creation_.CreateAt46FA30(root.EditWindowsAt54(),disable,alignment,border);
}
YtlReferenceSlotRuntime* RootEditWindowNativeSource::FindAt479DF0(RootRuntimePayload& root,YtlReferenceSlotRuntime* out,const void* target)
{return FindEditWindowNativeAt479DF0(root.EditWindowsAt54(),out,target);}
void RootEditWindowNativeSource::RemoveAt486B60(RootRuntimePayload& root,const void* target)
{
    EditWindowNativeScope scope(maps_,thunks_);
    RemoveEditWindowNativeAt486B60(root.EditWindowsAt54(),target);
}
}
