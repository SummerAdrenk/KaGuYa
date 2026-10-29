#pragma once

#include <Windows.h>
#include <cstdint>

namespace KaGuYa::Sound
{
// 原FileStream的同步只读文件域；不用CRT预读缓冲替代ReadFile/物理位置合同。
class AudioFile final
{
public:
    AudioFile() noexcept = default;
    ~AudioFile() noexcept;
    AudioFile(const AudioFile&) = delete;
    AudioFile& operator=(const AudioFile&) = delete;
    bool Open(const wchar_t* path) noexcept;
    bool Close() noexcept;
    bool Read(void* destination, DWORD requested, DWORD& actual) noexcept;
    bool Seek(std::int64_t offset, DWORD origin) noexcept;
    std::int64_t Position() const noexcept;
    std::uint64_t Size() const noexcept;
private:
    HANDLE handle_ = INVALID_HANDLE_VALUE;
};
}
