#include <Runtime/RootReadScriptTextNativeServices.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/NativeGraphicsRuntimeLibrary.h>
#include <Runtime/PostWindowNativeLayers.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/SoundParamsEnvironmentArchive.h>
#include <bit>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
RootReadScriptTextNativeServices::RootReadScriptTextNativeServices(RootReadScriptTextNativeDependencies d)noexcept
    :dependencies_(d),sound_(d.environment,d.root),format_(d.root,d.params),
     playbackSource_(sound_,d.root,d.voiceErrors),playback_(d.environment,d.input,playbackSource_,d.strings),
     restore_(d.environment,d.params,sound_),command_(d.root,d.environment),maintenance_(d.environment,sound_,playback_),
     advanceSource_(d.root,d.environment,d.input,d.memory),advance_{advanceSource_,playback_,restore_,sound_,command_,maintenance_},names_(d.params){}
RootReadScriptTextNativeServices::Invocation RootReadScriptTextNativeServices::CaptureText()
{
    dependencies_.root.EnsureCreatedAt413BE0();auto* const root=dependencies_.root.ReadPublishedAt6198C4();
    auto* draw=ResolveDrawManagerReferenceAt4175A0(root->DrawManagerSlotAt18());
    auto* body=reinterpret_cast<TextWindowLayerNativeBodyE8h*>(DrawLayerAt418C10(*draw,3));
    const auto services=root->PostWindow().BorrowNativeDrawCallServices();
    if(!services.layers)throw std::logic_error("native ReadScript Text requires captured Root text services");
    return {*body,services.layers->text};
}
void RootReadScriptTextNativeServices::SelectFormatAt4C2740(std::int32_t index,bool initialize)
{
    auto call=CaptureText();const TextWindowLayerNativeScope scope(call.window);
    TextWindowNativeTextServices fonts(call.body,call.window.state,call.window.environment,
        dependencies_.graphics.TextCalls(),dependencies_.fontFaceAt619824,dependencies_.errors,GameTextWindowNativeDescriptors());
    TextWindowNativeText layout(call.body,dependencies_.graphics,dependencies_.graphics,fonts,dependencies_.strings);
    TextWindowNativeFormat format(call.body,format_,call.window,layout,dependencies_.graphics,dependencies_.errors);
    static_cast<void>(format.SetFormatAt4C2740(index,std::uint32_t(initialize))); // 原caller忽略AL。
}
std::int32_t RootReadScriptTextNativeServices::TickTextAt4BB530(bool suppress)
{
    // 外层Root/slot3捕获不属于4BB530本地catch；不能合并成一个安全返回-1的wrapper。
    auto call=CaptureText();const TextWindowLayerNativeScope scope(call.window);
    TextWindowNativeTextServices fonts(call.body,call.window.state,call.window.environment,
        dependencies_.graphics.TextCalls(),dependencies_.fontFaceAt619824,dependencies_.errors,GameTextWindowNativeDescriptors());
    TextWindowNativeText layout(call.body,dependencies_.graphics,dependencies_.graphics,fonts,dependencies_.strings);
    TextWindowNativePlayback playback(call.body,dependencies_.environment,dependencies_.graphics,layout,playback_);
    return playback.TickAt4BB530(std::uint32_t(suppress));
}
bool RootReadScriptTextNativeServices::AdvanceTextAt4BC260(std::uint8_t input)
{
    auto call=CaptureText();const TextWindowLayerNativeScope scope(call.window);
    return AdvanceTextWindowNativeAt4BC260(call.body,input,advance_);
}
bool RootReadScriptTextNativeServices::VoiceArrayAllowedAtSoundParams60()
{
    auto* save=static_cast<SaveDataEnvironmentArchive*>(ResolveEnvironmentArchiveAt417990(*dependencies_.environment.CaptureAt4180C0(),1));
    const auto gate=TextSoundVoiceGateTableReceiver::Native(ResolveEnvironmentObjectReference<EnvironmentSoundParamsArchive>(save->soundParamsAt1C));
    const auto* voices=sound_.CaptureVoicesSlotAt3C();
    return gate.Allowed(voices);
}
bool RootReadScriptTextNativeServices::AnyVoicePlayingAt4196C0()
{return sound_.CaptureRootAfter413BE0().Resolve().effects.AnyVoiceSlotPlaying();}
void RootReadScriptTextNativeServices::StopVoiceAt4384D0()
{sound_.CaptureRootAfter413BE0().Resolve().voice.StopAndClearAllVoiceSlots();}
void RootReadScriptTextNativeServices::StopEffectsAt413270()
{sound_.CaptureRootAfter413BE0().Resolve().effects.StopEffectsKeepingMappingsAt413270();}
bool RootReadScriptTextNativeServices::ReplayVoiceAt42C4F0()
{
    const auto root=sound_.CaptureRootAfter413BE0();const auto* voices=sound_.CaptureVoicesSlotAt3C();
    return root.Resolve().voice.PlayBatchRawAt42C4F0(voices);
}
void RootReadScriptTextNativeServices::AdjustEffectsAt4C2410(){sound_.CaptureRootAfter413BE0().Resolve().volume.Adjust();}
void RootReadScriptTextNativeServices::RefreshEffectsAt433F60(){sound_.CaptureRootAfter413BE0().Resolve().volume.Refresh();}
std::uint8_t RootReadScriptTextNativeServices::ReadWaitMarkEnabledAt0C()
{return advanceSource_.CaptureLayersAfter413BE0(0).LayerAt418C10(4).EnabledByteAt0C();}
void RootReadScriptTextNativeServices::WriteWaitMarkEnabledAt0C(std::uint8_t value)
{advanceSource_.CaptureLayersAfter413BE0(0).LayerAt418C10(4).SetEnabledByteAt0C(value);}
bool RootReadScriptTextNativeServices::PollKeyboardAt47F4E0(std::uint32_t action)
{
    auto* input=dependencies_.input.CaptureAt418210();
    auto* keyboard=ResolveEnvironmentObjectReference<KeyboardManagerNativeBody438h>(input->keyboardAt18);
    return ReadKeyboardPressedAt47F4E0(*keyboard,std::bit_cast<std::int32_t>(action));
}
bool RootReadScriptTextNativeServices::PollMouseAt4B0740()
{
    const auto* mouse=advanceSource_.CaptureMouseAt418210(0);
    const auto address=reinterpret_cast<std::uintptr_t>(mouse);
    return *reinterpret_cast<const volatile std::uint8_t*>(address+0x31u)==0&&
        *reinterpret_cast<const volatile std::uint8_t*>(address+0x11u)!=0&&
        *reinterpret_cast<const volatile std::uint32_t*>(address+0x28u)==0;
}
const char* RootReadScriptTextNativeServices::ReadCurrentScriptNameForError()
{
    dependencies_.root.EnsureCreatedAt413BE0();auto* const root=dependencies_.root.ReadPublishedAt6198C4();
    auto* const save=root->ParamsAt47BDB0().ResolveSaveParams2At30();
    const auto id=*static_cast<const volatile std::int32_t*>(&save->scriptFileIdAtSaveParams04);
    // 负ID也必须捕获静态Params/Pattern；FindName本身才决定返回null。
    return names_.CapturePatternNames().FindNameAt4AD1B0(id);
}
}
