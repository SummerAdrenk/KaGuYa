#pragma once

#include <Windows.h>

#include <algorithm>
#include <limits>
#include <mutex>
#include <string>
#include <utility>

#include <spdlog/details/log_msg.h>
#include <spdlog/sinks/base_sink.h>

namespace KaGuYa::Logging
{
// 使用 Win32 宽字符路径创建文件，确保安装在中文或日文目录时仍能可靠写入 UTF-8 日志。
template <typename Mutex>
class WinUtf8FileSink final : public spdlog::sinks::base_sink<Mutex>
{
public:
    explicit WinUtf8FileSink(std::wstring path, const bool truncate = false)
        : path_(std::move(path)), truncate_(truncate)
    {
        if (!EnsureOpened())
        {
            ThrowFailure("KYG_LOG_OPEN", GetLastError());
        }
    }

    ~WinUtf8FileSink() override
    {
        Close();
    }

protected:
    void sink_it_(const spdlog::details::log_msg& message) override
    {
        if (!EnsureOpened())
        {
            ThrowFailure("KYG_LOG_OPEN", GetLastError());
        }

        spdlog::memory_buf_t formatted;
        this->formatter_->format(message, formatted);
        std::size_t offset = 0;
        while (offset < formatted.size())
        {
            const DWORD chunk = static_cast<DWORD>((std::min)(
                formatted.size() - offset,
                static_cast<std::size_t>((std::numeric_limits<DWORD>::max)())));
            DWORD written = 0;
            const BOOL writeOk = WriteFile(file_, formatted.data() + offset, chunk, &written, nullptr);
            if (!writeOk || written == 0 || written > chunk)
            {
                ThrowFailure("KYG_LOG_WRITE", writeOk ? ERROR_WRITE_FAULT : GetLastError());
            }
            offset += written;
        }
    }

    void flush_() override
    {
        if (file_ != INVALID_HANDLE_VALUE && !FlushFileBuffers(file_))
        {
            ThrowFailure("KYG_LOG_FLUSH", GetLastError());
        }
    }

private:
    [[noreturn]] static void ThrowFailure(const char* const code, const DWORD error)
    {
        SetLastError(error);
        spdlog::throw_spdlog_ex(code, static_cast<int>(error));
    }

    static std::wstring ParentDirectory(const std::wstring& path)
    {
        const auto separator = path.find_last_of(L"\\/");
        return separator == std::wstring::npos ? std::wstring{} : path.substr(0, separator);
    }

    static bool EnsureDirectory(const std::wstring& directory)
    {
        if (directory.empty())
        {
            return true;
        }

        DWORD attributes = GetFileAttributesW(directory.c_str());
        if (attributes != INVALID_FILE_ATTRIBUTES)
        {
            return (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
        }
        const DWORD lookupError = GetLastError();
        if (lookupError != ERROR_FILE_NOT_FOUND && lookupError != ERROR_PATH_NOT_FOUND)
        {
            return false;
        }

        const std::wstring parent = ParentDirectory(directory);
        if (!parent.empty() && parent != directory && !EnsureDirectory(parent))
        {
            return false;
        }
        if (CreateDirectoryW(directory.c_str(), nullptr))
        {
            return true;
        }
        if (GetLastError() != ERROR_ALREADY_EXISTS)
        {
            return false;
        }
        attributes = GetFileAttributesW(directory.c_str());
        return attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
    }

    bool EnsureOpened()
    {
        if (file_ != INVALID_HANDLE_VALUE)
        {
            return true;
        }
        if (!EnsureDirectory(ParentDirectory(path_)))
        {
            return false;
        }

        file_ = CreateFileW(
            path_.c_str(),
            truncate_ ? GENERIC_WRITE : FILE_APPEND_DATA,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
            nullptr,
            truncate_ ? CREATE_ALWAYS : OPEN_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            nullptr);
        truncate_ = false;
        if (file_ == INVALID_HANDLE_VALUE)
        {
            return false;
        }

        LARGE_INTEGER size{};
        if (!GetFileSizeEx(file_, &size))
        {
            return false;
        }
        if (size.QuadPart == 0)
        {
            // UTF-8 正文自身没有歧义，但部分 Windows 编辑器会把无 BOM 文件按系统 ANSI 打开。
            // sink 只在空文件开头写一次 BOM，后续仍由 spdlog 原样写入 UTF-8 字节。
            constexpr unsigned char bom[]{0xEF, 0xBB, 0xBF};
            DWORD written = 0;
            if (!WriteFile(file_, bom, sizeof(bom), &written, nullptr) || written != sizeof(bom))
            {
                return false;
            }
        }
        return true;
    }

    void Close() noexcept
    {
        if (file_ != INVALID_HANDLE_VALUE)
        {
            CloseHandle(file_);
            file_ = INVALID_HANDLE_VALUE;
        }
    }

    std::wstring path_;
    HANDLE file_ = INVALID_HANDLE_VALUE;
    bool truncate_ = false;
};

using WinUtf8FileSinkMt = WinUtf8FileSink<std::mutex>;
}
