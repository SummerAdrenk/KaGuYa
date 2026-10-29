#include <Runtime/RootSceneManagerNativeServicesRuntime.h>

namespace KaGuYa::Start::Runtime
{
RootSceneManagerNativeServicesRuntime::RootSceneManagerNativeServicesRuntime(RootSceneManagerNativeDependencies d) noexcept
    :ownedPublic_(std::in_place,RootScenePublicNativeDependencies{d.root,d.environment,d.input,d.graphics,d.files,d.key,
        d.codecs,d.messages,d.strings,d.memory,d.transaction}),
     services_{d.memory,d.strings,d.factory,ownedPublic_->Mode(),ownedPublic_->Dialog()}{}
RootSceneManagerNativeServicesRuntime::RootSceneManagerNativeServicesRuntime(
    RootScenePublicNativeServicesRuntime& services,ISceneFactoryNativeSource& factory)noexcept
    :services_{services.Mode().memory,services.Dialog().strings,factory,services.Mode(),services.Dialog()}{}
}
