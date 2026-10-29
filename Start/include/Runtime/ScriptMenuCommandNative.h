#pragma once

#include <Runtime/ScriptAudioCommandsNative.h>
#include <Runtime/ScriptSoundLeafSource.h>
#include <Runtime/ScriptMenuSource.h>

namespace KaGuYa::Start::Runtime
{
// 4CEEF9/4CF27C只借这两个图层操作；声音、slot7和choice来自各自原生来源。
class IScrVmMenuLayerBoundary
{
public:
    virtual ~IScrVmMenuLayerBoundary() = default;
    virtual void ApplyResultLayerList(std::int32_t listIndex) = 0;
    virtual void SelectEffect(std::int32_t selector) = 0;
};
class ScrVmScriptCommandState;
struct ScriptMenuCommandNativeContext final
{
    IScriptFileCommandSource& pc;
    IScrVmCommandErrorSink& errors;
    const IScrResourceNameSource& names;
    IScriptSoundGroupSource& groups;
    IScriptSoundLeafSource& soundLeaves;
    IScrVmSoundOpenGate& soundGate;
    IScriptMenuSource& menu;
};
std::int32_t ExecuteScriptMenuAt4CEDA0(ScrScriptRuntimeState& runtime,
    ScrVmScriptCommandState& state, ScrMusicInstructionHandler& music,
    IScrVmMenuLayerBoundary& menu, const ScriptMenuCommandNativeContext& context,
    IEnvironmentRootMemory& memory, IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
}
