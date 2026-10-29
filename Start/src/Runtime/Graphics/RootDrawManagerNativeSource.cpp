#include <Runtime/DrawManagerNative.h>
#include <Runtime/RootRuntimeOwner.h>

namespace KaGuYa::Start::Runtime
{
DrawManagerNativeBody2Ch* RootDrawManagerNativeSource::CaptureDrawManagerAfter413BE0()
{
    root_.EnsureCreatedAt413BE0();
    // 当前根的唯一+18 Ref，不借现代PostWindow manager或缓存构造前的正文。
    return ResolveDrawManagerReferenceAt4175A0(root_.Published()->DrawManagerSlotAt18());
}
}
