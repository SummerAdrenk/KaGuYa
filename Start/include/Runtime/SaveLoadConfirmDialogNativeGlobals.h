#pragma once

#include <Runtime/RuntimeCString.h>

namespace KaGuYa::Start::Runtime
{
using SaveLoadConfirmNativeExit = void(__cdecl*)();
using SaveLoadConfirmNativeExitRegistrar = int(__cdecl*)(SaveLoadConfirmNativeExit);

// 原61A608/61A60C可变CRT槽；getter只借用，不执行懒初始化。
RuntimeCString& ProcessSaveLoadConfirmMaskNative() noexcept;
RuntimeCString& ProcessSaveLoadConfirmFontNative() noexcept;

// 只接受fresh存储及有效回调；原构造helper先独占展开资格。
// 构造成功后registrar抛出仅释放当前槽；正常负返回也保留已构造值，不重试。
void InitializeSaveLoadConfirmMaskNativeAt406AA0(RuntimeCString& slotAt61A608,
    SaveLoadConfirmNativeExit callbackAt5AC410,
    SaveLoadConfirmNativeExitRegistrar registrarAt540012,
    IRuntimeStringManagerSource& stringsAt417360 = DefaultRuntimeStringManagerSource());
void InitializeSaveLoadConfirmFontNativeAt406B30(RuntimeCString& slotAt61A60C,
    SaveLoadConfirmNativeExit callbackAt5AC430,
    SaveLoadConfirmNativeExitRegistrar registrarAt540012,
    IRuntimeStringManagerSource& stringsAt417360 = DefaultRuntimeStringManagerSource());
}
