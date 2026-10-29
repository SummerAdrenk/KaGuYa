#pragma once

#include <cstdint>

namespace KaGuYa::Start::Runtime
{
class IStartFileApi;

// 47D160 的语义字段；不作为原 CFileStatus 的裸内存 ABI 使用。
struct FileStatusAt47D160 final
{
    std::int64_t creationTime = 0;
    std::int64_t lastAccessTime = 0;
    std::int64_t lastWriteTime = 0;
    std::uint64_t fileSize = 0;
    std::uint8_t attributes = 0;
};

// catch-all 返回 false，但保留失败前已按原顺序写入的字段。
[[nodiscard]] bool QueryFileStatusAt47D160(
    const char* path, IStartFileApi& files, FileStatusAt47D160& output) noexcept;
}
