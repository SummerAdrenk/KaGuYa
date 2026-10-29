#include <Runtime/PersistenceFileAccess.h>
#include <Logging/Logger.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
DWORD FileLengthAt417A80(const HANDLE file, IStartFileApi& api)
{
    try
    {
        DWORD sizeHigh = 0;
        const DWORD size = api.FileSizeLow(file, &sizeHigh);
        if (size == INVALID_FILE_SIZE && api.LastError() != ERROR_SUCCESS) throw 2;
        return size;
    }
    catch (...)
    {
        // 417AFF/5DEF00：原函数自己的catch(...)把EDX:EAX清零，异常不传给480550。
        KaGuYa::Logging::Write(KaGuYa::Logging::Level::Error,
            "CStreamFile size query recovered at 0x417AFF; original returns zero length.");
        return 0;
    }
}

class ScopedFile final
{
public:
    ScopedFile(const HANDLE file, IStartFileApi& api) noexcept : file_(file), api_(api) {}
    ~ScopedFile()
    {
        if (file_ != INVALID_HANDLE_VALUE)
        {
            api_.Close(file_);
        }
    }

    ScopedFile(const ScopedFile&) = delete;
    ScopedFile& operator=(const ScopedFile&) = delete;

private:
    HANDLE file_ = INVALID_HANDLE_VALUE;
    IStartFileApi& api_;
};

FileOpenRequest MakePersistenceRequest(
    const char* path,
    const DWORD desiredAccess,
    const DWORD creationDisposition,
    SECURITY_ATTRIBUTES& securityAttributes) noexcept
{
    securityAttributes.nLength = sizeof(SECURITY_ATTRIBUTES);
    securityAttributes.lpSecurityDescriptor = nullptr;
    securityAttributes.bInheritHandle = TRUE;
    return {path, desiredAccess, 0, creationDisposition, FILE_ATTRIBUTE_NORMAL,
            &securityAttributes};
}
}

HMODULE Win32StartFileApi::Kernel32Module()
{
    return GetModuleHandleA("kernel32.dll");
}

FARPROC Win32StartFileApi::ResolveCreateFileTransactedA(const HMODULE module)
{
    return GetProcAddress(module, "CreateFileTransactedA");
}

HANDLE Win32StartFileApi::CreateOrdinaryFile(const FileOpenRequest& request)
{
    return CreateFileA(request.path, request.desiredAccess, request.shareMode,
        request.securityAttributes, request.creationDisposition, request.flagsAndAttributes,
        nullptr);
}

HANDLE Win32StartFileApi::CreateTransactedFile(
    const FARPROC procedure,
    const FileOpenRequest& request,
    const HANDLE transaction)
{
    using CreateFileTransactedAProcedure = HANDLE(WINAPI*)(LPCSTR, DWORD, DWORD,
        LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE, HANDLE, PUSHORT, PVOID);
    const auto createFileTransacted = reinterpret_cast<CreateFileTransactedAProcedure>(procedure);
    return createFileTransacted(request.path, request.desiredAccess, request.shareMode,
        request.securityAttributes, request.creationDisposition, request.flagsAndAttributes,
        nullptr, transaction, nullptr, nullptr);
}

DWORD Win32StartFileApi::FileSizeLow(const HANDLE file, DWORD* const sizeHigh)
{
    return GetFileSize(file, sizeHigh);
}

DWORD Win32StartFileApi::GetAttributes(const char* const path)
{
    return GetFileAttributesA(path);
}

HANDLE Win32StartFileApi::FindFirst(
    const char* const path,
    WIN32_FIND_DATAA* const findData)
{
    return FindFirstFileA(path, findData);
}

BOOL Win32StartFileApi::FindCloseHandle(const HANDLE findHandle)
{
    return FindClose(findHandle);
}

BOOL Win32StartFileApi::SeekAbsolute(
    const HANDLE file,
    const std::uint64_t offset)
{
    LARGE_INTEGER distance{};
    distance.QuadPart = static_cast<LONGLONG>(offset);
    return SetFilePointerEx(file, distance, nullptr, FILE_BEGIN);
}

DWORD Win32StartFileApi::MoveFilePointer(
    const HANDLE file,
    const LONG distanceLow,
    LONG* const distanceHigh,
    const DWORD moveMethod)
{
    return SetFilePointer(file, distanceLow, distanceHigh, moveMethod);
}

BOOL Win32StartFileApi::SetEndOfFileAtCurrent(const HANDLE file)
{
    // 43696B..43696D: 直接传入句柄，按当前文件位置设置 EOF。
    return ::SetEndOfFile(file);
}

BOOL Win32StartFileApi::Read(
    const HANDLE file,
    void* const buffer,
    const DWORD byteCount,
    DWORD* const bytesRead)
{
    return ReadFile(file, buffer, byteCount, bytesRead, nullptr);
}

BOOL Win32StartFileApi::Write(
    const HANDLE file,
    const void* const buffer,
    const DWORD byteCount,
    DWORD* const bytesWritten)
{
    return WriteFile(file, buffer, byteCount, bytesWritten, nullptr);
}

DWORD Win32StartFileApi::LastError() const
{
    return GetLastError();
}

void Win32StartFileApi::Close(const HANDLE file)
{
    CloseHandle(file);
}

HANDLE OpenFileUsingStartPolicy(
    const FileOpenRequest& request,
    const FileTransactionContext* const transactionContext,
    IStartFileApi& api)
{
    // Start.exe:42BBBC..42BC26。事务入口缺失时原版直接失败，不退回普通文件。
    if (transactionContext != nullptr)
    {
        if (transactionContext->transactionHandleAt00 != nullptr)
        {
            const HMODULE module = api.Kernel32Module();
            if (module == nullptr)
            {
                return INVALID_HANDLE_VALUE;
            }

            const FARPROC procedure = api.ResolveCreateFileTransactedA(module);
            if (procedure == nullptr)
            {
                return INVALID_HANDLE_VALUE;
            }
            return api.CreateTransactedFile(
                procedure, request, transactionContext->transactionHandleAt00);
        }

        if (transactionContext->allowOrdinaryFileAt04 == 0)
        {
            return INVALID_HANDLE_VALUE;
        }
    }

    return api.CreateOrdinaryFile(request);
}

PersistenceIoStatus ReadWholePersistenceFile(
    const char* const path,
    std::vector<std::byte>& bytes,
    const FileTransactionContext* const transactionContext,
    IStartFileApi& api)
{
    SECURITY_ATTRIBUTES securityAttributes{};
    const FileOpenRequest request = MakePersistenceRequest(
        path, GENERIC_READ, OPEN_EXISTING, securityAttributes);
    const HANDLE file = OpenFileUsingStartPolicy(request, transactionContext, api);
    if (file == INVALID_HANDLE_VALUE)
    {
        return PersistenceIoStatus::Failure;
    }
    const ScopedFile closeFile(file, api);

    const DWORD size = FileLengthAt417A80(file, api);
    if (size == 0)
    {
        return PersistenceIoStatus::Failure;
    }

    bytes.resize(static_cast<std::size_t>(size));
    DWORD bytesRead = 0;
    const BOOL readResult = api.Read(file, bytes.data(), size, &bytesRead);
    if (readResult != FALSE)
    {
        return bytesRead == size ? PersistenceIoStatus::Success : PersistenceIoStatus::Failure;
    }
    return api.LastError() == ERROR_SUCCESS ? PersistenceIoStatus::Success
                                            : PersistenceIoStatus::Failure;
}

PersistenceIoStatus WriteWholePersistenceFile(
    const char* const path,
    const std::vector<std::byte>& bytes,
    const FileTransactionContext* const transactionContext,
    IStartFileApi& api)
{
    SECURITY_ATTRIBUTES securityAttributes{};
    const FileOpenRequest request = MakePersistenceRequest(
        path, GENERIC_WRITE, OPEN_ALWAYS, securityAttributes);
    const HANDLE file = OpenFileUsingStartPolicy(request, transactionContext, api);
    if (file == INVALID_HANDLE_VALUE)
    {
        return PersistenceIoStatus::Failure;
    }
    const ScopedFile closeFile(file, api);

    DWORD bytesWritten = 0;
    const BOOL writeResult = api.Write(
        file, bytes.data(), static_cast<DWORD>(bytes.size()), &bytesWritten);
    // Start.exe:43A1E6..43A20F 只检查 BOOL，不比较 bytesWritten。
    if (writeResult != FALSE)
    {
        return PersistenceIoStatus::Success;
    }
    return api.LastError() == ERROR_SUCCESS ? PersistenceIoStatus::Success
                                            : PersistenceIoStatus::Failure;
}
}
