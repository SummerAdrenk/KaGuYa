#include <Runtime/NativeLayerWindowScrVmTextRuntime.h>
#include <Runtime/ScriptAudioCommandsNative.h>

namespace KaGuYa::Start::Runtime
{
std::size_t NativeLayerWindowScrVmTextRuntime::FirstStringGroupCountAt50() const
{return groups_.CaptureAt418480()->GameSystem()->soundSlotCounts[3];}
std::size_t NativeLayerWindowScrVmTextRuntime::SecondStringGroupCountAt48() const
{return groups_.CaptureAt418480()->GameSystem()->soundSlotCounts[1];}
void NativeLayerWindowScrVmTextRuntime::ApplyResultLayerList(std::int32_t index)
{updates_.ApplyResultLayerList(index);}
void NativeLayerWindowScrVmTextRuntime::ApplyFirstStringSlot(std::int32_t id,std::size_t index)
{ApplyScriptFirstSoundAt4D6640(id,static_cast<std::uint32_t>(index),names_,sounds_,gate_);}
void NativeLayerWindowScrVmTextRuntime::ApplySecondStringSlot(std::int32_t id,std::size_t index)
{ApplyScriptSecondSoundAt4D62A0(id,static_cast<std::uint32_t>(index),names_,groups_,sounds_,gate_);}
void NativeLayerWindowScrVmTextRuntime::SelectEffect(std::int32_t selector)
{updates_.SelectLayerEffect(selector);}
}
