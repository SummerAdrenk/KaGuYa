#pragma once
#include <Runtime/DemoSequenceNative.h>
#include <Runtime/DemoSoundCommandServices.h>

namespace KaGuYa::Start::Runtime
{
class RootRuntimeOwner;
class IGuiControlResourceLoadServices;
class ISpriteNativeResourceSource;
class RootDemoSequenceNativeSource final : public IDemoSequenceNativeSource
{
public:
    RootDemoSequenceNativeSource(EnvironmentRootOwner&,RootRuntimeOwner&,
        IGuiControlResourceLoadServices&,ISpriteNativeResourceSource&,IDemoSoundCommandServices&,
        IGraphicsSurfaceApi&,IRuntimeStringManagerSource&) noexcept;
    EnvironmentRootStorage* CaptureEnvironmentAt4180C0() override;
    DrawManagerNativeBody2Ch* CaptureDrawAt413BE0() override;
    bool LoadMaskAt480000(void*,YtlReferenceSlotRuntime&,const char*) override;
    bool LoadSpriteAt4814F0(void*,SpriteNativeBodyA0h*,const char*,std::uint8_t) override;
    void ExecuteSoundAt4BF692(const ParamsDemoCmdSound&) override;
private:
    EnvironmentRootOwner& environment_;
    RootRuntimeOwner& root_;
    IGuiControlResourceLoadServices& pictures_;
    ISpriteNativeResourceSource& sprites_;
    IDemoSoundCommandServices& sounds_;
    IGraphicsSurfaceApi& surfaces_;
    IRuntimeStringManagerSource& strings_;
};
}
