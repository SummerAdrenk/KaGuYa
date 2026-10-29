#pragma once

#include <Runtime/MessageVoiceAndText.h>

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace KaGuYa::Start::Runtime
{
// 47ABEE..47ABFC 的编号容器；只保存三个指针，不附加分配器或所有权标记。
class MessageIndexVector final
{
public:
    struct Storage final
    {
        std::int32_t* begin;
        std::int32_t* end;
        std::int32_t* capacity;
    };

    MessageIndexVector() noexcept = default;
    ~MessageIndexVector() noexcept;
    MessageIndexVector(const MessageIndexVector&) = delete;
    MessageIndexVector& operator=(const MessageIndexVector&) = delete;
    MessageIndexVector(MessageIndexVector&&) = delete;
    MessageIndexVector& operator=(MessageIndexVector&&) = delete;

    // 47AD09..47AD2E：先捕获编号到局部值，扩容时传该局部槽的地址。
    void PushBack(std::int32_t value);
    void push_back(std::int32_t value) { PushBack(value); }
    [[nodiscard]] std::size_t size() const noexcept;
    [[nodiscard]] std::size_t capacity() const noexcept;
    [[nodiscard]] bool empty() const noexcept { return storage.begin == storage.end; }
    [[nodiscard]] std::int32_t* begin() noexcept { return storage.begin; }
    [[nodiscard]] const std::int32_t* begin() const noexcept { return storage.begin; }
    [[nodiscard]] std::int32_t* end() noexcept { return storage.end; }
    [[nodiscard]] const std::int32_t* end() const noexcept { return storage.end; }
    [[nodiscard]] std::int32_t& operator[](std::size_t index) noexcept { return storage.begin[index]; }
    [[nodiscard]] const std::int32_t& operator[](std::size_t index) const noexcept { return storage.begin[index]; }
    // 现代便利接口：越界抛出异常，不声称对应原程序的边界检查。
    [[nodiscard]] std::int32_t& at(std::size_t index);
    [[nodiscard]] const std::int32_t& at(std::size_t index) const;
    // 现代便利接口：保留容量，只将结束指针重置为起始指针。
    void clear() noexcept;
    void DestroyStorage() noexcept;

    // 供精确布局的所有者访问；调用方须维护有效存储范围与元素生存期。
    Storage storage{nullptr, nullptr, nullptr};

private:
    void GrowAndInsert(std::int32_t* position, const std::int32_t& source);
};

// 451570 中正文 +0C 的消息容器；元素是原生十二字节引用，不是共享指针包装。
class MessageReferenceVector final
{
public:
    struct Storage final
    {
        MessageVoiceAndTextRef* begin;
        MessageVoiceAndTextRef* end;
        MessageVoiceAndTextRef* capacity;
    };

    MessageReferenceVector() noexcept = default;
    ~MessageReferenceVector() noexcept;
    MessageReferenceVector(const MessageReferenceVector&) = delete;
    MessageReferenceVector& operator=(const MessageReferenceVector&) = delete;
    MessageReferenceVector(MessageReferenceVector&&) = delete;
    MessageReferenceVector& operator=(MessageReferenceVector&&) = delete;

    // 457E50：复制临时引用、复制插入、释放临时引用；右值也走同一路径。
    void PushBack(const MessageVoiceAndTextRef& source);
    void push_back(const MessageVoiceAndTextRef& source) { PushBack(source); }
    [[nodiscard]] std::size_t size() const noexcept;
    [[nodiscard]] std::size_t capacity() const noexcept;
    [[nodiscard]] bool empty() const noexcept { return storage.begin == storage.end; }
    [[nodiscard]] MessageVoiceAndTextRef* begin() noexcept { return storage.begin; }
    [[nodiscard]] const MessageVoiceAndTextRef* begin() const noexcept { return storage.begin; }
    [[nodiscard]] MessageVoiceAndTextRef* end() noexcept { return storage.end; }
    [[nodiscard]] const MessageVoiceAndTextRef* end() const noexcept { return storage.end; }
    [[nodiscard]] MessageVoiceAndTextRef& operator[](std::size_t index) noexcept { return storage.begin[index]; }
    [[nodiscard]] const MessageVoiceAndTextRef& operator[](std::size_t index) const noexcept { return storage.begin[index]; }
    // 现代便利接口：越界抛出异常，不声称对应原程序的边界检查。
    [[nodiscard]] MessageVoiceAndTextRef& at(std::size_t index);
    [[nodiscard]] const MessageVoiceAndTextRef& at(std::size_t index) const;
    // 现代便利接口：按 452680 正序释放，随后重读起始指针并重置结束指针。
    void clear() noexcept;
    void DestroyStorage() noexcept;

    // 公开布局不转移元素所有权，也不允许绕过引用计数进行浅复制。
    Storage storage{nullptr, nullptr, nullptr};

private:
    void GrowAndInsert(MessageVoiceAndTextRef* position, const YtlReferenceSlotRuntime& source);
};

static_assert(sizeof(void*) == 4);
static_assert(sizeof(MessageIndexVector) == 12 && alignof(MessageIndexVector) == 4);
static_assert(sizeof(MessageReferenceVector) == 12 && alignof(MessageReferenceVector) == 4);
static_assert(std::is_standard_layout_v<MessageIndexVector>);
static_assert(std::is_standard_layout_v<MessageReferenceVector>);
static_assert(offsetof(MessageIndexVector, storage) == 0);
static_assert(offsetof(MessageReferenceVector, storage) == 0);
static_assert(sizeof(MessageIndexVector::Storage) == 12);
static_assert(sizeof(MessageReferenceVector::Storage) == 12);
static_assert(offsetof(MessageIndexVector::Storage, begin) == 0);
static_assert(offsetof(MessageIndexVector::Storage, end) == 4);
static_assert(offsetof(MessageIndexVector::Storage, capacity) == 8);
static_assert(offsetof(MessageReferenceVector::Storage, begin) == 0);
static_assert(offsetof(MessageReferenceVector::Storage, end) == 4);
static_assert(offsetof(MessageReferenceVector::Storage, capacity) == 8);
}
