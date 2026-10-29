#pragma once

#include <Runtime/ScriptAudioCommandsNative.h>
#include <Runtime/ScriptSoundLeafSource.h>

namespace KaGuYa::Start::Runtime
{
class IScrVmTextRuntimeBoundary;
class ScrVmScriptCommandState;
struct ScriptTextCommandNativeContext final
{
    IScriptFileCommandSource& pc;
    IScrVmCommandErrorSink& errors;
    const IScrResourceNameSource& names;
    IScriptSoundGroupSource& groups;
    IScriptSoundLeafSource& soundLeaves;
    IScrVmSoundOpenGate& soundGate;
};

std::int32_t ExecuteScriptTextAt4D1E50(ScrScriptRuntimeState& runtime,
    ScrVmScriptCommandState& state, ScrMusicInstructionHandler& music,
    IScrVmTextRuntimeBoundary& text, const ScriptTextCommandNativeContext& context,
    IEnvironmentRootMemory& memory, IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
}
