#include "DebugMute.h"
#include <Logging/Logger.h>

#include <type_traits>

namespace KaGuYa::RenderDX
{
namespace
{
using MuteProc = void (__cdecl*)(BOOL);

struct MuteState
{
    DWORD previous = 0;
    DWORD registry = MAXDWORD;
    MuteProc regular = nullptr;
    MuteProc debug = nullptr;
    unsigned reported = 0;
};
static_assert(std::is_trivially_destructible_v<MuteState>);
constinit MuteState state;

void ReportOnce(unsigned bit, const char* operation, const char* name, DWORD result) noexcept
{
    if (state.reported & bit) return;
    state.reported |= bit;
    Logging::WriteFormat(Logging::Level::Warning, "D3D debug mute: %s (%s), status=%lu.",
        operation, name, result);
}

void ReturnModuleReference(HMODULE module, unsigned diagnosticBit, const char* name) noexcept
{
    if (!FreeLibrary(module)) ReportOnce(diagnosticBit, "FreeLibrary failed", name, GetLastError());
}

MuteProc FindMute(const char* name, unsigned diagnosticShift) noexcept
{
    // 原0505E/508E：缺席模块不主动加载；Load失败不借GetModuleHandle的句柄兜底。
    if (!GetModuleHandleA(name)) return nullptr;
    const HMODULE module = LoadLibraryA(name);
    if (!module)
    {
        ReportOnce(1u << diagnosticShift, "LoadLibrary failed", name, GetLastError());
        return nullptr;
    }
    const auto address = GetProcAddress(module, "DebugSetMute");
    if (!address) ReportOnce(2u << diagnosticShift, "optional export absent", name, GetLastError());

    // 原从不归还LoadLibrary引用，入口跨Release及RenderDX卸载持续有效。
    // PIN保留同样的进程生命周期；两笔临时引用都在此归还，不在DllMain操作加载器。
    HMODULE pinned = nullptr;
    if (!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_PIN | GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
        reinterpret_cast<LPCSTR>(module), &pinned))
    {
        ReportOnce(4u << diagnosticShift, "module pin failed; callback not published", name, GetLastError());
        ReturnModuleReference(module, 8u << diagnosticShift, name);
        return nullptr;
    }
    ReturnModuleReference(pinned, 8u << diagnosticShift, name);
    ReturnModuleReference(module, 8u << diagnosticShift, name);
    return reinterpret_cast<MuteProc>(address);
}

void ReadRegistrySetting() noexcept
{
    HKEY key = nullptr;
    const auto opened = RegOpenKeyA(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Direct3D", &key);
    if (opened != ERROR_SUCCESS)
    {
        // 原050CF失败覆盖为0，之后本映像内不重查；不是“失败就禁止调试调用”。
        state.registry = 0;
        ReportOnce(1u << 8, "registry open failed; original mute-enabled default", "D3DXDoNotMute", opened);
        return;
    }
    DWORD type = 0, bytes = sizeof(state.registry);
    const auto queried = RegQueryValueExA(key, "D3DXDoNotMute", nullptr, &type,
        reinterpret_cast<BYTE*>(&state.registry), &bytes);
    const auto closed = RegCloseKey(key);
    if (closed != ERROR_SUCCESS)
        ReportOnce(1u << 9, "registry close failed; query result retained", "D3DXDoNotMute", closed);
    if (queried != ERROR_SUCCESS || type != REG_DWORD)
    {
        state.registry = 0;
        ReportOnce(1u << 10, "registry query/type rejected; original mute-enabled default", "D3DXDoNotMute",
            queried != ERROR_SUCCESS ? static_cast<DWORD>(queried) : type);
    }
    else
    {
        // 原不检查返回bytes==4；FFFFFFFF亦归一为1，不能继续当作未查询。
        state.registry = state.registry != 0 ? 1u : 0u;
    }
}
}

DWORD SetDebugMute(DWORD requested) noexcept
{
    const DWORD previous = state.previous;
    state.previous = requested;
    if (!state.regular && !state.debug)
    {
        // 同一批regular已找到仍探测debug；任一入口存在后不再补发现其它模块。
        state.regular = FindMute("d3d9.dll", 0);
        state.debug = FindMute("d3d9d.dll", 4);
    }
    if (state.registry == MAXDWORD) ReadRegistrySetting();
    if (state.registry == 0)
    {
        if (state.regular) state.regular(static_cast<BOOL>(requested));
        if (state.debug) state.debug(static_cast<BOOL>(requested));
    }
    return previous;
}
}
