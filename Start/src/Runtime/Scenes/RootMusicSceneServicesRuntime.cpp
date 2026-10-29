#include <Runtime/RootMusicSceneServicesRuntime.h>
#include <Runtime/NativeGraphicsRuntimeLibrary.h>
#include <Runtime/ModeSelectButtonNative.h>
#include <Runtime/OptionCharSelectDialogNative.h>

namespace KaGuYa::Start::Runtime
{
RootMusicSceneServicesRuntime::RootMusicSceneServicesRuntime(RootMusicSceneNativeDependencies d,
    ModeSelectButtonNativeServices& publicButtons,OptionCharSelectDialogNativeDependencies& selector)noexcept
    :ownedMode_(std::in_place,ModeButtonNativeServices{publicButtons.button,publicButtons.memory}),
     ownedDialog_(std::in_place,MusicDialogNativeServices{d.pictures,d.graphics,selector.graphics,selector.input,
         selector.screen,d.graphics,d.strings,publicButtons.memory}),
     source_(d),dialog_(*ownedDialog_),button_{*ownedMode_,dialog_,source_,d.strings},buttons_(button_),
     scene_{source_,buttons_,publicButtons.memory,d.graphics}{}
}
