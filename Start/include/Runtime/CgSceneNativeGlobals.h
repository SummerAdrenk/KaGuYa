#pragma once
#include <Runtime/RuntimeCString.h>

namespace KaGuYa::Start::Runtime
{
using CgSceneNativeExit=void(__cdecl*)();
using CgSceneNativeExitRegistrar=int(__cdecl*)(CgSceneNativeExit);
// 4048C0/61A5B0：getter只借用真实raw槽；不在读取失败时补字面量默认值。
RuntimeCString& ProcessCgSceneNativeBackgroundMusic() noexcept;
const char* CgSceneNativeBackgroundMusicLiteralSample18() noexcept;
void InitializeCgSceneNativeBackgroundMusicAt4048C0(RuntimeCString&,CgSceneNativeExit,
    CgSceneNativeExitRegistrar,IRuntimeStringManagerSource& =DefaultRuntimeStringManagerSource());
}
