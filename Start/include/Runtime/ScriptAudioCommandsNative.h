#pragma once
#include <Runtime/ScriptFollowCommandNative.h>
#include <Runtime/ScrVmMusicHandler.h>
#include <Runtime/ScrVmSoundHandler.h>
#include <Runtime/ScrVmVoiceHandler.h>

namespace KaGuYa::Start::Runtime
{
// 原命令Ref/CString/EH边界；具体声音对象仍由显式运行时接口提供。
std::int32_t ExecuteScriptMusicAt4CF9E0(ScrScriptRuntimeState& runtime,
    IScriptFileCommandSource& pc, ScrMusicInstructionHandler& music, IEnvironmentRootMemory& memory);
std::int32_t ExecuteScriptSoundAt4D1520(ScrScriptRuntimeState& runtime,
    IScriptFileCommandSource& pc, ScrSoundInstructionHandler& sound, IScrVmCommandErrorSink& errors,
    IEnvironmentRootMemory& memory, IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
std::int32_t ExecuteScriptVoiceAt4D2FF0(ScrScriptRuntimeState& runtime,
    IScriptFileCommandSource& pc, const IScrVmVoiceNameSource& names, IScriptVoiceSource& source,
    IScrVmCommandErrorSink& errors, IEnvironmentRootMemory& memory,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
}
