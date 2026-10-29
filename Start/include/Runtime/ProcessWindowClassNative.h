#pragma once
#include <Runtime/WindowClassNative.h>

namespace KaGuYa::Start::Runtime
{
using WindowClassWinModuleNativeExit=void(__cdecl*)();
using WindowClassWinModuleNativeRegistrar=int(__cdecl*)(WindowClassWinModuleNativeExit);
WindowClassWinModuleNativeBody2Ch& ProcessWindowClassWinModuleNative()noexcept;
void InitializeProcessWindowClassWinModuleNativeAt4012F0(WindowClassWinModuleNativeBody2Ch&,BYTE& sharedFailed,
    IWindowClassNativeApi&,WindowClassWinModuleNativeExit,WindowClassWinModuleNativeRegistrar);
}
