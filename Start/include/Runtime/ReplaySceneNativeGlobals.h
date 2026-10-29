#pragma once
#include <Runtime/CgSceneNativeGlobals.h>

namespace KaGuYa::Start::Runtime
{
RuntimeCString& ProcessReplaySceneNativeBackgroundMusic() noexcept;
void InitializeReplaySceneNativeBackgroundMusicAt404C20(RuntimeCString&,CgSceneNativeExit,
    CgSceneNativeExitRegistrar,IRuntimeStringManagerSource& strings=DefaultRuntimeStringManagerSource());
}
