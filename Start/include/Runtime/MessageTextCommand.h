#pragma once

#include <Runtime/RuntimeCString.h>
#include <Runtime/YtlReferenceArrayRuntime.h>

namespace KaGuYa::Start::Runtime
{
// 5B25AC 的两个虚槽是长度和 CString 返回；原表没有虚析构槽。
class MessageTextCommand final
{
public:
    MessageTextCommand(const char* source, std::int32_t size,
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
    virtual std::int32_t GetLengthAt47D730() const noexcept;
    virtual RuntimeCString GetTextAt47D550() const;
    ~MessageTextCommand() = default;
    MessageTextCommand(const MessageTextCommand&) = delete;
    MessageTextCommand& operator=(const MessageTextCommand&) = delete;
    [[nodiscard]] std::string_view View() const noexcept { return RuntimeStringView(textAt04_); }
private:
    RuntimeCString textAt04_;
};
static_assert(sizeof(MessageTextCommand) == 8);

class MessageTextCommandReference final
{
public:
    MessageTextCommandReference() noexcept = default;
    MessageTextCommandReference(std::nullptr_t) noexcept {}
    explicit MessageTextCommandReference(const YtlReferenceSlotRuntime& source) noexcept;
    MessageTextCommandReference(const MessageTextCommandReference& source) noexcept;
    MessageTextCommandReference& operator=(const MessageTextCommandReference& source);
    MessageTextCommandReference(MessageTextCommandReference&& source) noexcept;
    MessageTextCommandReference& operator=(MessageTextCommandReference&& source);
    MessageTextCommandReference& operator=(std::nullptr_t) { Reset(); return *this; }
    ~MessageTextCommandReference() noexcept;
    void ReplaceOwnedAt451A80(MessageTextCommand* created);
    void Reset();
    void reset() { Reset(); }
    [[nodiscard]] MessageTextCommand* Get() const noexcept;
    [[nodiscard]] MessageTextCommand* get() const noexcept { return Get(); }
    [[nodiscard]] MessageTextCommand* operator->() const noexcept { return Get(); }
    [[nodiscard]] MessageTextCommand& operator*() const noexcept { return *Get(); }
    explicit operator bool() const noexcept { return Get() != nullptr; }
    [[nodiscard]] YtlReferenceSlotRuntime& RawSlot() noexcept { return reference_; }
    [[nodiscard]] const YtlReferenceSlotRuntime& RawSlot() const noexcept { return reference_; }
    friend bool operator==(const MessageTextCommandReference& a, const MessageTextCommandReference& b) noexcept
    { return a.Get() == b.Get(); }
    friend bool operator==(const MessageTextCommandReference& a, std::nullptr_t) noexcept { return !a; }
private:
    YtlReferenceSlotRuntime reference_{};
};
static_assert(sizeof(MessageTextCommandReference) == 12);

// 47D63B/47B498/47A724 的 raw-body 分配及构造展开；成功后由调用点发布拥有引用。
[[nodiscard]] MessageTextCommand* AllocateMessageTextCommand(const char* source, std::int32_t size,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
// 对应原 hidden-result 输出槽，调用者必须提供尚未拥有对象的槽；构造抛出时不提前写输出。
void ConstructMessageTextCommandAt47D570(YtlReferenceSlotRuntime& output,
    const char* source, std::int32_t size,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
[[nodiscard]] MessageTextCommandReference MakeMessageTextCommand(const char* source, std::int32_t size = 0,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
}
