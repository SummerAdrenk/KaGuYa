#include <Runtime/ArchiveCodecManagerRuntime.h>

#include <array>
#include <cstdlib>
#include <memory>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
// 显式存储不注册C++静态析构；owner和锁各只有下面的一次原退出回调。
alignas(EnvironmentRootCriticalSection) constinit std::array<std::byte,
    sizeof(EnvironmentRootCriticalSection)> lockStorage{};
alignas(ArchiveCodecManagerOwner) constinit std::array<std::byte,
    sizeof(ArchiveCodecManagerOwner)> ownerStorage{};
constinit std::array<std::uint8_t, 256> mtfDictionary{};

EnvironmentRootCriticalSection& ProcessLock() noexcept
{
    return *std::launder(reinterpret_cast<EnvironmentRootCriticalSection*>(lockStorage.data()));
}
ArchiveCodecManagerOwner& ProcessOwner() noexcept
{
    return *std::launder(reinterpret_cast<ArchiveCodecManagerOwner*>(ownerStorage.data()));
}

class ProcessSource final : public IArchiveCodecManagerSource
{
public:
    constexpr ProcessSource() noexcept = default;
    ArchiveCodecManagerRuntime* CaptureAt445910() override { return ProcessOwner().CaptureAt445910(); }
};
constinit ProcessSource source;

void __cdecl ReleaseLockAt5AA600() noexcept { std::destroy_at(&ProcessLock()); }
void __cdecl ReleaseManagerAt5AA640() noexcept { ProcessOwner().ReleaseAt5AA640(); }

void __cdecl InitializeCodecLockAt5AD508()
{
    // 单独链接Codec也必须让默认内存服务活过锁前晚取源，不能等50C注册owner时才创建。
    static_cast<void>(DefaultEnvironmentRootMemory());
    std::construct_at(reinterpret_cast<EnvironmentRootCriticalSection*>(lockStorage.data()));
    static_cast<void>(std::atexit(ReleaseLockAt5AA600));
}
void __cdecl RegisterCodecOwnerAt5AD50C()
{
    std::construct_at(reinterpret_cast<ArchiveCodecManagerOwner*>(ownerStorage.data()), ProcessLock());
    static_cast<void>(std::atexit(ReleaseManagerAt5AA640));
}
void __cdecl InitializeMtfAt5AD510()
{
    for (std::size_t index = 0; index < mtfDictionary.size(); ++index)
        mtfDictionary[index] = static_cast<std::uint8_t>(index);
}
}

IArchiveCodecManagerSource& ProcessArchiveCodecManagerSource() noexcept { return source; }
std::span<const std::uint8_t, 256> ProcessArchiveMtfDictionary() noexcept { return mtfDictionary; }

// 调用source/字典会拉入此静态库成员；需在实际调用的EXE验证CRT项，不强装其他根。
using Initializer = void (__cdecl*)();
#pragma section(".CRT$XCUKaGuYa5AD508", read)
#pragma section(".CRT$XCUKaGuYa5AD50C", read)
#pragma section(".CRT$XCUKaGuYa5AD510", read)
extern "C" __declspec(allocate(".CRT$XCUKaGuYa5AD508")) Initializer const KaGuYaArchiveCodecCrtInitializer = InitializeCodecLockAt5AD508;
extern "C" __declspec(allocate(".CRT$XCUKaGuYa5AD50C")) Initializer const KaGuYaArchiveCodecOwnerInitializer = RegisterCodecOwnerAt5AD50C;
extern "C" __declspec(allocate(".CRT$XCUKaGuYa5AD510")) Initializer const KaGuYaArchiveMtfInitializer = InitializeMtfAt5AD510;
#pragma comment(linker, "/include:_KaGuYaArchiveCodecCrtInitializer")
#pragma comment(linker, "/include:_KaGuYaArchiveCodecOwnerInitializer")
#pragma comment(linker, "/include:_KaGuYaArchiveMtfInitializer")
}
