#pragma once
#include <Runtime/WindowClassNative.h>

namespace KaGuYa::Start::Runtime::WindowClassNativeDetail
{
template<class T> T Read(const T& value) noexcept
{
    return *static_cast<const volatile T*>(&value);
}
IWindowClassNativeApi& CurrentApi() noexcept;
// 原[ebp-6C..ebp-1]的相对布局；不承诺重编译后仍使用原EBP或相同机器指令。
struct RegistrationFrame6Ch final
{
    WNDCLASSEXA existingAt00;
    WNDCLASSEXA superclassAt30;
    char aliasMarkerAt60;
    BYTE unusedAt61[3];
    WNDPROC savedProcedureAt64;
    LPCRITICAL_SECTION savedLockAt68;
};
static_assert(sizeof(RegistrationFrame6Ch) == 0x6C);
static_assert(offsetof(RegistrationFrame6Ch, superclassAt30) == 0x30);
static_assert(offsetof(RegistrationFrame6Ch, aliasMarkerAt60) == 0x60);
static_assert(offsetof(RegistrationFrame6Ch, savedProcedureAt64) == 0x64);
static_assert(offsetof(RegistrationFrame6Ch, savedLockAt68) == 0x68);
// marker是原局部var_C的地址，不是数组元素或atom值；显式参数便于静态分支直测。
bool AppendAtomAt451EF3(WindowClassWinModuleNativeBody2Ch&, ATOM, const void* marker, IWindowClassNativeApi&);
}
