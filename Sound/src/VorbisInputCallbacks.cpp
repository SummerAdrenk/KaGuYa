#include "VorbisInputCallbacks.h"
#include <Logging/Logger.h>
#include <bit>

namespace KaGuYa::Sound
{
std::size_t VorbisInputCallbacks::Read(void* destination, std::size_t size, std::size_t count, void* context) noexcept
{
    if (!context || !destination) return 0;
    auto& self = *static_cast<VorbisInputCallbacks*>(context);
    // 03 10006770/04 10006790：DWORD乘积/段尾裁剪，回传字节数，不除以size。
    const auto desired = static_cast<std::uint32_t>(size * count);
    const std::uint32_t expression = static_cast<std::uint32_t>(self.file.Position()) -
        self.segmentSize - static_cast<std::uint32_t>(self.segmentOffset) + desired;
    const auto excess = std::bit_cast<std::int32_t>(expression) >= 0 ? expression : 0u;
    const auto requested = desired - excess;
    if (requested > desired)
    {
        Logging::Write(Logging::Level::Error, "Vorbis callback read underflow exceeds destination: rejected.");
        return 0;
    }
    DWORD bytes = 0;
    return self.file.Read(destination, requested, bytes) ? bytes : 0;
}

int VorbisInputCallbacks::Seek(void* context, std::int64_t offset, int origin) noexcept
{
    if (!context) return -1;
    auto& self = *static_cast<VorbisInputCallbacks*>(context);
    std::int64_t position = offset;
    DWORD method = FILE_CURRENT;
    if (origin == 0)
    {
        if (offset < 0 || offset > self.segmentSize) return -1;
        position = std::bit_cast<std::int64_t>(self.segmentOffset + static_cast<std::uint64_t>(offset));
        method = FILE_BEGIN;
    }
    else if (origin == 2)
    {
        if (offset > 0 || offset < -static_cast<std::int64_t>(self.segmentSize)) return -1;
        // 原04 1000696E的END不加segment起点；SET才加，不能统一成相对段坐标。
        position = static_cast<std::int64_t>(self.segmentSize) + offset;
        method = FILE_BEGIN;
    }
    if (!self.file.Seek(position, method)) return -1;
    return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(self.file.Position()));
}

long VorbisInputCallbacks::Tell(void* context) noexcept
{
    if (!context) return -1;
    // 03 10006BD0/04 10006BF0直接返回物理位置低DWORD，库内部另维护逻辑offset。
    auto& self = *static_cast<VorbisInputCallbacks*>(context);
    return std::bit_cast<long>(static_cast<std::uint32_t>(self.file.Position()));
}

int VorbisInputCallbacks::Close(void* context) noexcept
{
    if (!context) return -1;
    // 10006B86调用FileStream+18h，普通返回不参与成功判定；不是bool -> 0/-1转换。
    if (!static_cast<VorbisInputCallbacks*>(context)->file.Close())
        Logging::Write(Logging::Level::Warning, "Vorbis file CloseHandle ordinary failure; original callback returns zero.");
    return 0;
}
}
