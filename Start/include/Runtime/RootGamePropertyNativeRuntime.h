#pragma once
#include <Runtime/GamePropertyNative.h>

namespace KaGuYa::Start::Runtime
{
struct RootGamePropertyNativeDependencies final
{
    RootRuntimeOwner& root;
    EnvironmentRootOwner& environment;
    ParamsRuntimeRootOwner& params;
    GraphicsRootOwner& graphics;
    IGraphicsSurfaceApi& surfaces;
    IGraphicsModuleNativeWin32Api& exports;
    SoundManagerNativeRuntime& sounds;
    ISoundManagerNativeOpen& open;
    WindowClassModuleNativeBody38h& module;
    DialogEndpointNativeServices endpoint;
    // 与共同页的resourceInstance分别输入，不能把两个原全局擅自合并。
    HMODULE bitmapResourceModule;
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource();
    // 正式Process借61A69C，即ATL.instanceAt04；旧按值宿主域不作失败切换。
    const HMODULE* nativeBitmapResourceModuleSlot = nullptr;
};
// 冷装配只绑定实际Root/Env/Graphics/Sound/Win32服务；不创建33Ch或原页，不查询singleton。
class RootGamePropertyNativeRuntime final
{
public:
    explicit RootGamePropertyNativeRuntime(RootGamePropertyNativeDependencies) noexcept;
    RootGamePropertyNativeRuntime(const RootGamePropertyNativeRuntime&) = delete;
    RootGamePropertyNativeRuntime& operator=(const RootGamePropertyNativeRuntime&) = delete;
    GamePropertyNativeServices& Services() noexcept { return services_; }
private:
    DialogEndpointNativeServices endpoint_;
    PropertySheetNativeServices sheet_;
    RootPropertyPageTextNativeSource textSource_;
    PropertyPageTextNativeWin32Api textApi_;
    PropertyPageTextNativeServices text_;
    RootPropertyPageSoundNativeSource soundSource_;
    PropertyPageSoundNativeServices sound_;
    RootPropertyPageSystemNativeSource systemSource_;
    PropertyPageSystemNativeServices system_;
    GamePropertyNativeServices services_;
};
}
