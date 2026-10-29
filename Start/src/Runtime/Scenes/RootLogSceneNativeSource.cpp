#include <Runtime/RootLogSceneNativeSource.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/SceneManagerView.h>

namespace KaGuYa::Start::Runtime
{
GuiDialogNativeBody5Ch* RootLogSceneNativeSource::ConstructLogDialogAt4E23D0(void* raw)
{return dialog_.ConstructLogDialogAt4E23D0(raw);}
EnvironmentRootStorage* RootLogSceneNativeSource::CaptureEnvironmentAt4180C0(std::uint32_t)
{return environment_.CaptureAt4180C0();}
void RootLogSceneNativeSource::EnsureRootAt413BE0(std::uint32_t){root_.EnsureCreatedAt413BE0();}
void* RootLogSceneNativeSource::RootAt6198C4(std::uint32_t){return root_.ReadPublishedAt6198C4();}
std::uint8_t RootLogSceneNativeSource::RootByteAt218(void* p){return static_cast<RootRuntimePayload*>(p)->ReadByteAt218();}
std::uint8_t RootLogSceneNativeSource::RootByteAt219(void* p){return static_cast<RootRuntimePayload*>(p)->ReadByteAt219();}
void RootLogSceneNativeSource::WriteRootByteAt218(void* p,std::uint8_t value){static_cast<RootRuntimePayload*>(p)->WriteByteAt218(value);}
void RootLogSceneNativeSource::WriteRootByteAt219(void* p,std::uint8_t value){static_cast<RootRuntimePayload*>(p)->WriteByteAt219(value);}
YtlReferenceSlotRuntime& RootLogSceneNativeSource::RootSceneManagerSlotAt30(void* p)
{return static_cast<RootRuntimePayload*>(p)->SceneManagerSlotAt30();}
void RootLogSceneNativeSource::PopThenActivatePendingAt4C1760(void* p)
{
    // 固定原48h receiver；只借当前ManagerScope，不重新Ensure/查询根或保存旧代服务。
    SceneManagerView(static_cast<SceneManagerNativeBody48h*>(p),RequireSceneManagerNativeServices()).PopThenActivatePendingAt4C1760();
}
}
