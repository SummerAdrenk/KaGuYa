#include <Runtime/RootSystemInformationDisplayNativeSource.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/NativeGraphicsRuntimeLibrary.h>
#include <Runtime/GraphicsSurfaceCaptureRuntime.h>

namespace KaGuYa::Start::Runtime
{
void RootSystemInformationDisplayNativeSource::EnsureRootAt413BE0(){dependencies_.root.EnsureCreatedAt413BE0();}
void* RootSystemInformationDisplayNativeSource::RootAt6198C4(){return dependencies_.root.ReadPublishedAt6198C4();}
YtlReferenceSlotRuntime& RootSystemInformationDisplayNativeSource::RootWindowModeSlotAt0C(void* root)
{return static_cast<RootRuntimePayload*>(root)->WindowModeSlotAt0C();}
YtlReferenceSlotRuntime& RootSystemInformationDisplayNativeSource::RootDrawSlotAt18(void* root)
{return static_cast<RootRuntimePayload*>(root)->DrawManagerSlotAt18();}
YtlReferenceSlotRuntime& RootSystemInformationDisplayNativeSource::RootSceneManagerSlotAt30(void* root)
{return static_cast<RootRuntimePayload*>(root)->SceneManagerSlotAt30();}
std::int32_t& RootSystemInformationDisplayNativeSource::RootModalFlagAt1B8(void* root)
{return reinterpret_cast<std::int32_t&>(static_cast<RootRuntimePayload*>(root)->Frame().Lifecycle().valueAt1B8);}
std::uint8_t RootSystemInformationDisplayNativeSource::RootMenuStateAt219(void* root)
{return static_cast<RootRuntimePayload*>(root)->ReadByteAt219();}
RECT* RootSystemInformationDisplayNativeSource::LogicalRectangleAt478E50(void*,RECT* out)
{
    // 478E7A只取显式RECT*，ECX在478E7D已改写；保留Query内部各次环境/根捕获。
    return &dependencies_.rectangle.QueryAt478E50(*out);
}
YtlReferenceSlotRuntime* RootSystemInformationDisplayNativeSource::CaptureBackgroundAt4ACDD0(void*,YtlReferenceSlotRuntime* out)
{
    // 原4ACDD0不消费caller的Draw ECX；不得为已固定Draw再追加Ensure。
    CaptureGraphicsScreenAt4ACDD0(*out,dependencies_.graphics,dependencies_.graphics,dependencies_.graphics);return out;
}
void* RootSystemInformationDisplayNativeSource::SystemAt4180C0(){return dependencies_.environment.CaptureAt4180C0();}
HWND RootSystemInformationDisplayNativeSource::SystemWindowAt2C(void* system)
{return reinterpret_cast<HWND>(static_cast<EnvironmentRootStorage*>(system)->windowAt2C);}
void RootSystemInformationDisplayNativeSource::DrawBackgroundAt52687F(std::int32_t handle,const RECT* clip)
{
    // 52687F..526928与原SurfaceDraw叶同义：一次4182F0/base门控，固定模块，11字节名称，七参。
    (void)dependencies_.graphics.SurfaceDrawRaw(handle,0,0,nullptr,nullptr,clip,0);
}
BOOL RootSystemInformationDisplayNativeSource::IsIconic(HWND window){return dependencies_.iconic(window);}
BOOL RootSystemInformationDisplayNativeSource::PostMessageA(HWND window,UINT message,WPARAM w,LPARAM l)
{return dependencies_.postMessage(window,message,w,l);}
GuiDialogNativeBody5Ch* RootSystemInformationDisplayNativeSource::ConstructSystemDialogAt4F41A0(void* raw)
{return KaGuYa::Start::Runtime::ConstructSystemDialogAt4F41A0(raw);}
std::uint32_t RootSystemInformationDisplayNativeSource::ReadRootModalGateAfter413BE0()
{
    EnsureRootAt413BE0();
    return static_cast<RootRuntimePayload*>(RootAt6198C4())->Frame().Lifecycle().valueAt1B8;
}
std::int32_t RootSystemInformationDisplayNativeSource::TriggerSystemDialogAt5104A0(GuiDialogNativeBody5Ch* body)
{return KaGuYa::Start::Runtime::TriggerSystemDialogAt5104A0(body);}
}
