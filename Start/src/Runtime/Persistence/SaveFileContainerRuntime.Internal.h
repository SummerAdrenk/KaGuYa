#pragma once

#include <Runtime/ByteArrayReferenceRuntime.h>
#include <Runtime/FileStreamRuntime.h>
#include <Runtime/SaveFileMetadataLoader.h>
#include <Runtime/SaveFileContainerRuntime.h>
#include <vector>

namespace KaGuYa::Start::Runtime::SaveFileContainerDetail
{
class ContainerFile final
{
public:
    ContainerFile(IStartFileApi&, const char* path, bool writing, bool native);
    ~ContainerFile() noexcept;
    ContainerFile(const ContainerFile&) = delete;
    ContainerFile& operator=(const ContainerFile&) = delete;
    void RequireOpen();
    void Read(void* data, std::uint32_t count);
    void Write(const void* data, std::uint32_t count);
private:
    IStartFileApi& files_;
    bool native_;
    bool nativeArmed_ = false;
    HANDLE file_ = nullptr;
    union Stream final
    {
        FileStreamRuntime value;
        Stream() noexcept : value() {}
        ~Stream() noexcept {}
    } stream_;
};
class WorkBuffer final
{
public:
    explicit WorkBuffer(bool native) noexcept : native_(native) {}
    void Allocate(std::uint32_t count);
    void XorAt4C7AA0(std::uint32_t seed) noexcept;
    [[nodiscard]] std::byte* Data() noexcept;
    [[nodiscard]] std::uint32_t Size() const noexcept;
    [[nodiscard]] bool IsNative() const noexcept { return native_; }
    [[nodiscard]] ByteArrayReferenceRuntime& Native() noexcept { return reference_; }
private:
    std::vector<std::byte> legacy_;
    ByteArrayReferenceRuntime reference_;
    bool native_;
};
void WriteContainerHeader(ContainerFile&, WorkBuffer&, const SaveFileFormatProfile&, std::uint32_t seed);
std::uint32_t ReadContainerHeader(ContainerFile&, WorkBuffer&, const SaveFileFormatProfile&, std::uint32_t& firstSize);
void ResolveContainerPath(RuntimeCString&, RuntimeStringBorrow, const char*,
    ISaveFileContainerEnvironmentSource*, IRuntimeStringManagerSource&);
std::uint64_t ReadContainerLastWriteTime(IStartFileApi&, const char*);
}
