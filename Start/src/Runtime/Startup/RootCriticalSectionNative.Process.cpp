#include <Runtime/RootCriticalSectionNative.h>
#include <Runtime/ProcessWinHookRuntime.h>
#include <array>
#include <cstdlib>
#include <memory>
#include <type_traits>

namespace KaGuYa::Start::Runtime
{
namespace
{
// 原60113C数据项：表在CRT开始前已经存在；OS临界区和追踪值由4010C0初始化。
constinit CriticalSectionNativeBody24h rootLock{&WinHookCriticalSectionNativeTable, {}, 0, 0};
alignas(RootCriticalSectionNativeReference) constinit std::array<std::byte, sizeof(RootCriticalSectionNativeReference)> referenceStorage{};
RootCriticalSectionNativeReference& Reference() noexcept
{ return *std::launder(reinterpret_cast<RootCriticalSectionNativeReference*>(referenceStorage.data())); }
void __cdecl ReleaseLockAt5AA1D0() noexcept
{ RetireRootCriticalSectionNativeAt5AA1D0(rootLock, ProcessWinHookNativeServices()); }
void __cdecl InitializeLockAt5AD4FC()
{
    auto& services = ProcessWinHookNativeServices(); // 5AD4E4已构造，只借原API/内存服务。
    InitializeRootCriticalSectionNativeAt4010C0(rootLock, services);
    static_cast<void>(std::atexit(ReleaseLockAt5AA1D0));
    std::construct_at(reinterpret_cast<RootCriticalSectionNativeReference*>(referenceStorage.data()), rootLock, services);
}
static_assert(std::is_trivially_destructible_v<RootCriticalSectionNativeReference>);
}
RootCriticalSectionNativeReference& ProcessRootCriticalSectionNativeReference() noexcept { return Reference(); }
CriticalSectionNativeBody24h& ProcessRootCriticalSectionNativeBody() noexcept { return rootLock; }
using Initializer = void(__cdecl*)();
#pragma section(".CRT$XCUKaGuYa5AD4FC", read)
extern "C" __declspec(allocate(".CRT$XCUKaGuYa5AD4FC")) Initializer const KaGuYaProcessRootLockInitializer = InitializeLockAt5AD4FC;
#pragma comment(linker, "/include:_KaGuYaProcessRootLockInitializer")
}
