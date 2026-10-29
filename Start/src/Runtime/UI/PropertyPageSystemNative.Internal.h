#pragma once
#include <Runtime/PropertyPageSystemNative.h>
#include <bit>
#include <cstring>
#include <initializer_list>

namespace KaGuYa::Start::Runtime::PropertyPageSystemDetail
{
template<class T> T Read(const void* p, DWORD offset = 0) noexcept
{ return *reinterpret_cast<const volatile T*>(reinterpret_cast<DWORD>(p) + offset); }
template<class T> void Write(void* p, DWORD offset, T value) noexcept
{ *reinterpret_cast<volatile T*>(reinterpret_cast<DWORD>(p) + offset) = value; }
inline RuntimeCString& Text(void* p, DWORD offset)
{ return *reinterpret_cast<RuntimeCString*>(reinterpret_cast<DWORD>(p) + offset); }
inline HWND Window(const void* p) noexcept { return Read<HWND>(p, 4); }
enum class RefOrder { BiasFirst, StrideFirst };
void* Resolve(const void*, RefOrder, bool reloadBase = false);
void* InlineRegistry(PropertyPageSystemNativeServices&, DWORD index, RefOrder, bool beginFirst = false, bool reloadBase = false);
void* Registry(PropertyPageSystemNativeServices&, DWORD index);
void* GameSystem(PropertyPageSystemNativeServices&);
void SheetButtons(HWND page, PropertyPageSystemNativeServices&);
LRESULT ItemMessage(const void* page, int id, UINT message, WPARAM, LPARAM, PropertyPageSystemNativeServices&);
void Enable(const void* page, int id, BOOL, PropertyPageSystemNativeServices&);
void Check(const void* page, int id, BYTE, PropertyPageSystemNativeServices&);
BYTE Checked(const void* page, int id, PropertyPageSystemNativeServices&);
void DdxRadio(const void* page, int id, DWORD&, BOOL save, PropertyPageSystemNativeServices&);
BOOL DdxText(const void* page, int id, RuntimeCString&, BOOL save, PropertyPageSystemNativeServices&);
BOOL PutText(const void* page, int id, const RuntimeCString&, PropertyPageSystemNativeServices&);
// 同415330/415490的真实缓冲区修改；返回GetWindowText的结果，不拿strlen替代短读判定。
int ReadText(HWND, RuntimeCString&, PropertyPageSystemNativeServices&, int* expectedLength = nullptr);
BOOL ReadSystem2(PropertyPageSystem2NativeBody68h&, PropertyPageSystemNativeServices&);
BOOL WriteSystem2(PropertyPageSystem2NativeBody68h&, PropertyPageSystemNativeServices&);
void EnableEffects(PropertyPageSystem1NativeBody60h&, PropertyPageSystemNativeServices&);

template<class Apply> BOOL Notify(const void* page, LPARAM l, LRESULT* out,
    PropertyPageSystemNativeServices& s, Apply apply)
{
    const auto* notification = reinterpret_cast<const void*>(l);
    const auto window = Window(page);
    if (Read<HWND>(notification) != window)
    {
        const auto parent = s.api.GetParent(window);
        if (Read<HWND>(notification) != parent) { *out = 1; return FALSE; }
    }
    // PE三张14项跳表相同；-202是PSN_APPLY，-203/-205均处理但没有业务调用。
    const DWORD code = Read<DWORD>(notification, 8);
    BOOL handled = TRUE;
    LRESULT result = 0;
    switch (code)
    {
    case DWORD(-213): case DWORD(-212): case DWORD(-209): case DWORD(-208):
    case DWORD(-207): case DWORD(-206): case DWORD(-205): case DWORD(-203):
    case DWORD(-201): case DWORD(-200): break;
    case DWORD(-202): result = apply() ? 0 : 2; break;
    default: handled = FALSE; break;
    }
    *out = result;
    return handled;
}
}
