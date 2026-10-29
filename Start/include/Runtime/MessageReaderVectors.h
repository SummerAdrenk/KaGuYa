#pragma once

#include <Runtime/MessageFormat.h>

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace KaGuYa::Start::Runtime
{
// 45A740..45A7E7 / 45A8F7..45A905：仅清三指针，不写相邻 mode/padding/累计量。
class MessageOffsetVector final
{
public:
    struct Storage final
    {
        std::uint32_t* begin;
        std::uint32_t* end;
        std::uint32_t* capacity;
    };

    MessageOffsetVector() noexcept = default;
    ~MessageOffsetVector() noexcept;
    MessageOffsetVector(const MessageOffsetVector&) = delete;
    MessageOffsetVector& operator=(const MessageOffsetVector&) = delete;
    MessageOffsetVector(MessageOffsetVector&&) = delete;
    MessageOffsetVector& operator=(MessageOffsetVector&&) = delete;

    // 494620 -> 453A70：接源槽地址；分配返回后才捕获 DWORD，不可改成按值传参。
    void PushBack(const std::uint32_t& source);
    void push_back(const std::uint32_t& source) { PushBack(source); }
    [[nodiscard]] std::size_t size() const noexcept;
    [[nodiscard]] std::size_t capacity() const noexcept;
    [[nodiscard]] bool empty() const noexcept { return storage.begin == storage.end; }
    [[nodiscard]] std::uint32_t* begin() noexcept { return storage.begin; }
    [[nodiscard]] const std::uint32_t* begin() const noexcept { return storage.begin; }
    [[nodiscard]] std::uint32_t* end() noexcept { return storage.end; }
    [[nodiscard]] const std::uint32_t* end() const noexcept { return storage.end; }
    [[nodiscard]] std::uint32_t& operator[](std::size_t index) noexcept { return storage.begin[index]; }
    [[nodiscard]] const std::uint32_t& operator[](std::size_t index) const noexcept { return storage.begin[index]; }
    void clear() noexcept;
    void DestroyStorage() noexcept;

    Storage storage{nullptr, nullptr, nullptr};

private:
    void GrowAndInsert(std::uint32_t* position, const std::uint32_t& source);
};

// name/choice 共用 457D80 / 454CD0 / 4525F0；不得借用 VoiceAndText Ref 的对象类型。
class MessageTextCommandReferenceVector final
{
public:
    struct Storage final
    {
        MessageTextCommandReference* begin;
        MessageTextCommandReference* end;
        MessageTextCommandReference* capacity;
    };

    MessageTextCommandReferenceVector() noexcept = default;
    ~MessageTextCommandReferenceVector() noexcept;
    MessageTextCommandReferenceVector(const MessageTextCommandReferenceVector&) = delete;
    MessageTextCommandReferenceVector& operator=(const MessageTextCommandReferenceVector&) = delete;
    MessageTextCommandReferenceVector(MessageTextCommandReferenceVector&&) = delete;
    MessageTextCommandReferenceVector& operator=(MessageTextCommandReferenceVector&&) = delete;

    // 即使 source 是右值也复制：先增持临时 Ref，再插入，最后释放临时 Ref。
    void PushBack(const MessageTextCommandReference& source);
    void push_back(const MessageTextCommandReference& source) { PushBack(source); }
    [[nodiscard]] std::size_t size() const noexcept;
    [[nodiscard]] std::size_t capacity() const noexcept;
    [[nodiscard]] bool empty() const noexcept { return storage.begin == storage.end; }
    [[nodiscard]] MessageTextCommandReference* begin() noexcept { return storage.begin; }
    [[nodiscard]] const MessageTextCommandReference* begin() const noexcept { return storage.begin; }
    [[nodiscard]] MessageTextCommandReference* end() noexcept { return storage.end; }
    [[nodiscard]] const MessageTextCommandReference* end() const noexcept { return storage.end; }
    [[nodiscard]] MessageTextCommandReference& operator[](std::size_t index) noexcept { return storage.begin[index]; }
    [[nodiscard]] const MessageTextCommandReference& operator[](std::size_t index) const noexcept { return storage.begin[index]; }
    void clear() noexcept;
    void DestroyStorage() noexcept;

    Storage storage{nullptr, nullptr, nullptr};
};

// 48482D..4848C5 / 456530 / 452D40：只缓存 raw 编号正文，不解析为 Message2Format。
class MessageRawFormatReferenceVector final
{
public:
    struct Storage final
    {
        MessageRawFormatRef* begin;
        MessageRawFormatRef* end;
        MessageRawFormatRef* capacity;
    };

    MessageRawFormatReferenceVector() noexcept = default;
    ~MessageRawFormatReferenceVector() noexcept;
    MessageRawFormatReferenceVector(const MessageRawFormatReferenceVector&) = delete;
    MessageRawFormatReferenceVector& operator=(const MessageRawFormatReferenceVector&) = delete;
    MessageRawFormatReferenceVector(MessageRawFormatReferenceVector&&) = delete;
    MessageRawFormatReferenceVector& operator=(MessageRawFormatReferenceVector&&) = delete;

    void PushBack(const MessageRawFormatRef& source);
    void push_back(const MessageRawFormatRef& source) { PushBack(source); }
    [[nodiscard]] std::size_t size() const noexcept;
    [[nodiscard]] std::size_t capacity() const noexcept;
    [[nodiscard]] bool empty() const noexcept { return storage.begin == storage.end; }
    [[nodiscard]] MessageRawFormatRef* begin() noexcept { return storage.begin; }
    [[nodiscard]] const MessageRawFormatRef* begin() const noexcept { return storage.begin; }
    [[nodiscard]] MessageRawFormatRef* end() noexcept { return storage.end; }
    [[nodiscard]] const MessageRawFormatRef* end() const noexcept { return storage.end; }
    [[nodiscard]] MessageRawFormatRef& operator[](std::size_t index) noexcept { return storage.begin[index]; }
    [[nodiscard]] const MessageRawFormatRef& operator[](std::size_t index) const noexcept { return storage.begin[index]; }
    void clear() noexcept;
    void DestroyStorage() noexcept;

    // 原布局访问口；所有者负责有效范围，不允许浅拷贝拥有引用。
    Storage storage{nullptr, nullptr, nullptr};
};

static_assert(sizeof(void*) == 4);
static_assert(sizeof(MessageOffsetVector) == 12 && alignof(MessageOffsetVector) == 4);
static_assert(sizeof(MessageTextCommandReferenceVector) == 12 && alignof(MessageTextCommandReferenceVector) == 4);
static_assert(sizeof(MessageRawFormatReferenceVector) == 12 && alignof(MessageRawFormatReferenceVector) == 4);
static_assert(std::is_standard_layout_v<MessageOffsetVector>);
static_assert(std::is_standard_layout_v<MessageTextCommandReferenceVector>);
static_assert(std::is_standard_layout_v<MessageRawFormatReferenceVector>);
static_assert(offsetof(MessageOffsetVector, storage) == 0);
static_assert(offsetof(MessageTextCommandReferenceVector, storage) == 0);
static_assert(offsetof(MessageRawFormatReferenceVector, storage) == 0);
static_assert(sizeof(MessageOffsetVector::Storage) == 12);
static_assert(sizeof(MessageTextCommandReferenceVector::Storage) == 12);
static_assert(sizeof(MessageRawFormatReferenceVector::Storage) == 12);
static_assert(offsetof(MessageOffsetVector::Storage, begin) == 0);
static_assert(offsetof(MessageOffsetVector::Storage, end) == 4);
static_assert(offsetof(MessageOffsetVector::Storage, capacity) == 8);
static_assert(offsetof(MessageTextCommandReferenceVector::Storage, begin) == 0);
static_assert(offsetof(MessageTextCommandReferenceVector::Storage, end) == 4);
static_assert(offsetof(MessageTextCommandReferenceVector::Storage, capacity) == 8);
static_assert(offsetof(MessageRawFormatReferenceVector::Storage, begin) == 0);
static_assert(offsetof(MessageRawFormatReferenceVector::Storage, end) == 4);
static_assert(offsetof(MessageRawFormatReferenceVector::Storage, capacity) == 8);
}
