#pragma once

#include <Runtime/EnvironmentStateArchives.h>
#include <Runtime/InstallTableCatalogBuilder.h>
#include <Runtime/MessageOwnerRuntime.h>
#include <Runtime/RuntimeStringBorrow.h>

#include <memory>

namespace KaGuYa::Start::Runtime
{
// 原环境+48.payload的已确认输入；prefix必须原样保留，不能通过目录拼接规则改写。
struct FileManagerConstructionInput final
{
    bool environmentReadyAt00;
    InstallTableCatalogSource installTable;
    RuntimeStringBorrow cachePrefixAt10;
    RuntimeStringBorrow messagePrefixAt08;
    RuntimeStringBorrow saveDirectory;
    // 生产启动填写同根来源；固定借用仅保留给明确的工具/测试输入。
    const IMessageEnvironmentPathSource* messagePaths = nullptr;
};

class IFileManagerMessageFactory
{
public:
    virtual ~IFileManagerMessageFactory() = default;
    virtual std::unique_ptr<MessageOwnerRuntime> CreateMessageOwner() = 0;
};

// 458B60的类型化生产owner；目录/key已使用原协议，外层诊断/message仍不是原28h布局。
class FileManagerRuntime final
{
public:
    FileManagerRuntime(const FileManagerConstructionInput& input, IStartFileApi& files,
        IFileInfoProgress& progress, const FileTransactionContext* transaction = nullptr);
    FileManagerRuntime(const FileManagerConstructionInput& input, IStartFileApi& files,
        IFileInfoProgress& progress, IFileManagerMessageFactory& messages,
        const FileTransactionContext* transaction = nullptr);
    FileManagerRuntime(const FileManagerConstructionInput& input, IStartFileApi& files,
        const YtlReferenceSlotRuntime& progress, const char* capturedTitle,
        const FileTransactionContext* transaction = nullptr);
    ~FileManagerRuntime() noexcept;
    FileManagerRuntime(const FileManagerRuntime&) = delete;
    FileManagerRuntime& operator=(const FileManagerRuntime&) = delete;

    [[nodiscard]] std::uint32_t ScanStatusAt0C() const noexcept { return scanStatusAt0C_; }
    [[nodiscard]] const InstallTableCatalogBuildResult& Directory() const noexcept { return *directory_; }
    [[nodiscard]] MessageOwnerRuntime* MessageAt10() const noexcept { return message_.get(); }
    [[nodiscard]] const ByteArrayReferenceRuntime* InstalledKeyAt1C() const noexcept { return key_ ? &key_ : nullptr; }
    [[nodiscard]] ByteArrayReferenceRuntime& KeyReferenceAt1C() noexcept { return key_; }
    [[nodiscard]] const ByteArrayReferenceRuntime& KeyReferenceAt1C() const noexcept { return key_; }

    // 现代明确所有权转移入口；根在发布manager后安装key。此处不是48A036的解码/分配实现。
    void AdoptInstalledKey(std::unique_ptr<EnvironmentKeyByteTable> key);
    // 显式工具快照入口，不包含原489F7C多次取源、Header拥有及源Ref释放合同。
    // 正式启动通过InstallParamsKeyAt489F7C直接操作KeyReferenceAt1C。
    void InstallKeyFromSnapshot(std::vector<std::byte>& source);
    // 486990只清目录与消息，保留+0C状态和+1C已安装key。
    void ClearAt486990() noexcept;

private:
    void ConstructAt458B60(const FileManagerConstructionInput& input, IStartFileApi& files,
        const YtlReferenceSlotRuntime& progress, IFileManagerMessageFactory& messages,
        const FileTransactionContext* transaction, const char* capturedTitle);

    // 诊断owner及原map sentinel均在构造正文catch之外；这一额外owner分配仍是待迁移边界。
    std::unique_ptr<InstallTableCatalogBuildResult> directory_;
    std::uint32_t scanStatusAt0C_ = 0;
    std::unique_ptr<MessageOwnerRuntime> message_;
    ByteArrayReferenceRuntime key_;
};
}
