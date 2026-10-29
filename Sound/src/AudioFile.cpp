#include "AudioFile.h"
#include <bit>
#include <limits>
#include <utility>

namespace KaGuYa::Sound
{
AudioFile::~AudioFile() noexcept { static_cast<void>(Close()); }

bool AudioFile::Open(const wchar_t* path) noexcept
{
    if (handle_ != INVALID_HANDLE_VALUE) return false;
    // 03/04输入工厂均是2/1/1模式：GENERIC_READ、FILE_SHARE_READ、OPEN_EXISTING。
    handle_ = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    return handle_ != INVALID_HANDLE_VALUE;
}

bool AudioFile::Close() noexcept
{
    if (handle_ == INVALID_HANDLE_VALUE) return true;
    // FileStream 10005910先清引用，handle deleter 10004EE0忽略普通关闭失败后仍清槽。
    const auto closing = std::exchange(handle_, INVALID_HANDLE_VALUE);
    return CloseHandle(closing) != FALSE;
}

bool AudioFile::Read(void* destination, DWORD requested, DWORD& actual) noexcept
{
    actual = 0;
    if (handle_ == INVALID_HANDLE_VALUE) return false;
    return ReadFile(handle_, destination, requested, &actual, nullptr) != FALSE;
}

bool AudioFile::Seek(std::int64_t offset, DWORD origin) noexcept
{
    if (handle_ == INVALID_HANDLE_VALUE) return false;
    const auto bits = std::bit_cast<std::uint64_t>(offset);
    LONG high = std::bit_cast<LONG>(static_cast<std::uint32_t>(bits >> 32));
    const DWORD low = SetFilePointer(handle_, std::bit_cast<LONG>(static_cast<std::uint32_t>(bits)), &high, origin);
    return low != INVALID_SET_FILE_POINTER || GetLastError() == NO_ERROR;
}

std::int64_t AudioFile::Position() const noexcept
{
    if (handle_ == INVALID_HANDLE_VALUE) return -1;
    LONG high = 0;
    const DWORD low = SetFilePointer(handle_, 0, &high, FILE_CURRENT);
    if (low == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR) return -1;
    return std::bit_cast<std::int64_t>((static_cast<std::uint64_t>(static_cast<std::uint32_t>(high)) << 32) | low);
}

std::uint64_t AudioFile::Size() const noexcept
{
    if (handle_ == INVALID_HANDLE_VALUE) return (std::numeric_limits<std::uint64_t>::max)();
    DWORD high = 0;
    const DWORD low = GetFileSize(handle_, &high);
    if (low == INVALID_FILE_SIZE && GetLastError() != NO_ERROR)
        return (std::numeric_limits<std::uint64_t>::max)();
    return (static_cast<std::uint64_t>(high) << 32) | low;
}
}
