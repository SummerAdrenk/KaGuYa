#include <Runtime/MainFrameCloseNative.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/SystemInformationDisplayNative.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
template<class T>T Read(const T& value)noexcept{return *static_cast<const volatile T*>(&value);}
void* Capture(IMainFrameNativeRootSource& roots){roots.EnsureRootAt413BE0();return roots.RootAt6198C4();}
LRESULT Previous(MainFrameNativeBody90h& frame,UINT message,WPARAM w,LPARAM l,IMainFrameNativeWindowApi& windows)
{return windows.CallWindowProcA(Read(frame.windowAt00.previousAt20),Read(frame.windowAt00.windowAt04),message,w,l);}
void* Information(IMainFrameNativeRootSource& source,void* root)
{
    const auto& ref=source.InformationAt3C(root);const auto base=Read(ref.baseAt00);if(!base)return nullptr;
    const auto control=Read(ref.controlAt08);
    const auto stride=control?Read(*reinterpret_cast<const DWORD*>(reinterpret_cast<DWORD>(control)+0x0Cu)):0u;
    return reinterpret_cast<void*>(base+Read(ref.biasAt04)*stride);
}
struct RefUnwind final
{
    YtlReferenceSlotRuntime& ref;bool active=true;
    ~RefUnwind()noexcept{if(active)ReleaseYtlReference(ref);}
    // 原state3/state5一旦释放抛出，unwind action541E55终止，不转成模态catch。
    void Finish()noexcept{active=false;ReleaseYtlReference(ref);}
};
}
void GeneralConfirmMainFrameCloseNativeDialogs::CreateReferenceAt42380D(YtlReferenceSlotRuntime& fresh)
{CreateGeneralConfirmDialogNativeReference(fresh,0);}
int GeneralConfirmMainFrameCloseNativeDialogs::RunAt5071D0(void*,const YtlReferenceSlotRuntime* ref)
{return RunNativeGuiModalReferenceAt5071D0(ref,calls_.modal);}
LRESULT CloseMainFrameNativeAt423750(MainFrameNativeBody90h& frame,UINT message,WPARAM w,LPARAM l,
    IMainFrameNativeRootSource& roots,IMainFrameNativeWindowApi& windows,IMainFrameCloseNativeDialogs& dialogs)
{
    if(!windows.IsWindowVisible(Read(frame.windowAt00.windowAt04)))return Previous(frame,message,w,l,windows);
    if(Read(roots.ControlAt1AC(Capture(roots)).valueAt1BC))return Previous(frame,message,w,l,windows);
    if(windows.IsIconic(Read(frame.windowAt00.windowAt04)))static_cast<void>(windows.OpenIcon(Read(frame.windowAt00.windowAt04)));
    if(Read(roots.ControlAt1AC(Capture(roots)).valueAt1B8))return 0;
    if(!ResolveEnvironmentArchiveAt417990(*roots.EnvironmentAt4180C0(),1))return Previous(frame,message,w,l,windows);
    auto* save=ResolveEnvironmentArchiveAt417990(*roots.EnvironmentAt4180C0(),1);
    if(!Read(*reinterpret_cast<const BYTE*>(reinterpret_cast<DWORD>(save)+0x7Cu)))return Previous(frame,message,w,l,windows);
    GeneralConfirmDialogNativeScope scope(dialogs.NativeCalls());
    union Fresh{YtlReferenceSlotRuntime ref;Fresh()noexcept{}~Fresh()noexcept{}} fresh;
    dialogs.CreateReferenceAt42380D(fresh.ref);
    RefUnwind lifetime{fresh.ref};
    try
    {
        auto* information=Information(roots,Capture(roots));
        if(dialogs.RunAt5071D0(information,&fresh.ref)!=6){lifetime.Finish();return 0;}
    }
    catch(...)
    {
        // 423930正常捕获后到423939，仅此模态范围异常按“继续关闭”处理。
    }
    lifetime.Finish();return Previous(frame,message,w,l,windows);
}
}
