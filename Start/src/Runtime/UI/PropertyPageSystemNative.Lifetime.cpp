#include "PropertyPageSystemNative.Internal.h"
#include <Runtime/EnvironmentRootRuntime.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
thread_local PropertyPageSystemNativeServices* current;
using Endpoint = DialogEndpointNativeBody20h;
template<class T> T& Body(Endpoint* p) { return *reinterpret_cast<T*>(p); }
BOOL __fastcall SaveProcess(Endpoint* b, void*, HWND w, UINT m, WPARAM p, LPARAM l, LRESULT* r, DWORD map)
{ return ProcessPropertyPageSaveLoadNativeAt42FDE0(Body<PropertyPageSaveLoadNativeBody5Ch>(b), w, m, p, l, r, map, RequirePropertyPageSystemNativeServices()); }
BOOL __fastcall FirstProcess(Endpoint* b, void*, HWND w, UINT m, WPARAM p, LPARAM l, LRESULT* r, DWORD map)
{ return ProcessPropertyPageSystem1NativeAt430490(Body<PropertyPageSystem1NativeBody60h>(b), w, m, p, l, r, map, RequirePropertyPageSystemNativeServices()); }
BOOL __fastcall SecondProcess(Endpoint* b, void*, HWND w, UINT m, WPARAM p, LPARAM l, LRESULT* r, DWORD map)
{ return ProcessPropertyPageSystem2NativeAt4306D0(Body<PropertyPageSystem2NativeBody68h>(b), w, m, p, l, r, map, RequirePropertyPageSystemNativeServices()); }
Endpoint* __fastcall SaveDelete(Endpoint* b, void*, DWORD flags)
{
    auto& s = RequireDialogEndpointNativeServices();
    DestroyPropertyPageSaveLoadNativeAt40EB00(Body<PropertyPageSaveLoadNativeBody5Ch>(b), s);
    if (flags & 1u) s.memory.DeleteBody(b, 0x5C);
    return b;
}
Endpoint* __fastcall FirstDelete(Endpoint* b, void*, DWORD flags)
{
    auto& s = RequireDialogEndpointNativeServices();
    DestroyPropertyPageSystem1NativeAt40EB90(Body<PropertyPageSystem1NativeBody60h>(b), s);
    if (flags & 1u) s.memory.DeleteBody(b, 0x60);
    return b;
}
Endpoint* __fastcall SecondDelete(Endpoint* b, void*, DWORD flags)
{
    auto& s = RequireDialogEndpointNativeServices();
    DestroyPropertyPageSystem2NativeAt40EBF0(Body<PropertyPageSystem2NativeBody68h>(b), s);
    if (flags & 1u) s.memory.DeleteBody(b, 0x68);
    return b;
}
DLGPROC __fastcall Procedure(Endpoint*, void*) { return &BoundPropertyPageSystemNativeAt415710; }
void __fastcall Final(Endpoint*, void*, HWND) {} // PE424DB0确为retn4，不销毁窗口或正文。
using Table = DialogEndpointNativeVtable4;
const Table saveTable{reinterpret_cast<decltype(Table::processAt00)>(&SaveProcess),
    reinterpret_cast<decltype(Table::deleteAt04)>(&SaveDelete),
    reinterpret_cast<decltype(Table::procedureAt08)>(&Procedure), reinterpret_cast<decltype(Table::finalMessageAt0C)>(&Final)};
const Table firstTable{reinterpret_cast<decltype(Table::processAt00)>(&FirstProcess),
    reinterpret_cast<decltype(Table::deleteAt04)>(&FirstDelete),
    reinterpret_cast<decltype(Table::procedureAt08)>(&Procedure), reinterpret_cast<decltype(Table::finalMessageAt0C)>(&Final)};
const Table secondTable{reinterpret_cast<decltype(Table::processAt00)>(&SecondProcess),
    reinterpret_cast<decltype(Table::deleteAt04)>(&SecondDelete),
    reinterpret_cast<decltype(Table::procedureAt08)>(&Procedure), reinterpret_cast<decltype(Table::finalMessageAt0C)>(&Final)};
}
PropertyPageSystemNativeScope::PropertyPageSystemNativeScope(PropertyPageSystemNativeServices& s) noexcept
    : previous_(current), endpoint_(s.endpoint) { current = &s; }
PropertyPageSystemNativeScope::~PropertyPageSystemNativeScope() noexcept { current = previous_; }
PropertyPageSystemNativeServices& RequirePropertyPageSystemNativeServices()
{
    if (!current) throw std::logic_error("Win32 property system pages require explicit services");
    return *current;
}
const Table& PropertyPageSaveLoadNativeVtable() noexcept { return saveTable; }
const Table& PropertyPageSystem1NativeVtable() noexcept { return firstTable; }
const Table& PropertyPageSystem2NativeVtable() noexcept { return secondTable; }
void InitializePropertyPageSaveLoadNativeAt40BBE6(PropertyPageSaveLoadNativeBody5Ch& b, WindowClassModuleNativeBody38h& module) noexcept
{ InitializePropertyPageNativeAt40B91C(b.pageAt00, saveTable, 0x6A, &PropertyPageCreationNativeAt430F20, module); }
void InitializePropertyPageSystem1NativeAt40BC89(PropertyPageSystem1NativeBody60h& b, WindowClassModuleNativeBody38h& module, IRuntimeStringManagerSource& strings)
{
    InitializePropertyPageNativeAt40B91C(b.pageAt00, firstTable, 0x6B, &PropertyPageCreationNativeAt430F20, module);
    InitializeEmptyRuntimeCString(b.rendererTextAt58, strings);
}
void InitializePropertyPageSystem2NativeAt40BD0F(PropertyPageSystem2NativeBody68h& b, WindowClassModuleNativeBody38h& module, IRuntimeStringManagerSource& strings)
{
    InitializePropertyPageNativeAt40B91C(b.pageAt00, secondTable, 0x6C, &PropertyPageCreationNativeAt430F20, module);
    InitializeEmptyRuntimeCString(b.givenNameAt5C, strings);
    InitializeEmptyRuntimeCString(b.familyNameAt60, strings);
}
void DestroyPropertyPageSaveLoadNativeAt40EB00(PropertyPageSaveLoadNativeBody5Ch& b, DialogEndpointNativeServices& s)
{ ReleaseDialogEndpointThunkNativeAt40E730(b.pageAt00.endpointAt00, s); }
void DestroyPropertyPageSystem1NativeAt40EB90(PropertyPageSystem1NativeBody60h& b, DialogEndpointNativeServices& s)
{
    // FuncInfo5E2D98/5E2DBC无unwind态；正常Free抛出不补thunk/正文释放，不清字段。
    DestroyRuntimeCStringAt40CD40(&b.rendererTextAt58);
    ReleaseDialogEndpointThunkNativeAt40E730(b.pageAt00.endpointAt00, s);
}
void DestroyPropertyPageSystem2NativeAt40EBF0(PropertyPageSystem2NativeBody68h& b, DialogEndpointNativeServices& s)
{
    DestroyRuntimeCStringAt40CD40(&b.familyNameAt60);
    DestroyRuntimeCStringAt40CD40(&b.givenNameAt5C);
    ReleaseDialogEndpointThunkNativeAt40E730(b.pageAt00.endpointAt00, s);
}
INT_PTR CALLBACK BoundPropertyPageSystemNativeAt415710(HWND object, UINT m, WPARAM p, LPARAM l)
{
    // 415710与已复核415810的24h消息上下文、结果发布、NCDESTROY核完全同义。
    return DispatchDialogEndpointNativeAt415810(*reinterpret_cast<Endpoint*>(object), m, p, l, RequireDialogEndpointNativeServices().api);
}
}
