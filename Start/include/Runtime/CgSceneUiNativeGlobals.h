#pragma once
#include <Runtime/RuntimeCString.h>

namespace KaGuYa::Start::Runtime
{
using CgSceneUiNativeExit = void(__cdecl*)();
using CgSceneUiNativeExitRegistrar = int(__cdecl*)(CgSceneUiNativeExit);

// 61A564/61A568/61A660：借用CRT已初始化的唯一raw槽；不懒建、不补字面量。
RuntimeCString& ProcessCgSceneNativeConfirmCue() noexcept;
RuntimeCString& ProcessCgSceneNativeFocusCue() noexcept;
RuntimeCString& ProcessCgSceneNativePopupButtonName() noexcept;

// 未构造槽入口；GetNil发布后才取得EH清理资格，Set成功后才注册退出叶。
// registrar返回码按原版忽略；正常退出Free可抛，展开Free再抛则terminate。
void InitializeCgSceneNativeConfirmCueAt404B90(RuntimeCString&, CgSceneUiNativeExit,
    CgSceneUiNativeExitRegistrar, IRuntimeStringManagerSource& = DefaultRuntimeStringManagerSource());
void InitializeCgSceneNativeFocusCueAt404DD0(RuntimeCString&, CgSceneUiNativeExit,
    CgSceneUiNativeExitRegistrar, IRuntimeStringManagerSource& = DefaultRuntimeStringManagerSource());
void InitializeCgSceneNativePopupButtonNameAt406740(RuntimeCString&, CgSceneUiNativeExit,
    CgSceneUiNativeExitRegistrar, IRuntimeStringManagerSource& = DefaultRuntimeStringManagerSource());
}
