#pragma once

#include <Windows.h>

#include <cstddef>
#include <cstdint>
#include <vector>

namespace KaGuYa::Start::Runtime
{
struct FileTransactionContext final
{
    HANDLE transactionHandleAt00 = nullptr;
    std::uint32_t allowOrdinaryFileAt04 = 0;
};

struct FileOpenRequest final
{
    const char* path = nullptr;
    DWORD desiredAccess = 0;
    DWORD shareMode = 0;
    DWORD creationDisposition = 0;
    DWORD flagsAndAttributes = 0;
    LPSECURITY_ATTRIBUTES securityAttributes = nullptr;
};

class IStartFileApi
{
public:
    virtual ~IStartFileApi() = default;

    [[nodiscard]] virtual HMODULE Kernel32Module() = 0;
    [[nodiscard]] virtual FARPROC ResolveCreateFileTransactedA(HMODULE module) = 0;
    [[nodiscard]] virtual HANDLE CreateOrdinaryFile(const FileOpenRequest& request) = 0;
    [[nodiscard]] virtual HANDLE CreateTransactedFile(
        FARPROC procedure,
        const FileOpenRequest& request,
        HANDLE transaction) = 0;
    [[nodiscard]] virtual DWORD FileSizeLow(HANDLE file, DWORD* sizeHigh) = 0;
    [[nodiscard]] virtual DWORD GetAttributes(const char* path) = 0;
    [[nodiscard]] virtual HANDLE FindFirst(
        const char* path,
        WIN32_FIND_DATAA* findData) = 0;
    [[nodiscard]] virtual BOOL FindCloseHandle(HANDLE findHandle) = 0;
    [[nodiscard]] virtual BOOL SeekAbsolute(HANDLE file, std::uint64_t offset) = 0;
    [[nodiscard]] virtual DWORD MoveFilePointer(
        HANDLE file, LONG distanceLow, LONG* distanceHigh, DWORD moveMethod) = 0;
    [[nodiscard]] virtual BOOL SetEndOfFileAtCurrent(HANDLE file) = 0;
    [[nodiscard]] virtual BOOL Read(
        HANDLE file,
        void* buffer,
        DWORD byteCount,
        DWORD* bytesRead) = 0;
    [[nodiscard]] virtual BOOL Write(
        HANDLE file,
        const void* buffer,
        DWORD byteCount,
        DWORD* bytesWritten) = 0;
    [[nodiscard]] virtual DWORD LastError() const = 0;
    virtual void Close(HANDLE file) = 0;
};

class Win32StartFileApi final : public IStartFileApi
{
public:
    [[nodiscard]] HMODULE Kernel32Module() override;
    [[nodiscard]] FARPROC ResolveCreateFileTransactedA(HMODULE module) override;
    [[nodiscard]] HANDLE CreateOrdinaryFile(const FileOpenRequest& request) override;
    [[nodiscard]] HANDLE CreateTransactedFile(
        FARPROC procedure,
        const FileOpenRequest& request,
        HANDLE transaction) override;
    [[nodiscard]] DWORD FileSizeLow(HANDLE file, DWORD* sizeHigh) override;
    [[nodiscard]] DWORD GetAttributes(const char* path) override;
    [[nodiscard]] HANDLE FindFirst(
        const char* path,
        WIN32_FIND_DATAA* findData) override;
    [[nodiscard]] BOOL FindCloseHandle(HANDLE findHandle) override;
    [[nodiscard]] BOOL SeekAbsolute(HANDLE file, std::uint64_t offset) override;
    [[nodiscard]] DWORD MoveFilePointer(
        HANDLE file, LONG distanceLow, LONG* distanceHigh, DWORD moveMethod) override;
    [[nodiscard]] BOOL SetEndOfFileAtCurrent(HANDLE file) override;
    [[nodiscard]] BOOL Read(
        HANDLE file,
        void* buffer,
        DWORD byteCount,
        DWORD* bytesRead) override;
    [[nodiscard]] BOOL Write(
        HANDLE file,
        const void* buffer,
        DWORD byteCount,
        DWORD* bytesWritten) override;
    [[nodiscard]] DWORD LastError() const override;
    void Close(HANDLE file) override;
};

enum class PersistenceIoStatus : int
{
    Success = 0,
    Failure = 1,
};

[[nodiscard]] HANDLE OpenFileUsingStartPolicy(
    const FileOpenRequest& request,
    const FileTransactionContext* transactionContext,
    IStartFileApi& api);

[[nodiscard]] PersistenceIoStatus ReadWholePersistenceFile(
    const char* path,
    std::vector<std::byte>& bytes,
    const FileTransactionContext* transactionContext,
    IStartFileApi& api);

[[nodiscard]] PersistenceIoStatus WriteWholePersistenceFile(
    const char* path,
    const std::vector<std::byte>& bytes,
    const FileTransactionContext* transactionContext,
    IStartFileApi& api);
}
