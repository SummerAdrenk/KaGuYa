#pragma once

#include <Runtime/RuntimeCString.h>
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace KaGuYa::Start::Runtime
{
enum class ScriptHeaderId : std::uint8_t { V5, V51, V52, V53 };
inline constexpr std::size_t ScriptHeaderCount = 4;

class IScriptHeaderSource
{
public:
    [[nodiscard]] virtual const RuntimeCString& Borrow(ScriptHeaderId id) const = 0;
protected:
    ~IScriptHeaderSource() = default;
};
[[nodiscard]] const IScriptHeaderSource& ProcessScriptHeaderSource() noexcept;
[[nodiscard]] std::string_view ScriptHeaderLiteral(ScriptHeaderId id) noexcept;

using ScriptHeaderExitCallback = void (__cdecl*)();
class IScriptHeaderExitRegistrar
{
public:
    virtual int RegisterAtExit(ScriptHeaderExitCallback callback) = 0;
protected:
    ~IScriptHeaderExitRegistrar() = default;
};

// 原CRT 5AD6C4..5AD6D0：V5/V51/V52/V53；成功Set后才登记各自退出叶。
void InitializeScriptHeaderGlobal(ScriptHeaderId id, RuntimeCString& slot,
    ScriptHeaderExitCallback callback, IScriptHeaderExitRegistrar& exits,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
}
