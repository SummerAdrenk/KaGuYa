#include <Runtime/CgSceneUiNativeGlobals.h>
#include <Runtime/AppreciationSceneResourceProfile.h>
#include <Runtime/CgOperationResourceProfile.h>
#include <cstddef>
#include <cstdlib>
#include <cstring>

namespace KaGuYa::Start::Runtime
{
namespace
{
// 仅61A564/568相邻两槽和61A660单槽；不占用、不初始化既有61A56C。
alignas(RuntimeCString) constinit std::byte cueStorage[2 * sizeof(RuntimeCString)]{};
alignas(RuntimeCString) constinit std::byte popupStorage[sizeof(RuntimeCString)]{};
// 独立持有取消cue，绝不在此重复初始化共用ModeButton的61A56C槽。
alignas(RuntimeCString) constinit std::byte cancelStorage[sizeof(RuntimeCString)]{};
RuntimeCString& Confirm() noexcept { return *reinterpret_cast<RuntimeCString*>(cueStorage); }
RuntimeCString& Focus() noexcept { return *reinterpret_cast<RuntimeCString*>(cueStorage + sizeof(RuntimeCString)); }
RuntimeCString& Popup() noexcept { return *reinterpret_cast<RuntimeCString*>(popupStorage); }
RuntimeCString& Cancel() noexcept { return *reinterpret_cast<RuntimeCString*>(cancelStorage); }

// 原PE的CP932字节，不经源文件代码页转换；长度不含末尾NUL。
constexpr char confirm[] = "sed:\\"
    "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xB9\x81\x79"
    "\x83\x4E\x83\x8A\x83\x62\x83\x4E\x89\xB9\x81\x7A" ".ogg";
constexpr char focus[] = "sed:\\"
    "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xB9\x81\x79"
    "\x91\x49\x91\xF0\x89\xB9\x81\x7A" ".ogg";
constexpr char popup[] = "CG"
    "\x8A\xD3\x8F\xDC\x92\x86\x89\xE6\x96\xCA" "_"
    "\x8D\xB7\x95\xAA\x90\x94\x92\x6C\x83\x7B\x83\x5E\x83\x93\x83\x49\x83\x93" ".vrm";
static_assert(sizeof(confirm) == 0x22 && sizeof(focus) == 0x1E && sizeof(popup) == 0x24);

void Initialize(RuntimeCString& slot, const char* text, int length, CgSceneUiNativeExit callback,
    CgSceneUiNativeExitRegistrar registrar, IRuntimeStringManagerSource& strings)
{
    InitializeEmptyRuntimeCString(slot, strings);
    // 原state0在GetNil发布后取得；Set/注册抛出时释放当前槽，Free再抛则终止。
    struct Prefix final
    {
        RuntimeCString& slot;
        bool active = true;
        ~Prefix() noexcept { if (active) DestroyRuntimeCStringAt40CD40(&slot); }
    } prefix{slot};
    SetRuntimeCStringAt437680(slot, text, length);
    static_cast<void>(registrar(callback)); // 原_atexit返回值不参与分支。
    prefix.active = false;
}

// 普通退出叶无noexcept、不清槽；每次按当前chars/header减持，不缓存初始化时的header。
void __cdecl ExitAt5ABBA0() { DestroyRuntimeCStringAt40CD40(&Confirm()); }
void __cdecl ExitAt5ABC20() { DestroyRuntimeCStringAt40CD40(&Focus()); }
void __cdecl ExitAt5AC350() { DestroyRuntimeCStringAt40CD40(&Popup()); }
void __cdecl ExitCancel() { DestroyRuntimeCStringAt40CD40(&Cancel()); }
void __cdecl ConfirmCrt() { InitializeCgSceneNativeConfirmCueAt404B90(Confirm(), ExitAt5ABBA0, &std::atexit); }
void __cdecl FocusCrt() { InitializeCgSceneNativeFocusCueAt404DD0(Focus(), ExitAt5ABC20, &std::atexit); }
void __cdecl PopupCrt() { InitializeCgSceneNativePopupButtonNameAt406740(Popup(), ExitAt5AC350, &std::atexit); }
void __cdecl CancelCrt()
{
    const auto name=CurrentAppreciationSceneProfile().cancelCue;
    Initialize(Cancel(),name.data(),static_cast<int>(name.size()),ExitCancel,&std::atexit,DefaultRuntimeStringManagerSource());
}
}

RuntimeCString& ProcessCgSceneNativeConfirmCue() noexcept { return Confirm(); }
RuntimeCString& ProcessCgSceneNativeFocusCue() noexcept { return Focus(); }
RuntimeCString& ProcessCgSceneNativePopupButtonName() noexcept { return Popup(); }
RuntimeCString& ProcessAppreciationSceneCancelCue() noexcept { return Cancel(); }
void InitializeCgSceneNativeConfirmCueAt404B90(RuntimeCString& slot, CgSceneUiNativeExit callback,
    CgSceneUiNativeExitRegistrar registrar, IRuntimeStringManagerSource& strings)
{
    const auto name=CurrentAppreciationSceneProfile().confirmCue;
    Initialize(slot,name.data(),static_cast<int>(name.size()),callback,registrar,strings);
}
void InitializeCgSceneNativeFocusCueAt404DD0(RuntimeCString& slot, CgSceneUiNativeExit callback,
    CgSceneUiNativeExitRegistrar registrar, IRuntimeStringManagerSource& strings)
{
    const auto name=CurrentAppreciationSceneProfile().focusCue;
    Initialize(slot,name.data(),static_cast<int>(name.size()),callback,registrar,strings);
}
void InitializeCgSceneNativePopupButtonNameAt406740(RuntimeCString& slot, CgSceneUiNativeExit callback,
    CgSceneUiNativeExitRegistrar registrar, IRuntimeStringManagerSource& strings)
{
    // 09–18为已证同名stamp；旧50格显式无此资源，构造/合成按native策略处理。
    const auto* name=CurrentCgOperationResourceProfile().popupCellBackground;
    Initialize(slot,name?name:"",name?static_cast<int>(std::strlen(name)):0,callback,registrar,strings);
}

using Initializer = void(__cdecl*)();
// 按原CRT表5AD664/5AD668/5AD72C插入；三项独立Capture/GetNil/Set/atexit。
#pragma section(".CRT$XCUKaGuYa5AD664", read)
#pragma section(".CRT$XCUKaGuYa5AD668", read)
#pragma section(".CRT$XCUKaGuYa5AD66A", read)
#pragma section(".CRT$XCUKaGuYa5AD72C", read)
extern "C" __declspec(allocate(".CRT$XCUKaGuYa5AD664")) Initializer const KaGuYaCgConfirmCueInitializer = ConfirmCrt;
extern "C" __declspec(allocate(".CRT$XCUKaGuYa5AD668")) Initializer const KaGuYaCgFocusCueInitializer = FocusCrt;
extern "C" __declspec(allocate(".CRT$XCUKaGuYa5AD66A")) Initializer const KaGuYaAppreciationCancelCueInitializer = CancelCrt;
extern "C" __declspec(allocate(".CRT$XCUKaGuYa5AD72C")) Initializer const KaGuYaCgPopupButtonInitializer = PopupCrt;
#pragma comment(linker, "/include:_KaGuYaCgConfirmCueInitializer")
#pragma comment(linker, "/include:_KaGuYaCgFocusCueInitializer")
#pragma comment(linker, "/include:_KaGuYaAppreciationCancelCueInitializer")
#pragma comment(linker, "/include:_KaGuYaCgPopupButtonInitializer")
}
