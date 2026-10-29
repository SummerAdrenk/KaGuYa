#include <Runtime/EnvironmentPostWindowStartupRuntime.h>
#include <Runtime/PostWindowNativeLayers.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
void EnvironmentPostWindowStartupRuntime::BindNativeDrawServices(
    PostWindowNativeLayerServices& layers, LayerWindowNativeServices& runtime,
    DrawLayersNativeWindowServices& windowEffects, DrawGuiNativeServices& gui) noexcept
{
    nativeLayers_ = &layers;
    nativeLayerRuntime_ = &runtime;
    nativeWindowEffects_ = &windowEffects;
    nativeGui_ = &gui;
}
void EnvironmentPostWindowStartupRuntime::ClearNativeDrawServices() noexcept
{
    nativeLayers_ = nullptr;
    nativeLayerRuntime_ = nullptr;
    nativeWindowEffects_ = nullptr;
    nativeGui_ = nullptr;
}
PostWindowNativeLayerServices& EnvironmentPostWindowStartupRuntime::NativeLayerServices() const
{
    if (!nativeLayers_) throw std::logic_error("Native Draw layer services have not been bound");
    return *nativeLayers_;
}
}
