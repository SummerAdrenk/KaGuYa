#include <Runtime/MainFrameContextMenuNative.h>
#include <Runtime/RootRuntimeOwner.h>

namespace KaGuYa::Start::Runtime
{
RootMainFrameContextMenuNativeSource::RootMainFrameContextMenuNativeSource(RootRuntimeOwner& root,
    EnvironmentRootOwner& environment,InputRootOwner& input,WindowClassModuleNativeBody38h& nativeBase)noexcept
    :root_(root),environment_(environment),input_(input),nativeBase_(nativeBase){}
HINSTANCE RootMainFrameContextMenuNativeSource::ResourceInstanceAt61A6A0()
{return *static_cast<const volatile HINSTANCE*>(&nativeBase_.resourceInstanceAt08);}
EnvironmentRootStorage* RootMainFrameContextMenuNativeSource::EnvironmentAt4180C0(){return environment_.CaptureAt4180C0();}
void RootMainFrameContextMenuNativeSource::EnsureRootAt413BE0(){root_.EnsureCreatedAt413BE0();}
void* RootMainFrameContextMenuNativeSource::RootAt6198C4(){return root_.ReadPublishedAt6198C4();}
const YtlReferenceSlotRuntime& RootMainFrameContextMenuNativeSource::ModeAt0C(void* root)
{
    if(!root)return *reinterpret_cast<const YtlReferenceSlotRuntime*>(0x0Cu);
    return static_cast<RootRuntimePayload*>(root)->WindowModeSlotAt0C();
}
const YtlReferenceSlotRuntime& RootMainFrameContextMenuNativeSource::DrawAt18(void* root)
{
    if(!root)return *reinterpret_cast<const YtlReferenceSlotRuntime*>(0x18u);
    return static_cast<RootRuntimePayload*>(root)->DrawManagerSlotAt18();
}
void RootMainFrameContextMenuNativeSource::WritePopupAt1B1(void* root,BYTE value)
{
    if(!root){*reinterpret_cast<volatile BYTE*>(0x1B1u)=value;return;}
    static_cast<RootRuntimePayload*>(root)->Frame().Lifecycle().Native().popupAt1B1=value;
}
InputRootNativeBody24h* RootMainFrameContextMenuNativeSource::InputAt418210(){return input_.CaptureAt418210();}
}
