#include <Runtime/StartupSettingsDialogNative.h>
#include <Runtime/NativeStartupSettingsDialogRuntime.h>
#include <Runtime/EnvironmentRootRuntime.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
thread_local StartupSettingsDialogNativeServices* current;
template<class T> T Read(const T& value) noexcept { return *static_cast<const volatile T*>(&value); }
BOOL __fastcall Process(DialogEndpointNativeBody20h* body, void*, HWND window, UINT message,
    WPARAM w, LPARAM l, LRESULT* result, DWORD map)
{
    return ProcessStartupSettingsDialogNativeAt485A50(*reinterpret_cast<StartupSettingsDialogNativeBody28h*>(body),
        window, message, w, l, result, map, RequireStartupSettingsDialogNativeServices());
}
DialogEndpointNativeBody20h* __fastcall Delete(DialogEndpointNativeBody20h* body, void*, DWORD flags)
{
    auto& services = RequireStartupSettingsDialogNativeServices();
    ReleaseStartupSettingsDialogNativeAt489D5F(*reinterpret_cast<StartupSettingsDialogNativeBody28h*>(body), services);
    if (flags & 1u) services.endpoint.memory.DeleteBody(body, 0x28);
    return body;
}
DLGPROC __fastcall Procedure(DialogEndpointNativeBody20h*, void*) { return &BoundDialogEndpointNativeAt415810; }
void __fastcall Final(DialogEndpointNativeBody20h*, void*, HWND) {} // 原424DC0仅ret4。
// 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
// 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const DialogEndpointNativeVtable4 table{
    reinterpret_cast<decltype(DialogEndpointNativeVtable4::processAt00)>(&Process),
    reinterpret_cast<decltype(DialogEndpointNativeVtable4::deleteAt04)>(&Delete),
    reinterpret_cast<decltype(DialogEndpointNativeVtable4::procedureAt08)>(&Procedure),
    reinterpret_cast<decltype(DialogEndpointNativeVtable4::finalMessageAt0C)>(&Final)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
}
StartupSettingsDialogNativeScope::StartupSettingsDialogNativeScope(StartupSettingsDialogNativeServices& services) noexcept
    : previous_(current), endpoint_(services.endpoint) { current = &services; }
StartupSettingsDialogNativeScope::~StartupSettingsDialogNativeScope() noexcept { current = previous_; }
StartupSettingsDialogNativeServices& RequireStartupSettingsDialogNativeServices()
{
    if (!current) throw std::logic_error("startup SettingDlg requires native dialog services");
    return *current;
}
const DialogEndpointNativeVtable4& StartupSettingsDialogNativeVtable() noexcept { return table; }
void InitializeStartupSettingsDialogNativeAt489D15(StartupSettingsDialogNativeBody28h& body) noexcept
{
    auto& dialog = body.dialogAt00;
    dialog.windowAt04 = nullptr;
    dialog.thunkAt14 = nullptr;
    dialog.currentMessageAt18 = nullptr;
    dialog.destroyPendingAt1C = 0;
    dialog.vptrAt00 = &table;
    // 489D15..489D31不写创建记录、20/21/22/23及24；不能整块清零。
}
INT_PTR RunStartupSettingsDialogNativeAt489DDF(StartupSettingsDialogNativeBody28h& body,
    HINSTANCE instance, StartupSettingsDialogNativeServices& services)
{ return RunStartupSettingsDialogNativeWithResourceSlotAt489DDF(body, instance, services); }
INT_PTR RunStartupSettingsDialogNativeWithResourceSlotAt489DDF(StartupSettingsDialogNativeBody28h& body,
    const HINSTANCE& instance, StartupSettingsDialogNativeServices& services)
{
    StartupSettingsDialogNativeScope scope(services);
    const auto parent = services.api.ActiveWindow();
    auto* thunk = Read(body.dialogAt00.thunkAt14);
    if (!thunk)
    {
        thunk = AllocateWindowThunkNativeAt53F0E4(services.endpoint.thunks);
        body.dialogAt00.thunkAt14 = thunk;
    }
    if (!thunk) { ::SetLastError(ERROR_OUTOFMEMORY); return -1; }
    InitializeWindowThunkNativeAt53F1E6(thunk, 0, nullptr, services.endpoint.thunks);
    PushDialogCreationNativeAt42F8B9(body.dialogAt00, services.endpoint);
    return services.api.DialogBoxAt489DDF(Read(instance), MAKEINTRESOURCEA(111), parent,
        &InitialDialogEndpointNativeAt4382D0, 0);
}
void ReleaseStartupSettingsDialogNativeAt489D5F(StartupSettingsDialogNativeBody28h& body,
    StartupSettingsDialogNativeServices& services)
{
    // 原463EC0/489D5F只检查并释放14；不清槽、不移除未消费record、不DestroyWindow。
    ReleaseDialogEndpointThunkNativeAt40E730(body.dialogAt00, services.endpoint);
}
}
