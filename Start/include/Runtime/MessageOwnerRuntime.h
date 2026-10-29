#pragma once

#include <Runtime/MessageReadFlagsRuntime.h>
#include <Runtime/MessageReaderReference.h>
#include <Runtime/MessageReaderRuntimeAdapter.h>

namespace KaGuYa::Start::Runtime
{
// 原45A5D0的三个具体分配入口；生产默认实现不依赖测试reader。
class IMessageReaderFactory
{
public:
    virtual ~IMessageReaderFactory() = default;
    virtual void CreateMessage2(MessageReaderReference& destination) = 0;
    virtual void CreateLegacyMessage(MessageReaderReference& destination) = 0;
    virtual void CreateNullMessage(MessageReaderReference& destination) = 0;
};

// 无状态默认工厂；现代owner与native CMessage均在调用栈上复用原三个分配入口。
class DefaultMessageReaderFactory final : public IMessageReaderFactory
{
public:
    void CreateMessage2(MessageReaderReference& destination) override;
    void CreateLegacyMessage(MessageReaderReference& destination) override;
    void CreateNullMessage(MessageReaderReference& destination) override;
};

// File::CMessage 的 reader 和三组已读数组 owner，不是 CFileManager 的替代物。
class MessageOwnerRuntime final
{
public:
    MessageOwnerRuntime(IStartFileApi& files, RuntimeStringBorrow messagePrefixCp932,
        RuntimeStringBorrow saveDirectoryCp932, const FileTransactionContext* transaction = nullptr);
    MessageOwnerRuntime(IStartFileApi& files, RuntimeStringBorrow messagePrefixCp932,
        RuntimeStringBorrow saveDirectoryCp932, IMessageReaderFactory& factory);
    MessageOwnerRuntime(IStartFileApi& files, const IMessageEnvironmentPathSource& paths,
        const FileTransactionContext* transaction = nullptr);
    MessageOwnerRuntime(IStartFileApi& files, const IMessageEnvironmentPathSource& paths,
        IMessageReaderFactory& factory);
    ~MessageOwnerRuntime() noexcept;
    MessageOwnerRuntime(const MessageOwnerRuntime&) = delete;
    MessageOwnerRuntime& operator=(const MessageOwnerRuntime&) = delete;

    [[nodiscard]] const MessageReaderReference& Reader() const noexcept { return reader_; }
    [[nodiscard]] MessageReaderReference& ReaderReferenceAt04() noexcept { return reader_; }
    [[nodiscard]] MessageReaderRuntimeAdapter Source() const noexcept { return {reader_, context_}; }
    [[nodiscard]] MessageReadFlagsRuntime& ReadFlags() noexcept { return flags_; }
    [[nodiscard]] const MessageReadFlagsRuntime& ReadFlags() const noexcept { return flags_; }

private:
    void ConstructAt45A5D0(std::optional<RuntimeStringBorrow> messagePrefix,
        const IMessageEnvironmentPathSource* paths, IMessageReaderFactory& factory);
    // 声明顺序使三组byte数组先结束生命周期、reader最后释放，保存目录借用根环境同源字符串。
    MessageReaderReference reader_;
    MessageReadFlagsRuntime flags_;
    MessageReaderCallContext context_;
};
}
