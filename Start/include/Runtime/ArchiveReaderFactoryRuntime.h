#pragma once

#include <Runtime/ArchiveEntryReader.h>
#include <Runtime/ArchiveReaderNative.h>
#include <Runtime/ArchiveCodecServices.h>
#include <Runtime/RootArchiveResourceSource.h>
#include <Runtime/YtlReferenceArrayRuntime.h>

#include <optional>

namespace KaGuYa::Start::Runtime
{
enum class ArchiveReaderAllocation { Reader, ReaderControl };
struct ToolArchiveCodecsTag final {};

class IArchiveReaderMemoryApi
{
public:
    virtual ~IArchiveReaderMemoryApi() = default;
    // 注入存储必须可由全局 sized delete 释放；原14h正文/18h控制块不存allocator。
    virtual void* Allocate(ArchiveReaderAllocation kind, std::size_t bytes) = 0;
};

class ArchiveReaderMemoryApi final : public IArchiveReaderMemoryApi
{
public:
    void* Allocate(ArchiveReaderAllocation kind, std::size_t bytes) override;
};

class ArchiveReaderReferenceControl : public YtlReferenceControlRuntime
{
public:
    ArchiveReaderReferenceControl() noexcept : strideAt0C(0x14), extentAt10(1) {}
    std::uint32_t strideAt0C;
    std::uint32_t extentAt10;
protected:
    explicit ArchiveReaderReferenceControl(UninitializedFieldsTag tag) noexcept
        : YtlReferenceControlRuntime(tag) {}
    ~ArchiveReaderReferenceControl() = default;
};
static_assert(sizeof(ArchiveReaderReferenceControl) == 0x14);

struct ArchiveReaderReferenceSlot final
{
    std::uintptr_t baseAt00 = 0;
    std::uint32_t biasAt04 = 0;
    ArchiveReaderReferenceControl* controlAt08 = nullptr;
};
static_assert(sizeof(ArchiveReaderReferenceSlot) == 12);

// 原 Ref 的 payload/control 分离；明确的 Reset 可抛，析构遵循工厂析构的终止边界。
// 正文是唯一14h CFileReader；工具DTO不得驻留在生产Ref内。
class ArchiveReaderReference final
{
public:
    ArchiveReaderReference() noexcept = default;
    ArchiveReaderReference(const ArchiveReaderReference& source) noexcept;
    ArchiveReaderReference(ArchiveReaderReference&& source) noexcept;
    ArchiveReaderReference& operator=(const ArchiveReaderReference& source);
    ArchiveReaderReference& operator=(ArchiveReaderReference&& source);
    ~ArchiveReaderReference() noexcept;

    [[nodiscard]] ArchiveStreamReaderRuntime* Get() const noexcept;
    [[nodiscard]] const ArchiveReaderReferenceSlot& Slot() const noexcept { return slot_; }
    // 现代显式转移一个已有计数，不新增引用。被接管槽清零。
    [[nodiscard]] static ArchiveReaderReference Adopt(ArchiveReaderReferenceSlot& slot) noexcept;
    void Reset();
private:
    friend class ArchiveReaderFactoryRuntime;
    ArchiveReaderReferenceSlot slot_;
};

// 483D90/4D44C0 的持久 reader owning 槽；依赖须长于工厂及任何外部 reader 引用。
// 原factory+10由具备该原生槽的调用者显式传入；不解释其业务值或假装本类具有14h布局。
class ArchiveReaderFactoryRuntime final
{
public:
    ArchiveReaderFactoryRuntime(IArchiveResourceSource& source,
        const FileTransactionContext* transaction, IStartFileApi& files,
        IArchiveReaderMemoryApi& memory, ArchiveCodecServices codecs) noexcept;
    ArchiveReaderFactoryRuntime(const ResourceCatalog& catalog,
        const FileTransactionContext* transaction, IStartFileApi& files,
        IArchiveReaderMemoryApi& memory, ArchiveCodecServices codecs) noexcept;
    // 显式工具/历史夹具域；必须写tag，生产不再有隐式四参入口。
    ArchiveReaderFactoryRuntime(IArchiveResourceSource& source,
        const FileTransactionContext* transaction, IStartFileApi& files,
        IArchiveReaderMemoryApi& memory, ToolArchiveCodecsTag) noexcept;
    // 显式固定目录适配；生产原入口必须选用上方真实 source 构造。
    ArchiveReaderFactoryRuntime(const ResourceCatalog& catalog,
        const FileTransactionContext* transaction, IStartFileApi& files,
        IArchiveReaderMemoryApi& memory, ToolArchiveCodecsTag) noexcept;
    ArchiveReaderFactoryRuntime(const ArchiveReaderFactoryRuntime&) = delete;
    ArchiveReaderFactoryRuntime& operator=(const ArchiveReaderFactoryRuntime&) = delete;

    bool Load(const std::string& name, std::span<const std::uint8_t> key);
    bool Load(const std::string& name, const IEnvironmentKeyTableSource& key);
    bool LoadBorrowedAt483D90(const char* name, const IEnvironmentKeyTableSource& key,
        std::int32_t* nativeQueryFieldAt10 = nullptr);
    bool LoadBorrowedAt4D44C0(const char* name, const IEnvironmentKeyTableSource& key,
        std::int32_t* nativeQueryFieldAt10 = nullptr);
    // SCR 现代错误分类适配口；清理顺序同 Load，保留原始读取/转换异常供外层分类。
    [[nodiscard]] std::uint32_t LoadOrThrow(const std::string& name, const IEnvironmentKeyTableSource& key);
    [[nodiscard]] const ArchiveReaderReference& Reader() const noexcept { return reader_; }
    void Clear() { reader_.Reset(); }
    // 4CCCDD..4CCD28：仅normal Direct正常尾声；回调后不重取control，失败终止。
    void ReleaseReaderNormalAt4CCCDD() noexcept;
    // 原14h栈工厂接管已有一次拥有计数；不Clone、不减持，不回读reader正文。
    [[nodiscard]] ArchiveReaderReferenceSlot DetachReaderSlot() noexcept
    {const auto result=reader_.slot_;reader_.slot_={};return result;}
private:
    void PublishNewReader();
    bool LoadUsing(const std::string* name, const char* borrowedName, std::span<const std::uint8_t> key,
        const IEnvironmentKeyTableSource* environmentKey, bool propagateFailure,
        std::uint32_t* transformVersion = nullptr, std::int32_t* nativeQueryFieldAt10 = nullptr);

    std::optional<FixedArchiveResourceSource> fixtureSource_;
    IArchiveResourceSource& source_;
    const FileTransactionContext* transaction_;
    IStartFileApi& files_;
    IArchiveReaderMemoryApi& memory_;
    std::optional<ArchiveCodecServices> codecs_;
    ArchiveReaderReference reader_;
};
}
