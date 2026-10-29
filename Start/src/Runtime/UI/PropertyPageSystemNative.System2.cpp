#include "PropertyPageSystemNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
using namespace PropertyPageSystemDetail;
LRESULT InitializePropertyPageSystem2NativeAt427A40(PropertyPageSystem2NativeBody68h& b, PropertyPageSystemNativeServices& s)
{
    SheetButtons(Window(&b), s);
    b.skipNameDialogAt64 = Read<BYTE>(InlineRegistry(s, 1, RefOrder::StrideFirst), 0x54);
    AssignRuntimeCStringAt410220(b.givenNameAt5C, Text(InlineRegistry(s, 1, RefOrder::StrideFirst), 0x58));
    if (!(Read<BYTE>(GameSystem(s), 0x2C) & 1))
    {
        Enable(&b, 0x579, FALSE, s);
        AssignRuntimeCStringAt410220(b.givenNameAt5C, Text(GameSystem(s), 0x30));
    }
    AssignRuntimeCStringAt410220(b.familyNameAt60, Text(InlineRegistry(s, 1, RefOrder::StrideFirst), 0x5C));
    if (!(Read<BYTE>(GameSystem(s), 0x2C) & 2))
    {
        Enable(&b, 0x578, FALSE, s);
        AssignRuntimeCStringAt410220(b.familyNameAt60, Text(GameSystem(s), 0x34));
    }
    // sentinel测试与非sentinel取值使用不同来源；不能缓存第一次slot7。
    b.firstChoiceAt54 = Read<WORD>(InlineRegistry(s, 7, RefOrder::StrideFirst), 0x10) == 0xFFFF
        ? 0u : DWORD(Read<WORD>(Registry(s, 7), 0x10)) + 1u;
    b.secondChoiceAt58 = Read<WORD>(InlineRegistry(s, 7, RefOrder::StrideFirst), 0x12) == 0xFFFF
        ? 0u : DWORD(Read<WORD>(Registry(s, 7), 0x12)) + 1u;
    static_cast<void>(WriteSystem2(b, s));
    return 1;
}
BOOL ApplyPropertyPageSystem2NativeAt422B90(PropertyPageSystem2NativeBody68h& b, PropertyPageSystemNativeServices& s)
{
    static_cast<void>(ReadSystem2(b, s)); // 原422BD3/422BEF仅跳过剩余DDX，不跳校验/提交。
    if ((Read<BYTE>(GameSystem(s), 0x2C) & 1) && Read<LONG>(Read<const void*>(&b, 0x5C), DWORD(-12)) == 0)
    {
        auto* env = s.source.EnvironmentAt4180C0();
        static_cast<void>(s.api.MessageBoxA(Read<HWND>(env, 0x2C),
            "\x93\x6F\x98\x5E\x96\xBC\x82\xCC\x96\xBC\x91\x4F\x82\xAA\x90\xDD\x92\xE8\x82\xB3\x82\xEA\x82\xC4\x82\xA2\x82\xDC\x82\xB9\x82\xF1\x81\x49",
            Read<const char*>(env, 0x10), 0x10));
        return FALSE;
    }
    if ((Read<BYTE>(GameSystem(s), 0x2C) & 2) && Read<LONG>(Read<const void*>(&b, 0x60), DWORD(-12)) == 0)
    {
        auto* env = s.source.EnvironmentAt4180C0();
        static_cast<void>(s.api.MessageBoxA(Read<HWND>(env, 0x2C),
            "\x93\x6F\x98\x5E\x96\xBC\x82\xCC\x96\xBC\x8E\x9A\x82\xAA\x90\xDD\x92\xE8\x82\xB3\x82\xEA\x82\xC4\x82\xA2\x82\xDC\x82\xB9\x82\xF1\x81\x49",
            Read<const char*>(env, 0x10), 0x10));
        return FALSE;
    }
    auto* save = Registry(s, 1); Write<BYTE>(save, 0x54, Read<BYTE>(&b, 0x64));
    AssignRuntimeCStringAt410220(Text(Registry(s, 1), 0x58), b.givenNameAt5C);
    AssignRuntimeCStringAt410220(Text(Registry(s, 1), 0x5C), b.familyNameAt60);
    const auto first = Read<DWORD>(&b, 0x54);
    auto* globals = Registry(s, 7); Write<WORD>(globals, 0x10, WORD(first ? first - 1u : 0xFFFFu));
    const auto second = Read<DWORD>(&b, 0x58);
    globals = Registry(s, 7); Write<WORD>(globals, 0x12, WORD(second ? second - 1u : 0xFFFFu));
    return TRUE;
}
LRESULT DefaultNamesPropertyPageSystem2NativeAt423E20(PropertyPageSystem2NativeBody68h& b, PropertyPageSystemNativeServices& s)
{
    static_cast<void>(ReadSystem2(b, s));
    if (Read<BYTE>(GameSystem(s), 0x2C) & 2)
        AssignRuntimeCStringAt410220(b.familyNameAt60, Text(GameSystem(s), 0x34));
    if (Read<BYTE>(GameSystem(s), 0x2C) & 1)
        AssignRuntimeCStringAt410220(b.givenNameAt5C, Text(GameSystem(s), 0x30));
    static_cast<void>(WriteSystem2(b, s));
    return 0; // 原成功、写文本失败均返回0，不保存至Environment。
}
LRESULT LimitNamePropertyPageSystem2NativeAt424BC0(PropertyPageSystem2NativeBody68h& b, WORD control, PropertyPageSystemNativeServices& s)
{
    union Slot { RuntimeCString text; Slot() noexcept {} ~Slot() noexcept {} } original, left;
    InitializeEmptyRuntimeCString(original.text, s.strings);
    struct Unwind final
    {
        Slot& original;
        Slot& left;
        int state = 0;
        ~Unwind() noexcept
        {
            // 5E2DE0：展开时先left再original；第二次异常终止，不在catch内伪造正常Free。
            if (state == 1) DestroyRuntimeCStringAt40CD40(&left.text);
            if (state >= 0) DestroyRuntimeCStringAt40CD40(&original.text);
        }
    } unwind{original, left};
    {
        const auto item = s.api.GetDlgItem(Window(&b), control);
        static_cast<void>(ReadText(item, original.text, s)); // 此caller忽略短读，不Beep/Focus。
        const auto* chars = Read<const char*>(&original.text);
        if (Read<LONG>(chars, DWORD(-12)) > 6)
        {
            int count = 0;
            // 424CB3每轮movsx；<=6字节的文本完全不调用_ismbblead。
            while (count < Read<LONG>(Read<const void*>(&original.text), DWORD(-12)))
            {
                chars = Read<const char*>(&original.text);
                if (count < 0 || count > Read<LONG>(chars, DWORD(-12))) throw ATL::CAtlException(E_INVALIDARG);
                int next = count + (s.api.IsMbLead(static_cast<signed char>(chars[count])) ? 2 : 1);
                if (next > 6) break;
                count = next;
            }
            // 419D00与424CD6..424D1C相同Clone/Allocate/发布核；count为原保留前缀字节数。
            ConstructRuntimeCStringLeftAt419D00(&left.text, original.text, count, s.strings);
            unwind.state = 1;
            AssignRuntimeCStringAt410220(original.text, left.text);
            unwind.state = 0;
            DestroyRuntimeCStringAt40CD40(&left.text);
            static_cast<void>(s.api.SetDlgItemTextA(Window(&b), control, Read<const char*>(&original.text)));
        }
        unwind.state = -1;
        DestroyRuntimeCStringAt40CD40(&original.text);
    }
    return 0;
}
BOOL ProcessPropertyPageSystem2NativeAt4306D0(PropertyPageSystem2NativeBody68h& b, HWND, UINT m,
    WPARAM p, LPARAM l, LRESULT* out, DWORD map, PropertyPageSystemNativeServices& s)
{
    if (map) return FALSE;
    if (m == 0x110) { *out = InitializePropertyPageSystem2NativeAt427A40(b, s); return TRUE; }
    if (m == 0x111)
    {
        if (LOWORD(p) == 0x44C) { *out = DefaultNamesPropertyPageSystem2NativeAt423E20(b, s); return TRUE; }
        if ((LOWORD(p) == 0x578 || LOWORD(p) == 0x579) && HIWORD(p) == 0x300)
        { *out = LimitNamePropertyPageSystem2NativeAt424BC0(b, LOWORD(p), s); return TRUE; }
    }
    if (m == 0x4E) return Notify(&b, l, out, s, [&] { return ApplyPropertyPageSystem2NativeAt422B90(b, s); });
    return FALSE;
}
}
