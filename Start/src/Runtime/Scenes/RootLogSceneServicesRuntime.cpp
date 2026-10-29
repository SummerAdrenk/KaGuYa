#include <Runtime/RootLogSceneServicesRuntime.h>

namespace KaGuYa::Start::Runtime
{
RootLogSceneServicesRuntime::RootLogSceneServicesRuntime(RootLogSceneServicesDependencies d) noexcept
    :source_({d.root,d.environment,d.params,d.input.RootSource(),d.graphics,d.pictures,d.files,d.messagePaths,d.font,d.strings}),
     screen_(d.graphics),input_(d.input),text_(d.graphics.TextCalls()),
     gui_{source_,d.pictures,d.graphics,d.guiGraphics,input_,screen_,text_,d.graphics,d.strings,d.memory},
     construction_(gui_),sceneSource_(d.root,d.environment,construction_),scene_{sceneSource_,d.memory}{}
}
