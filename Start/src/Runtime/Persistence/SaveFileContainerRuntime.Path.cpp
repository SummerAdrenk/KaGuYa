#include "SaveFileContainerRuntime.Internal.h"
#include <Runtime/FileStatusRuntime.h>
#include <Runtime/RuntimeCStringConcatenation.h>
#include <bit>
#include <mbstring.h>

namespace KaGuYa::Start::Runtime::SaveFileContainerDetail
{
void ResolveContainerPath(RuntimeCString& path, RuntimeStringBorrow fixedDirectory,
    const char* input, ISaveFileContainerEnvironmentSource* environment,
    IRuntimeStringManagerSource& strings)
{
    // 4C7450..4C74E0 / 4C7B3E..4C7BCE：先按 C 字符串复制，再用原 MBCS 查找。
    // 4C7210 仅拼接目录原文；不补分隔符，不把 '/' 或 ':' 当作该分支的 '\\'。
    const auto* hit = path.GetLength() > 0 ?
        _mbschr(reinterpret_cast<const unsigned char*>(path.GetString()), '\\') : nullptr;
    if (hit && std::bit_cast<std::int32_t>(std::uint32_t(reinterpret_cast<std::uintptr_t>(hit)-
        reinterpret_cast<std::uintptr_t>(path.GetString()))) >= 0) return;
    // 构造模式明确选择固定或逐次来源，不是失败后换路；suffix沿用原ESI输入指针。
    const auto directory = environment ? environment->CaptureDirectoryAt4180C0() : fixedDirectory;
    RuntimeCStringConcatenation joined(directory, input, strings);
    AssignRuntimeCStringAt410220(path, joined.Value());
}

std::uint64_t ReadContainerLastWriteTime(IStartFileApi& files, const char* path)
{
    FileStatusAt47D160 status;
    // 4C755E/4C7568 -> 4C7910：47D160 的局部 false 在容器层重新抛 int 2。
    if (!QueryFileStatusAt47D160(path, files, status)) throw 2;
    return static_cast<std::uint64_t>(status.lastWriteTime);
}
}
