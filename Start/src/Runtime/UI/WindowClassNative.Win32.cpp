#include "WindowClassNative.Internal.h"
#include <cstdio>
#include <cstdlib>

namespace KaGuYa::Start::Runtime
{
namespace
{
thread_local IWindowClassNativeApi* activeApi = nullptr;
class Win32Api final : public IWindowClassNativeApi
{
public:
    void EnterCriticalSection(LPCRITICAL_SECTION lock) override { ::EnterCriticalSection(lock); }
    void LeaveCriticalSection(LPCRITICAL_SECTION lock) override { ::LeaveCriticalSection(lock); }
    BOOL GetClassInfoExA(HINSTANCE instance, LPCSTR name, WNDCLASSEXA* out) override
    { return ::GetClassInfoExA(instance, name, out); }
    HCURSOR LoadCursorA(HINSTANCE instance, LPCSTR name) override { return ::LoadCursorA(instance, name); }
    ATOM RegisterClassExA(const WNDCLASSEXA* descriptor) override { return ::RegisterClassExA(descriptor); }
    int FormatAtlName(char* buffer, std::size_t count, const void* pointer) override
    { return ::sprintf_s(buffer, count, "ATL:%p", pointer); }
    void* Recalloc(void* data, std::size_t count, std::size_t width) override { return ::_recalloc(data, count, width); }
    void Free(void* data) override { std::free(data); }
    BOOL UnregisterClassA(LPCSTR name, HINSTANCE instance) override { return ::UnregisterClassA(name, instance); }
    BOOL InitializeCriticalSectionAndSpinCount(LPCRITICAL_SECTION lock, DWORD spin) override
    { return ::InitializeCriticalSectionAndSpinCount(lock, spin); }
    DWORD GetLastError() override { return ::GetLastError(); }
    void DeleteCriticalSection(LPCRITICAL_SECTION lock) override { ::DeleteCriticalSection(lock); }
    void RaiseException(DWORD code, DWORD flags, DWORD count, const ULONG_PTR* args) override
    { ::RaiseException(code, flags, count, args); }
};
static_assert(std::is_trivially_destructible_v<Win32Api>);
constinit Win32Api processApi{};
}
IWindowClassNativeApi& DefaultWindowClassNativeApi() noexcept { return processApi; }
WindowClassNativeScope::WindowClassNativeScope(IWindowClassNativeApi& api) noexcept : previous_(activeApi)
{ activeApi = &api; }
WindowClassNativeScope::~WindowClassNativeScope() noexcept { activeApi = previous_; }
namespace WindowClassNativeDetail
{
IWindowClassNativeApi& CurrentApi() noexcept { return activeApi ? *activeApi : processApi; }
}
}
