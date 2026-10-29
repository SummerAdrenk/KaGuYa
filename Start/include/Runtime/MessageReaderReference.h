#pragma once

#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/MessageReaderNative.h>
#include <type_traits>

namespace KaGuYa::Start::Runtime
{
enum class MessageReaderKind { Message2, Legacy, Null };

namespace Detail
{
template<class Reader, MessageReaderKind Kind>
class MessageReaderControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit MessageReaderControl(Reader* payload) noexcept : EnvironmentObjectReferenceControl(payload) {}
    void DestroyPayloadAtVslot00() override
    {
        // 492EA0/492EC0/492F00：具体析构正常返回后才释放正文。
        // 尤其Null的CString Free若抛出，不能由delete表达式自动回收正文。
        auto* body = static_cast<Reader*>(payloadAt14);
        if (!body) return;
        if constexpr (std::is_same_v<Reader, NativeNullMessageRuntime>)
            body->DestroyPathAt40CD40(); // 492F08直接释放CString，不写正文vptr。
        else body->~Reader();
        ::operator delete(body, sizeof(Reader));
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    { RetireEnvironmentObjectControl(this, flags); }
};
}

// CMessage+04 的三字 Ref；base直接指向单业务虚表的原生reader，不拥有现代适配器。
class MessageReaderReference final
{
public:
    MessageReaderReference() noexcept = default;
    explicit MessageReaderReference(const YtlReferenceSlotRuntime& source) noexcept;
    MessageReaderReference(const MessageReaderReference& source) noexcept;
    MessageReaderReference& operator=(const MessageReaderReference& source);
    MessageReaderReference(MessageReaderReference&& source) noexcept;
    MessageReaderReference& operator=(MessageReaderReference&& source);
    ~MessageReaderReference() noexcept;

    // 正文由调用方先构造；控制块失败保留已发布 base，不自动回滚正文。
    template<class Reader>
    void ReplaceOwned(Reader* created, MessageReaderKind kind)
    {
        static_assert(std::is_base_of_v<NativeMessageReader, Reader>);
        static_assert(!std::is_same_v<NativeMessageReader, Reader>);
        switch (kind)
        {
        case MessageReaderKind::Message2:
            ReplaceOwnedImpl(created, kind, &MakeControl<Reader, MessageReaderKind::Message2>); break;
        case MessageReaderKind::Legacy:
            ReplaceOwnedImpl(created, kind, &MakeControl<Reader, MessageReaderKind::Legacy>); break;
        case MessageReaderKind::Null:
            ReplaceOwnedImpl(created, kind, &MakeControl<Reader, MessageReaderKind::Null>); break;
        default: ReplaceOwnedImpl(created, kind, nullptr); break;
        }
    }
    void ReplaceOwned(std::nullptr_t, MessageReaderKind kind) { ReplaceOwnedImpl(nullptr, kind, nullptr); }
    void Reset();
    [[nodiscard]] NativeMessageReader* Get() const noexcept;
    [[nodiscard]] NativeMessageReader* get() const noexcept { return Get(); }
    [[nodiscard]] NativeMessageReader* operator->() const noexcept { return Get(); }
    [[nodiscard]] NativeMessageReader& operator*() const noexcept { return *Get(); }
    [[nodiscard]] explicit operator bool() const noexcept { return Get() != nullptr; }
    [[nodiscard]] const YtlReferenceSlotRuntime& RawSlot() const noexcept { return reference_; }
    [[nodiscard]] YtlReferenceSlotRuntime& RawSlot() noexcept { return reference_; }
private:
    template<class Reader, MessageReaderKind Kind>
    static EnvironmentObjectReferenceControl* MakeControl(NativeMessageReader* body)
    {
        using Control = Detail::MessageReaderControl<Reader, Kind>;
        static_assert(sizeof(Control) == 0x18);
        void* storage = ::operator new(sizeof(Control));
        return storage ? ::new (storage) Control(static_cast<Reader*>(body)) : nullptr;
    }
    using CreateControl = EnvironmentObjectReferenceControl* (*)(NativeMessageReader*);
    void ReplaceOwnedImpl(NativeMessageReader* created, MessageReaderKind kind, CreateControl createControl);
    YtlReferenceSlotRuntime reference_{};
};
static_assert(sizeof(MessageReaderReference) == 12);
}
