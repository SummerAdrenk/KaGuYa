#include <Runtime/RootScenePublicNativeServicesRuntime.h>

namespace KaGuYa::Start::Runtime
{
RootScenePublicNativeServicesRuntime::RootScenePublicNativeServicesRuntime(RootScenePublicNativeDependencies d)noexcept
    :input_(d.input),guiInput_(d.input),screen_(d.graphics),
     pictures_(d.root,d.environment,d.files,d.key,d.codecs,d.graphics.SurfaceCalls(),d.messages,d.strings,d.transaction),
     modes_(d.root,d.environment,d.files,d.key,d.codecs,d.graphics.SurfaceCalls(),d.messages,d.strings,d.transaction),
     characters_(d.environment,d.memory),buttons_{input_,d.graphics,d.graphics,d.strings},
     modeServices_{d.memory,modes_,d.graphics.SurfaceCalls(),buttons_},
     characterServices_{characters_,pictures_,d.graphics.SurfaceCalls(),d.graphics.GuiCalls(),guiInput_,screen_,d.strings}{}
}
