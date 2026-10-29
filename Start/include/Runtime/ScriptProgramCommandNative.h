#pragma once

#include <Runtime/ScriptAudioCommandsNative.h>
#include <Runtime/ScriptSoundLeafSource.h>

namespace KaGuYa::Start::Runtime
{
class IScriptProgramMessageSource;
class IScrVmUpdateRuntimeBoundary;
class IScrVmProgramRuntimeBoundary;
class ScrProgramInstructionHandler;
struct ScriptProgramCommandNativeContext final
{
    IScriptFileCommandSource& pc;
    IScrVmCommandErrorSink& errors;
    IScriptProgramMessageSource& messages;
    const IScrResourceNameSource& names;
    IScriptSoundGroupSource& groups;
    IScriptSoundLeafSource& soundLeaves;
    IScrVmSoundOpenGate& soundGate;
    IScrVmUpdateRuntimeBoundary& layers;
    ScrMusicInstructionHandler& music;
};
std::int32_t ExecuteScriptProgramAt4CFB30(ScrScriptRuntimeState& runtime,
    ScrProgramInstructionHandler& handler, const ScriptProgramCommandNativeContext& context,
    IEnvironmentRootMemory& memory, IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
void RestoreScriptProgramAt4D4F20(ScrScriptRuntimeState& runtime,
    IScrVmProgramRuntimeBoundary& program, const ScriptProgramCommandNativeContext& context,
    IEnvironmentRootMemory& memory);
}
