#include <Runtime/ProcessGraphicsRuntime.h>
#include <Runtime/GraphicsPatternFilterNativeApi.h>
#include <Runtime/GraphicsScreenNativeApi.h>
#include <Runtime/GraphicsTransitNativeApi.h>
#include <Runtime/GraphicsGuiNativeApi.h>
#include <Runtime/GraphicsPresentNativeApi.h>

#include <array>
#include <cstdlib>
#include <memory>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
template<class T> struct ProcessStorage final
{
    T& Get() noexcept { return *std::launder(reinterpret_cast<T*>(bytes.data())); }
    template<class... Args> void Construct(Args&&... args)
    { std::construct_at(reinterpret_cast<T*>(bytes.data()), std::forward<Args>(args)...); }
    alignas(T) std::array<std::byte, sizeof(T)> bytes{};
};

constinit ProcessStorage<EnvironmentRootCriticalSection> graphicsLock;
constinit ProcessStorage<GraphicsRootOwner> graphicsOwner;
constinit ProcessStorage<GraphicsSurfaceNativeApi> surfaceApi;
constinit ProcessStorage<GraphicsPatternFilterNativeApi> filterApi;
constinit ProcessStorage<GraphicsScreenNativeApi> screenApi;
constinit ProcessStorage<GraphicsTransitNativeApi> transitApi;
constinit ProcessStorage<GraphicsGuiNativeApi> guiApi;
constinit ProcessStorage<GraphicsPresentNativeApi> presentApi;

void __cdecl ReleaseGraphicsLockAt5AA190() noexcept
{ std::destroy_at(&graphicsLock.Get()); }
void __cdecl ReleaseGraphicsAt5AADF0() noexcept
{
    // 只释放当前发布正文；不结束owner生命期，不补第二次静态析构或ExitSystem。
    graphicsOwner.Get().ReleaseAt5AADF0();
}
void __cdecl InitializeGraphicsLockAt5AD4EC()
{
    // 401060：独立Graphics锁，不能复用Environment/应用Root的锁或拥有槽。
    graphicsLock.Construct();
    static_cast<void>(std::atexit(ReleaseGraphicsLockAt5AA190));
    graphicsOwner.Construct(graphicsLock.Get(), DefaultEnvironmentRootMemory(),
        DefaultRuntimeStringManagerSource(), DefaultGraphicsModuleNativeWin32Api());
    // 正文外仅登记借用服务，不Capture、不LoadLibrary、不增加原生根的退出扫描。
    surfaceApi.Construct(graphicsOwner.Get(), DefaultRuntimeStringManagerSource(),
        DefaultGraphicsModuleNativeWin32Api());
    filterApi.Construct(graphicsOwner.Get(), DefaultRuntimeStringManagerSource(),
        DefaultGraphicsModuleNativeWin32Api());
    screenApi.Construct(graphicsOwner.Get(), DefaultRuntimeStringManagerSource(),
        DefaultGraphicsModuleNativeWin32Api());
    transitApi.Construct(graphicsOwner.Get(), DefaultRuntimeStringManagerSource(),
        DefaultGraphicsModuleNativeWin32Api());
    guiApi.Construct(graphicsOwner.Get(), DefaultRuntimeStringManagerSource(),
        DefaultGraphicsModuleNativeWin32Api(), surfaceApi.Get());
    presentApi.Construct(graphicsOwner.Get(), DefaultRuntimeStringManagerSource(),
        DefaultGraphicsModuleNativeWin32Api());
}
void __cdecl RegisterGraphicsAt5AD584()
{ static_cast<void>(std::atexit(ReleaseGraphicsAt5AADF0)); }
}

GraphicsRootOwner& ProcessGraphicsRootOwner() noexcept { return graphicsOwner.Get(); }
GraphicsRootNativeBody1Ch* CaptureProcessGraphicsRootAt4182F0()
{ return graphicsOwner.Get().CaptureAt4182F0(); }
GraphicsSurfaceNativeApi& ProcessGraphicsSurfaceNativeApi() noexcept { return surfaceApi.Get(); }
GraphicsPatternFilterNativeApi& ProcessGraphicsPatternFilterNativeApi() noexcept { return filterApi.Get(); }
GraphicsScreenNativeApi& ProcessGraphicsScreenNativeApi() noexcept { return screenApi.Get(); }
GraphicsTransitNativeApi& ProcessGraphicsTransitNativeApi() noexcept { return transitApi.Get(); }
GraphicsGuiNativeApi& ProcessGraphicsGuiNativeApi() noexcept { return guiApi.Get(); }
GraphicsPresentNativeApi& ProcessGraphicsPresentNativeApi() noexcept { return presentApi.Get(); }

using Initializer = void (__cdecl*)();
#pragma section(".CRT$XCUKaGuYa5AD4EC", read)
#pragma section(".CRT$XCUKaGuYa5AD584", read)
extern "C" __declspec(allocate(".CRT$XCUKaGuYa5AD4EC")) Initializer const KaGuYaProcessGraphicsLockInitializer = InitializeGraphicsLockAt5AD4EC;
extern "C" __declspec(allocate(".CRT$XCUKaGuYa5AD584")) Initializer const KaGuYaProcessGraphicsInitializer = RegisterGraphicsAt5AD584;
#pragma comment(linker, "/include:_KaGuYaProcessGraphicsLockInitializer")
#pragma comment(linker, "/include:_KaGuYaProcessGraphicsInitializer")
}
