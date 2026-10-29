#include "PropertyPageSystemNative.Internal.h"
#include <Runtime/EnvironmentRootRuntime.h>

namespace KaGuYa::Start::Runtime::PropertyPageSystemDetail
{
void* Resolve(const void* ref, RefOrder order, bool reloadBase)
{
    const auto base = Read<DWORD>(ref);
    if (!base) return nullptr;
    const auto* control = Read<const void*>(ref, 8);
    DWORD bias, stride;
    if (order == RefOrder::BiasFirst)
    { bias = Read<DWORD>(ref, 4); stride = control ? Read<DWORD>(control, 0x0C) : 0u; }
    else
    { stride = control ? Read<DWORD>(control, 0x0C) : 0u; bias = Read<DWORD>(ref, 4); }
    return reinterpret_cast<void*>((reloadBase ? Read<DWORD>(ref) : base) + bias * stride);
}
void* InlineRegistry(PropertyPageSystemNativeServices& s, DWORD index, RefOrder order, bool beginFirst, bool reloadBase)
{
    auto* env = s.source.EnvironmentAt4180C0();
    DWORD begin, end;
    if (beginFirst)
    {
        // 42763E..427D0F这些站点先保存begin，再取end，随后重新读begin算差。
        begin = Read<DWORD>(env, 0x30); end = Read<DWORD>(env, 0x34);
        end -= Read<DWORD>(env, 0x30);
    }
    else { end = Read<DWORD>(env, 0x34); begin = Read<DWORD>(env, 0x30); end -= begin; }
    const auto count = DWORD(std::bit_cast<LONG>(end) / 12);
    if (count <= index) return nullptr;
    return Resolve(reinterpret_cast<void*>(begin + index * 12u), order, reloadBase);
}
void* Registry(PropertyPageSystemNativeServices& s, DWORD index)
{ return s.source.RegistryAt417990(s.source.EnvironmentAt4180C0(), index); }
void* GameSystem(PropertyPageSystemNativeServices& s)
{
    auto* params = s.source.ParamsAt418480();
    return Resolve(reinterpret_cast<void*>(reinterpret_cast<DWORD>(params) + 4u), RefOrder::StrideFirst);
}
void SheetButtons(HWND page, PropertyPageSystemNativeServices& s)
{
    const auto parent = s.api.GetParent(page); // 原caller即使6197EC非零也先GetParent。
    if (*static_cast<const volatile DWORD*>(&s.sheetButtonsAt6197EC)) return;
    const auto cancel = s.api.GetDlgItem(parent, 2);
    static_cast<void>(s.api.SetWindowTextA(cancel, "\x95\xC2\x82\xB6\x82\xE9")); // 5AE644 閉じる
    const auto ok = s.api.GetDlgItem(parent, 1);
    static_cast<void>(s.api.SetWindowTextA(ok, "\x8D\x58\x90\x56")); // 5AE64C 更新
    const auto old = s.api.GetWindowLongA(parent, -20);
    const auto next = std::bit_cast<LONG>(DWORD(old) & 0xFFFFFBFFu);
    if (old != next) static_cast<void>(s.api.SetWindowLongA(parent, -20, next));
    s.sheetButtonsAt6197EC = 1;
}
LRESULT ItemMessage(const void* page, int id, UINT m, WPARAM p, LPARAM l, PropertyPageSystemNativeServices& s)
{ return s.api.SendMessageA(s.api.GetDlgItem(Window(page), id), m, p, l); }
void Enable(const void* page, int id, BOOL enabled, PropertyPageSystemNativeServices& s)
{ static_cast<void>(s.api.EnableWindow(s.api.GetDlgItem(Window(page), id), enabled)); }
void Check(const void* page, int id, BYTE value, PropertyPageSystemNativeServices& s)
{ static_cast<void>(ItemMessage(page, id, 0xF1, value != 0, 0, s)); }
BYTE Checked(const void* page, int id, PropertyPageSystemNativeServices& s)
{ return ItemMessage(page, id, 0xF0, 0, 0, s) == 1; }
void DdxRadio(const void* page, int id, DWORD& value, BOOL save, PropertyPageSystemNativeServices& s)
{
    auto item = s.api.GetDlgItem(Window(page), id);
    if (save) value = 0xFFFFFFFFu;
    DWORD index = 0;
    // 415290：首项无null检查；只数DLGC_RADIOBUTTON，下一兄弟WS_GROUP才结束。
    for (;;)
    {
        if (s.api.SendMessageA(item, 0x87, 0, 0) & 0x40)
        {
            if (save)
            { if (s.api.SendMessageA(item, 0xF0, 0, 0)) value = index; }
            else static_cast<void>(s.api.SendMessageA(item, 0xF1, index == *static_cast<const volatile DWORD*>(&value), 0));
            ++index;
        }
        item = s.api.GetWindow(item, 2);
        if (!item || (s.api.GetWindowLongA(item, -16) & 0x20000)) break;
    }
}
int ReadText(HWND item, RuntimeCString& text, PropertyPageSystemNativeServices& s, int* expectedLength)
{
    const auto length = s.api.GetWindowTextLengthA(item);
    if (expectedLength) *expectedLength = length;
    if (length < 0) throw ATL::CAtlException(E_INVALIDARG);
    auto* chars = Read<char*>(&text);
    auto* header = reinterpret_cast<ATL::CStringData*>(reinterpret_cast<DWORD>(chars) - 0x10u);
    const auto allocated = Read<DWORD>(header, 8), refs = Read<DWORD>(header, 0x0C);
    if (std::bit_cast<LONG>((1u - refs) | (allocated - DWORD(length))) < 0)
    { PrepareRuntimeCStringWriteAt42D590(text, length); chars = Read<char*>(&text); }
    header = reinterpret_cast<ATL::CStringData*>(reinterpret_cast<DWORD>(chars) - 0x10u);
    if (length > Read<LONG>(header, 8)) throw ATL::CAtlException(E_INVALIDARG);
    Write<LONG>(header, 4, length);
    Read<char*>(&text)[length] = 0;
    const auto returned = s.api.GetWindowTextA(item, chars, std::bit_cast<LONG>(DWORD(length) + 1u));
    chars = Read<char*>(&text);
    DWORD actual = 0;
    if (chars)
    {
        const auto capacity = Read<DWORD>(chars, DWORD(-8));
        // 54C210是有界首NUL长度，不用GetWindowText返回值当CString长度。
        actual = DWORD(::strnlen(chars, capacity));
        if (std::bit_cast<LONG>(actual) < 0) throw ATL::CAtlException(E_INVALIDARG);
    }
    chars = Read<char*>(&text);
    if (std::bit_cast<LONG>(actual) > Read<LONG>(chars, DWORD(-8))) throw ATL::CAtlException(E_INVALIDARG);
    Write<DWORD>(chars, DWORD(-12), actual);
    Read<char*>(&text)[actual] = 0;
    return returned;
}
BOOL PutText(const void* page, int id, const RuntimeCString& text, PropertyPageSystemNativeServices& s)
{
    const auto result = s.api.SetDlgItemTextA(Window(page), id, Read<const char*>(&text));
    if (result) return result;
    static_cast<void>(s.api.MessageBeep(UINT(-1)));
    static_cast<void>(s.api.SetFocus(s.api.GetDlgItem(Window(page), id)));
    return FALSE;
}
BOOL DdxText(const void* page, int id, RuntimeCString& text, BOOL save, PropertyPageSystemNativeServices& s)
{
    // 本三页调用的size=4/maxEnabled=0/max=0；原助手不使用size，无长度验证分支。
    if (!save) return PutText(page, id, text, s);
    const auto item = s.api.GetDlgItem(Window(page), id);
    int length;
    const auto returned = ReadText(item, text, s, &length);
    if (returned >= length) return TRUE;
    static_cast<void>(s.api.MessageBeep(UINT(-1)));
    static_cast<void>(s.api.SetFocus(s.api.GetDlgItem(Window(page), id)));
    return FALSE;
}
BOOL ReadSystem2(PropertyPageSystem2NativeBody68h& b, PropertyPageSystemNativeServices& s)
{
    DdxRadio(&b, 0x514, b.firstChoiceAt54, TRUE, s);
    DdxRadio(&b, 0x517, b.secondChoiceAt58, TRUE, s);
    if (!DdxText(&b, 0x578, b.familyNameAt60, TRUE, s)) return FALSE;
    if (!DdxText(&b, 0x579, b.givenNameAt5C, TRUE, s)) return FALSE;
    b.skipNameDialogAt64 = Checked(&b, 0x4B0, s);
    return TRUE;
}
BOOL WriteSystem2(PropertyPageSystem2NativeBody68h& b, PropertyPageSystemNativeServices& s)
{
    DdxRadio(&b, 0x514, b.firstChoiceAt54, FALSE, s);
    DdxRadio(&b, 0x517, b.secondChoiceAt58, FALSE, s);
    if (!PutText(&b, 0x578, b.familyNameAt60, s)) return FALSE;
    if (!PutText(&b, 0x579, b.givenNameAt5C, s)) return FALSE;
    Check(&b, 0x4B0, Read<BYTE>(&b, 0x64), s);
    return TRUE;
}
void EnableEffects(PropertyPageSystem1NativeBody60h& b, PropertyPageSystemNativeServices& s)
{
    for (const auto id : {0x5DC, 0x640, 0x641, 0x642})
    {
        const auto item = s.api.GetDlgItem(Window(&b), id);
        static_cast<void>(s.api.EnableWindow(item, Read<BYTE>(&b, 0x56) == 0));
    }
}
}
