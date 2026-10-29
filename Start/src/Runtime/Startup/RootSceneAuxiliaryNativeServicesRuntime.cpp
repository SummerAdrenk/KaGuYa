#include <Runtime/RootSceneAuxiliaryNativeServicesRuntime.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/EnvironmentStartupRuntime.h>
#include <Runtime/GraphicsSurfaceCaptureRuntime.h>

namespace KaGuYa::Start::Runtime
{
GraphicsSurfaceReference RootSceneAuxiliaryNativeServicesRuntime::Capture::CaptureScreenNativeAt4ACDD0()
{
    GraphicsSurfaceReference result(graphics_);
    CaptureGraphicsScreenAt4ACDD0(result.Slot(),graphics_,graphics_,graphics_);
    return result;
}
RootSceneAuxiliaryNativeServicesRuntime::RootSceneAuxiliaryNativeServicesRuntime(
    RootSceneAuxiliaryNativeDependencies d)noexcept
    :capture_(d.graphics),screen_(d.graphics),input_(d.input),
     rectangles_(d.environment,d.root,d.root,d.rectangles),
     systemSource_({d.root,d.environment.RootSource(),d.graphics,rectangles_,d.modal}),
     systemDialog_{systemSource_,d.pictures,d.graphics,d.graphics.GuiCalls(),input_,screen_,d.strings},
     information_{systemSource_,d.memory,&systemDialog_},
     modalRoot_(d.root,d.graphics,capture_),general_(d.root,d.graphics,d.input,d.pictures,capture_,d.modal,d.strings),
     nameSource_({d.root,d.environment.RootSource(),d.params,d.input.RootSource(),d.graphics,d.pictures,d.edits,d.viewport,d.strings}),
     names_{nameSource_,d.pictures,d.graphics,d.graphics.GuiCalls(),input_,screen_,d.strings},
     saveConfirmationSource_(d.pictures,d.graphics,d.confirmationMask,d.confirmationFont,d.strings),
     saveConfirmation_{saveConfirmationSource_,d.graphics,d.graphics,d.graphics,d.graphics.GuiCalls(),input_,screen_,modalRoot_,d.modal,d.strings},
     resourceNames_(d.params),previewSource_(d.root,d.params),
     previewServices_{d.graphics,d.graphics,d.graphics,d.graphics,resourceNames_,resourceNames_,d.sprites,d.pictures,d.strings,d.memory},
     preview_(previewSource_,previewServices_),
     persistence_({d.root,d.environment.RootSource(),d.params,d.graphics,d.files,d.messagePaths,d.random,d.saveProfile,
        preview_,saveConfirmation_,d.modal,d.strings,d.memory}),
     pageSource_(d.environment.RootSource()),slotSource_(persistence_,d.graphics),
     systemPageSource_(d.root,d.input.RootSource()),popupSource_(d.environment.RootSource(),d.input.RootSource()),
     commonPage_{pageSource_,d.pictures,d.graphics,d.graphics.GuiCalls(),input_,screen_,d.strings},
     slots_{slotSource_,d.graphics,d.graphics,d.memory},regularPages_{commonPage_,slotSource_,d.graphics,slots_},
     systemPage_{systemPageSource_,d.pictures,d.graphics,d.graphics.GuiCalls(),input_,screen_,d.strings},
     popup_{popupSource_,d.pictures,d.graphics,d.graphics.GuiCalls(),input_,screen_,d.strings},
     pages_(regularPages_,systemPage_,popup_),deletionChildren_({d.pictures,d.graphics,d.strings}),
     deletion_{systemSource_,d.pictures,d.graphics,d.graphics.GuiCalls(),d.graphics,input_,screen_,deletionChildren_,d.strings}
{}
}
