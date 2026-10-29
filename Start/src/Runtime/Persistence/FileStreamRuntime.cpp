#include <Runtime/FileStreamRuntime.h>
#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/FileStreamCursorRuntime.h>

#include <cstring>
#include <cstddef>
#include <cstdlib>
#include <exception>
#include <limits>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
[[noreturn]] void PureStreamCall()
{
    _purecall();
    std::terminate();
}
class RetiredFileStream final : public IFileStreamRuntime
{
public:
    HANDLE HandleAtVslot00() override { PureStreamCall(); }
    void CloseAtVslot04() override { PureStreamCall(); }
    int IsOpenAtVslot08() override { PureStreamCall(); }
    int ReadSomeAtVslot0C(void*, DWORD, DWORD*) override { PureStreamCall(); }
    int ReadExactAtVslot10(void*, DWORD) override { PureStreamCall(); }
    int SeekAtVslot14(std::uint64_t, DWORD) override { PureStreamCall(); }
    std::uint64_t TellAtVslot18() override { PureStreamCall(); }
    std::uint64_t SizeAtVslot1C() override { PureStreamCall(); }
};
static_assert(sizeof(RetiredFileStream) == 4);
void RetireStream(void* address) noexcept
{
    // 40EE07/41229A：只退化vptr。不能依赖被MSVC消除的空基类析构隐式写表。
    ::new (address) RetiredFileStream;
}

void DestroyFile(AtlFileRuntime* file, IStartFileApi& api)
{
    // 43C840：只在非零HANDLE上Close；回调成功后才清句柄、释放8-byte正文。
    if (!file) return;
    if (file->handleAt00)
    {
        api.Close(file->handleAt00);
        file->handleAt00 = nullptr;
    }
    ::operator delete(file, sizeof(AtlFileRuntime));
}

class AtlFileControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit AtlFileControl(AtlFileRuntime* file) noexcept : EnvironmentObjectReferenceControl(file) {}
    void DestroyPayloadAtVslot00() override
    {
        Win32StartFileApi api;
        DestroyFile(static_cast<AtlFileRuntime*>(payloadAt14), api);
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        RetireEnvironmentObjectControl(this, flags);
    }
};
static_assert(sizeof(AtlFileControl) == 24);

const void* Vtable(const void* object) noexcept
{
    const void* result;
    std::memcpy(&result, object, sizeof(result));
    return result;
}

YtlReferenceControlRuntime* CurrentFileControl(const void* slot) noexcept
{
    YtlReferenceControlRuntime* control;
    std::memcpy(&control, static_cast<const std::byte*>(slot) + 8, sizeof(control));
    return control;
}

void ReleaseWithApi(void* slot, IStartFileApi& api)
{
    // raw入口也接收退表后的存储，不通过已结束生命周期的Ref成员访问字段。
    auto* control = CurrentFileControl(slot);
    if (!control) return; // 原无control孤立正文不能由现代RAII自动回收。
    if (--control->referenceCountAt04 == 0)
    {
        if (control->owningAt08)
        {
            // 只有现代注入入口替换该具体control的Win32服务；其他control仍调原虚槽。
            const AtlFileControl type(nullptr);
            if (Vtable(control) == Vtable(&type))
                DestroyFile(static_cast<AtlFileRuntime*>(
                    static_cast<EnvironmentObjectReferenceControl*>(control)->payloadAt14), api);
            else
                control->DestroyPayloadAtVslot00();
        }
        // 42BB21/4145BA/40EDB2：正文析构回调可能改槽，必须重新取control。
        if (auto* current = CurrentFileControl(slot)) current->DeleteControlAtVslot04(1);
    }
    const std::uintptr_t zero = 0;
    std::memcpy(static_cast<std::byte*>(slot) + 8, &zero, sizeof(zero));
    std::memcpy(slot, &zero, sizeof(zero));
}
}

void DestroyFileStreamAt40ED70(void* storage, IStartFileApi& api) noexcept
{
    // 仅取编译器生成的具体表；空槽不持有资源，且不调用其析构造成额外清理。
    union TableSource
    {
        TableSource() noexcept : stream() {}
        ~TableSource() noexcept {}
        FileStreamRuntime stream;
    } source;
    static_assert(offsetof(FileStreamRuntime, fileAt04) == 4);
    const auto* table = Vtable(&source.stream);
    std::memcpy(storage, &table, sizeof(table));
    ReleaseWithApi(static_cast<std::byte*>(storage) + 4, api);
    // 40EDC7把ECX清零；40EDD7不重读槽，不能再执行第二/第三次Release。
    RetireStream(storage);
}

void* IFileStreamRuntime::DeleteAtVslot20(std::uint32_t flags)
{
    RetireStream(this);
    if (flags & 1) ::operator delete(this, sizeof(IFileStreamRuntime));
    return this;
}

FileStreamRuntime::~FileStreamRuntime() noexcept
{
    // 40ED70/5DEE64 EHFlags=5：关闭异常终止，不能改成成功或Load=false。
    Win32StartFileApi api;
    DestroyFileStreamAt40ED70(this, api);
}

HANDLE FileStreamRuntime::HandleAtVslot00()
{
    // 4104A0：空正文是原非法解引用，不用INVALID_HANDLE伪造有效失败值。
    return ResolveEnvironmentObjectReference<AtlFileRuntime>(fileAt04)->handleAt00;
}
void FileStreamRuntime::CloseAtVslot04() { Win32StartFileApi api; CloseWithApi(api); }
int FileStreamRuntime::IsOpenAtVslot08() { return fileAt04.baseAt00 != 0; }
int FileStreamRuntime::ReadSomeAtVslot0C(void* buffer, DWORD size, DWORD* actual)
{
    Win32StartFileApi api;
    return ReadSomeWithApi(buffer, size, actual, api);
}
int FileStreamRuntime::ReadExactAtVslot10(void* buffer, DWORD size)
{
    Win32StartFileApi api;
    return ReadExactWithApi(buffer, size, api);
}
int FileStreamRuntime::SeekAtVslot14(std::uint64_t distance, DWORD method)
{
    Win32StartFileApi api;
    return SeekWithApi(distance, method, api);
}
std::uint64_t FileStreamRuntime::TellAtVslot18() { Win32StartFileApi api; return TellWithApi(api); }
std::uint64_t FileStreamRuntime::SizeAtVslot1C() { Win32StartFileApi api; return SizeWithApi(api); }
int FileStreamRuntime::WriteAtVslot24(const void* buffer, DWORD size)
{
    Win32StartFileApi api;
    return WriteWithApi(buffer, size, api);
}
int FileStreamRuntime::SetLengthAtVslot28(std::uint64_t size)
{
    Win32StartFileApi api;
    return SetLengthWithApi(size, api);
}
void* FileStreamRuntime::DeleteAtVslot20(std::uint32_t flags)
{
    this->~FileStreamRuntime();
    if (flags & 1) ::operator delete(this, sizeof(FileStreamRuntime));
    return this;
}
int FileStreamRuntime::OpenAt42BAE0(const char* path, DWORD access, DWORD share,
    DWORD disposition, DWORD flags, LPSECURITY_ATTRIBUTES attributes)
{
    Win32StartFileApi api;
    return OpenWithApi({path, access, share, disposition, flags, attributes}, api);
}

int FileStreamRuntime::OpenWithApi(const FileOpenRequest& request, IStartFileApi& api,
    const FileTransactionContext* transaction)
{
    // 42BAE7..42BB91：新正文 -> 释放旧槽 -> 发布base -> 新control。失败不回滚孤立正文。
    void* storage = ::operator new(sizeof(AtlFileRuntime));
    auto* file = storage ? ::new (storage) AtlFileRuntime : nullptr;
    ReleaseWithApi(&fileAt04, api);
    if (file)
    {
        if (fileAt04.controlAt08) fileAt04.controlAt08->DeleteControlAtVslot04(1);
        fileAt04.biasAt04 = 0;
        fileAt04.baseAt00 = reinterpret_cast<std::uintptr_t>(file);
        void* controlStorage = ::operator new(sizeof(AtlFileControl));
        auto* control = controlStorage ? ::new (controlStorage) AtlFileControl(file) : nullptr;
        fileAt04.controlAt08 = control;
        control->referenceCountAt04 = 1;
        control->owningAt08 = 1;
        control->strideAt0C = 8;
        control->countAt10 = 1;
    }
    file = ResolveEnvironmentObjectReference<AtlFileRuntime>(fileAt04);
    // 原入口始终保留新正文的null事务；非null参数仅属于显式现代注入扩展。
    const auto handle = OpenFileUsingStartPolicy(request,
        transaction ? transaction : file->transactionAt04, api);
    if (handle != INVALID_HANDLE_VALUE)
    {
        file->handleAt00 = handle;
        return 1;
    }
    if (api.LastError() == ERROR_SUCCESS) return 1; // 42BC31..42BC45：零handle仍为打开状态。
    ReleaseWithApi(&fileAt04, api);
    return 0;
}
void FileStreamRuntime::CloseWithApi(IStartFileApi& api) { ReleaseWithApi(&fileAt04, api); }
int FileStreamRuntime::ReadSomeWithApi(void* buffer, DWORD size, DWORD* actual, IStartFileApi& api)
{
    if (!IsOpenAtVslot08()) return 0;
    // 431450：由调用方接收actual；TRUE不要求实读长度与请求相同。
    return api.Read(HandleAtVslot00(), buffer, size, actual) || api.LastError() == ERROR_SUCCESS;
}
int FileStreamRuntime::ReadExactWithApi(void* buffer, DWORD size, IStartFileApi& api)
{
    if (!IsOpenAtVslot08()) return 0;
    DWORD actual = 0;
    const BOOL success = api.Read(HandleAtVslot00(), buffer, size, &actual);
    return success ? actual == size : api.LastError() == ERROR_SUCCESS;
}
int FileStreamRuntime::SeekWithApi(std::uint64_t distance, DWORD method, IStartFileApi& api)
{
    if (!IsOpenAtVslot08()) return 0;
    return MoveFilePositionAt4348B0(api, HandleAtVslot00(), distance, method);
}
std::uint64_t FileStreamRuntime::TellWithApi(IStartFileApi& api) noexcept
{
    try
    {
        if (!IsOpenAtVslot08()) throw 1;
        return ReadFilePositionAt418590(api, HandleAtVslot00());
    }
    catch (...) { return std::numeric_limits<std::uint64_t>::max(); }
}
std::uint64_t FileStreamRuntime::SizeWithApi(IStartFileApi& api) noexcept
{
    try
    {
        if (!IsOpenAtVslot08()) throw 1;
        DWORD high;
        const DWORD low = api.FileSizeLow(HandleAtVslot00(), &high);
        if (low == INVALID_FILE_SIZE && api.LastError() != ERROR_SUCCESS) throw 2;
        return (static_cast<std::uint64_t>(high) << 32) | low;
    }
    catch (...) { return 0; }
}
int FileStreamRuntime::WriteWithApi(const void* buffer, DWORD size, IStartFileApi& api)
{
    if (!IsOpenAtVslot08()) return 0;
    // 43A1D8..43A212：实写数量不预清零，也不比较短写；FALSE仅按LastError判断。
    DWORD actual;
    return api.Write(HandleAtVslot00(), buffer, size, &actual) || api.LastError() == ERROR_SUCCESS;
}
int FileStreamRuntime::SetLengthWithApi(std::uint64_t size, IStartFileApi& api) noexcept
{
    try
    {
        if (!IsOpenAtVslot08()) throw 1;
        // 436942..43696D：捕获CAtlFile正文，不捕获HANDLE；定位后重读同正文的句柄。
        auto* file = ResolveEnvironmentObjectReference<AtlFileRuntime>(fileAt04);
        if (!MoveFilePositionAt4348B0(api, file->handleAt00, size, FILE_BEGIN)) throw 2;
        if (!api.SetEndOfFileAtCurrent(file->handleAt00) && api.LastError() != ERROR_SUCCESS) throw 2;
        return 1;
    }
    catch (...) { return 0; } // 436999，与Seek无局部catch的语义不同。
}
ScopedFileStreamRuntime::~ScopedFileStreamRuntime() noexcept
{
    if (cleanupArmed_)
        DestroyFileStreamAt40ED70(&storage_, api_);
}
int ScopedFileStreamRuntime::OpenReaderWithApi(const FileOpenRequest& request,
    const FileTransactionContext* transaction)
{
    cleanupArmed_ = false;
    static_cast<void>(Native().OpenWithApi(request, api_, transaction));
    cleanupArmed_ = true;
    // 48446C/484B5B/47B230：调用者检查base，非42BAE0的返回值。
    return Native().IsOpenAtVslot08();
}
}
