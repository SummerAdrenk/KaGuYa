#include <Runtime/WindowClassNative.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
void InitializePeRecord(WindowClassNativeInfo68h& info, WNDPROC initial) noexcept
{
    info = {};
    info.descriptorAt00.cbSize = 0x30;
    info.descriptorAt00.style = 0x0B;
    info.descriptorAt00.lpfnWndProc = initial;
    info.descriptorAt00.hbrBackground = reinterpret_cast<HBRUSH>(6);
    info.cursorNameAt38 = reinterpret_cast<LPCSTR>(0x7F00);
    info.systemCursorAt3C = 1;
}
}
void InitializeEditWindowClassNativeAt601838(WindowClassNativeInfo68h& info, WNDPROC initialAt48E090) noexcept
{
    // 权威PE raw00200238；仅重定位+8的48E090，其他字段逐项来自原68h字节。
    InitializePeRecord(info, initialAt48E090);
}
void InitializeEditControlClassNativeAt6015F0(WindowClassNativeInfo68h& info, WNDPROC initialAt48E020) noexcept
{
    // 权威PE raw001FFFF0；+30原为0，不能把后续caller的EDIT发布混入初值。
    InitializePeRecord(info, initialAt48E020);
}
void PrepareEditControlClassNativeAt485631(WindowClassNativeInfo68h& info) noexcept
{
    const auto current = *static_cast<LPCSTR const volatile*>(&info.superclassAt30);
    info.superclassAt30 = current ? current : "EDIT";
}
}
