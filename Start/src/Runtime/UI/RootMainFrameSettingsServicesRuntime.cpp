#include <Runtime/RootMainFrameSettingsServicesRuntime.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/EnvironmentStartupRuntime.h>
#include <Runtime/NativeGraphicsRuntimeLibrary.h>
#include <Runtime/NativeSoundRuntimeLibrary.h>
#include <Runtime/SceneCueNativeGlobals.h>

namespace KaGuYa::Start::Runtime
{
RootMainFrameSettingsServicesRuntime::RootMainFrameSettingsServicesRuntime(
    RootMainFrameSettingsServicesDependencies d) noexcept
    : apply_({.root=d.root, .environment=d.environment.RootSource(), .params=d.params,
          .graphics=d.graphics, .sound=d.sound, .draws=d.draws, .textBody=std::nullopt,
          .fontFace=ProcessSceneFontFaceNativeCharsSlot(), .errors=d.errors,
          .messageBox=&::MessageBoxA, .strings=d.strings, .wide=DefaultRuntimeWideStringNativeApi()}),
      property_({.root=d.root, .environment=d.environment.RootSource(), .params=d.params,
          .graphics=d.graphics.RootSource(), .surfaces=d.graphics.SurfaceCalls(), .exports=d.graphics.Win32Source(),
          .sounds=d.root.NativeSoundServices(), .open=apply_.Services().open, .module=d.module,
          .endpoint=d.endpoint, .bitmapResourceModule=nullptr, .strings=d.strings,
          .nativeBitmapResourceModuleSlot=&d.bitmapResourceModule}),
      volume_(d.params, d.sound.ModuleSource(), d.sound.StringSource(), d.sound.Win32Source()),
      access_(d.errors, d.strings, GameScriptNamedValueTables()),
      selectionSound_(d.environment.RootSource(), d.root),
      choices_(d.root, d.environment.RootSource(), d.input.RootSource(), GameScriptNamedValueTables(),
          d.errors, d.messageFiles, d.environment, d.strings),
      selection_{choices_, d.environment.RootSource(), d.graphics, d.graphics.TextCalls(),
          ProcessSceneFontFaceNativeCharsSlot(), selectionSound_},
      services_{apply_.Services(), property_.Services(), volume_, access_, DefaultMainFrameSettingsNativeApi(),
          d.environment.RootSource(), d.draws.Layers().select, selection_, d.draws.Layers().tool, d.memory},
      command_(services_) {}
}
