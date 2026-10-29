#pragma once

#include <Runtime/MessageOwnerRuntime.h>
#include <cstddef>

namespace KaGuYa::Start::Runtime
{
// 仅调用期借用；不能存入正文、control或进程绑定表。
struct MessageOwnerNativeWithSources final
{
    MessageReaderCallContext context;
    const IMessageEnvironmentPathSource& paths;
    IMessageReaderFactory& factory;
};

class MessageOwnerNative34h final
{
public:
    MessageOwnerNative34h();
    explicit MessageOwnerNative34h(MessageOwnerNativeWithSources sources);
    ~MessageOwnerNative34h() noexcept;
    MessageOwnerNative34h(const MessageOwnerNative34h&) = delete;
    MessageOwnerNative34h& operator=(const MessageOwnerNative34h&) = delete;

    // 5B29F8唯一虚槽；无虚析构。缺省路径只使用真实Process环境。
    virtual MessageOwnerNative34h* DeleteAtVslot00(std::uint32_t flags) noexcept;

    // 显式原生销毁叶：调用后不得再隐式析构同一正文；flags仅bit0决定delete34h。
    void DestroyWithSourcesAt463220(IStartFileApi& files,
        const IMessageEnvironmentPathSource& paths) noexcept;
    MessageOwnerNative34h* DeleteWithSourcesAt468E20(std::uint32_t flags,
        IStartFileApi& files, const IMessageEnvironmentPathSource& paths) noexcept;

    [[nodiscard]] MessageReaderReference& ReaderReferenceAt04() noexcept { return readerAt04; }
    [[nodiscard]] const MessageReaderReference& Reader() const noexcept { return readerAt04; }
    [[nodiscard]] MessageReaderRuntimeAdapter SourceWithContext(MessageReaderCallContext context) const noexcept
    { return {readerAt04, context}; }
    [[nodiscard]] MessageReadFlagsRuntime ReadFlagsWithSources(IStartFileApi& files,
        const IMessageEnvironmentPathSource& paths) noexcept
    { return {flagsAt10, files, paths}; }

    // 即时业务叶只在调用栈上借正文与默认服务，不返回临时view或缓存Process路径。
    void PersistUnreadFlagsAt43D3D0();
    [[nodiscard]] std::int32_t ReadTextAt4AD590(std::int32_t index,
        MessageFormatSelectionTarget output);
    [[nodiscard]] std::int32_t ReadFormatAt417C40(std::int32_t index,
        MessageFormatSelectionTarget output, std::uint32_t variant, bool markRead);
    [[nodiscard]] std::int32_t ReadFormatRawAt417C40(std::int32_t index,
        MessageFormatSelectionRawTarget output, std::uint32_t variant, bool markRead);

    // union禁止默认成员析构额外扫描；constructor EH与463220分别显式控制顺序。
    union { MessageReaderReference readerAt04; };
    union { MessageReadFlagsStorage flagsAt10; };
private:
    struct TableOnlyTag final {};
    explicit MessageOwnerNative34h(TableOnlyTag) noexcept;
    void ConstructAt45A5D0(MessageOwnerNativeWithSources sources);
    void RestoreVptrAt463245() noexcept;
};
static_assert(sizeof(MessageOwnerNative34h) == 0x34);
static_assert(alignof(MessageOwnerNative34h) == 4);
static_assert(offsetof(MessageOwnerNative34h, readerAt04) == 4);
static_assert(offsetof(MessageOwnerNative34h, flagsAt10) == 0x10);

// raw guard只覆盖45A5D0；正文完成以后，旧Ref释放/control分配失败不得回滚正文。
[[nodiscard]] MessageOwnerNative34h* NewMessageOwnerAt4593AD();
[[nodiscard]] MessageOwnerNative34h* NewMessageOwnerWithSourcesAt4593AD(MessageOwnerNativeWithSources sources);
void ReplaceMessageReferenceAt4593CF(YtlReferenceSlotRuntime& slot, MessageOwnerNative34h* created);
void CreateAndReplaceMessageAt4593AD(YtlReferenceSlotRuntime& slot);
void CreateAndReplaceMessageWithSourcesAt4593AD(YtlReferenceSlotRuntime& slot, MessageOwnerNativeWithSources sources);
[[nodiscard]] MessageOwnerNative34h* ResolveMessageReference(const YtlReferenceSlotRuntime& slot) noexcept;

// FileManager可直接嵌入的三字拥有Ref；销毁从control.payload的当前slot0派发。
class MessageReference final
{
public:
    MessageReference() noexcept = default;
    explicit MessageReference(const YtlReferenceSlotRuntime& source) noexcept;
    MessageReference(const MessageReference& source) noexcept;
    MessageReference& operator=(const MessageReference& source);
    MessageReference(MessageReference&& source) noexcept;
    MessageReference& operator=(MessageReference&& source);
    ~MessageReference() noexcept;
    void Reset();
    void ReplaceOwned(MessageOwnerNative34h* created);
    [[nodiscard]] MessageOwnerNative34h* Get() const noexcept;
    [[nodiscard]] MessageOwnerNative34h* operator->() const noexcept { return Get(); }
    [[nodiscard]] explicit operator bool() const noexcept { return Get() != nullptr; }
    [[nodiscard]] YtlReferenceSlotRuntime& RawSlot() noexcept { return reference_; }
    [[nodiscard]] const YtlReferenceSlotRuntime& RawSlot() const noexcept { return reference_; }
private:
    YtlReferenceSlotRuntime reference_{};
};
static_assert(sizeof(MessageReference) == 12);
static_assert(alignof(MessageReference) == 4);
}
