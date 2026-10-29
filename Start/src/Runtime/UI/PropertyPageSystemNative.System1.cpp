#include "PropertyPageSystemNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
using namespace PropertyPageSystemDetail;
BOOL DdxPropertyPageSystem1NativeAt415D80(PropertyPageSystem1NativeBody60h& b, BOOL save, DWORD control, PropertyPageSystemNativeServices& s)
{
    for (DWORD i = 0; i != 4; ++i)
    {
        if (control != DWORD(-1) && control != 0x4B0 + i) continue;
        if (save) Write<BYTE>(&b, 0x54 + i, Checked(&b, 0x4B0 + int(i), s));
        else Check(&b, 0x4B0 + int(i), Read<BYTE>(&b, 0x54 + i), s);
    }
    if (control == DWORD(-1) || control == 0x644)
        if (!DdxText(&b, 0x644, b.rendererTextAt58, save, s)) return FALSE;
    if (control == DWORD(-1) || control == 0x514) DdxRadio(&b, 0x514, b.rightClickModeAt5C, save, s);
    return TRUE;
}
LRESULT InitializePropertyPageSystem1NativeAt4274E0(PropertyPageSystem1NativeBody60h& b, PropertyPageSystemNativeServices& s)
{
    SheetButtons(Window(&b), s);
    b.skipStartupAt54 = Read<BYTE>(InlineRegistry(s, 1, RefOrder::StrideFirst), 4);
    SetRuntimeCStringAt437680(b.rendererTextAt58, "API \x95\x60\x89\xE6", 8);
    const auto renderer = Read<DWORD>(InlineRegistry(s, 1, RefOrder::StrideFirst), 8);
    // 5AE7E8原字节确为DirextX，不能按资源里的DirectX改正。
    if (renderer == 1) SetRuntimeCStringAt437680(b.rendererTextAt58, "DirextX \x95\x60\x89\xE6", 12);
    else if (renderer == 2) SetRuntimeCStringAt437680(b.rendererTextAt58, "OpenGL \x95\x60\x89\xE6", 11);
    const auto font = s.api.SendMessageA(Window(&b), 0x31, 0, 0);
    LOGFONTA description;
    if (s.api.GetObjectA(reinterpret_cast<HGDIOBJ>(font), 0x3C, &description) == 0x3C)
    {
        description.lfWeight = 700;
        const auto bold = s.api.CreateFontIndirectA(&description);
        static_cast<void>(ItemMessage(&b, 0x644, 0x30, reinterpret_cast<WPARAM>(bold), 1, s));
        // 原427619不保存HFONT且原三页析构没有DeleteObject；不擅自新增释放资格。
    }
    b.stopAnimationAt55 = Read<BYTE>(InlineRegistry(s, 1, RefOrder::BiasFirst, true), 0x6D) == 0;
    b.stopSelectionAnimationAt57 = Read<BYTE>(InlineRegistry(s, 1, RefOrder::BiasFirst, true), 0x6E) == 0;
    const auto animation = s.api.GetDlgItem(Window(&b), 0x4B3);
    static_cast<void>(s.api.EnableWindow(animation, Read<BYTE>(&b, 0x55) == 0));
    b.disableEffectAt56 = Read<BYTE>(InlineRegistry(s, 1, RefOrder::BiasFirst, true), 0x6F) == 0;
    const auto track = s.api.GetDlgItem(Window(&b), 0x5DC);
    static_cast<void>(s.api.SendMessageA(track, 0x407, 0, 0));
    static_cast<void>(s.api.SendMessageA(track, 0x408, 0, 3));
    const auto effect = Read<DWORD>(InlineRegistry(s, 1, RefOrder::StrideFirst, true, true), 0x70);
    static_cast<void>(s.api.SendMessageA(track, 0x405, 1, std::bit_cast<LONG>(3u - effect)));
    EnableEffects(b, s);
    b.rightClickModeAt5C = Read<DWORD>(InlineRegistry(s, 1, RefOrder::StrideFirst), 0x8C);
    for (DWORD i = 0; i != 4; ++i) Check(&b, 0x4B0 + int(i), Read<BYTE>(&b, 0x54 + i), s);
    if (PutText(&b, 0x644, b.rendererTextAt58, s)) DdxRadio(&b, 0x514, b.rightClickModeAt5C, FALSE, s);
    return 1; // SetDlgItemText失败仍返回1，已发生Beep/Focus，不追加radio DDX。
}
BOOL ApplyPropertyPageSystem1NativeAt422940(PropertyPageSystem1NativeBody60h& b, PropertyPageSystemNativeServices& s)
{
    static_cast<void>(DdxPropertyPageSystem1NativeAt415D80(b, TRUE, DWORD(-1), s));
    auto* save = InlineRegistry(s, 1, RefOrder::BiasFirst); Write<BYTE>(save, 4, Read<BYTE>(&b, 0x54));
    save = InlineRegistry(s, 1, RefOrder::BiasFirst); Write<BYTE>(save, 0x6D, Read<BYTE>(&b, 0x55) == 0);
    save = InlineRegistry(s, 1, RefOrder::BiasFirst); Write<BYTE>(save, 0x6E, Read<BYTE>(&b, 0x57) == 0);
    save = InlineRegistry(s, 1, RefOrder::BiasFirst); Write<BYTE>(save, 0x6F, Read<BYTE>(&b, 0x56) == 0);
    const auto position = DWORD(ItemMessage(&b, 0x5DC, 0x400, 0, 0, s));
    save = InlineRegistry(s, 1, RefOrder::StrideFirst); Write<DWORD>(save, 0x70, 3u - position);
    save = InlineRegistry(s, 1, RefOrder::StrideFirst); Write<DWORD>(save, 0x8C, Read<DWORD>(&b, 0x5C));
    return TRUE; // 原422948不检查DDX返回值，不能把失败变成Apply拒绝。
}
BOOL ProcessPropertyPageSystem1NativeAt430490(PropertyPageSystem1NativeBody60h& b, HWND, UINT m,
    WPARAM p, LPARAM l, LRESULT* out, DWORD map, PropertyPageSystemNativeServices& s)
{
    if (map) return FALSE;
    if (m == 0x110) { *out = InitializePropertyPageSystem1NativeAt4274E0(b, s); return TRUE; }
    if (m == 0x138)
    {
        const auto label = s.api.GetDlgItem(Window(&b), 0x644);
        if (label != reinterpret_cast<HWND>(l)) { *out = 0; return TRUE; }
        const auto dc = reinterpret_cast<HDC>(p);
        static_cast<void>(s.api.SetTextColor(dc, 0x00E00000));
        static_cast<void>(s.api.SetBkMode(dc, 1));
        *out = reinterpret_cast<LRESULT>(s.api.GetStockObject(5)); return TRUE;
    }
    if (m == 0x111 && (LOWORD(p) == 0x4B1 || LOWORD(p) == 0x4B2))
    {
        static_cast<void>(DdxPropertyPageSystem1NativeAt415D80(b, TRUE, DWORD(-1), s));
        if (LOWORD(p) == 0x4B1)
        {
            const auto item = s.api.GetDlgItem(Window(&b), 0x4B3);
            static_cast<void>(s.api.EnableWindow(item, Read<BYTE>(&b, 0x55) == 0));
        }
        else EnableEffects(b, s);
        *out = 0; return TRUE;
    }
    if (m == 0x4E) return Notify(&b, l, out, s, [&] { return ApplyPropertyPageSystem1NativeAt422940(b, s); });
    return FALSE;
}
}
