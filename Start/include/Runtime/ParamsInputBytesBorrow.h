#pragma once

#include <cstddef>
#include <cstring>
#include <optional>
#include <vector>

namespace KaGuYa::Start::Runtime
{
// getter14结果保留载体身份，不缓存begin/end，也不复制或拥有正文。
class ParamsInputBytesBorrow final
{
public:
    [[nodiscard]] static ParamsInputBytesBorrow Bounded(const std::vector<std::byte>& bytes) noexcept
        { return {&bytes, Access::BoundedVector}; }
    [[nodiscard]] static ParamsInputBytesBorrow Bounded(std::vector<std::byte>& bytes) noexcept
        { return {&bytes, Access::MutableBoundedVector}; }
    [[nodiscard]] static ParamsInputBytesBorrow NativeVector12(const void* carrier) noexcept
        { return {carrier, Access::NativeVector12}; }
    [[nodiscard]] const void* Identity() const noexcept { return carrier_; }
    [[nodiscard]] bool IsBounded() const noexcept { return access_ != Access::NativeVector12; }
    // 原Serialize的vector12正文可写；现代const输入不能被旧版XOR偷偷修改。
    [[nodiscard]] bool IsWritable() const noexcept { return access_ != Access::BoundedVector; }
    [[nodiscard]] const std::byte* Begin() const noexcept
    {
        if (IsBounded()) return static_cast<const std::vector<std::byte>*>(carrier_)->data();
        // 413B81/420A7A只重读原生载体首DWORD，不额外访问end/capacity。
        const std::byte* begin;
        std::memcpy(&begin, carrier_, sizeof(begin));
        return begin;
    }
    [[nodiscard]] std::optional<std::size_t> BoundedSize() const noexcept
    {
        if (IsBounded()) return static_cast<const std::vector<std::byte>*>(carrier_)->size();
        return std::nullopt;
    }

private:
    enum class Access { BoundedVector, MutableBoundedVector, NativeVector12 };
    ParamsInputBytesBorrow(const void* carrier, Access access) noexcept : carrier_(carrier), access_(access) {}
    const void* carrier_;
    Access access_;
};
}
