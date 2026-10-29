#include <Platform/NativeTimerProcess.h>
#include <Runtime/EnvironmentRootRuntime.h>

#include <array>
#include <cstdlib>
#include <memory>
#include <new>
#include <type_traits>
#include <utility>

#if !defined(_MSC_VER) || !defined(_M_IX86) || _MSVC_LANG < 202002L
#error NativeTimerProcess requires MSVC x86 C++20.
#endif

namespace KaGuYa::Start::Platform
{
namespace
{
template<class T> struct ProcessStorage final
{
    template<class... Args> T* Construct(Args&&... args)
        noexcept(std::is_nothrow_constructible_v<T, Args...>)
    {
        return std::construct_at(reinterpret_cast<T*>(bytes.data()), std::forward<Args>(args)...);
    }
    alignas(T) std::array<std::byte, sizeof(T)> bytes{};
};

// 43FB87为operator new(14h)，43FEC4经53FBFB释放；沿既有memory桥保留尺寸。
// 只复用接口，不借Environment owner/facade或带自动析构的默认memory对象。
class NativeTimerProcessMemory final : public Runtime::IEnvironmentRootMemory
{
public:
    void* AllocateBody(std::size_t bytes) override { return ::operator new(bytes); }
    void DeleteBody(void* address, std::size_t bytes) override { ::operator delete(address, bytes); }
    void DeleteVector(void* address, std::size_t bytes) override { ::operator delete(address, bytes); }
};

constinit ProcessStorage<Win32TimerPlatformApi> apiStorage;
constinit ProcessStorage<NativeTimerProcessMemory> memoryStorage;
constinit ProcessStorage<ProcessTimerSource> sourceStorage;
static_assert(std::is_trivially_destructible_v<decltype(apiStorage)>);
static_assert(std::is_trivially_destructible_v<decltype(memoryStorage)>);
static_assert(std::is_trivially_destructible_v<decltype(sourceStorage)>);

NativeTimerProcessMemory& ProcessMemory() noexcept
{
    // 局部静态仅持指针；一次构造同步不附带所指对象的atexit析构。
    static auto* const memory = memoryStorage.Construct();
    return *memory;
}

void __cdecl ReleaseTimerAt5AAD80()
{
    ReleaseNativeTimerCreatorAt5AAD80(ProcessNativeTimerSource().Dependencies());
}

void __cdecl RegisterTimerAt5AD580()
{
    // 原5AD580->402BF0只有push 5AAD80/call _atexit；不在登记时取getter或Capture。
    // 原402BF0不检查atexit返回值；不新增失败兜底或第二次回收。
    static_cast<void>(std::atexit(ReleaseTimerAt5AAD80));
}
}

Win32TimerPlatformApi& ProcessNativeTimerPlatformApi() noexcept
{
    static auto* const api = apiStorage.Construct();
    return *api;
}

ProcessTimerSource& ProcessNativeTimerSource() noexcept
{
    static auto* const source = []() noexcept
    {
        auto& api = ProcessNativeTimerPlatformApi();
        auto& memory = ProcessMemory();
        auto& creator = ProcessNativeTimerCreator();
        return sourceStorage.Construct(api, creator, memory);
    }();
    return *source;
}

void ReleaseNativeTimerCreatorAt5AAD80(NativeTimerDependencies& dependencies)
{
    NativeTimerScope scope(dependencies);
    // 原5AAD80与43FBF0的source选择/表重置/虚删/后清槽完全对应；flags=0不free creator。
    DeleteNativeTimerCreatorAt43FBF0(dependencies.creator, 0);
}

using Initializer = void (__cdecl*)();
#pragma section(".CRT$XCUKaGuYa5AD580", read)
extern "C" __declspec(allocate(".CRT$XCUKaGuYa5AD580")) Initializer const KaGuYaNativeTimerProcessInitializer = RegisterTimerAt5AD580;
#pragma comment(linker, "/include:_KaGuYaNativeTimerProcessInitializer")
}
