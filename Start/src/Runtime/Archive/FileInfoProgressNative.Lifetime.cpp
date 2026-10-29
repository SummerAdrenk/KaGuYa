#include "FileInfoProgressNative.Internal.h"
#include <cstdlib>
#include <new>

namespace KaGuYa::Start::Runtime
{
using namespace FileInfoProgressNativeDetail;
namespace
{
BYTE __fastcall Poll(FileInfoProgressNativeBody10h* p, void*)
{ return PollFileInfoProgressNativeAt47F0B0(*p, RequireFileInfoProgressNativeServices()); }
LRESULT __fastcall Step(FileInfoProgressNativeBody10h* p, void*)
{ return StepFileInfoProgressNativeAt46E820(*p, RequireFileInfoProgressNativeServices()); }
LRESULT __fastcall Range(FileInfoProgressNativeBody10h* p, void*, DWORD n)
{ return RangeFileInfoProgressNativeAt46A5A0(*p, n, RequireFileInfoProgressNativeServices()); }
HWND __fastcall Window(const FileInfoProgressNativeBody10h* p, void*)
{ return WindowFileInfoProgressNativeAt47ADF0(*p); }
FileInfoProgressNativeBody10h* __fastcall Delete(FileInfoProgressNativeBody10h* p, void*, DWORD flags) noexcept
{ return DeleteFileInfoProgressNativeAt468910(p, flags, RequireFileInfoProgressNativeServices()); }
FileInfoProgressNativeBody10h* __fastcall DeleteBase(FileInfoProgressNativeBody10h* p, void*, DWORD flags)
{
    p->vptrAt00 = &BaseVtable();
    if (flags & 1u) RequireFileInfoProgressNativeServices().endpoint.memory.DeleteBody(p, 4); // 469930
    return p;
}
const FileInfoProgressNativeVtable5 table{
    reinterpret_cast<decltype(FileInfoProgressNativeVtable5::pollAt00)>(&Poll),
    reinterpret_cast<decltype(FileInfoProgressNativeVtable5::stepAt04)>(&Step),
    reinterpret_cast<decltype(FileInfoProgressNativeVtable5::rangeAt08)>(&Range),
    reinterpret_cast<decltype(FileInfoProgressNativeVtable5::windowAt0C)>(&Window),
    reinterpret_cast<decltype(FileInfoProgressNativeVtable5::deleteAt10)>(&Delete)};
const FileInfoProgressNativeVtable5 baseTable{
    reinterpret_cast<decltype(FileInfoProgressNativeVtable5::pollAt00)>(&_purecall),
    reinterpret_cast<decltype(FileInfoProgressNativeVtable5::stepAt04)>(&_purecall),
    reinterpret_cast<decltype(FileInfoProgressNativeVtable5::rangeAt08)>(&_purecall),
    reinterpret_cast<decltype(FileInfoProgressNativeVtable5::windowAt0C)>(&_purecall),
    reinterpret_cast<decltype(FileInfoProgressNativeVtable5::deleteAt10)>(&DeleteBase)};
void __fastcall DestroyDialog(FileInfoProgressDialogNativeControl18h* p, void*)
{
    if (auto* body = Read(p->payloadAt14))
        Read(body->endpointAt00.vptrAt00)->deleteAt04(&body->endpointAt00, 1); // 492CD0
}
FileInfoProgressDialogNativeControl18h* __fastcall DeleteRefBase(
    FileInfoProgressDialogNativeControl18h*, void*, DWORD);
const FileInfoProgressDialogNativeControlVtable2 retiredRefTable{
    reinterpret_cast<decltype(FileInfoProgressDialogNativeControlVtable2::destroyAt00)>(&_purecall),
    reinterpret_cast<decltype(FileInfoProgressDialogNativeControlVtable2::deleteAt04)>(&DeleteRefBase)};
FileInfoProgressDialogNativeControl18h* __fastcall DeleteRefBase(
    FileInfoProgressDialogNativeControl18h* p, void*, DWORD flags)
{
    p->vptrAt00 = &retiredRefTable;
    if (flags & 1u) RequireFileInfoProgressNativeServices().endpoint.memory.DeleteBody(p, 0x14); // 412460
    return p;
}
FileInfoProgressDialogNativeControl18h* __fastcall DeleteControl(
    FileInfoProgressDialogNativeControl18h* p, void*, DWORD flags)
{
    p->vptrAt00 = &retiredRefTable;
    if (flags & 1u) RequireFileInfoProgressNativeServices().endpoint.memory.DeleteBody(p, 0x18); // 467BC0
    return p;
}
const FileInfoProgressDialogNativeControlVtable2 controlTable{
    reinterpret_cast<decltype(FileInfoProgressDialogNativeControlVtable2::destroyAt00)>(&DestroyDialog),
    reinterpret_cast<decltype(FileInfoProgressDialogNativeControlVtable2::deleteAt04)>(&DeleteControl)};
struct ConstructionCleanup final
{
    FileInfoProgressNativeBody10h& body;
    bool active = true;
    ~ConstructionCleanup() noexcept
    {
        if (!active) return;
        Release(body.dialogAt04); // 5963A8 -> 460180, then 5963A0 -> 4649A0
        body.vptrAt00 = &BaseVtable();
    }
};
}
const FileInfoProgressNativeVtable5& FileInfoProgressNativeVtable() noexcept { return table; }
namespace FileInfoProgressNativeDetail
{
const FileInfoProgressNativeVtable5& BaseVtable() noexcept { return baseTable; }
const FileInfoProgressDialogNativeControlVtable2& DialogControlVtable() noexcept { return controlTable; }
FileInfoProgressDialogNativeBody24h* Resolve(const FileInfoProgressDialogNativeReference0Ch& ref) noexcept
{
    auto* base = Read(ref.baseAt00);
    if (!base) return nullptr;
    auto* control = Read(ref.controlAt08);
    const auto stride = control ? Read(control->strideAt0C) : 0u;
    return reinterpret_cast<FileInfoProgressDialogNativeBody24h*>(reinterpret_cast<DWORD>(base) + Read(ref.biasAt04) * stride);
}
void Release(FileInfoProgressDialogNativeReference0Ch& ref)
{
    auto* control = Read(ref.controlAt08);
    if (!control) return; // No control: do not clear partially published base/bias.
    const auto remaining = Read(control->referencesAt04) - 1u;
    control->referencesAt04 = remaining;
    if (!remaining)
    {
        if (Read(control->owningAt08)) Read(control->vptrAt00)->destroyAt00(control);
        control = Read(ref.controlAt08); // Callback may replace or clear the control.
        if (control) Read(control->vptrAt00)->deleteAt04(control, 1);
    }
    ref.controlAt08 = nullptr;
    ref.baseAt00 = nullptr; // Original bias is never cleared here.
}
}
FileInfoProgressNativeBody10h* ConstructFileInfoProgressNativeAt4588F0(
    FileInfoProgressNativeBody10h* body, FileInfoProgressNativeServices& s)
{
    FileInfoProgressNativeScope scope(s);
    ::new (body) FileInfoProgressNativeBody10h;
    body->vptrAt00 = &table;
    body->dialogAt04.baseAt00 = nullptr;
    body->dialogAt04.biasAt04 = 0;
    body->dialogAt04.controlAt08 = nullptr;
    ConstructionCleanup cleanup{*body};
    auto* dialog = static_cast<FileInfoProgressDialogNativeBody24h*>(s.endpoint.memory.AllocateBody(0x24));
    if (dialog)
    {
        ::new (dialog) FileInfoProgressDialogNativeBody24h;
        auto& e = dialog->endpointAt00;
        e.windowAt04 = nullptr;
        e.thunkAt14 = nullptr;
        e.currentMessageAt18 = nullptr;
        e.destroyPendingAt1C = 0;
        e.vptrAt00 = &FileInfoProgressDialogNativeVtable();
        constexpr WORD zero = 0;
        std::memcpy(&dialog->reservedAt20, &zero, sizeof(zero)); // 458978仅写+20/+21。
    }
    Release(body->dialogAt04);
    if (dialog)
    {
        if (auto* old = Read(body->dialogAt04.controlAt08)) Read(old->vptrAt00)->deleteAt04(old, 1);
        body->dialogAt04.biasAt04 = 0;
        body->dialogAt04.baseAt00 = dialog; // Before allocation; failure leaks this unowned body in the original.
        auto* control = static_cast<FileInfoProgressDialogNativeControl18h*>(s.endpoint.memory.AllocateBody(0x18));
        if (control)
        {
            ::new (control) FileInfoProgressDialogNativeControl18h;
            control->vptrAt00 = &controlTable;
            control->payloadAt14 = dialog;
        }
        body->dialogAt04.controlAt08 = control;
        control->referencesAt04 = 1; // Null allocation is an original fault, not a success/fallback path.
        Read(body->dialogAt04.controlAt08)->owningAt08 = 1;
        Read(body->dialogAt04.controlAt08)->strideAt0C = 0x24;
        Read(body->dialogAt04.controlAt08)->countAt10 = 1;
    }
    auto* selected = Resolve(body->dialogAt04);
    auto& e = selected->endpointAt00; // Original null-dialog allocation faults at +14.
    auto* thunk = Read(e.thunkAt14);
    if (!thunk)
    {
        thunk = AllocateWindowThunkNativeAt53F0E4(s.endpoint.thunks);
        e.thunkAt14 = thunk;
    }
    if (!thunk) s.api.SetLastError(0x0E);
    else
    {
        InitializeWindowThunkNativeAt53F1E6(thunk, 0, nullptr, s.endpoint.thunks);
        if (reinterpret_cast<DWORD>(selected) + 8u == 0 || !selected)
        {
            s.endpoint.thunks.api.RaiseException(0xC0000005u, 1, 0, nullptr); // 458B45 -> 43A2A0
            __assume(0);
        }
        PushDialogCreationNativeAt42F8B9(e, s.endpoint);
        const auto instance = s.source.ResourceInstanceAt458AA8();
        static_cast<void>(s.api.CreateDialogParamA(instance, MAKEINTRESOURCEA(0x65), nullptr,
            &InitialDialogEndpointNativeAt4382D0, 0)); // Return HWND is deliberately ignored.
    }
    auto window = Read(Resolve(body->dialogAt04)->endpointAt00.windowAt04);
    auto item = s.api.GetDlgItem(window, 0x3E8);
    static_cast<void>(s.api.SendMessageA(item, 0x401, 0, 0x10000));
    window = Read(Resolve(body->dialogAt04)->endpointAt00.windowAt04);
    item = s.api.GetDlgItem(window, 0x3E8);
    static_cast<void>(s.api.SendMessageA(item, 0x404, 1, 0));
    cleanup.active = false;
    return body;
}
FileInfoProgressNativeBody10h* DeleteFileInfoProgressNativeAt468910(
    FileInfoProgressNativeBody10h* body, DWORD flags, FileInfoProgressNativeServices& s) noexcept
{
    FileInfoProgressNativeScope scope(s);
    body->vptrAt00 = &table;
    Release(body->dialogAt04); // 5E900C -> 541E55: exceptions terminate, no suffix cleanup.
    body->vptrAt00 = &baseTable;
    if (flags & 1u) s.endpoint.memory.DeleteBody(body, 0x10);
    return body;
}
}
