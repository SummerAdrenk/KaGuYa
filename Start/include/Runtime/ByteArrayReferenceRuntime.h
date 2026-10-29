#pragma once

#include <Runtime/YtlReferenceArrayRuntime.h>

#include <cstdint>
#include <span>

namespace KaGuYa::Start::Runtime
{
// 原 uchar[] Ref 的三字槽；不追加现代容器或独立长度。
class ByteArrayReferenceRuntime final
{
public:
    ByteArrayReferenceRuntime() noexcept = default;
    explicit ByteArrayReferenceRuntime(const YtlReferenceSlotRuntime& source) noexcept;
    ByteArrayReferenceRuntime(const ByteArrayReferenceRuntime& source) noexcept;
    ByteArrayReferenceRuntime& operator=(const ByteArrayReferenceRuntime& source);
    // 现代明确转交一个已有引用；不声称对应原版 move 入口。
    ByteArrayReferenceRuntime(ByteArrayReferenceRuntime&& source) noexcept;
    ByteArrayReferenceRuntime& operator=(ByteArrayReferenceRuntime&& source);
    ~ByteArrayReferenceRuntime() noexcept;

    void AllocateAt412700(std::uint32_t count);
    void Reset();
    [[nodiscard]] std::uint8_t* Data() const noexcept;
    [[nodiscard]] std::uint8_t* DataAt(std::uint32_t index) const noexcept;
    [[nodiscard]] std::uint32_t Size() const noexcept;
    // span 为借用视图，仅用于生产 bias=0/stride=1；不对原偏移槽猜测范围校正。
    [[nodiscard]] std::span<const std::uint8_t> Bytes() const noexcept;
    [[nodiscard]] std::span<std::uint8_t> MutableBytes() noexcept;
    [[nodiscard]] explicit operator bool() const noexcept;
    [[nodiscard]] const YtlReferenceSlotRuntime& RawSlot() const noexcept { return reference_; }
    [[nodiscard]] YtlReferenceSlotRuntime& RawSlot() noexcept { return reference_; }

private:
    YtlReferenceSlotRuntime reference_{};
};
static_assert(sizeof(ByteArrayReferenceRuntime) == 12, "uchar[] Ref requires x86");
}
