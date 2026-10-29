#pragma once
#include <Runtime/RuntimeCString.h>

namespace KaGuYa::Start::Runtime
{
using ModeButtonNativeExit=void(__cdecl*)();
using ModeButtonNativeExitRegistrar=int(__cdecl*)(ModeButtonNativeExit);
// 404830/61A56C：真实raw全局槽，getter不Capture/分配，不在失败时补默认常量。
RuntimeCString& ProcessModeButtonNativeExitCue() noexcept;
const char* ModeButtonNativeExitCueLiteralSample18() noexcept;
void InitializeModeButtonNativeExitCueAt404830(RuntimeCString&,ModeButtonNativeExit,
    ModeButtonNativeExitRegistrar,IRuntimeStringManagerSource& =DefaultRuntimeStringManagerSource());
}
