#include <Runtime/RootProcessRetirementNative.h>
#include <Runtime/RootCriticalSectionNative.h>
#include <cstdlib>

namespace KaGuYa::Start::Runtime
{
namespace
{
constinit RootRuntimeOwner* volatile ownerMetadata = nullptr;
void __cdecl ReleaseRootAt5AB3C0() noexcept
{ RetireRootProcessNativeAt5AB3C0(ProcessRootCriticalSectionNativeReference(), ownerMetadata); }
void __cdecl RegisterRootAt5AD5F0()
{
    // 原4036D0只有atexit登记，不为退出构造owner/Env/完整图。
    static_cast<void>(std::atexit(ReleaseRootAt5AB3C0));
}
}
void BindProcessRootRuntimeOwnerMetadata(RootRuntimeOwner& owner) noexcept { ownerMetadata = &owner; }
RootRuntimeOwner* BorrowProcessRootRuntimeOwnerMetadata() noexcept { return ownerMetadata; }
using Initializer = void(__cdecl*)();
#pragma section(".CRT$XCUKaGuYa5AD5F0", read)
extern "C" __declspec(allocate(".CRT$XCUKaGuYa5AD5F0")) Initializer const KaGuYaProcessRootRetirementInitializer = RegisterRootAt5AD5F0;
#pragma comment(linker, "/include:_KaGuYaProcessRootRetirementInitializer")
}
