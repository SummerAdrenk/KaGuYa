#include <Runtime/FileStatusRuntime.h>
#include <Runtime/PersistenceFileAccess.h>

#include <ctime>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
std::int64_t ConvertTimeAt45E1A0(const FILETIME& value)
{
    FILETIME local{};
    SYSTEMTIME system{};
    if (!FileTimeToLocalFileTime(&value, &local) || !FileTimeToSystemTime(&local, &system))
        throw std::runtime_error("file status FILETIME conversion failed");
    if (system.wYear < 1900) return 0;
    if (system.wMonth < 1 || system.wMonth > 12 || system.wDay < 1 || system.wDay > 31 ||
        system.wHour > 23 || system.wMinute > 59 || system.wSecond > 59)
        throw std::runtime_error("file status timestamp fields are invalid");
    std::tm time{};
    time.tm_year = system.wYear - 1900;
    time.tm_mon = system.wMonth - 1;
    time.tm_mday = system.wDay;
    time.tm_hour = system.wHour;
    time.tm_min = system.wMinute;
    time.tm_sec = system.wSecond;
    time.tm_isdst = -1;
    const auto result = _mktime64(&time);
    if (result == -1) throw std::runtime_error("file status timestamp is outside the CTime range");
    return result;
}
}

bool QueryFileStatusAt47D160(
    const char* path, IStartFileApi& files, FileStatusAt47D160& output) noexcept
{
    try
    {
        WIN32_FIND_DATAA data{};
        const HANDLE find = files.FindFirst(path, &data);
        if (find == INVALID_HANDLE_VALUE) throw 1;
        if (!files.FindCloseHandle(find)) throw 2;

        // 47D1D2..47D23F：关闭查找句柄后先写属性/大小，再依次提交三个时间。
        output.attributes = static_cast<std::uint8_t>(data.dwFileAttributes & 0x7fu);
        output.fileSize = (static_cast<std::uint64_t>(data.nFileSizeHigh) << 32u) | data.nFileSizeLow;
        output.creationTime = ConvertTimeAt45E1A0(data.ftCreationTime);
        output.lastAccessTime = ConvertTimeAt45E1A0(data.ftLastAccessTime);
        output.lastWriteTime = ConvertTimeAt45E1A0(data.ftLastWriteTime);
        // 47D242..47D263：仅在三个转换均成功后，零 creation/access 才取 write。
        if (output.creationTime == 0) output.creationTime = output.lastWriteTime;
        if (output.lastAccessTime == 0) output.lastAccessTime = output.lastWriteTime;
        return true;
    }
    catch (...)
    {
        // 47D26A/47D270：此层负责吞异常，调用方分别决定是否继续或另抛 int。
        return false;
    }
}
}
