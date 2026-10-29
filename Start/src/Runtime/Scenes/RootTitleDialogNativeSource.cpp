#include <Runtime/RootTitleDialogNativeSource.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/NativeGraphicsRuntimeLibrary.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/SystemInformationDisplayNative.h>
#include <Runtime/QuickOperationsNative.h>
#include <Runtime/GameResourceNamePrompt.h>
#include <Runtime/ProcessAtlBaseModuleNative.h>
#include <Runtime/ParamsRuntimeRoot.h>
#include <Logging/Logger.h>
#include "../../../games/baku-ane-otouto-shibocchau-zo/TitleData.h"

namespace KaGuYa::Start::Runtime
{
namespace
{
template<class T>T Read(const T& value) noexcept{return *static_cast<const volatile T*>(&value);}
const std::int32_t* Volume(void* receiver,unsigned offset)
{
    using Function=const std::int32_t* (__thiscall*)(void*);
    auto* table=*static_cast<const std::uintptr_t* const*>(receiver);
    return reinterpret_cast<Function>(table[offset/4])(receiver);
}
}
RootTitleDialogNativeSource::RootTitleDialogNativeSource(RootTitleDialogNativeDependencies d) noexcept:dependencies_(d){}
void RootTitleDialogNativeSource::EnsureRootAt413BE0(){dependencies_.root.EnsureCreatedAt413BE0();}
void* RootTitleDialogNativeSource::RootAt6198C4(){return dependencies_.root.Published();}
YtlReferenceSlotRuntime& RootTitleDialogNativeSource::RootDrawSlotAt18(void* root)
{return static_cast<RootRuntimePayload*>(root)->DrawManagerSlotAt18();}
YtlReferenceSlotRuntime& RootTitleDialogNativeSource::RootSoundSlotAt24(void* root)
{return static_cast<RootRuntimePayload*>(root)->SoundManagerSlotAt24();}
YtlReferenceSlotRuntime& RootTitleDialogNativeSource::RootScenesSlotAt30(void* root)
{return static_cast<RootRuntimePayload*>(root)->SceneManagerSlotAt30();}
void* RootTitleDialogNativeSource::SystemAt4180C0(){return dependencies_.environment.CaptureAt4180C0();}
void* RootTitleDialogNativeSource::RegistryAt417990(void* system,std::int32_t index)
{return ResolveEnvironmentArchiveAt417990(*static_cast<EnvironmentRootStorage*>(system),std::uint32_t(index));}
void* RootTitleDialogNativeSource::SoundParamsAt4188C0(void* registry)
{return ResolveEnvironmentObjectReference<void>(*reinterpret_cast<YtlReferenceSlotRuntime*>(reinterpret_cast<std::uintptr_t>(registry)+0x1Cu));}
const std::int32_t* RootTitleDialogNativeSource::BgmVolumeAtVslot14(void* p){return Volume(p,0x14);}
const std::int32_t* RootTitleDialogNativeSource::EffectVolumeAtVslot24(void* p){return Volume(p,0x24);}
const char* RootTitleDialogNativeSource::TitleBgmAt61A654()
{
    if(dependencies_.resources==TitleButtonResources::Configured)
        return RequireConfiguredTitleResources(dependencies_.resourceProfile).bgmName;
    return dependencies_.resources==TitleButtonResources::BakuAne?BakuTitleData::Scene().bgmName:dependencies_.cues.titleBgmAt61A654.GetString();
}
const char* RootTitleDialogNativeSource::ConfirmSoundAt61A658()
{
    if(dependencies_.resources==TitleButtonResources::Configured)
        return RequireConfiguredTitleResources(dependencies_.resourceProfile).clickCue;
    return dependencies_.resources==TitleButtonResources::BakuAne?BakuTitleData::Names().clickCue:dependencies_.cues.confirmAt61A658.GetString();
}
const char* RootTitleDialogNativeSource::HoverSoundAt61A65C()
{
    if(dependencies_.resources==TitleButtonResources::Configured)
        return RequireConfiguredTitleResources(dependencies_.resourceProfile).hoverCue;
    return dependencies_.resources==TitleButtonResources::BakuAne?BakuTitleData::Names().selectCue:dependencies_.cues.hoverAt61A65C.GetString();
}
std::uint8_t RootTitleDialogNativeSource::LoadSurfaceAt480000(void* draw,YtlReferenceSlotRuntime* output,const char* name)
{
    if(!Read(output->baseAt00))
        AdoptGraphicsSurfaceAt407A80(*output,NewGraphicsSurfaceAt40C2E0(dependencies_.graphics,dependencies_.strings));
    auto* surface=ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(*output);
    return dependencies_.pictures.LoadIntoAt4800A0(draw,*surface,name);
}
std::uint8_t RootTitleDialogNativeSource::InputAt4C3430(void*)
{return ReadScriptSuppressInputAt4C3430(dependencies_.environment,dependencies_.input);}
std::uint8_t RootTitleDialogNativeSource::InputAt4AA970(void*)
{return ReadScriptAccelerateInputAt4AA970(dependencies_.input);}
std::uint8_t RootTitleDialogNativeSource::InputAt47F6D0(void*)
{return ReadScriptQuickLoadKeyAt47F6D0(dependencies_.environment,dependencies_.input,dependencies_.root,
    dependencies_.scriptErrors,dependencies_.names,dependencies_.strings);}
std::uint8_t RootTitleDialogNativeSource::LoadAvailableAt485CE0(void*)
{
    // 522C47与Program共用原485CE0；callee不消费ECX，但每次自行捕获Env/Root。
    QuickOperationsNativeScope scope(dependencies_.quick);
    return QuickLoadNativeAt485CE0();
}
std::uint8_t RootTitleDialogNativeSource::PlayBgmAt42BFE0(void* sound,const char* name,std::int32_t volume,std::int32_t loop)
{return dependencies_.root.NativeSoundServices().PlayBgmCapturedAt42BFE0(static_cast<SoundManagerNativeBody50h*>(sound),name,volume,loop);}
std::uint8_t RootTitleDialogNativeSource::ClearSoundAt4384D0(void* sound)
{return dependencies_.root.NativeSoundServices().ClearVoicesCapturedAt4384D0(static_cast<SoundManagerNativeBody50h*>(sound));}
void RootTitleDialogNativeSource::StopSoundAt413270(void* sound,EnvironmentVectorStorage<RuntimeCString>* names)
{dependencies_.root.NativeSoundServices().StopEffectsCapturedAt413270(static_cast<SoundManagerNativeBody50h*>(sound),names);}
void* RootTitleDialogNativeSource::SoundAt4188F0(void* root)
{return ResolveEnvironmentObjectReference<SoundManagerNativeBody50h>(RootSoundSlotAt24(root));}
std::uint8_t RootTitleDialogNativeSource::StartSoundAt4BCD70(void* sound,const char* name,std::int32_t channel,std::int32_t volume,std::uint8_t flag)
{return dependencies_.root.NativeSoundServices().PlayCueAt4BCD70(static_cast<SoundManagerNativeBody50h*>(sound),name,std::uint32_t(channel),volume,flag);}
void* RootTitleDialogNativeSource::ObjectAt50A620(void* system)
{return ResolveEnvironmentObjectReference<void>(static_cast<EnvironmentRootStorage*>(system)->pathsAt48);}
void* RootTitleDialogNativeSource::CurrentSceneAt4186D0(void* root)
{return ResolveEnvironmentObjectReference<SceneManagerNativeBody48h>(RootScenesSlotAt30(root));}
void* RootTitleDialogNativeSource::ConstructConfirmationAt4E4340(void* raw)
{return dependencies_.confirmation.ConstructAt4E4340(raw);}
void* RootTitleDialogNativeSource::PrepareModalAt4AD230(void* root)
{
    // 4AD230只解本次固定Root+3C；5071D0不消费返回的ECX，不额外Ensure或增持。
    return ResolveEnvironmentObjectReference<SystemInformationDisplayNativeBody14h>(
        static_cast<RootRuntimePayload*>(root)->InformationDisplaySlotAt3C());
}
std::int32_t RootTitleDialogNativeSource::RunModalAt5071D0(void*,const YtlReferenceSlotRuntime* target)
{return RunNativeGuiModalReferenceAt5071D0(target,dependencies_.modal);}
std::int32_t RootTitleDialogNativeSource::PostMessageAt522D2F(void* window,std::uint32_t message,std::uint32_t w,std::int32_t l)
{return dependencies_.postMessage(static_cast<HWND>(window),message,w,l);}
void* RootTitleDialogNativeSource::GraphicsAt4182F0(){return dependencies_.graphics.CaptureAt4182F0();}
void RootTitleDialogNativeSource::CommitAt4B02E0(void* receiver,std::int32_t argument)
{
    dependencies_.graphics.InvalidatedRectCapturedAt4B02E0(*static_cast<GraphicsRootNativeBody1Ch*>(receiver),
        reinterpret_cast<const RECT*>(std::uint32_t(argument)));
}
std::int32_t RootTitleDialogNativeSource::RunResourceNamePrompt()
{
    if (!dependencies_.params) throw std::logic_error("Resource name prompt requires the current Params owner");
    const auto instance = dependencies_.resourceInstance ? *dependencies_.resourceInstance :
        ProcessAtlBaseModuleNative().resourceInstanceAt08;
    const auto limits = dependencies_.resources==TitleButtonResources::Configured
        ? RequireConfiguredTitleResources(dependencies_.resourceProfile).nameLimits : GameNameByteLimits{};
    const auto result = ShowGameResourceNamePrompt({dependencies_.root, dependencies_.environment,
        *dependencies_.params, instance, limits});
    switch (result.status)
    {
    case GameResourceNamePromptStatus::Accepted: return 1;
    case GameResourceNamePromptStatus::Cancelled: return 0;
    case GameResourceNamePromptStatus::Error:
        Logging::Write(Logging::Level::Error, "Game resource name dialog failed.");
        if (result.exception) std::rethrow_exception(result.exception);
        ::SetLastError(result.win32Error);
        return -1;
    }
    throw std::logic_error("Invalid resource name dialog result");
}
std::uint8_t RootTitleDialogNativeSource::NameEditFlags()
{
    if (!dependencies_.params) throw std::logic_error("Title name flags require the current Params owner");
    return dependencies_.params->CaptureAt418480()->GameSystem()->defaultNameMode;
}
}
