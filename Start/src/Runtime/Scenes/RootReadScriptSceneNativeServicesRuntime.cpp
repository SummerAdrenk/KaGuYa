#include <Runtime/RootReadScriptSceneNativeServicesRuntime.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/RootDrawServicesRuntime.h>
#include <Runtime/EnvironmentStartupRuntime.h>
#include <Runtime/NativeInputRuntime.h>
#include <Runtime/NativeGraphicsRuntimeLibrary.h>

namespace KaGuYa::Start::Runtime
{
DrawLayersView RootReadScriptSceneNativeServicesRuntime::Layers::CaptureDrawLayersAt413BE0()
{
    root_.EnsureCreatedAt413BE0();
    return root_.ReadPublishedAt6198C4()->PostWindow().DrawLayersAt18();
}
RootReadScriptSceneNativeServicesRuntime::RootReadScriptSceneNativeServicesRuntime(
    RootReadScriptSceneNativeDependencies d)noexcept
    :layers_(d.root),
     recovery_(d.environment,d.root,d.messageBox,d.memory,d.strings,&d.draws.Layers().text),
     yield_(d.root,recovery_,d.memory,d.strings),
     transition_(d.draws.DrawSource(),d.graphics),packets_{transition_,d.graphics},packetEnvironment_(d.environment.RootSource()),
     flashCalls_{d.graphics,d.graphics,d.graphics},
     effects_{d.draws.DrawSource(),d.draws.LayerRuntime().pattern,packets_,packetEnvironment_,
        d.graphics,d.graphics,d.graphics,flashCalls_,d.draws.LayerRuntime(),
        {&d.draws.Layers(),&d.draws.LayerRuntime(),&d.graphics,&d.draws.WindowEffects(),&d.draws.Gui()}},
     archives_(d.root),masks_({d.environment.RootSource(),archives_,d.root.Files(),d.key,d.codecs,d.readers,
        d.graphics,d.graphics.SurfaceCalls(),d.strings,d.messageBox,d.transaction}),
     moves_(d.root,d.params,d.environment.RootSource(),d.input.RootSource(),d.draws.DrawSource(),d.graphics),
     flash_(d.draws.LayerRuntime().pattern.resources,d.draws.DrawSource(),d.graphics,d.graphics,d.graphics,d.strings),
     waits_(d.environment.RootSource(),d.input.RootSource(),d.root,d.errors,d.names,d.strings),soundWait_(d.root),
     interaction_(d.environment.RootSource(),d.root),files_({d.root,d.environment.RootSource(),d.persistence,d.deleteFile}),
     quick_({d.root,d.environment.RootSource(),files_,d.confirmation,d.memory,d.messageBox}),
     automatic_{files_,files_.Catalog(),d.memory},
     host_({d.root,d.environment.RootSource(),d.params,d.input.RootSource(),d.graphics,d.draws.ToolInteraction(),d.preview,quick_.Services(),automatic_,d.memory}),
     returnTitle_({d.root,d.environment.RootSource(),d.graphics,d.draws.ToolInteraction(),d.confirmation,d.memory,d.postMessage}),
     tools_({d.root,d.environment.RootSource(),d.input.RootSource(),d.draws.ToolInteraction(),d.errors,d.names,d.strings,d.memory}),
     text_({d.root,d.environment.RootSource(),d.params,d.input.RootSource(),d.graphics,d.errors,d.voiceErrors,d.fontFaceAt619824,d.strings,d.memory}),
     sound_(d.environment.RootSource(),d.root),
     choices_(d.root,d.environment.RootSource(),d.input.RootSource(),d.names,d.errors,d.messageFiles,d.messagePaths,d.strings),
     selection_{choices_,d.environment.RootSource(),d.graphics,d.graphics.TextCalls(),d.fontFaceAt619824,sound_},
     menu_({d.root,d.environment.RootSource(),selection_,d.errors,d.names,d.strings}),
     commandMenu_({d.root,d.environment.RootSource(),selection_,d.errors,d.names,d.strings},true),startup_(d.params),
     canvasCopy_(d.graphics.RootSource(),d.strings,d.graphics.Win32Source()),
     scrollDrawing_{d.graphics,d.graphics,d.graphics,d.strings,canvasCopy_},
     scrollInput_{d.input.RootSource(),d.input.NativeServices(),d.environment.RootSource(),d.input.NativeServices().cursor},
     animation_(d.environment.RootSource()),
     source_({d.root,d.environment.RootSource(),d.input.RootSource(),d.execution,yield_,effects_,masks_,moves_,flash_,waits_,soundWait_,
        interaction_,host_.Host(),returnTitle_,tools_,text_,menu_,
        {d.draws.DrawSource(),startup_,scrollDrawing_,d.draws.LayerRuntime().pattern,scrollInput_,animation_,d.graphics,d.memory,d.draws.OpacityInput()},
        layers_,d.graphics,d.draws.LayerRuntime().pattern,recovery_,d.messageBox,d.names,d.strings,&commandMenu_}),
     restore_(d.root,d.environment.RootSource(),layers_,d.draws.ToolInteraction()),
     services_{d.graphics,d.strings,restore_,source_,&effects_}
{}
}
