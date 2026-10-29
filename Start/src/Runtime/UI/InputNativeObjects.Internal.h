#pragma once
#include <Runtime/InputNativeObjects.h>
namespace KaGuYa::Start::Runtime
{
InputNativeServices& CurrentInputNativeServices();
const InputRawMessageVtable* MouseRawNativeMethods() noexcept;
const InputRawMessageVtable* KeyboardRawNativeMethods() noexcept;
const InputRawMessageVtable* CursorNativeMethods() noexcept;
bool DispatchInputNativeHook(void*,void*,unsigned int,UINT,WPARAM,LPARAM,LRESULT&);
void DestroyKeyboardManagerNativeAt4501F0(KeyboardManagerNativeBody438h&) noexcept;
}
