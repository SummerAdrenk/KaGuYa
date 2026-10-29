#pragma once
#include <Runtime/AtlBaseModuleNative.h>

namespace KaGuYa::Start::Runtime
{
using AtlBaseModuleNativeExit=void(__cdecl*)();
using AtlBaseModuleNativeRegistrar=int(__cdecl*)(AtlBaseModuleNativeExit);
WindowClassModuleNativeBody38h& ProcessAtlBaseModuleNative()noexcept;
BYTE& ProcessFrameworkStartupFailedNative()noexcept;
// 40752C只有构造后登记；ctor负锁结果仍登记，注册负返回或异常不追加析构/回滚。
void InitializeAtlBaseModuleProcessNativeAt40752C(WindowClassModuleNativeBody38h&,HINSTANCE,
    const GUID&,BYTE& sharedFailed,AtlBaseModuleNativeServices&,AtlBaseModuleNativeExit,AtlBaseModuleNativeRegistrar);
}
