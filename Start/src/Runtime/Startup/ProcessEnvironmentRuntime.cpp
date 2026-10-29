#include <Runtime/ProcessEnvironmentRuntime.h>

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

constinit ProcessStorage<EnvironmentRootCriticalSection> paramsLock;
constinit ProcessStorage<EnvironmentRootCriticalSection> environmentLock;
constinit ProcessStorage<ParamsRuntimeRootOwner> paramsOwner;
constinit ProcessStorage<EnvironmentRootOwner> environmentOwner;
constinit ProcessStorage<EnvironmentStartupRuntime> environmentServices;

void __cdecl ReleaseParamsLockAt5AA230() noexcept { std::destroy_at(&paramsLock.Get()); }
void __cdecl ReleaseEnvironmentLockAt5AA150() noexcept { std::destroy_at(&environmentLock.Get()); }
// 原回调结束后getter仍可晚建正文；只释放当前发布槽，不结束owner/借用服务的C++生命期。
// 显式存储不会额外登记静态析构，也不会补删回调后新发布的正文。
void __cdecl ReleaseParamsAt5ABE20() noexcept { paramsOwner.Get().ReleaseAt5ABE20(); }
void __cdecl ReleaseEnvironmentAt5AB7D0() noexcept { environmentOwner.Get().ReleaseAt5AB7D0(); }

void __cdecl InitializeStringsAt5AD4AC()
{
    // 4013D2在根退出回调注册前调用417360，默认manager及其source必须活过根释放。
    static_cast<void>(DefaultRuntimeStringManagerSource().CaptureAt417360());
    static_cast<void>(DefaultEnvironmentRootMemory());
}
void __cdecl InitializeParamsLockAt5AD4F8()
{
    paramsLock.Construct();
    static_cast<void>(std::atexit(ReleaseParamsLockAt5AA230));
    paramsOwner.Construct(paramsLock.Get());
}
void __cdecl InitializeEnvironmentLockAt5AD500()
{
    environmentLock.Construct();
    static_cast<void>(std::atexit(ReleaseEnvironmentLockAt5AA150));
    environmentOwner.Construct(environmentLock.Get());
}
void __cdecl RegisterEnvironmentAt5AD62C()
{
    // 仅借已存在的两个进程owner；不调用498280，不建立第二个60h正文。
    environmentServices.Construct(paramsOwner.Get(), environmentOwner.Get());
    static_cast<void>(std::atexit(ReleaseEnvironmentAt5AB7D0));
}
void __cdecl RegisterParamsAt5AD68C()
{ static_cast<void>(std::atexit(ReleaseParamsAt5ABE20)); }
}

ParamsRuntimeRootOwner& ProcessParamsRuntimeRootOwner() noexcept { return paramsOwner.Get(); }
EnvironmentStartupRuntime& ProcessEnvironmentStartupRuntime() noexcept { return environmentServices.Get(); }
EnvironmentPathRuntime* CaptureProcessEnvironmentPathsAt4180C0()
{
    // 45A673/43D410/458BFD：根为空是原无效访问；paths base为空则返回0给后续原字段读取。
    auto* root = environmentOwner.Get().CaptureAt4180C0();
    return ResolveEnvironmentObjectReference<EnvironmentPathRuntime>(root->pathsAt48);
}

// v143/MSVC限定：以原PE初始化表位置排序，不能用函数地址或链接输入次序代替。
// 运行根4FC/5F0分别由RootCriticalSectionNative.Process和RootProcessRetirementNative.Process登记。
// 此处只保留Environment/Params原位置，不能重复登记运行根回调。
using Initializer = void (__cdecl*)();
#pragma section(".CRT$XCUKaGuYa5AD4AC", read)
#pragma section(".CRT$XCUKaGuYa5AD4F8", read)
#pragma section(".CRT$XCUKaGuYa5AD500", read)
#pragma section(".CRT$XCUKaGuYa5AD62C", read)
#pragma section(".CRT$XCUKaGuYa5AD68C", read)
extern "C" __declspec(allocate(".CRT$XCUKaGuYa5AD4AC")) Initializer const KaGuYaProcessStringsInitializer = InitializeStringsAt5AD4AC;
extern "C" __declspec(allocate(".CRT$XCUKaGuYa5AD4F8")) Initializer const KaGuYaProcessParamsLockInitializer = InitializeParamsLockAt5AD4F8;
extern "C" __declspec(allocate(".CRT$XCUKaGuYa5AD500")) Initializer const KaGuYaProcessEnvironmentLockInitializer = InitializeEnvironmentLockAt5AD500;
extern "C" __declspec(allocate(".CRT$XCUKaGuYa5AD62C")) Initializer const KaGuYaProcessEnvironmentInitializer = RegisterEnvironmentAt5AD62C;
extern "C" __declspec(allocate(".CRT$XCUKaGuYa5AD68C")) Initializer const KaGuYaProcessParamsInitializer = RegisterParamsAt5AD68C;
#pragma comment(linker, "/include:_KaGuYaProcessStringsInitializer")
#pragma comment(linker, "/include:_KaGuYaProcessParamsLockInitializer")
#pragma comment(linker, "/include:_KaGuYaProcessEnvironmentLockInitializer")
#pragma comment(linker, "/include:_KaGuYaProcessEnvironmentInitializer")
#pragma comment(linker, "/include:_KaGuYaProcessParamsInitializer")
}
