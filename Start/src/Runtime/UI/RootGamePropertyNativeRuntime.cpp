#include <Runtime/RootGamePropertyNativeRuntime.h>

namespace KaGuYa::Start::Runtime
{
RootGamePropertyNativeRuntime::RootGamePropertyNativeRuntime(RootGamePropertyNativeDependencies d) noexcept
    : endpoint_(d.endpoint), sheet_{DefaultPropertySheetNativeApi(), endpoint_},
      textSource_(d.environment, d.graphics),
      text_{textSource_, textApi_, endpoint_, d.surfaces, d.strings, d.exports,
          d.bitmapResourceModule, ProcessPropertySheetButtonsAt6197EC(), d.nativeBitmapResourceModuleSlot},
      soundSource_(d.root, d.environment, d.sounds, d.open),
      sound_{soundSource_, DefaultPropertyPageSoundNativeApi(), endpoint_, d.module,
          ProcessPropertySheetButtonsAt6197EC(), d.strings, endpoint_.memory},
      systemSource_(d.environment, d.params),
      system_{systemSource_, DefaultPropertyPageSystemNativeApi(), endpoint_, d.strings,
          ProcessPropertySheetButtonsAt6197EC()},
      services_{sheet_, d.module, text_, sound_, system_, d.strings, ProcessPropertySheetButtonsAt6197EC()} {}
}
