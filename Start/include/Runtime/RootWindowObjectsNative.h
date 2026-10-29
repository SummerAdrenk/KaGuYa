#pragma once
#include <Runtime/GraphicsRootNative.h>

namespace KaGuYa::Start::Runtime
{
class RootRuntimePayload;
// caller已选定root；不额外Ensure/取根，普通4826A0资格允许前后Ref释放外传。
void ReleaseRootWindowObjectsNativeAt4826A0(RootRuntimePayload&);
// PAINTSTRUCT为原64字节按值参数；不是HDC/RECT指针接口，不将两个Graphics读取合并。
void PaintRootWindowNativeAt483B70(RootRuntimePayload&,PAINTSTRUCT,GraphicsRootOwner&,
    IRuntimeStringManagerSource&,IGraphicsModuleNativeWin32Api&);
}
