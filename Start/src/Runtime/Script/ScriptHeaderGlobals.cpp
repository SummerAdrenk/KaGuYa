#include <Runtime/ScriptHeaderGlobals.h>

#include <array>
#include <cstdlib>

namespace KaGuYa::Start::Runtime
{
namespace
{
constexpr std::array<std::string_view, ScriptHeaderCount> literals{
    "[SCR-Ver5]", "[SCR-Ver5.1]", "[SCR-Ver5.2]", "[SCR-Ver5.3]"};
// 61A5F8..61A604，地址与CRT构造顺序相反。原长度10/12均不包含NUL。
alignas(RuntimeCString) constinit std::array<std::byte, 16> storage{};
std::size_t Index(ScriptHeaderId id) noexcept
{
    const auto index = static_cast<std::size_t>(id);
    if (index >= ScriptHeaderCount) _invalid_parameter_noinfo_noreturn();
    return index;
}
RuntimeCString& Slot(ScriptHeaderId id) noexcept
{
    return *reinterpret_cast<RuntimeCString*>(storage.data() + (3 - Index(id)) * 4);
}
class ProcessSource final : public IScriptHeaderSource
{
public:
    constexpr ProcessSource() noexcept = default;
    const RuntimeCString& Borrow(ScriptHeaderId id) const noexcept override { return Slot(id); }
};
constinit const ProcessSource source;
class CrtRegistrar final : public IScriptHeaderExitRegistrar
{
public:
    int RegisterAtExit(ScriptHeaderExitCallback callback) override { return std::atexit(callback); }
};
template<ScriptHeaderId Id> void __cdecl ReleaseGlobal()
{
    // 5AC170/5AC110/5AC130/5AC150：普通退出叶，不清槽、不吞Free异常。
    DestroyRuntimeCStringAt40CD40(&Slot(Id));
}
constexpr std::array<ScriptHeaderExitCallback, ScriptHeaderCount> callbacks{
    ReleaseGlobal<ScriptHeaderId::V5>, ReleaseGlobal<ScriptHeaderId::V51>,
    ReleaseGlobal<ScriptHeaderId::V52>, ReleaseGlobal<ScriptHeaderId::V53>};
void __cdecl InitializeProcessHeaders()
{
    CrtRegistrar exits;
    for (std::size_t index = 0; index < ScriptHeaderCount; ++index)
    {
        const auto id = static_cast<ScriptHeaderId>(index);
        InitializeScriptHeaderGlobal(id, Slot(id), callbacks[index], exits);
    }
}
}

const IScriptHeaderSource& ProcessScriptHeaderSource() noexcept { return source; }
std::string_view ScriptHeaderLiteral(ScriptHeaderId id) noexcept { return literals[Index(id)]; }
void InitializeScriptHeaderGlobal(ScriptHeaderId id, RuntimeCString& slot,
    ScriptHeaderExitCallback callback, IScriptHeaderExitRegistrar& exits,
    IRuntimeStringManagerSource& strings)
{
    const auto literal = ScriptHeaderLiteral(id);
    InitializeEmptyRuntimeCString(slot, strings);
    struct Unwind final
    {
        RuntimeCString& slot;
        bool active = true;
        ~Unwind() noexcept { if (active) DestroyRuntimeCStringAt40CD40(&slot); }
    } unwind{slot};
    SetRuntimeCStringAt437680(slot, literal.data(), static_cast<int>(literal.size()));
    static_cast<void>(exits.RegisterAtExit(callback));
    unwind.active = false;
}

#pragma section(".CRT$XCU", read)
extern "C" __declspec(allocate(".CRT$XCU")) ScriptHeaderExitCallback const KaGuYaScriptHeaderCrtInitializer =
    InitializeProcessHeaders;
#pragma comment(linker, "/include:_KaGuYaScriptHeaderCrtInitializer")
}
