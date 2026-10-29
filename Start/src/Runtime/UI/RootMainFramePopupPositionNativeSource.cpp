#include <Runtime/MainFramePopupPositionNative.h>
#include <Runtime/RootRuntimeOwner.h>

namespace KaGuYa::Start::Runtime
{
RootMainFramePopupPositionNativeSource::RootMainFramePopupPositionNativeSource(RootRuntimeOwner& root,
    ParamsRuntimeRootOwner& params,EnvironmentRootOwner& environment)noexcept:root_(root),params_(params),environment_(environment){}
void RootMainFramePopupPositionNativeSource::EnsureRootAt413BE0(){root_.EnsureCreatedAt413BE0();}
void* RootMainFramePopupPositionNativeSource::RootAt6198C4(){return root_.ReadPublishedAt6198C4();}
RootMenuPositionNativeState0Ch& RootMainFramePopupPositionNativeSource::PositionAt208(void* root)
{
    if(!root)return *reinterpret_cast<RootMenuPositionNativeState0Ch*>(0x208u);
    return static_cast<RootRuntimePayload*>(root)->MenuPositionAt208();
}
const YtlReferenceSlotRuntime& RootMainFramePopupPositionNativeSource::ModeAt0C(void* root)
{
    if(!root)return *reinterpret_cast<const YtlReferenceSlotRuntime*>(0x0Cu);
    return static_cast<RootRuntimePayload*>(root)->WindowModeSlotAt0C();
}
void* RootMainFramePopupPositionNativeSource::GameSystemAt418480()
{
    auto* params=params_.CaptureAt418480();const volatile auto& ref=params->gameSystemAt04.Native();
    const auto base=ref.baseAt00;if(!base)return nullptr;auto* control=ref.controlAt08;
    const auto stride=control?*reinterpret_cast<const volatile DWORD*>(reinterpret_cast<DWORD>(control)+0x0Cu):0u;
    return reinterpret_cast<void*>(base+ref.biasAt04*stride);
}
EnvironmentRootStorage* RootMainFramePopupPositionNativeSource::EnvironmentAt4180C0(){return environment_.CaptureAt4180C0();}
}
