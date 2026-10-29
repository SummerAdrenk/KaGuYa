#pragma once

#include "AudioFile.h"
#include <cstddef>
#include <cstdint>

namespace KaGuYa::Sound
{
// 借用输入文件，生命周期由VorbisDecoder保持到ov_clear之后；没有另一份游标镜像。
struct VorbisInputCallbacks final
{
    AudioFile& file;
    std::uint64_t segmentOffset;
    std::uint32_t segmentSize;

    static std::size_t Read(void*, std::size_t, std::size_t, void*) noexcept;
    static int Seek(void*, std::int64_t, int) noexcept;
    static long Tell(void*) noexcept;
    static int Close(void*) noexcept;
};
}
