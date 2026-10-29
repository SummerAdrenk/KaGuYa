#pragma once

#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/MessageFormatVectors.h>
#include <Runtime/MessageTextCommand.h>

#include <cstddef>
#include <utility>

namespace KaGuYa::Start::Runtime
{
// 47ABEE 只构造编号 vector，姓名编号在47ACD7首次写入，不能默认初始化为0。
struct MessageRawFormat final
{
    std::int32_t nameId;
    MessageIndexVector messages;
};
static_assert(sizeof(MessageRawFormat) == 16);
static_assert(offsetof(MessageRawFormat, messages) == 4);

// 47A190 返回解析后的姓名与消息引用；461B70先正序释放vector，再释放姓名。
struct Message2Format final
{
    MessageTextCommandReference name;
    MessageReferenceVector messages;
};
static_assert(sizeof(Message2Format) == 24);
static_assert(offsetof(Message2Format, messages) == 12);

template<class Body>
class MessageFormatReference final
{
public:
    MessageFormatReference() noexcept = default;
    MessageFormatReference(std::nullptr_t) noexcept {}
    explicit MessageFormatReference(const YtlReferenceSlotRuntime& source) noexcept : reference_(source)
    {
        if (reference_.controlAt08) ++reference_.controlAt08->referenceCountAt04;
    }
    MessageFormatReference(const MessageFormatReference& source) noexcept
        : MessageFormatReference(source.reference_) {}
    MessageFormatReference& operator=(const MessageFormatReference& source)
    {
        if (this != &source)
        {
            // 465990：旧正文回调之后重读source，不提前增持或采用copy-and-swap。
            Reset();
            reference_ = source.reference_;
            if (reference_.controlAt08) ++reference_.controlAt08->referenceCountAt04;
        }
        return *this;
    }
    MessageFormatReference(MessageFormatReference&& source) noexcept
        : reference_(std::exchange(source.reference_, {})) {}
    MessageFormatReference& operator=(MessageFormatReference&& source)
    {
        if (this != &source)
        {
            Reset();
            reference_ = std::exchange(source.reference_, {});
        }
        return *this;
    }
    MessageFormatReference& operator=(std::nullptr_t) { Reset(); return *this; }
    ~MessageFormatReference() noexcept { Reset(); }
    void Reset() { ReleaseYtlReference(reference_); }
    void reset() { Reset(); }
    void ReleaseCapturedOnReturn(YtlReferenceControlRuntime* captured) noexcept
    {
        // 47A4C2 / 47AAAE正常尾声使用EDI/EBX，不重读被正文回调改写的槽。
        // EH及显式Reset仍使用上面的重读路径；两种行为不能合并。
        if (captured && --captured->referenceCountAt04 == 0)
        {
            if (captured->owningAt08) captured->DestroyPayloadAtVslot00();
            captured->DeleteControlAtVslot04(1);
        }
        // 原局部槽此后直接退出作用域；解除现代析构，不能再释放回调写入的新control。
        reference_.controlAt08 = nullptr;
    }
    [[nodiscard]] Body* Get() const noexcept { return ResolveEnvironmentObjectReference<Body>(reference_); }
    [[nodiscard]] Body* get() const noexcept { return Get(); }
    [[nodiscard]] Body* operator->() const noexcept { return Get(); }
    [[nodiscard]] Body& operator*() const noexcept { return *Get(); }
    explicit operator bool() const noexcept { return Get() != nullptr; }
    [[nodiscard]] YtlReferenceSlotRuntime& RawSlot() noexcept { return reference_; }
    [[nodiscard]] const YtlReferenceSlotRuntime& RawSlot() const noexcept { return reference_; }
    friend bool operator==(const MessageFormatReference& a, const MessageFormatReference& b) noexcept
        { return a.Get() == b.Get(); }
    friend bool operator==(const MessageFormatReference& a, std::nullptr_t) noexcept { return !a; }
private:
    YtlReferenceSlotRuntime reference_{};
};
using MessageRawFormatRef = MessageFormatReference<MessageRawFormat>;
using Message2FormatRef = MessageFormatReference<Message2Format>;
static_assert(sizeof(MessageRawFormatRef) == 12);
static_assert(sizeof(Message2FormatRef) == 12);

// 两种输出槽均要求原来的拥有引用已由调用者处理；不隐式清理被覆盖的旧值。
void ConstructMessageRawFormatAt47AB80(YtlReferenceSlotRuntime& output);
void ConstructMessage2FormatAt451570(YtlReferenceSlotRuntime& output,
    Message2Format* body, std::uint8_t owning = 1);
[[nodiscard]] MessageRawFormatRef MakeMessageRawFormat();
[[nodiscard]] Message2FormatRef MakeMessage2Format();
}
