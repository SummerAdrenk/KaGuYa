#include "SaveFileContainerRuntime.Internal.h"
#include <bit>
#include <limits>
#include <stdexcept>

namespace KaGuYa::Start::Runtime::SaveFileContainerDetail
{
ContainerFile::ContainerFile(IStartFileApi& files, const char* path, bool writing, bool native)
    : files_(files), native_(native)
{
    SECURITY_ATTRIBUTES security{sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE};
    const FileOpenRequest request{path, writing ? GENERIC_WRITE : GENERIC_READ, 0,
        static_cast<DWORD>(writing ? CREATE_ALWAYS : OPEN_EXISTING), FILE_ATTRIBUTE_NORMAL, &security};
    if (native_)
    {
        // 原state3在42BAE0正常返回后才取得；Open抛出不补回收其部分发布。
        stream_.value.OpenWithApi(request, files_);
        nativeArmed_ = true;
        return;
    }
    file_ = OpenFileUsingStartPolicy(request, nullptr, files_);
    if (file_ == INVALID_HANDLE_VALUE)
    {
        if (files_.LastError() != ERROR_SUCCESS) throw 1;
        file_ = nullptr;
    }
}
ContainerFile::~ContainerFile() noexcept
{
    if (nativeArmed_) DestroyFileStreamAt40ED70(&stream_.value, files_);
    else if (!native_ && file_) files_.Close(file_);
}
void ContainerFile::RequireOpen()
{
    if (native_ && !stream_.value.fileAt04.baseAt00) throw 1; // 原419350只查Ref；允许非空Ref/空HANDLE。
}
void ContainerFile::Read(void* data, std::uint32_t count)
{
    if (native_)
    {
        if (!stream_.value.ReadExactWithApi(data, count, files_)) throw 3;
        return;
    }
    DWORD actual = 0;
    const BOOL ok = files_.Read(file_, data, count, &actual);
    if (ok ? actual != count : files_.LastError() != ERROR_SUCCESS) throw 3;
}
void ContainerFile::Write(const void* data, std::uint32_t count)
{
    if (native_)
    {
        if (!stream_.value.WriteWithApi(data, count, files_)) throw 2;
        return;
    }
    DWORD actual = 0;
    if (!files_.Write(file_, data, count, &actual) && files_.LastError() != ERROR_SUCCESS) throw 2;
}
void WorkBuffer::Allocate(std::uint32_t count)
{
    if (native_) { reference_.AllocateAt412700(count); return; }
    std::vector<std::byte>().swap(legacy_);
    if (count > static_cast<std::uint32_t>((std::numeric_limits<std::int32_t>::max)()))
        throw std::length_error("save block exceeds original signed allocation range");
    legacy_.resize(count); // 旧显式域仍保留未读尾部清零政策，不挪入原生数组。
}
std::byte* WorkBuffer::Data() noexcept
{ return native_ ? reinterpret_cast<std::byte*>(reference_.Data()) : legacy_.data(); }
std::uint32_t WorkBuffer::Size() const noexcept
{ return native_ ? reference_.Size() : static_cast<std::uint32_t>(legacy_.size()); }
void WorkBuffer::XorAt4C7AA0(std::uint32_t seed) noexcept
{
    const auto key = static_cast<std::uint8_t>(seed);
    if (!native_)
    {
        for (auto& value : legacy_) value ^= static_cast<std::byte>(key);
        return;
    }
    // 4C7AA8..4C7B07：每轮重取control/count，以有符号比较结束；
    // 读、写分别取stride/bias，不能将原Ref简化成连续span或缓存整段长度。
    const auto* slot = reinterpret_cast<const volatile std::uint32_t*>(&reference_.RawSlot());
    for (std::uint32_t index = 0;; ++index)
    {
        const auto* control = reinterpret_cast<const volatile std::uint32_t*>(slot[2]);
        const auto count = control ? control[4] : 0u;
        if (std::bit_cast<std::int32_t>(index) >= std::bit_cast<std::int32_t>(count)) return;
        const auto base = slot[0];
        std::uint32_t address = 0;
        if (base)
        {
            const auto stride = control ? control[3] : 0u;
            const auto bias = slot[1];
            address = base + (bias + index) * stride;
        }
        const auto value = static_cast<std::uint8_t>(
            *reinterpret_cast<const volatile std::uint8_t*>(address) ^ key);
        address = 0;
        if (base)
        {
            const auto stride = control ? control[3] : 0u;
            const auto bias = slot[1];
            address = base + (bias + index) * stride;
        }
        *reinterpret_cast<volatile std::uint8_t*>(address) = value;
    }
}
}
