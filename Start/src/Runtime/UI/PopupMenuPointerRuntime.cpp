#include <Runtime/PopupMenuPointerRuntime.h>
#include <Runtime/WindowModeManagerRuntime.h>
#include <bit>

namespace KaGuYa::Start::Runtime
{
BOOL Win32PopupMenuPointerPlatformApi::ClientToScreen(HWND window, POINT& point)
{ return ::ClientToScreen(window, &point); }
BOOL Win32PopupMenuPointerPlatformApi::SetCursorPos(int x, int y)
{ return ::SetCursorPos(x, y); }
namespace
{
LONG AddDword(LONG left, LONG right) noexcept
{
    return std::bit_cast<LONG>(static_cast<std::uint32_t>(left) + static_cast<std::uint32_t>(right));
}
LONG ScalePointer(LONG value, LONG factor, LONG denominator)
{
    // 4513A5/A8、4513BC/BF：低DWORD乘积、EDX=0后DIV；保留硬件除零。
    LONG result;
    __asm
    {
        mov eax, value
        imul eax, factor
        xor edx, edx
        div denominator
        mov result, eax
    }
    return result;
}
}
void MovePopupPointerAt4512E0(IPopupMenuPointerContext& source,
    IPopupMenuPointerPlatformApi& platform, POINT logical)
{
    source.SynchronizePointerRootAt413BE0();
    if (source.PointerModeAt0C().ModeAt04() != 0)
    {
        source.SynchronizePointerRootAt413BE0();
        const auto& offsets = source.PointerModeAt0C().RequireViewportAt0C();
        const LONG offsetX = offsets[0], offsetY = offsets[1];
        source.SynchronizePointerRootAt413BE0();
        const auto& extent = source.PointerModeAt0C().RequireViewportAt0C();
        const LONG width = extent[2], height = extent[3];
        // 两次4180C0先X后Y，不把环境尺寸保存成同一代快照。
        logical.x = ScalePointer(AddDword(offsetX, logical.x), width, source.ReadGuiWidthAt18());
        logical.y = ScalePointer(AddDword(offsetY, logical.y), height, source.ReadGuiHeightAt1C());
    }
    const HWND window = source.PointerWindowAt2C(); // 4513D2最后重新取得环境HWND。
    static_cast<void>(platform.ClientToScreen(window, logical));
    static_cast<void>(platform.SetCursorPos(logical.x, logical.y));
}
}
