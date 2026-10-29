#include "FileInfoProgressNative.Internal.h"
#include <Runtime/FileInfoProgressRuntime.h>
#include <Runtime/RuntimeCString.h>
#include <CommCtrl.h>
#include <bit>

namespace KaGuYa::Start::Runtime
{
using namespace FileInfoProgressNativeDetail;
static_assert(sizeof(PBRANGE) == 8 && offsetof(PBRANGE, iHigh) == 4);
namespace
{
BOOL __fastcall Process(DialogEndpointNativeBody20h* p, void*, HWND w, UINT m, WPARAM a,
    LPARAM l, LRESULT* out, DWORD map)
{
    return ProcessFileInfoProgressDialogNativeAt4859E0(*reinterpret_cast<FileInfoProgressDialogNativeBody24h*>(p),
        w, m, a, l, out, map, RequireFileInfoProgressNativeServices());
}
DialogEndpointNativeBody20h* __fastcall DeleteDialog(DialogEndpointNativeBody20h* p, void*, DWORD flags)
{
    DeleteFileInfoProgressDialogNativeAt468FF0(reinterpret_cast<FileInfoProgressDialogNativeBody24h*>(p),
        flags, RequireFileInfoProgressNativeServices());
    return p;
}
DLGPROC __fastcall Procedure(DialogEndpointNativeBody20h*, void*) { return &BoundDialogEndpointNativeAt415810; }
__declspec(naked) void __fastcall Final(DialogEndpointNativeBody20h*, void*, HWND) { __asm { ret 4 } } // 424DC0
const DialogEndpointNativeVtable4 dialogTable{
    reinterpret_cast<decltype(DialogEndpointNativeVtable4::processAt00)>(&Process),
    reinterpret_cast<decltype(DialogEndpointNativeVtable4::deleteAt04)>(&DeleteDialog),
    reinterpret_cast<decltype(DialogEndpointNativeVtable4::procedureAt08)>(&Procedure),
    reinterpret_cast<decltype(DialogEndpointNativeVtable4::finalMessageAt0C)>(&Final)};
constexpr char CancelText[] =
    "\x83\x51\x81\x5B\x83\x80\x82\xF0\x8F\x49\x97\xB9"
    "\x82\xB5\x82\xDC\x82\xB7\x81\x42\x82\xE6\x82\xEB"
    "\x82\xB5\x82\xA2\x82\xC5\x82\xB7\x82\xA9\x81\x48";
static_assert(sizeof(CancelText) == 0x25);
union TextStorage
{
    TextStorage() noexcept {}
    ~TextStorage() noexcept {}
    RuntimeCString value;
};
struct TextCleanup final
{
    RuntimeCString& text;
    bool active = true;
    ~TextCleanup() noexcept { if (active) DestroyRuntimeCStringAt40CD40(&text); }
};
}
const DialogEndpointNativeVtable4& FileInfoProgressDialogNativeVtable() noexcept { return dialogTable; }
BOOL ProcessFileInfoProgressDialogNativeAt4859E0(FileInfoProgressDialogNativeBody24h& body,
    HWND, UINT message, WPARAM w, LPARAM, LRESULT* result, DWORD map, FileInfoProgressNativeServices& s)
{
    if (map) return FALSE;
    if (message == WM_INITDIALOG)
    {
        const auto parent = s.endpoint.api.GetParent(Read(body.endpointAt00.windowAt04));
        static_cast<void>(CenterDialogWindowNativeAt413990(&body.endpointAt00.windowAt04, parent, s.endpoint.api));
        *result = 1;
        return TRUE;
    }
    if (message == WM_COMMAND && LOWORD(w) == 2)
    {
        body.cancelAt21 = 1;
        *result = 0;
        return TRUE;
    }
    return FALSE;
}
void PumpFileInfoProgressDialogNativeAt485C70(FileInfoProgressDialogNativeBody24h& body, FileInfoProgressNativeServices& s)
{
    FileInfoProgressNativeScope scope(s);
    MSG message;
    while (s.api.PeekMessageA(&message, nullptr, 0, 0, 1))
    {
        if (!s.api.IsDialogMessageA(Read(body.endpointAt00.windowAt04), &message))
        {
            static_cast<void>(s.api.TranslateMessage(&message));
            static_cast<void>(s.api.DispatchMessageA(&message));
        }
    }
}
BYTE PollFileInfoProgressNativeAt47F0B0(FileInfoProgressNativeBody10h& body, FileInfoProgressNativeServices& s)
{
    FileInfoProgressNativeScope scope(s);
    auto* selected = Resolve(body.dialogAt04);
    PumpFileInfoProgressDialogNativeAt485C70(*selected, s);
    const auto requested = Read(selected->cancelAt21);
    selected->cancelAt21 = 0;
    if (!requested) return 0;
    TextStorage text;
    InitializeEmptyRuntimeCString(text.value, s.strings); // No cleanup qualification until GetNil returns.
    TextCleanup cleanup{text.value};
    SetRuntimeCStringAt437680(text.value, CancelText, 0x24);
    auto* environment = s.source.EnvironmentAt4180C0(); // 47F15F, after text construction.
    auto* base = Read(body.dialogAt04.baseAt00); // 47F164 precedes caption capture.
    const auto* caption = environment->titleAt10.GetString();
    FileInfoProgressDialogNativeBody24h* currentDialog = nullptr;
    if (base)
    {
        auto* control = Read(body.dialogAt04.controlAt08);
        const auto stride = control ? Read(control->strideAt0C) : 0u;
        currentDialog = reinterpret_cast<FileInfoProgressDialogNativeBody24h*>(
            reinterpret_cast<DWORD>(base) + Read(body.dialogAt04.biasAt04) * stride);
    }
    const auto* chars = text.value.GetString(); // Cached before MessageBox; normal release uses this header.
    const auto owner = Read(currentDialog->endpointAt00.windowAt04); // Not Environment +2C.
    const int answer = s.confirmation.Confirm(owner, chars, caption, 0x21);
    cleanup.active = false; // 47F1A2: Free failure must not re-release the string.
    ReleaseRuntimeCStringHeader(reinterpret_cast<ATL::CStringData*>(const_cast<char*>(chars) - 0x10));
    return answer == 1 ? BYTE{1} : BYTE{0};
}
LRESULT StepFileInfoProgressNativeAt46E820(FileInfoProgressNativeBody10h& body, FileInfoProgressNativeServices& s)
{
    FileInfoProgressNativeScope scope(s);
    auto* selected = Resolve(body.dialogAt04); // Kept across pump; do not re-resolve outer Ref afterwards.
    PumpFileInfoProgressDialogNativeAt485C70(*selected, s);
    const auto item = s.api.GetDlgItem(Read(selected->endpointAt00.windowAt04), 0x3E8);
    return s.api.SendMessageA(item, 0x405, 0, 0);
}
LRESULT RangeFileInfoProgressNativeAt46A5A0(FileInfoProgressNativeBody10h& body, DWORD delta, FileInfoProgressNativeServices& s)
{
    FileInfoProgressNativeScope scope(s);
    PBRANGE range; // 46A5A3 reserves 8 bytes without writing them. Never value-initialize.
    auto item = s.api.GetDlgItem(Read(Resolve(body.dialogAt04)->endpointAt00.windowAt04), 0x3E8);
    static_cast<void>(s.api.SendMessageA(item, 0x407, 1, reinterpret_cast<LPARAM>(&range)));
    item = s.api.GetDlgItem(Read(Resolve(body.dialogAt04)->endpointAt00.windowAt04), 0x3E8);
    // Both reads occur after the second GetDlgItem, as at 46A619..46A622.
    const auto high = static_cast<WORD>(static_cast<DWORD>(Read(range.iHigh)) + delta);
    const auto packed = static_cast<DWORD>(static_cast<WORD>(Read(range.iLow))) | (static_cast<DWORD>(high) << 16u);
    return s.api.SendMessageA(item, 0x401, 0, std::bit_cast<LONG>(packed));
}
HWND WindowFileInfoProgressNativeAt47ADF0(const FileInfoProgressNativeBody10h& body) noexcept
{ return Read(Resolve(body.dialogAt04)->endpointAt00.windowAt04); }
FileInfoProgressDialogNativeBody24h* DeleteFileInfoProgressDialogNativeAt468FF0(
    FileInfoProgressDialogNativeBody24h* body, DWORD flags, FileInfoProgressNativeServices& s)
{
    FileInfoProgressNativeScope scope(s);
    auto& e = body->endpointAt00;
    const auto window = Read(e.windowAt04); // 468FF6 reads HWND before resetting vptr.
    e.vptrAt00 = &dialogTable;
    if (window) static_cast<void>(s.api.DestroyWindow(window));
    ReleaseDialogEndpointThunkNativeAt40E730(e, s.endpoint); // Synchronous destroy messages run before this read.
    if (flags & 1u) s.endpoint.memory.DeleteBody(body, 0x24);
    return body;
}
}
