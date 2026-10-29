#pragma once

#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/RuntimeCString.h>

#include <cstddef>
#include <cstdint>
#include <span>

namespace KaGuYa::Start::Runtime
{
// 这里只适配原文件对象的 Write(+24) 叶调用，不宣称复刻整个文件对象虚表。
class IArchiveDirectoryWriterRuntime
{
public:
    virtual std::int32_t Write(const void* bytes, std::uint32_t length) = 0;
protected:
    ~IArchiveDirectoryWriterRuntime() = default;
};

class ArchiveDirectoryRuntime final
{
public:
    ArchiveDirectoryRuntime() noexcept = default;
    ~ArchiveDirectoryRuntime() noexcept;
    ArchiveDirectoryRuntime(const ArchiveDirectoryRuntime&) = delete;
    ArchiveDirectoryRuntime& operator=(const ArchiveDirectoryRuntime&) = delete;

    // 4939DF/49412C/494AEE -> 453250：复制并加计数，不接管 source 的引用。
    // source 可以是本容器中的槽；增长先构造新增槽，再复制、释放旧范围。
    void Append(const YtlReferenceSlotRuntime& source);
    void DestroyStorage() noexcept; // 461F30；正序释放，最后才清三个指针。
    [[nodiscard]] std::size_t size() const noexcept;
    [[nodiscard]] std::size_t Capacity() const noexcept;
    [[nodiscard]] bool empty() const noexcept { return beginAt00 == endAt04; }
    [[nodiscard]] std::span<YtlReferenceSlotRuntime> References() noexcept;
    [[nodiscard]] std::span<const YtlReferenceSlotRuntime> References() const noexcept;

    YtlReferenceSlotRuntime* beginAt00 = nullptr;
    YtlReferenceSlotRuntime* endAt04 = nullptr;
    YtlReferenceSlotRuntime* capacityAt08 = nullptr;

private:
    void GrowAndInsert(YtlReferenceSlotRuntime* position, const YtlReferenceSlotRuntime& source);
};
static_assert(sizeof(ArchiveDirectoryRuntime) == 12);
static_assert(offsetof(ArchiveDirectoryRuntime, beginAt00) == 0);
static_assert(offsetof(ArchiveDirectoryRuntime, endAt04) == 4);
static_assert(offsetof(ArchiveDirectoryRuntime, capacityAt08) == 8);

struct ArchiveDirectoryRuntimeLayout;

// 原 PE 5B29B8：三个 purecall。没有虚析构；正文由具体类型的控制块释放。
class ArchiveNodeRuntime
{
public:
    [[nodiscard]] virtual std::uint32_t IsFile() const noexcept = 0;          // +00
    [[nodiscard]] virtual std::uint32_t SerializedSize() const noexcept = 0; // +04，仅本节点
    virtual std::int32_t Write(IArchiveDirectoryWriterRuntime& writer) const = 0; // +08

    [[nodiscard]] RuntimeCString& NameAt04() noexcept { return nameAt04_; }
    [[nodiscard]] const RuntimeCString& NameAt04() const noexcept { return nameAt04_; }
    ArchiveNodeRuntime(const ArchiveNodeRuntime&) = delete;
    ArchiveNodeRuntime& operator=(const ArchiveNodeRuntime&) = delete;

protected:
    explicit ArchiveNodeRuntime(IRuntimeStringManagerSource& strings);
    ~ArchiveNodeRuntime() = default;

private:
    RuntimeCString nameAt04_;
    friend struct ArchiveDirectoryRuntimeLayout;
};
static_assert(sizeof(ArchiveNodeRuntime) == 8);

class ArchiveFileNodeRuntime final : public ArchiveNodeRuntime
{
public:
    [[nodiscard]] std::uint32_t IsFile() const noexcept override;          // 494CA0
    [[nodiscard]] std::uint32_t SerializedSize() const noexcept override; // 494C80
    std::int32_t Write(IArchiveDirectoryWriterRuntime& writer) const override; // 48F300
    [[nodiscard]] RuntimeCString& TitleAt08() noexcept { return titleAt08_; }
    [[nodiscard]] const RuntimeCString& TitleAt08() const noexcept { return titleAt08_; }

private:
    // 45EF30 的 EH 表 5E89A8 没有回滚项。Title 在 placement 正文构造后才构造，
    // 避免第二次 Capture/GetNilString 失败时 C++ 自动释放已经完成的 Name。
    explicit ArchiveFileNodeRuntime(IRuntimeStringManagerSource& strings);
    ~ArchiveFileNodeRuntime() {}
    union { RuntimeCString titleAt08_; };

public:
    // 4949F6/494A00/494A22/494A50；构造不写数值。
    // +0C..0F、+1E..1F 在已核链路中没有读写，不定义业务字段、不补零。
    std::uint64_t offsetAt10;
    std::uint32_t lengthAt18;
    std::uint16_t flagsAt1C;

private:
    friend ArchiveFileNodeRuntime* CreateArchiveFileBody(IRuntimeStringManagerSource&);
    friend struct ArchiveDirectoryRuntimeLayout;
};
static_assert(sizeof(ArchiveFileNodeRuntime) == 0x20);

class ArchiveFolderNodeRuntime final : public ArchiveNodeRuntime
{
public:
    [[nodiscard]] std::uint32_t IsFile() const noexcept override;          // 494CB0
    [[nodiscard]] std::uint32_t SerializedSize() const noexcept override; // 494C90
    std::int32_t Write(IArchiveDirectoryWriterRuntime& writer) const override; // 48F410
    [[nodiscard]] ArchiveDirectoryRuntime& ChildrenAt08() noexcept { return childrenAt08_; }
    [[nodiscard]] const ArchiveDirectoryRuntime& ChildrenAt08() const noexcept { return childrenAt08_; }

private:
    explicit ArchiveFolderNodeRuntime(IRuntimeStringManagerSource& strings);
    ~ArchiveFolderNodeRuntime() = default;
    ArchiveDirectoryRuntime childrenAt08_;
    friend ArchiveFolderNodeRuntime* CreateArchiveFolderBody(IRuntimeStringManagerSource&);
    friend struct ArchiveDirectoryRuntimeLayout;
};
static_assert(sizeof(ArchiveFolderNodeRuntime) == 0x14);

// 4946E8/494793、4937E4、493E45：独立分配 raw body，再执行 45EF30/45EFC0。
// 构造失败不释放 raw body 或已构造 CString；标题派生、名称解析由调用者完成。
[[nodiscard]] ArchiveFileNodeRuntime* CreateArchiveFileBody(
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
[[nodiscard]] ArchiveFolderNodeRuntime* CreateArchiveFolderBody(
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());

// body 必须先创建完成。494709..49483C：捕获并释放旧 control，随后发布 base/bias，
// 最后分配具体类型的 24-byte control；控制块分配失败保留 base，不回滚 body。
// 新局部先声明 Ref{} 再调用同一接口；不得先创建完整新 Ref 再赋给旧 Ref。
void ReplaceArchiveFileReference(YtlReferenceSlotRuntime& reference, ArchiveFileNodeRuntime* body);
void ReplaceArchiveFolderReference(YtlReferenceSlotRuntime& reference, ArchiveFolderNodeRuntime* body);
// 494650 的 esi 跨正文构造/进度回调保留；需要表达这段寿命时使用显式捕获口。
// 两参数便利口仅在进入 Replace 时捕获，适用于新局部或期间未被改写的槽。
void ReplaceArchiveFileReference(YtlReferenceSlotRuntime& reference, ArchiveFileNodeRuntime* body,
    YtlReferenceControlRuntime* capturedPreviousControl);
void ReplaceArchiveFolderReference(YtlReferenceSlotRuntime& reference, ArchiveFolderNodeRuntime* body,
    YtlReferenceControlRuntime* capturedPreviousControl);

// 复制构造口，destination 不得已拥有引用。保留 base/bias/control，不解析正文。
void CopyArchiveReference(YtlReferenceSlotRuntime& destination,
    const YtlReferenceSlotRuntime& source) noexcept;
[[nodiscard]] ArchiveNodeRuntime* ResolveArchiveNode(const YtlReferenceSlotRuntime& reference) noexcept;

// 494B59、4938EC：正常局部退出使用捕获的 control，两次虚调用间不重读槽，且不清槽。
// 此后不得再次释放这个局部槽；原活动析构 state 指向 541E55/terminate。
void ReleaseArchiveLocalReference(const YtlReferenceSlotRuntime& reference) noexcept;
void ReleaseArchiveLocalReference(YtlReferenceControlRuntime* capturedControl) noexcept;
// 4941CC..4941F9：直扫正常退出同样捕获 control，但有 control 时还清 base/control。
void ReleaseArchiveDirectLocalReference(YtlReferenceSlotRuntime& reference) noexcept;
void ReleaseArchiveDirectLocalReference(YtlReferenceSlotRuntime& reference,
    YtlReferenceControlRuntime* capturedControl) noexcept;
// 59B8C0/59B820/59B7A8 -> 460D50：EH 重读 control，释放完成后才清槽；无 control 不清。
void ReleaseArchiveReferenceOnUnwind(YtlReferenceSlotRuntime& reference) noexcept;
}
