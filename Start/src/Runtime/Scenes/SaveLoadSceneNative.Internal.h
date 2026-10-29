#pragma once
#include <Runtime/SaveLoadSceneNative.h>

namespace KaGuYa::Start::Runtime::SaveLoadSceneNativeDetail
{
using Body = SaveLoadSceneNativeBody24h;
using Table = SaveLoadSceneNativeVtable4;
template<class T> T Read(const T& value) noexcept
{ return *static_cast<const volatile T*>(&value); }
void RetireTask(Body&) noexcept;
void AdoptDialogAt4A1850(YtlReferenceSlotRuntime&, GuiDialogNativeBody5Ch*, IEnvironmentRootMemory&);
}
