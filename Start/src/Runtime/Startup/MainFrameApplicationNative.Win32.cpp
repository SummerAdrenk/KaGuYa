#include <Runtime/MainFrameApplicationNativeApi.h>
#include <type_traits>

namespace KaGuYa::Start::Runtime
{
namespace
{
class Api final : public IMainFrameApplicationNativeApi
{
public:
    int GetSystemMetrics(int index) override { return ::GetSystemMetrics(index); }
    BOOL AdjustWindowRectEx(RECT* rect, DWORD style, BOOL menu, DWORD exStyle) override
    { return ::AdjustWindowRectEx(rect, style, menu, exStyle); }
    BOOL OffsetRect(RECT* rect, int x, int y) override { return ::OffsetRect(rect, x, y); }
    HWND CreateWindowExA(DWORD exStyle, LPCSTR type, LPCSTR title, DWORD style, int x, int y, int width, int height,
        HWND parent, HMENU menu, HINSTANCE instance, void* parameter) override
    { return ::CreateWindowExA(exStyle, type, title, style, x, y, width, height, parent, menu, instance, parameter); }
    BOOL ShowWindow(HWND window, int show) override { return ::ShowWindow(window, show); }
    void SetLastError(DWORD error) override { ::SetLastError(error); }
    int MessageBoxA(HWND window, LPCSTR text, LPCSTR caption, UINT flags) override
    { return ::MessageBoxA(window, text, caption, flags); }
};
static_assert(std::is_trivially_destructible_v<Api>);
constinit Api api;
}
IMainFrameApplicationNativeApi& DefaultMainFrameApplicationNativeApi() noexcept { return api; }
}
