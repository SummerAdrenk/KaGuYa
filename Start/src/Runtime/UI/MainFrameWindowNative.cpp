#include <Runtime/MainFrameWindowNative.h>
#include <Runtime/EnvironmentRootRuntime.h>
#include <bit>
#include <cstring>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
thread_local MainFrameWindowNativeServices* current;
template<class T> T Read(const T& value) noexcept { return *static_cast<const volatile T*>(&value); }
BOOL __fastcall Process(EditWindowEndpointNativeBody24h* body, void*, HWND w, UINT m, WPARAM p, LPARAM l, LRESULT* r, DWORD map)
{ return ProcessMainFrameNativeAt42E810(*reinterpret_cast<MainFrameNativeBody90h*>(body), w, m, p, l, r, map, RequireMainFrameWindowNativeServices().dispatch); }
EditWindowEndpointNativeBody24h* __fastcall Delete(EditWindowEndpointNativeBody24h* body, void*, DWORD flags)
{
    auto& services = RequireMainFrameWindowNativeServices();
    static_cast<void>(DeleteMainFrameNativeAt411CC0(reinterpret_cast<MainFrameNativeBody90h*>(body), flags,
        services.update, services.creation.thunks, services.memory));
    // 原411CC0返回缓存的this；本体可能已释放，不能再取它的成员地址。
    return body;
}
WNDPROC __fastcall Procedure(EditWindowEndpointNativeBody24h*, void*) { return &BoundMainFrameWindowNativeAt43A080; }
void __fastcall Final(EditWindowEndpointNativeBody24h*, void*, HWND) {} // 原424E00仅ret4。
BOOL __fastcall Filter(void* body, void*, MSG* message)
{ return TranslateMainFrameNativeAt42D1A0(body, message, RequireMainFrameWindowNativeServices().callbacks); }
BOOL __fastcall Idle(void* body, void*)
{
    auto& services = RequireMainFrameWindowNativeServices();
    return IdleMainFrameNativeAt424E10(body, services.idle, services.callbacks);
}
const EditWindowEndpointNativeVtable4 window{
    reinterpret_cast<decltype(EditWindowEndpointNativeVtable4::processAt00)>(&Process),
    reinterpret_cast<decltype(EditWindowEndpointNativeVtable4::deleteAt04)>(&Delete),
    reinterpret_cast<decltype(EditWindowEndpointNativeVtable4::procedureAt08)>(&Procedure),
    reinterpret_cast<decltype(EditWindowEndpointNativeVtable4::finalMessageAt0C)>(&Final)};
struct FilterTable final { BOOL (__thiscall* method)(void*, MSG*); };
struct IdleTable final { BOOL (__thiscall* method)(void*); };
const FilterTable filter{reinterpret_cast<decltype(FilterTable::method)>(&Filter)};
const IdleTable idle{reinterpret_cast<decltype(IdleTable::method)>(&Idle)};
static_assert(sizeof(FilterTable) == 4 && sizeof(IdleTable) == 4);
}
MainFrameWindowNativeScope::MainFrameWindowNativeScope(MainFrameWindowNativeServices& services) noexcept : previous_(current) { current = &services; }
MainFrameWindowNativeScope::~MainFrameWindowNativeScope() noexcept { current = previous_; }
MainFrameWindowNativeServices& RequireMainFrameWindowNativeServices()
{ if (!current) throw std::logic_error("MainFrame requires complete native window services"); return *current; }
MainFrameNativeConstructorTables MainFrameWindowNativeConstructorTables() noexcept
{ return {window, &filter, &idle, &MainFrameMenuNativeVtable(), MainFramePopupPositionNativeVtable()}; }
void PushMainFrameWindowCreationNativeAt434592(MainFrameNativeBody90h& frame, MainFrameWindowNativeServices& services)
{
    auto& s = services.creation;
    auto* record = &frame.windowAt00.creationAt08;
    void* object = &frame;
    std::memcpy(record, &object, 4);
    const auto thread = s.api.GetCurrentThreadId();
    std::memcpy(reinterpret_cast<BYTE*>(record) + 4, &thread, 4);
    s.api.EnterCriticalSection(&s.winModule.lockAt04);
    auto* next = Read(s.winModule.creationHeadAt1C);
    std::memcpy(reinterpret_cast<BYTE*>(record) + 8, &next, 4);
    s.winModule.creationHeadAt1C = record;
    s.api.LeaveCriticalSection(&s.winModule.lockAt04);
}
LRESULT CALLBACK InitialMainFrameWindowNativeAt438340(HWND w, UINT m, WPARAM p, LPARAM l)
{
    auto& services = RequireMainFrameWindowNativeServices();
    auto& creation = services.creation;
    auto* body = static_cast<EditWindowEndpointNativeBody24h*>(PopWindowCreationNativeAt416B00(&creation.winModule, creation.api));
    if (!body) return 0;
    const auto* methods = Read(body->vptrAt00);
    body->windowAt04 = w;
    const auto procedure = methods->procedureAt08(body);
    auto* thunk = Read(body->thunkAt14);
    if (!thunk) { thunk = AllocateWindowThunkNativeAt53F0E4(creation.thunks); body->thunkAt14 = thunk; }
    if (thunk)
    {
        InitializeWindowThunkNativeAt53F1E6(thunk, reinterpret_cast<DWORD>(procedure), body, creation.thunks);
        thunk = Read(body->thunkAt14);
    }
    const auto code = WindowThunkNativeCodeAt53F149(thunk, creation.thunks);
    static_cast<void>(creation.api.SetWindowLongA(w, GWL_WNDPROC, std::bit_cast<LONG>(reinterpret_cast<DWORD>(code))));
    return code(w, m, p, l);
}
LRESULT CALLBACK BoundMainFrameWindowNativeAt43A080(HWND object, UINT m, WPARAM p, LPARAM l)
{ return DispatchMainFrameNativeMessageAt43A080(*reinterpret_cast<EditWindowEndpointNativeBody24h*>(object), m, p, l, RequireMainFrameWindowNativeServices().messages); }
}
