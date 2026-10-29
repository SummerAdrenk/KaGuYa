#include <Runtime/SaveLoadConfirmDialogNativeGlobals.h>
#include <Runtime/RootSaveLoadConfirmDialogNativeSource.h>
#include <cstddef>
#include <cstdlib>

namespace KaGuYa::Start::Runtime
{
namespace
{
// 原PE两个相邻槽均位于.data虚拟零填充尾部，不能采用导出memory里的FF。
alignas(RuntimeCString) constinit std::byte storage[2 * sizeof(RuntimeCString)]{};
RuntimeCString& Mask() noexcept { return *reinterpret_cast<RuntimeCString*>(storage); }
RuntimeCString& Font() noexcept
{ return *reinterpret_cast<RuntimeCString*>(storage + sizeof(RuntimeCString)); }

struct RegistrationUnwind final
{
    RuntimeCString& slot;
    bool active = true;
    ~RegistrationUnwind() noexcept { if (active) DestroyRuntimeCStringAt40CD40(&slot); }
};

// 正常退出读当前槽，不清槽；保留原释放叶的异常资格。
void __cdecl ExitAt5AC410() { DestroyRuntimeCStringAt40CD40(&Mask()); }
void __cdecl ExitAt5AC430() { DestroyRuntimeCStringAt40CD40(&Font()); }
void __cdecl InitializeMask()
{ InitializeSaveLoadConfirmMaskNativeAt406AA0(Mask(), ExitAt5AC410, &std::atexit); }
void __cdecl InitializeFont()
{ InitializeSaveLoadConfirmFontNativeAt406B30(Font(), ExitAt5AC430, &std::atexit); }
}

RuntimeCString& ProcessSaveLoadConfirmMaskNative() noexcept { return Mask(); }
RuntimeCString& ProcessSaveLoadConfirmFontNative() noexcept { return Font(); }

void InitializeSaveLoadConfirmMaskNativeAt406AA0(RuntimeCString& slotAt61A608,
    SaveLoadConfirmNativeExit callbackAt5AC410,
    SaveLoadConfirmNativeExitRegistrar registrarAt540012,
    IRuntimeStringManagerSource& stringsAt417360)
{
    ConstructSaveLoadConfirmMaskNameNativeAt406AA0(slotAt61A608, stringsAt417360);
    // 406ADF的state0覆盖406B0E；构造失败已由helper展开，不能再取得第二份清理资格。
    RegistrationUnwind unwind{slotAt61A608};
    static_cast<void>(registrarAt540012(callbackAt5AC410));
    unwind.active = false;
}

void InitializeSaveLoadConfirmFontNativeAt406B30(RuntimeCString& slotAt61A60C,
    SaveLoadConfirmNativeExit callbackAt5AC430,
    SaveLoadConfirmNativeExitRegistrar registrarAt540012,
    IRuntimeStringManagerSource& stringsAt417360)
{
    ConstructSaveLoadConfirmFontFaceNativeAt406B30(slotAt61A60C, stringsAt417360);
    // 406B6F的state0同样覆盖406B9E，原代码不检查注册返回值。
    RegistrationUnwind unwind{slotAt61A60C};
    static_cast<void>(registrarAt540012(callbackAt5AC430));
    unwind.active = false;
}

using Initializer = void(__cdecl*)();
// 原CRT两个相邻独立初始化项：先mask，后font；raw槽不参加C++自动静态析构。
#pragma section(".CRT$XCUKaGuYa5AD710", read)
#pragma section(".CRT$XCUKaGuYa5AD714", read)
extern "C" __declspec(allocate(".CRT$XCUKaGuYa5AD710")) Initializer const KaGuYaSaveLoadConfirmMaskInitializer = InitializeMask;
extern "C" __declspec(allocate(".CRT$XCUKaGuYa5AD714")) Initializer const KaGuYaSaveLoadConfirmFontInitializer = InitializeFont;
#pragma comment(linker, "/include:_KaGuYaSaveLoadConfirmMaskInitializer")
#pragma comment(linker, "/include:_KaGuYaSaveLoadConfirmFontInitializer")
}
