#pragma once

#include <Runtime/PersistenceFileAccess.h>
#include <Runtime/YtlReferenceArrayRuntime.h>

namespace KaGuYa::Start::Runtime
{
// 原 CAtlFile 的两个 DWORD；注入的文件 API 不属于原正文。
struct AtlFileRuntime final
{
    HANDLE handleAt00 = nullptr;
    const FileTransactionContext* transactionAt04 = nullptr;
};
static_assert(sizeof(AtlFileRuntime) == 8);

// 5ADDB0：前八槽为业务接口，+20 才是带删除标志的析构入口。
class IFileStreamRuntime
{
public:
    virtual HANDLE HandleAtVslot00() = 0;
    virtual void CloseAtVslot04() = 0;
    virtual int IsOpenAtVslot08() = 0;
    virtual int ReadSomeAtVslot0C(void* buffer, DWORD size, DWORD* actual) = 0;
    virtual int ReadExactAtVslot10(void* buffer, DWORD size) = 0;
    virtual int SeekAtVslot14(std::uint64_t distance, DWORD method) = 0;
    virtual std::uint64_t TellAtVslot18() = 0;
    virtual std::uint64_t SizeAtVslot1C() = 0;
    virtual void* DeleteAtVslot20(std::uint32_t flags);
protected:
    ~IFileStreamRuntime() noexcept = default;
};
static_assert(sizeof(IFileStreamRuntime) == 4);

// +24/+28是具体流追加的原虚槽，final不代表可以改成普通成员。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunnecessary-virtual-specifier"
#endif
class FileStreamRuntime final : public IFileStreamRuntime
{
public:
    FileStreamRuntime() noexcept = default;
    ~FileStreamRuntime() noexcept;
    FileStreamRuntime(const FileStreamRuntime&) = delete;
    FileStreamRuntime& operator=(const FileStreamRuntime&) = delete;

    HANDLE HandleAtVslot00() override;
    void CloseAtVslot04() override;
    int IsOpenAtVslot08() override;
    int ReadSomeAtVslot0C(void* buffer, DWORD size, DWORD* actual) override;
    int ReadExactAtVslot10(void* buffer, DWORD size) override;
    int SeekAtVslot14(std::uint64_t distance, DWORD method) override;
    std::uint64_t TellAtVslot18() override;
    std::uint64_t SizeAtVslot1C() override;
    void* DeleteAtVslot20(std::uint32_t flags) override;
    virtual int WriteAtVslot24(const void* buffer, DWORD size);
    virtual int SetLengthAtVslot28(std::uint64_t size);
    int OpenAt42BAE0(const char* path, DWORD access, DWORD share,
        DWORD disposition, DWORD flags, LPSECURITY_ATTRIBUTES attributes);

    // 显式现代调用边界，不改变上方原虚槽签名，也不在正文/control里藏上下文。
    int OpenWithApi(const FileOpenRequest& request, IStartFileApi& api,
        const FileTransactionContext* transaction = nullptr);
    void CloseWithApi(IStartFileApi& api);
    int ReadSomeWithApi(void* buffer, DWORD size, DWORD* actual, IStartFileApi& api);
    int ReadExactWithApi(void* buffer, DWORD size, IStartFileApi& api);
    int SeekWithApi(std::uint64_t distance, DWORD method, IStartFileApi& api);
    std::uint64_t TellWithApi(IStartFileApi& api) noexcept;
    std::uint64_t SizeWithApi(IStartFileApi& api) noexcept;
    int WriteWithApi(const void* buffer, DWORD size, IStartFileApi& api);
    int SetLengthWithApi(std::uint64_t size, IStartFileApi& api) noexcept;

    YtlReferenceSlotRuntime fileAt04{};
};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
static_assert(sizeof(FileStreamRuntime) == 16);

// 40ED70显式生命周期入口：storage为本流仍有效的16-byte存储，可已退表。
// 先恢复具体表，按当前Ref释放一次，最后退表；不重建/清零Ref，不释放流存储。
// 调用后只能以原始存储观察；重复调用资格由调用者EH状态决定。
void DestroyFileStreamAt40ED70(void* storage, IStartFileApi& api) noexcept;

// 仅现代装配层持有API引用；原16-byte具体流提供十一槽，退化基表为九槽。
class ScopedFileStreamRuntime final
{
public:
    explicit ScopedFileStreamRuntime(IStartFileApi& api) noexcept : api_(api) {}
    ~ScopedFileStreamRuntime() noexcept;
    ScopedFileStreamRuntime(const ScopedFileStreamRuntime&) = delete;
    ScopedFileStreamRuntime& operator=(const ScopedFileStreamRuntime&) = delete;
    FileStreamRuntime& Native() noexcept { return storage_.stream; }
    int OpenReaderWithApi(const FileOpenRequest& request,
        const FileTransactionContext* transaction = nullptr);
private:
    IStartFileApi& api_;
    // 原reader把析构EH状态写在Open正常返回后；Open抛出时不得提前回收已发布正文。
    union Storage
    {
        Storage() noexcept : stream() {}
        ~Storage() noexcept {}
        FileStreamRuntime stream;
    } storage_;
    bool cleanupArmed_ = true;
};
}
