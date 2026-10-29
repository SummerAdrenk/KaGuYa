#include <Runtime/ParamsHeaderGlobals.h>

#include <array>
#include <cstdlib>

namespace KaGuYa::Start::Runtime
{
namespace
{
constexpr std::array<std::string_view, ParamsHeaderCount> literals{
    "[SCR-PARAMS]v02", "[SCR-PARAMS]v03", "[SCR-PARAMS]v04", "[SCR-PARAMS]v05",
    "[SCR-PARAMS]v05.1", "[SCR-PARAMS]v05.2", "[SCR-PARAMS]v05.3", "[SCR-PARAMS]v05.4",
    "[SCR-PARAMS]v05.5", "[SCR-PARAMS]v05.6", "[SCR-PARAMS]v05.7", "[SCR-PARAMS]v05.8"};

// 对应6197F0..61981F的唯一48-byte存储；BSS零初值不是已构造的空CString。
alignas(RuntimeCString) constinit std::array<std::byte, ParamsHeaderCount * sizeof(RuntimeCString)> storage{};

std::size_t Index(ParamsHeaderId id) noexcept
{
    const auto index = static_cast<std::size_t>(id);
    if (index >= ParamsHeaderCount) _invalid_parameter_noinfo_noreturn();
    return index;
}

RuntimeCString& Slot(ParamsHeaderId id) noexcept
{
    const auto physicalIndex = ParamsHeaderCount - 1 - Index(id);
    return *reinterpret_cast<RuntimeCString*>(storage.data() + physicalIndex * sizeof(RuntimeCString));
}

class ProcessSource final : public IParamsHeaderSource
{
public:
    constexpr ProcessSource() noexcept = default;
    const RuntimeCString& Borrow(ParamsHeaderId id) const noexcept override { return Slot(id); }
};
constinit const ProcessSource source;

class CrtRegistrar final : public IParamsHeaderExitRegistrar
{
public:
    int RegisterAtExit(ParamsHeaderExitCallback callback) override { return std::atexit(callback); }
};

template<ParamsHeaderId Id> void __cdecl ReleaseGlobal()
{
    // 原5AA440..5AA5A0普通退出叶，没有noexcept/重试，也不清全局槽。
    DestroyRuntimeCStringAt40CD40(&Slot(Id));
}

constexpr std::array<ParamsHeaderExitCallback, ParamsHeaderCount> releaseCallbacks{
    ReleaseGlobal<ParamsHeaderId::V02>, ReleaseGlobal<ParamsHeaderId::V03>,
    ReleaseGlobal<ParamsHeaderId::V04>, ReleaseGlobal<ParamsHeaderId::V05>,
    ReleaseGlobal<ParamsHeaderId::V051>, ReleaseGlobal<ParamsHeaderId::V052>,
    ReleaseGlobal<ParamsHeaderId::V053>, ReleaseGlobal<ParamsHeaderId::V054>,
    ReleaseGlobal<ParamsHeaderId::V055>, ReleaseGlobal<ParamsHeaderId::V056>,
    ReleaseGlobal<ParamsHeaderId::V057>, ReleaseGlobal<ParamsHeaderId::V058>};

void __cdecl InitializeProcessHeaders()
{
    CrtRegistrar exits;
    for (std::size_t index = 0; index < ParamsHeaderCount; ++index)
    {
        const auto id = static_cast<ParamsHeaderId>(index);
        // 原12项在CRT表内连续；每项仍独立Capture/GetNil/Set/注册，不合并清理资格。
        InitializeParamsHeaderGlobal(id, Slot(id), releaseCallbacks[index], exits);
    }
}
}

const IParamsHeaderSource& ProcessParamsHeaderSource() noexcept { return source; }
std::string_view ParamsHeaderLiteral(ParamsHeaderId id) noexcept { return literals[Index(id)]; }

void InitializeParamsHeaderGlobal(ParamsHeaderId id, RuntimeCString& slot,
    ParamsHeaderExitCallback callback, IParamsHeaderExitRegistrar& exits,
    IRuntimeStringManagerSource& strings)
{
    const auto literal = ParamsHeaderLiteral(id);
    InitializeEmptyRuntimeCString(slot, strings);
    struct UnwindSlot final
    {
        RuntimeCString& slot;
        bool active = true;
        ~UnwindSlot() noexcept { if (active) DestroyRuntimeCStringAt40CD40(&slot); }
    } cleanup{slot};
    SetRuntimeCStringAt437680(slot, literal.data(), static_cast<int>(literal.size()));
    static_cast<void>(exits.RegisterAtExit(callback));
    cleanup.active = false;
}

// 源访问器使静态库成员被引用；显式保留CRT项，不能用首次Serialize或访问器懒建代替。
// 仅声明本连续header批次；根及其他模块的退出注册仍由实际宿主装配负责。
#pragma section(".CRT$XCU", read)
extern "C" __declspec(allocate(".CRT$XCU")) ParamsHeaderExitCallback const KaGuYaParamsHeaderCrtInitializer =
    InitializeProcessHeaders;
#pragma comment(linker, "/include:_KaGuYaParamsHeaderCrtInitializer")
}
