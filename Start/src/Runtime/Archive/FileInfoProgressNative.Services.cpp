#include "FileInfoProgressNative.Internal.h"
#include <Runtime/FileInfoProgressRuntime.h>
#include <stdexcept>
#include <type_traits>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
thread_local FileInfoProgressNativeServices* current;
class Api final : public IFileInfoProgressNativeApi
{
public:
    void SetLastError(DWORD error) override { ::SetLastError(error); }
    HWND CreateDialogParamA(HINSTANCE i, LPCSTR r, HWND p, DLGPROC f, LPARAM l) override
    { return ::CreateDialogParamA(i, r, p, f, l); }
    BOOL DestroyWindow(HWND w) override { return ::DestroyWindow(w); }
    HWND GetDlgItem(HWND w, int id) override { return ::GetDlgItem(w, id); }
    LRESULT SendMessageA(HWND w, UINT m, WPARAM p, LPARAM l) override { return ::SendMessageA(w, m, p, l); }
    BOOL PeekMessageA(MSG* m, HWND w, UINT a, UINT b, UINT f) override { return ::PeekMessageA(m, w, a, b, f); }
    BOOL IsDialogMessageA(HWND w, MSG* m) override { return ::IsDialogMessageA(w, m); }
    BOOL TranslateMessage(const MSG* m) override { return ::TranslateMessage(m); }
    LRESULT DispatchMessageA(const MSG* m) override { return ::DispatchMessageA(m); }
};
class Confirmation final : public IFileInfoProgressConfirmation
{
public:
    int Confirm(HWND w, const char* text, const char* title, UINT flags) override
    { return ::MessageBoxA(w, text, title, flags); }
};
static_assert(std::is_trivially_destructible_v<Api>);
constinit Api processApi{};
alignas(Confirmation) constinit std::byte processConfirmationStorage[sizeof(Confirmation)]{};
}
IFileInfoProgressNativeApi& DefaultFileInfoProgressNativeApi() noexcept { return processApi; }
IFileInfoProgressConfirmation& DefaultFileInfoProgressNativeConfirmation() noexcept
{
    // 仅借用的Win32叶须活过Root及逃逸Ref；不能由虚析构的局部static附带登记另一个CRT退出。
    static auto* const confirmation = ::new(static_cast<void*>(processConfirmationStorage)) Confirmation;
    return *confirmation;
}
FileInfoProgressNativeSource::FileInfoProgressNativeSource(EnvironmentRootOwner& environment,
    WindowClassModuleNativeBody38h& nativeBase) noexcept : environment_(environment), nativeBase_(nativeBase) {}
HINSTANCE FileInfoProgressNativeSource::ResourceInstanceAt458AA8()
{ return FileInfoProgressNativeDetail::Read(nativeBase_.resourceInstanceAt08); }
EnvironmentRootStorage* FileInfoProgressNativeSource::EnvironmentAt4180C0()
{ return environment_.CaptureAt4180C0(); }
FileInfoProgressNativeScope::FileInfoProgressNativeScope(FileInfoProgressNativeServices& services) noexcept
    : previous_(current), endpoint_(services.endpoint) { current = &services; }
FileInfoProgressNativeScope::~FileInfoProgressNativeScope() noexcept { current = previous_; }
FileInfoProgressNativeServices& RequireFileInfoProgressNativeServices()
{
    if (!current) throw std::logic_error("FileInfoProgress native callbacks require borrowed services");
    return *current;
}
FileInfoProgressNativeView::FileInfoProgressNativeView(FileInfoProgressNativeBody10h& body,
    FileInfoProgressNativeServices& services) noexcept : body_(body), services_(services) {}
bool FileInfoProgressNativeView::PollCancelAt47F0B0()
{
    FileInfoProgressNativeScope scope(services_);
    return FileInfoProgressNativeDetail::Read(body_.vptrAt00)->pollAt00(&body_) != 0;
}
LRESULT FileInfoProgressNativeView::StepAt46E820()
{
    FileInfoProgressNativeScope scope(services_);
    return FileInfoProgressNativeDetail::Read(body_.vptrAt00)->stepAt04(&body_);
}
LRESULT FileInfoProgressNativeView::AddRangeAt46A5A0(std::uint32_t delta)
{
    FileInfoProgressNativeScope scope(services_);
    return FileInfoProgressNativeDetail::Read(body_.vptrAt00)->rangeAt08(&body_, delta);
}
HWND FileInfoProgressNativeView::WindowAt47ADF0() const noexcept
{ return FileInfoProgressNativeDetail::Read(body_.vptrAt00)->windowAt0C(&body_); }
}
