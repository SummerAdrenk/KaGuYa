#pragma once

#include <Runtime/RuntimeCString.h>
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace KaGuYa::Start::Runtime
{
// 原CRT表5AD4AC..5AD4D8的逻辑顺序；物理CString槽地址按此顺序递减。
enum class ParamsHeaderId : std::uint8_t
{
    V02, V03, V04, V05, V051, V052, V053, V054, V055, V056, V057, V058
};
inline constexpr std::size_t ParamsHeaderCount = 12;

class IParamsHeaderSource
{
public:
    [[nodiscard]] virtual const RuntimeCString& Borrow(ParamsHeaderId id) const = 0;
protected:
    ~IParamsHeaderSource() = default;
};

// 只借真实4-byte槽；不捕获manager、不Clone、不分配、不首次初始化。
[[nodiscard]] const IParamsHeaderSource& ProcessParamsHeaderSource() noexcept;
// 固定literal仅用于原初始化和显式诊断标签；生产比较必须借上面的CString槽。
[[nodiscard]] std::string_view ParamsHeaderLiteral(ParamsHeaderId id) noexcept;

using ParamsHeaderExitCallback = void (__cdecl*)();
class IParamsHeaderExitRegistrar
{
public:
    virtual int RegisterAtExit(ParamsHeaderExitCallback callback) = 0;
protected:
    ~IParamsHeaderExitRegistrar() = default;
};

// slot尚未构造；raw GetNil发布后才取得展开资格，Set成功后才注册callback。
// 注册非零返回按原码忽略；成功槽的生命周期由注册回调负责，不由本函数复制拥有。
void InitializeParamsHeaderGlobal(ParamsHeaderId id, RuntimeCString& slot,
    ParamsHeaderExitCallback callback, IParamsHeaderExitRegistrar& exits,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
}
