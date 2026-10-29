#include <Runtime/RootSaveLoadDialogNativeSource.h>
#include <Runtime/GraphicsSurfaceCaptureRuntime.h>
#include <Runtime/NativeGraphicsRuntimeLibrary.h>
#include <Runtime/NativeInputRuntime.h>

namespace KaGuYa::Start::Runtime
{
YtlReferenceSlotRuntime* RootSaveLoadDialogNativeSource::CaptureBackgroundAfter413BE0(YtlReferenceSlotRuntime* fresh)
{
    // 50E8DA..50E90F先Ensure/读Root/解Draw，再将未写的栈Ref交4ACDD0。
    // callee不消费Draw ECX；不得省掉该捕获，也不能先清槽或用407A80替代451500。
    static_cast<void>(CaptureDrawAfter413BE0());
    CaptureGraphicsScreenAt4ACDD0(*fresh, dependencies_.graphics, dependencies_.graphics, dependencies_.graphics);
    return fresh;
}

InputRootNativeBody24h* RootSaveLoadDialogNativeSource::CaptureInputAt418210()
{ return dependencies_.input.CaptureAt418210(); }
}
