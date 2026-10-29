#include <Runtime/RootDemoSequenceNativeSource.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/GuiControlResourceLoadRuntime.h>
#include <Runtime/DrawManagerResourceRuntime.h>

namespace KaGuYa::Start::Runtime
{
RootDemoSequenceNativeSource::RootDemoSequenceNativeSource(EnvironmentRootOwner& environment,
    RootRuntimeOwner& root,IGuiControlResourceLoadServices& pictures,ISpriteNativeResourceSource& sprites,
    IDemoSoundCommandServices& sounds,IGraphicsSurfaceApi& surfaces,IRuntimeStringManagerSource& strings) noexcept
    :environment_(environment),root_(root),pictures_(pictures),sprites_(sprites),sounds_(sounds),surfaces_(surfaces),strings_(strings){}
EnvironmentRootStorage* RootDemoSequenceNativeSource::CaptureEnvironmentAt4180C0()
{return environment_.CaptureAt4180C0();}
DrawManagerNativeBody2Ch* RootDemoSequenceNativeSource::CaptureDrawAt413BE0()
{root_.EnsureCreatedAt413BE0();return root_.Published()->PostWindow().DrawManagerAt18();}
bool RootDemoSequenceNativeSource::LoadMaskAt480000(void* draw,YtlReferenceSlotRuntime& output,const char* name)
{return LoadGuiControlSurfaceReferenceAt480000(output,draw,name,pictures_,surfaces_,strings_);}
bool RootDemoSequenceNativeSource::LoadSpriteAt4814F0(void* draw,SpriteNativeBodyA0h* output,
    const char* name,std::uint8_t restart)
{return sprites_.LoadAt4814F0(draw,output,name,restart);}
void RootDemoSequenceNativeSource::ExecuteSoundAt4BF692(const ParamsDemoCmdSound& command)
{sounds_.ExecuteDemoSoundAt4BF692(command);}
}
