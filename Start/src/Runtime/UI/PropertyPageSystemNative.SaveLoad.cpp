#include "PropertyPageSystemNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
using namespace PropertyPageSystemDetail;
LRESULT InitializePropertyPageSaveLoadNativeAt4253C0(PropertyPageSaveLoadNativeBody5Ch& b, PropertyPageSystemNativeServices& s)
{
    SheetButtons(Window(&b), s);
    b.thumbnailAt54 = Read<BYTE>(InlineRegistry(s, 1, RefOrder::StrideFirst), 0x74) != 0;
    b.historyAt58 = Read<BYTE>(InlineRegistry(s, 1, RefOrder::StrideFirst), 0x75) != 0;
    b.autoSaveAt55 = Read<BYTE>(InlineRegistry(s, 1, RefOrder::StrideFirst), 0x76) != 0;
    b.quickSaveConfirmAt56 = Read<BYTE>(InlineRegistry(s, 1, RefOrder::StrideFirst), 0x79) != 0;
    b.quickLoadConfirmAt57 = Read<BYTE>(InlineRegistry(s, 1, RefOrder::StrideFirst), 0x7A) != 0;
    for (DWORD i = 0; i != 5; ++i) Check(&b, 0x4B0 + int(i), Read<BYTE>(&b, 0x54 + i), s);
    return 1;
}
BOOL ApplyPropertyPageSaveLoadNativeAt421450(PropertyPageSaveLoadNativeBody5Ch& b, PropertyPageSystemNativeServices& s)
{
    for (DWORD i = 0; i != 5; ++i) Write<BYTE>(&b, 0x54 + i, Checked(&b, 0x4B0 + int(i), s));
    // 原五次独立4180C0；页字段在每次捕获后才读，不把它们提前做DTO快照。
    auto* save = InlineRegistry(s, 1, RefOrder::BiasFirst); Write<BYTE>(save, 0x74, Read<BYTE>(&b, 0x54));
    save = InlineRegistry(s, 1, RefOrder::BiasFirst); Write<BYTE>(save, 0x75, Read<BYTE>(&b, 0x58));
    save = InlineRegistry(s, 1, RefOrder::BiasFirst); Write<BYTE>(save, 0x76, Read<BYTE>(&b, 0x55));
    save = InlineRegistry(s, 1, RefOrder::StrideFirst); Write<BYTE>(save, 0x79, Read<BYTE>(&b, 0x56));
    save = InlineRegistry(s, 1, RefOrder::StrideFirst); Write<BYTE>(save, 0x7A, Read<BYTE>(&b, 0x57));
    return TRUE;
}
BOOL ProcessPropertyPageSaveLoadNativeAt42FDE0(PropertyPageSaveLoadNativeBody5Ch& b, HWND, UINT m,
    WPARAM p, LPARAM l, LRESULT* out, DWORD map, PropertyPageSystemNativeServices& s)
{
    if (map) return FALSE;
    if (m == 0x110) { *out = InitializePropertyPageSaveLoadNativeAt4253C0(b, s); return TRUE; }
    if (m == 0x111 && (LOWORD(p) == 0x44C || LOWORD(p) == 0x44D))
    {
        // 42FE36..42FE76：无通知码门控，MessageBox返回值直接丢弃；没有删除callee。
        const bool quick = LOWORD(p) == 0x44D;
        const char* caption = quick
            ? "\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x5A\x81\x5B\x83\x75\x82\xCC\x8F\x89\x8A\xFA\x89\xBB"
            : "\x83\x49\x81\x5B\x83\x67\x83\x5A\x81\x5B\x83\x75\x82\xCC\x8F\x89\x8A\xFA\x89\xBB";
        const char* text = quick
            ? "\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x5A\x81\x5B\x83\x75\x82\xB3\x82\xEA\x82\xBD\x83\x66\x81\x5B\x83\x5E\x82\xF0\x91\x53\x82\xC4\x8D\xED\x8F\x9C\x82\xB5\x82\xDC\x82\xB7\x81\x42\x82\xE6\x82\xEB\x82\xB5\x82\xA2\x82\xC5\x82\xB7\x82\xA9\x81\x48"
            : "\x83\x49\x81\x5B\x83\x67\x83\x5A\x81\x5B\x83\x75\x82\xB3\x82\xEA\x82\xBD\x83\x66\x81\x5B\x83\x5E\x82\xF0\x91\x53\x82\xC4\x8D\xED\x8F\x9C\x82\xB5\x82\xDC\x82\xB7\x81\x42\x82\xE6\x82\xEB\x82\xB5\x82\xA2\x82\xC5\x82\xB7\x82\xA9\x81\x48";
        static_cast<void>(s.api.MessageBoxA(Window(&b), text, caption, 0x24));
        *out = 0; return TRUE;
    }
    if (m == 0x4E) return Notify(&b, l, out, s, [&] { return ApplyPropertyPageSaveLoadNativeAt421450(b, s); });
    return FALSE;
}
}
