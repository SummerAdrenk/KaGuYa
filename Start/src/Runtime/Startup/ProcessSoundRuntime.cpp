#include <Runtime/ProcessSoundRuntime.h>
#include <Runtime/NativeSoundRuntimeLibrary.h>
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
constinit ProcessStorage<EnvironmentRootCriticalSection> soundLock;
constinit ProcessStorage<SoundRootOwner> soundOwner;
constinit ProcessStorage<SoundModuleNativeApi> soundApi;
constinit ProcessStorage<NativeSoundRuntimeLibrary> soundLibrary;
void __cdecl ReleaseSoundLockAt5AA1B0() noexcept { std::destroy_at(&soundLock.Get()); }
void __cdecl ReleaseSoundAt5AC690() noexcept { soundOwner.Get().ReleaseAt5AC690(); }
void __cdecl InitializeSoundLockAt5AD4F0()
{
    // PE5AD4F0->401090：独立6010D0锁，不能复用Graphics/Environment的锁。
    soundLock.Construct();
    (void)std::atexit(ReleaseSoundLockAt5AA1B0);
    soundOwner.Construct(soundLock.Get(), DefaultEnvironmentRootMemory(), DefaultGraphicsModuleNativeWin32Api());
    soundApi.Construct(soundOwner.Get(), DefaultRuntimeStringManagerSource(), DefaultGraphicsModuleNativeWin32Api());
    soundLibrary.Construct(soundOwner.Get(), DefaultEnvironmentRootMemory(), DefaultRuntimeStringManagerSource(),
        DefaultGraphicsModuleNativeWin32Api(), DefaultSoundInitializationPathsNativeSource());
}
void __cdecl RegisterSoundAt5AD750()
{
    // PE5AD750->407520只登记5AC690；显式存储不添加第二次静态owner析构。
    (void)std::atexit(ReleaseSoundAt5AC690);
}
}
SoundRootOwner& ProcessSoundRootOwner() noexcept { return soundOwner.Get(); }
SoundModuleNativeApi& ProcessSoundModuleNativeApi() noexcept { return soundApi.Get(); }
NativeSoundRuntimeLibrary& ProcessSoundRuntimeLibrary() noexcept { return soundLibrary.Get(); }
YtlReferenceSlotRuntime* CaptureProcessSoundRootAt4183D0() { return soundOwner.Get().CaptureAt4183D0(); }
bool InitializeProcessSoundAt53DFC0(HWND window)
{
    auto* const root = CaptureProcessSoundRootAt4183D0();
    return InitializeSoundRootNativeAt53DFC0(*root, window, DefaultEnvironmentRootMemory(),
        DefaultRuntimeStringManagerSource(), DefaultGraphicsModuleNativeWin32Api(), DefaultSoundInitializationPathsNativeSource());
}
void ShutdownProcessSoundAt53DF20()
{
    auto* const root = CaptureProcessSoundRootAt4183D0();
    ShutdownSoundRootNativeAt53DF20(*root, DefaultEnvironmentRootMemory(),
        DefaultRuntimeStringManagerSource(), DefaultGraphicsModuleNativeWin32Api());
}
using Initializer = void (__cdecl*)();
#pragma section(".CRT$XCUKaGuYa5AD4F0", read)
#pragma section(".CRT$XCUKaGuYa5AD750", read)
extern "C" __declspec(allocate(".CRT$XCUKaGuYa5AD4F0")) Initializer const KaGuYaProcessSoundLockInitializer = InitializeSoundLockAt5AD4F0;
extern "C" __declspec(allocate(".CRT$XCUKaGuYa5AD750")) Initializer const KaGuYaProcessSoundInitializer = RegisterSoundAt5AD750;
#pragma comment(linker, "/include:_KaGuYaProcessSoundLockInitializer")
#pragma comment(linker, "/include:_KaGuYaProcessSoundInitializer")
}
