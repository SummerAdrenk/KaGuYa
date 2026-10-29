#pragma once

#include <Runtime/PersistenceFileAccess.h>

namespace KaGuYa::Start::Runtime
{
// 已打开文件的游标叶；不代表完整 CStreamFile 的正文或虚表布局。
[[nodiscard]] std::uint64_t ReadFilePositionAt418590(IStartFileApi& api, HANDLE file) noexcept;
[[nodiscard]] bool MoveFilePositionAt4348B0(
    IStartFileApi& api, HANDLE file, std::uint64_t distance, DWORD method);
}
