#pragma once

#include <cstddef>
#include <cstdint>
#include <span>

namespace KaGuYa::Start::Runtime
{
enum class ArchiveCodecInputMode { Bounded, Raw };

// 正文外的显式输入策略；Raw没有伪造长度，原可读生命期完全由调用方保证。
// 仅区分现代物理范围检查，不取消格式内长度、终止符或原错误报告。
class ArchiveCodecInputView final
{
public:
    [[nodiscard]] static ArchiveCodecInputView Raw(const std::uint8_t* data) noexcept
        { return {data, 0, ArchiveCodecInputMode::Raw}; }
    [[nodiscard]] static ArchiveCodecInputView Bounded(std::span<const std::uint8_t> bytes) noexcept
        { return {bytes.data(), bytes.size(), ArchiveCodecInputMode::Bounded}; }
    [[nodiscard]] const std::uint8_t* data() const noexcept { return data_; }
    [[nodiscard]] bool Contains(const std::uint8_t* address, std::size_t count) const noexcept
    {
        if (mode_ == ArchiveCodecInputMode::Raw) return true;
        const auto base = reinterpret_cast<std::uintptr_t>(data_);
        const auto at = reinterpret_cast<std::uintptr_t>(address);
        return at >= base && at - base <= size_ && count <= size_ - (at - base);
    }
    // 调用方已经在原读取点确认该前缀可用；Raw只推进地址，不形成无界span。
    [[nodiscard]] ArchiveCodecInputView After(std::size_t count) const noexcept
    {
        const auto* next = reinterpret_cast<const std::uint8_t*>(
            reinterpret_cast<std::uintptr_t>(data_) + count);
        return {next, mode_ == ArchiveCodecInputMode::Bounded ? size_ - count : 0, mode_};
    }
private:
    ArchiveCodecInputView(const std::uint8_t* data, std::size_t size, ArchiveCodecInputMode mode) noexcept
        : data_(data), size_(size), mode_(mode) {}
    const std::uint8_t* data_;
    std::size_t size_;
    ArchiveCodecInputMode mode_;
};
}
