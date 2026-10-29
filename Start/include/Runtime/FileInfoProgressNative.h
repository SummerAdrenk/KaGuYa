#pragma once

#include <Runtime/DialogEndpointNative.h>
#include <Runtime/IFileInfoProgress.h>
#include <cstddef>
#include <cstdint>

namespace KaGuYa::Start::Runtime
{
class EnvironmentRootOwner;
struct EnvironmentRootStorage;
class IRuntimeStringManagerSource;
class IFileInfoProgressConfirmation;
struct FileInfoProgressNativeBody10h;
struct FileInfoProgressDialogNativeControl18h;

// Sample 18, 5B2A28: explicit x86 slots, not a C++ inheritance/layout assumption.
struct FileInfoProgressNativeVtable5 final
{
    BYTE(__thiscall* pollAt00)(FileInfoProgressNativeBody10h*);
    LRESULT(__thiscall* stepAt04)(FileInfoProgressNativeBody10h*);
    LRESULT(__thiscall* rangeAt08)(FileInfoProgressNativeBody10h*, DWORD);
    HWND(__thiscall* windowAt0C)(const FileInfoProgressNativeBody10h*);
    FileInfoProgressNativeBody10h*(__thiscall* deleteAt10)(FileInfoProgressNativeBody10h*, DWORD);
};
struct FileInfoProgressDialogNativeBody24h final
{
    DialogEndpointNativeBody20h endpointAt00;
    BYTE reservedAt20;
    BYTE cancelAt21;
    BYTE unwrittenAt22[2];
    FileInfoProgressDialogNativeBody24h() noexcept {}
};
struct FileInfoProgressDialogNativeReference0Ch final
{
    FileInfoProgressDialogNativeBody24h* baseAt00;
    DWORD biasAt04;
    FileInfoProgressDialogNativeControl18h* controlAt08;
};
struct FileInfoProgressNativeBody10h final
{
    const FileInfoProgressNativeVtable5* vptrAt00;
    FileInfoProgressDialogNativeReference0Ch dialogAt04;
    FileInfoProgressNativeBody10h() noexcept {}
};
static_assert(sizeof(void*) == 4 && sizeof(FileInfoProgressNativeVtable5) == 0x14);
static_assert(offsetof(FileInfoProgressNativeVtable5, deleteAt10) == 0x10);
static_assert(sizeof(FileInfoProgressNativeBody10h) == 0x10);
static_assert(offsetof(FileInfoProgressNativeBody10h, dialogAt04) == 4);
static_assert(sizeof(FileInfoProgressDialogNativeReference0Ch) == 0x0C);
static_assert(sizeof(FileInfoProgressDialogNativeBody24h) == 0x24);
static_assert(offsetof(FileInfoProgressDialogNativeBody24h, cancelAt21) == 0x21);

class IFileInfoProgressNativeApi
{
protected:
    ~IFileInfoProgressNativeApi() = default;
public:
    virtual void SetLastError(DWORD) = 0;
    virtual HWND CreateDialogParamA(HINSTANCE, LPCSTR, HWND, DLGPROC, LPARAM) = 0;
    virtual BOOL DestroyWindow(HWND) = 0;
    virtual HWND GetDlgItem(HWND, int) = 0;
    virtual LRESULT SendMessageA(HWND, UINT, WPARAM, LPARAM) = 0;
    virtual BOOL PeekMessageA(MSG*, HWND, UINT, UINT, UINT) = 0;
    virtual BOOL IsDialogMessageA(HWND, MSG*) = 0;
    virtual BOOL TranslateMessage(const MSG*) = 0;
    virtual LRESULT DispatchMessageA(const MSG*) = 0;
};
IFileInfoProgressNativeApi& DefaultFileInfoProgressNativeApi() noexcept;
// Real MessageBoxA provider, also usable by the existing voiceErrors consumer.
IFileInfoProgressConfirmation& DefaultFileInfoProgressNativeConfirmation() noexcept;

class IFileInfoProgressNativeSource
{
public:
    virtual ~IFileInfoProgressNativeSource() = default;
    virtual HINSTANCE ResourceInstanceAt458AA8() = 0;
    virtual EnvironmentRootStorage* EnvironmentAt4180C0() = 0;
};
// Borrows the actual owner and ATL +08 slot; neither value is cached.
class FileInfoProgressNativeSource final : public IFileInfoProgressNativeSource
{
public:
    FileInfoProgressNativeSource(EnvironmentRootOwner&, WindowClassModuleNativeBody38h&) noexcept;
    HINSTANCE ResourceInstanceAt458AA8() override;
    EnvironmentRootStorage* EnvironmentAt4180C0() override;
private:
    EnvironmentRootOwner& environment_;
    WindowClassModuleNativeBody38h& nativeBase_;
};
struct FileInfoProgressNativeServices final
{
    DialogEndpointNativeServices endpoint;
    IFileInfoProgressNativeApi& api;
    IFileInfoProgressNativeSource& source;
    IRuntimeStringManagerSource& strings;
    IFileInfoProgressConfirmation& confirmation;
};
// Keep a scope active on the owning thread for the entire modeless lifetime,
// including external message dispatch and the outer Ref's scalar-delete call.
class FileInfoProgressNativeScope final
{
public:
    explicit FileInfoProgressNativeScope(FileInfoProgressNativeServices&) noexcept;
    ~FileInfoProgressNativeScope() noexcept;
    FileInfoProgressNativeScope(const FileInfoProgressNativeScope&) = delete;
    FileInfoProgressNativeScope& operator=(const FileInfoProgressNativeScope&) = delete;
private:
    FileInfoProgressNativeServices* previous_;
    DialogEndpointNativeScope endpoint_;
};
FileInfoProgressNativeServices& RequireFileInfoProgressNativeServices();
const FileInfoProgressNativeVtable5& FileInfoProgressNativeVtable() noexcept;
const DialogEndpointNativeVtable4& FileInfoProgressDialogNativeVtable() noexcept;

// Caller owns outer 10h allocation and caller-EH deallocation (489D70/state2).
// This ctor owns only the proven inner 24h body and 18h control; no outer Ref.
FileInfoProgressNativeBody10h* ConstructFileInfoProgressNativeAt4588F0(
    FileInfoProgressNativeBody10h*, FileInfoProgressNativeServices&);
BYTE PollFileInfoProgressNativeAt47F0B0(FileInfoProgressNativeBody10h&, FileInfoProgressNativeServices&);
LRESULT StepFileInfoProgressNativeAt46E820(FileInfoProgressNativeBody10h&, FileInfoProgressNativeServices&);
LRESULT RangeFileInfoProgressNativeAt46A5A0(FileInfoProgressNativeBody10h&, DWORD, FileInfoProgressNativeServices&);
HWND WindowFileInfoProgressNativeAt47ADF0(const FileInfoProgressNativeBody10h&) noexcept;
FileInfoProgressNativeBody10h* DeleteFileInfoProgressNativeAt468910(
    FileInfoProgressNativeBody10h*, DWORD, FileInfoProgressNativeServices&) noexcept;
FileInfoProgressDialogNativeBody24h* DeleteFileInfoProgressDialogNativeAt468FF0(
    FileInfoProgressDialogNativeBody24h*, DWORD, FileInfoProgressNativeServices&);
BOOL ProcessFileInfoProgressDialogNativeAt4859E0(FileInfoProgressDialogNativeBody24h&,
    HWND, UINT, WPARAM, LPARAM, LRESULT*, DWORD, FileInfoProgressNativeServices&);
void PumpFileInfoProgressDialogNativeAt485C70(FileInfoProgressDialogNativeBody24h&, FileInfoProgressNativeServices&);

// Non-owning business bridge only. Never reinterpret raw body as IFileInfoProgress
// or delete it through that C++ interface: flags/EAX=this/base-retirement differ.
class FileInfoProgressNativeView final : public IFileInfoProgress
{
public:
    FileInfoProgressNativeView(FileInfoProgressNativeBody10h&, FileInfoProgressNativeServices&) noexcept;
    bool PollCancelAt47F0B0() override;
    LRESULT StepAt46E820() override;
    LRESULT AddRangeAt46A5A0(std::uint32_t) override;
    HWND WindowAt47ADF0() const noexcept override;
private:
    FileInfoProgressNativeBody10h& body_;
    FileInfoProgressNativeServices& services_;
};
}
