#include <Runtime/RootSettingPageNativeSource.h>
#include <Runtime/RootRuntimeOwner.h>

namespace KaGuYa::Start::Runtime
{
RootSettingPageNativeSource::RootSettingPageNativeSource(RootRuntimeOwner& root,
    EnvironmentRootOwner& environment, decltype(&::PostMessageA) postMessage) noexcept
    : root_(root), environment_(environment), postMessage_(postMessage) {}
EnvironmentRootStorage* RootSettingPageNativeSource::CaptureEnvironmentAt4180C0()
{ return environment_.CaptureAt4180C0(); }
EnvironmentRootStorage* RootSettingPageNativeSource::CaptureEnvironmentInlineAt6198C8()
{
    // 513D20..513E23：同一独立锁/惰性60h根/GetNil/发布/解锁序列。
    // 复用已恢复的owner核，不能绕锁返回Published或捕获另一份Environment。
    return environment_.CaptureAt4180C0();
}
void RootSettingPageNativeSource::EnsureRootAt413BE0()
{ root_.EnsureCreatedAt413BE0(); }
void* RootSettingPageNativeSource::RootAt6198C4()
{ return root_.ReadPublishedAt6198C4(); }
YtlReferenceSlotRuntime& RootSettingPageNativeSource::RootWindowModeSlotAt0C(void* fixedRoot)
{
    // 固定caller已经选出的根；这里不得再次Ensure，也不按现代对象的+0C裸读。
    return static_cast<RootRuntimePayload*>(fixedRoot)->WindowModeSlotAt0C();
}
BOOL RootSettingPageNativeSource::PostMessageA(HWND window, UINT message, WPARAM w, LPARAM l)
{ return postMessage_(window, message, w, l); }
YtlReferenceSlotRuntime& RootSettingPageNativeSource::RootSoundSlotAt24(void* fixedRoot)
{ return static_cast<RootRuntimePayload*>(fixedRoot)->SoundManagerSlotAt24(); }
}
