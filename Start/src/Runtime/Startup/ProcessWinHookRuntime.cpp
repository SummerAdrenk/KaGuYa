#include <Runtime/ProcessWinHookRuntime.h>
#include <array>
#include <cstdlib>
#include <memory>
#include <utility>
namespace KaGuYa::Start::Runtime
{
namespace
{
template<class T>struct Storage
{
    alignas(T)std::array<std::byte,sizeof(T)> bytes{};
    T& Get() noexcept{return *std::launder(reinterpret_cast<T*>(bytes.data()));}
    template<class...Args>void Construct(Args&&...args){std::construct_at(reinterpret_cast<T*>(bytes.data()),std::forward<Args>(args)...);}
};
constinit Storage<EnvironmentRootCriticalSection> hookLock;
constinit Storage<WinHookNativeServices> hookServices;
constinit Storage<WinHookRootNativeOwner> hookOwner;
constinit Storage<NativeWindowHookMessageSource> messageSource;
void __cdecl ReleaseHookLockAt5AA1F0() noexcept{std::destroy_at(&hookLock.Get());}
void __cdecl ReleaseHookAt5AACB0() noexcept{hookOwner.Get().ReleaseAt5AACB0();}
void __cdecl InitializeHookLockAt5AD4E4()
{
    // 4010F0独立601064锁；不共用Input的601088或应用根锁。
    hookLock.Construct();static_cast<void>(std::atexit(ReleaseHookLockAt5AA1F0));
    hookServices.Construct(DefaultEnvironmentRootMemory(),DefaultWinHookNativePlatform());
    hookOwner.Construct(hookLock.Get(),hookServices.Get());
    messageSource.Construct(hookOwner.Get(),hookServices.Get());
}
void __cdecl RegisterHookAt5AD57C(){static_cast<void>(std::atexit(ReleaseHookAt5AACB0));}
}
WinHookRootNativeOwner& ProcessWinHookRootNativeOwner() noexcept{return hookOwner.Get();}
WinHookNativeServices& ProcessWinHookNativeServices() noexcept{return hookServices.Get();}
NativeWindowHookMessageSource& ProcessWindowHookMessageSource() noexcept{return messageSource.Get();}
WinHookNativeBody30h* CaptureProcessWinHookAt450E20(){return hookOwner.Get().CaptureAt450E20();}
using Initializer=void(__cdecl*)();
#pragma section(".CRT$XCUKaGuYa5AD4E4",read)
#pragma section(".CRT$XCUKaGuYa5AD57C",read)
extern "C" __declspec(allocate(".CRT$XCUKaGuYa5AD4E4")) Initializer const KaGuYaWinHookLockInitializer=InitializeHookLockAt5AD4E4;
extern "C" __declspec(allocate(".CRT$XCUKaGuYa5AD57C")) Initializer const KaGuYaWinHookInitializer=RegisterHookAt5AD57C;
#pragma comment(linker,"/include:_KaGuYaWinHookLockInitializer")
#pragma comment(linker,"/include:_KaGuYaWinHookInitializer")
}
namespace KaGuYa::Start::Platform
{
IWindowHookMessageSource& DefaultWindowHookMessageSource() noexcept
{return Runtime::ProcessWindowHookMessageSource();}
}
