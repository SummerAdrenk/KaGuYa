#include <Runtime/RootSettingSceneServicesRuntime.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/NativeGraphicsRuntimeLibrary.h>
#include <Runtime/NativeSoundRuntimeLibrary.h>

namespace KaGuYa::Start::Runtime
{
RootSettingSceneServicesRuntime::RootSettingSceneServicesRuntime(RootSettingSceneServicesDependencies d) noexcept
    :parentSource_(d.root,d.environment,d.input.RootSource(),d.graphics,d.root.NativeSoundServices(),d.post),
     pagesSource_(d.root,d.environment,d.post),textSource_(d.environment,d.nameFontFace,d.systemFontFace,d.root,d.params),
     screen_(d.graphics),input_(d.input),resources_(d.root),open_(resources_,d.sound.Calls(),d.wide,d.strings),
     previewText_(d.graphics),text_(d.graphics.TextCalls()),
     apply_({d.root,d.environment,d.params,d.graphics,d.sound,d.draws,d.textBody,d.textFontFace,d.errors,d.messageBox,d.strings,d.wide}),
     confirm_({d.root,d.environment,d.params,d.general,d.names,d.modal,d.memory,d.strings,d.files,d.post}),
     business_(apply_.Services(),confirm_.Services()),
     pagesServices_{pagesSource_,d.pictures,d.graphics,d.draws.Gui().graphics,input_,screen_,d.strings,d.memory},
     previewServices_{d.draws.Layers().message.panes.colorTrans,previewText_,d.previewGlobals},
     soundServices_{pagesSource_,d.pictures,d.graphics,d.draws.Gui().graphics,input_,screen_,d.sound.Calls(),open_,d.strings,d.memory},
     textServices_{textSource_,d.pictures,d.graphics,d.draws.Gui().graphics,input_,screen_,text_,d.graphics,d.strings,d.memory},
     parent_{parentSource_,pages_,business_,d.pictures,d.graphics,d.draws.Gui().graphics,input_,screen_,d.strings,d.memory},
     dialog_{parent_,pagesServices_,previewServices_,soundServices_,textServices_},construction_(&dialog_),
     sceneSource_({d.root,d.environment,construction_}),scene_{sceneSource_,d.memory}{}
}
