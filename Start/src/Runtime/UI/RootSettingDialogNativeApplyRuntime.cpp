#include <Runtime/RootSettingDialogNativeApplyRuntime.h>
#include <Runtime/NativeGraphicsRuntimeLibrary.h>
#include <Runtime/NativeSoundRuntimeLibrary.h>

namespace KaGuYa::Start::Runtime
{
RootSettingDialogNativeApplyRuntime::RootSettingDialogNativeApplyRuntime(RootSettingDialogNativeApplyDependencies d) noexcept
    :source_(d.root,d.environment,d.params,d.graphics,d.messageBox),resources_(d.root),
      open_(resources_,d.sound.Calls(),d.wide,d.strings),
      playback_(d.params,d.sound.Calls()),music_(d.sound.Calls(),open_),effects_(open_),
      text_(d.textBody,d.draws.Layers().text,d.graphics.TextCalls(),d.fontFace,d.errors,GameTextWindowNativeDescriptors()),
      services_{source_,d.sound.Calls(),playback_,music_,effects_,open_,d.draws.Layers().message,text_,d.graphics.Win32Source(),d.strings}{}
void RootSettingDialogNativeApplyRuntime::ApplyAt51CFC0(SettingDialogNativeBodyACh& body)
{
    SettingDialogNativeApplyScope scope(services_);
    ApplySettingDialogNativeAt51CFC0(body);
}
}
