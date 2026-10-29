#pragma once

#include <Windows.h>
#include <bit>
#include <concepts>
#include <cstdint>
#include <type_traits>
#include <variant>

namespace KaGuYa::Start::Runtime
{
// Environment 的 +18/+1C 存储为 DWORD，消费方按有符号位模式读取。
// 只绑定真实左值；禁止隐式转换临时量和构造时取值快照。被借用字段须覆盖消费者寿命。
class ProgramDwordReference final
{
public:
    template<class T> requires (std::same_as<std::remove_cv_t<T>, std::int32_t> ||
        std::same_as<std::remove_cv_t<T>, std::uint32_t>)
    ProgramDwordReference(T& value) noexcept : value_(&value) {}

    [[nodiscard]] std::int32_t Read() const noexcept
    {
        return std::visit([](auto p) { return std::bit_cast<std::int32_t>(*p); }, value_);
    }

private:
    std::variant<const std::int32_t*, const std::uint32_t*> value_;
};

// 同样允许原 root+2C 的 DWORD 槽与已有 Win32 HWND 左值，读时才转换，不保留转换结果。
class ProgramWindowHandleReference final
{
public:
    template<class T> requires (std::same_as<std::remove_cv_t<T>, HWND> ||
        std::same_as<std::remove_cv_t<T>, std::uint32_t>)
    ProgramWindowHandleReference(T& value) noexcept : value_(&value) {}

    [[nodiscard]] HWND Read() const noexcept
    {
        static_assert(sizeof(HWND) == sizeof(std::uint32_t));
        return std::visit([](auto p) -> HWND
        {
            if constexpr (std::same_as<std::remove_cvref_t<decltype(*p)>, HWND>) return *p;
            else return reinterpret_cast<HWND>(static_cast<std::uintptr_t>(*p));
        }, value_);
    }

private:
    std::variant<const HWND*, const std::uint32_t*> value_;
};
}
