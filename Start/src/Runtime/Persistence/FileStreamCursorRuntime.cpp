#include <Runtime/FileStreamCursorRuntime.h>

#include <bit>
#include <limits>

namespace KaGuYa::Start::Runtime
{
std::uint64_t ReadFilePositionAt418590(IStartFileApi& api, HANDLE file) noexcept
{
    try
    {
        LONG high = 0;
        const auto low = api.MoveFilePointer(file, 0, &high, FILE_CURRENT);
        // 413770：非零Win32错误转换后均为负；低DWORD全1本身不是失败。
        if (low == INVALID_SET_FILE_POINTER && api.LastError() != ERROR_SUCCESS)
            return std::numeric_limits<std::uint64_t>::max();
        return (static_cast<std::uint64_t>(static_cast<std::uint32_t>(high)) << 32) | low;
    }
    catch (...) // 41861B：原局部catch将高、低DWORD均置FFFFFFFF。
    {
        return std::numeric_limits<std::uint64_t>::max();
    }
}

bool MoveFilePositionAt4348B0(
    IStartFileApi& api, HANDLE file, std::uint64_t distance, DWORD method)
{
    if (method != FILE_BEGIN && method != FILE_CURRENT && method != FILE_END)
        return false;
    auto high = std::bit_cast<LONG>(static_cast<DWORD>(distance >> 32));
    const auto low = api.MoveFilePointer(file,
        std::bit_cast<LONG>(static_cast<DWORD>(distance)), &high, method);
    // 原定位入口不设局部catch；调用者可忽略普通false，不能吞掉C++异常。
    return low != INVALID_SET_FILE_POINTER || api.LastError() == ERROR_SUCCESS;
}
}
