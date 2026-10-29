#pragma once

#include <Runtime/RuntimeCString.h>

namespace KaGuYa::Start::Runtime
{
using SceneCueNativeExit = void(__cdecl*)();
using SceneCueNativeExitRegistrar = int(__cdecl*)(SceneCueNativeExit);

// 原619824/61A654/61A688/68C/690的可变Process槽；只借用，不懒建、不Clone。
RuntimeCString& ProcessSceneFontFaceNative() noexcept;
RuntimeCString& ProcessTitleBgmNative() noexcept;
RuntimeCString& ProcessGraffitiLikeCueNative() noexcept;
RuntimeCString& ProcessGraffitiWeightCueNative() noexcept;
RuntimeCString& ProcessGraffitiTrainingCueNative() noexcept;

// 与ProcessSceneFontFaceNative同一个4-byte槽，不是GetString()的临时返回值。
// 可直接借给TextWindowNativeTextServices的const char* const&，后续写槽仍可见。
const char*& ProcessSceneFontFaceNativeCharsSlot() noexcept;

// 仅接受fresh存储及有效回调。GetNil发布后取得唯一state0，覆盖Set和registrar。
// registrar的任何普通返回均不回滚/重试；抛出只释放当次槽，展开Free再抛则终止。
void InitializeSceneFontFaceNativeAt401180(RuntimeCString& slotAt619824,
    SceneCueNativeExit callbackAt5AA250, SceneCueNativeExitRegistrar registrarAt540012,
    IRuntimeStringManagerSource& stringsAt417360 = DefaultRuntimeStringManagerSource());
void InitializeTitleBgmNativeAt4068F0(RuntimeCString& slotAt61A654,
    SceneCueNativeExit callbackAt5AC3B0, SceneCueNativeExitRegistrar registrarAt540012,
    IRuntimeStringManagerSource& stringsAt417360 = DefaultRuntimeStringManagerSource());
void InitializeGraffitiLikeCueNativeAt406590(RuntimeCString& slotAt61A688,
    SceneCueNativeExit callbackAt5AC2F0, SceneCueNativeExitRegistrar registrarAt540012,
    IRuntimeStringManagerSource& stringsAt417360 = DefaultRuntimeStringManagerSource());
void InitializeGraffitiWeightCueNativeAt406A10(RuntimeCString& slotAt61A68C,
    SceneCueNativeExit callbackAt5AC3F0, SceneCueNativeExitRegistrar registrarAt540012,
    IRuntimeStringManagerSource& stringsAt417360 = DefaultRuntimeStringManagerSource());
void InitializeGraffitiTrainingCueNativeAt406500(RuntimeCString& slotAt61A690,
    SceneCueNativeExit callbackAt5AC2D0, SceneCueNativeExitRegistrar registrarAt540012,
    IRuntimeStringManagerSource& stringsAt417360 = DefaultRuntimeStringManagerSource());

// 原退出叶：读当前chars、原子减持、必要时raw manager+04；不清槽、不吞异常。
// 叶函数显式收槽，便于隔离验证；Process回调固定传各自真实槽，无第二套登记。
void DestroySceneFontFaceNativeAt5AA250(RuntimeCString& slotAt619824);
void DestroyTitleBgmNativeAt5AC3B0(RuntimeCString& slotAt61A654);
void DestroyGraffitiLikeCueNativeAt5AC2F0(RuntimeCString& slotAt61A688);
void DestroyGraffitiWeightCueNativeAt5AC3F0(RuntimeCString& slotAt61A68C);
void DestroyGraffitiTrainingCueNativeAt5AC2D0(RuntimeCString& slotAt61A690);
}
