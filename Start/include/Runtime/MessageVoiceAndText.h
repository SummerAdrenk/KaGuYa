#pragma once

#include <Runtime/MessageTextCommand.h>
#include <Runtime/RuntimeCStringArray.h>

namespace KaGuYa::Start::Runtime
{
// 47B44E..47B470；464F30 的析构次序是正文 +0C，随后声音数组 +00。
struct MessageVoiceAndText final
{
    RuntimeCStringArrayReference voices;
    MessageTextCommandReference text;
};
static_assert(sizeof(MessageVoiceAndText) == 0x18);
static_assert(offsetof(MessageVoiceAndText, text) == 0x0C);

class MessageVoiceAndTextRef final
{
public:
    MessageVoiceAndTextRef() noexcept = default;
    MessageVoiceAndTextRef(std::nullptr_t) noexcept {}
    explicit MessageVoiceAndTextRef(const YtlReferenceSlotRuntime& source) noexcept;
    MessageVoiceAndTextRef(const MessageVoiceAndTextRef& source) noexcept;
    MessageVoiceAndTextRef& operator=(const MessageVoiceAndTextRef& source);
    MessageVoiceAndTextRef(MessageVoiceAndTextRef&& source) noexcept;
    MessageVoiceAndTextRef& operator=(MessageVoiceAndTextRef&& source);
    MessageVoiceAndTextRef& operator=(std::nullptr_t) { Reset(); return *this; }
    ~MessageVoiceAndTextRef() noexcept;
    void Reset();
    void reset() { Reset(); }
    [[nodiscard]] MessageVoiceAndText* Get() const noexcept;
    [[nodiscard]] MessageVoiceAndText* get() const noexcept { return Get(); }
    [[nodiscard]] MessageVoiceAndText* operator->() const noexcept { return Get(); }
    [[nodiscard]] MessageVoiceAndText& operator*() const noexcept { return *Get(); }
    explicit operator bool() const noexcept { return Get() != nullptr; }
    [[nodiscard]] YtlReferenceSlotRuntime& RawSlot() noexcept { return reference_; }
    [[nodiscard]] const YtlReferenceSlotRuntime& RawSlot() const noexcept { return reference_; }
    friend bool operator==(const MessageVoiceAndTextRef& a, const MessageVoiceAndTextRef& b) noexcept
        { return a.Get() == b.Get(); }
    friend bool operator==(const MessageVoiceAndTextRef& a, std::nullptr_t) noexcept { return !a; }
private:
    YtlReferenceSlotRuntime reference_{};
};
static_assert(sizeof(MessageVoiceAndTextRef) == 12);

// 此输出槽尚未取得 EH 清理资格；control 分配失败不回收已分配的正文。
void ConstructMessageVoiceAndTextAt451650(YtlReferenceSlotRuntime& output,
    MessageVoiceAndText* body, std::uint8_t owning = 1);
[[nodiscard]] MessageVoiceAndTextRef MakeMessageVoiceAndText();
}
