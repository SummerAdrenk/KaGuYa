#include <Runtime/ScrVmSoundHandler.h>
#include <Runtime/ScriptSoundGroupSource.h>
#include <Runtime/ScriptSoundLeafSource.h>
#include <Runtime/SoundParamsEnvironmentArchive.h>

#include <bit>
#include <cstring>

namespace KaGuYa::Start::Runtime
{
namespace
{
const ParamsGameSystem& Resolve(ParamsRuntimeRoot* root)
{
    // 原版直接解址；现代安全边界明确失败，不改用缓存/空配置。
    if (!root || !root->GameSystem())
        throw std::logic_error("SOUND null params/GameSystem (modern safety boundary)");
    return *root->GameSystem();
}
const ParamsGameSystem& Capture(IScriptSoundGroupSource& source)
{
    return Resolve(source.CaptureAt418480());
}
bool SignedLess(std::int32_t port, std::uint32_t count) noexcept
{
    return port < std::bit_cast<std::int32_t>(count);
}
ScriptSoundManagerReceiver Manager(ScriptSoundManagerReceiver manager)
{
    if (!manager) throw std::logic_error("SOUND null manager (modern safety boundary)");
    return manager;
}
RuntimeCString& Mapping(EnvironmentVectorStorage<RuntimeCString>* table, std::uint32_t port)
{
    if (!table || !table->begin || port >= static_cast<std::uint32_t>(table->end - table->begin))
        throw std::out_of_range("SOUND mapping index (modern safety boundary)");
    return table->begin[port];
}
void StoreMapping(IScriptSoundLeafSource& source, std::uint32_t port,
    ScrResourceNameReceiver names, std::int32_t resourceId)
{
    // 4D66EE/4D63D3先借vector对象，lookup后才读取它的begin并定位CString。
    auto* const table = source.CaptureMappingsAt30();
    const char* const name = names.FindNameAt4AD1B0(resourceId);
    auto& target = Mapping(table, port);
    SetRuntimeCStringAt437680(target, name, name ? static_cast<int>(std::strlen(name)) : 0);
}
}

void ApplyScriptFirstSoundAt4D6640(std::int32_t resourceId, std::uint32_t index,
    const IScrResourceNameSource& names, IScriptSoundLeafSource& source, IScrVmSoundOpenGate& gate)
{
    if (resourceId < 0)
    {
        auto& target = Mapping(source.CaptureMappingsAt30(), index);
        const auto manager = source.CaptureRootAfterEnsureAt413BE0().Resolve();
        SetRuntimeCStringAt437680(target, "", 0);
        Manager(manager).StopAndClearMappedSlotAtPort(index);
        return;
    }
    StoreMapping(source, index, names.CapturePatternNames(), resourceId);
    if (gate.ShouldSkipOpening(ScrVmSoundSlotGroup::FirstAt50, index)) return;
    // 4D678A的manager查询语音，4D67CF的另一次捕获才执行播放。
    const auto query = source.CaptureRootAfterEnsureAt413BE0().Resolve();
    const char* const name = Mapping(source.CaptureMappingsAt30(), index).GetString();
    const auto player = source.CaptureRootAfterEnsureAt413BE0().Resolve();
    const bool voice = Manager(query).AnyVoiceSlotPlaying();
    (void)Manager(player).RestoreFirstMappedEffectRaw(index, name, voice);
}
void ApplyScriptSecondSoundAt4D62A0(std::int32_t resourceId, std::uint32_t index,
    const IScrResourceNameSource& names, IScriptSoundGroupSource& groups,
    IScriptSoundLeafSource& source, IScrVmSoundOpenGate& gate)
{
    if (resourceId < 0)
    {
        // 4D62A6与4D62D6两次取源：第一份计数在Ensure以后才读，决定停止哪个槽。
        const auto& stopping = Capture(groups);
        const auto mappingPort = index + Capture(groups).soundSlotCounts[3];
        auto& target = Mapping(source.CaptureMappingsAt30(), mappingPort);
        const auto manager = source.CaptureRootAfterEnsureAt413BE0().Resolve();
        const auto stopPort = index + stopping.soundSlotCounts[3];
        SetRuntimeCStringAt437680(target, "", 0);
        Manager(manager).StopAndClearMappedSlotAtPort(stopPort);
        return;
    }
    const auto pattern = names.CapturePatternNames();
    const auto mappedPort = index + Capture(groups).soundSlotCounts[3];
    StoreMapping(source, mappedPort, pattern, resourceId);
    if (gate.ShouldSkipOpening(ScrVmSoundSlotGroup::SecondAt48, index)) return;
    const bool loop = source.CaptureSoundParametersAt1C().HasLoopEffectType();
    const auto parameters = source.CaptureSoundParametersAt1C();
    const auto playbackPort = index + Capture(groups).soundSlotCounts[3];
    const char* const name = Mapping(source.CaptureMappingsAt30(), playbackPort).GetString();
    const auto manager = source.CaptureRootAfterEnsureAt413BE0().Resolve();
    const auto volume = loop ? parameters.LoopEffectValueOrThrow() : parameters.NormalEffectValueOrThrow();
    (void)Manager(manager).RestoreSecondMappedEffectRaw(index, name, volume);
}

void ScrSoundInstructionHandler::ApplyNativeOperands(const std::int8_t port,
    const std::int32_t resourceId)
{
    if (!groups_ || !leaves_) throw std::logic_error("SOUND native operands require params/leaf sources");
    auto& groups = *groups_;
    auto& leaves = *leaves_;
    if (port < 0)
    {
        if (resourceId >= 0) throw "\x83\x7C\x81\x5B\x83\x67\x94\xD4\x8D\x86";
        const auto manager = leaves.CaptureRootAfterEnsureAt413BE0().Resolve();
        auto* const table = leaves.CaptureMappingsAt30();
        // 413270接收调用方捕获的映射表；清表期间不重新取得全局Environment。
        if (table)
            for (auto* item = table->begin; item != table->end; ++item)
                SetRuntimeCStringAt437680(*item, "", 0);
        Manager(manager).StopEffectsKeepingMappingsAt413270();
        return;
    }
    const auto absolute = static_cast<std::uint32_t>(port);
    if (resourceId < 0)
    {
        const auto& first = Capture(groups);
        const auto& second = Capture(groups);
        if (SignedLess(port, second.soundSlotCounts[1] + first.soundSlotCounts[3]))
        {
            auto& target = Mapping(leaves.CaptureMappingsAt30(), absolute);
            const auto manager = leaves.CaptureRootAfterEnsureAt413BE0().Resolve();
            SetRuntimeCStringAt437680(target, "", 0);
            Manager(manager).StopAndClearMappedSlotAtPort(absolute);
        }
        else
        {
            // 4D17D3/4D17FD重新解址两个GameSystem，不能复用上面判定的计数。
            const auto& remainingFirst = Capture(groups);
            const auto& remainingSecond = Capture(groups);
            const auto manager = leaves.CaptureRootAfterEnsureAt413BE0().Resolve();
            // 4D186B/4D1873在manager捕获之后才读取旧GameSystem正文中的计数。
            Manager(manager).StopAndClearRemainingSlot(
                absolute - remainingSecond.soundSlotCounts[1] - remainingFirst.soundSlotCounts[3]);
        }
        return;
    }
    if (Capture(groups).soundSlotCounts[3] != 0 &&
        SignedLess(port, Capture(groups).soundSlotCounts[3]))
    {
        ApplyScriptFirstSoundAt4D6640(resourceId, absolute, resourceNames_, leaves, gate_);
        return;
    }
    if (Capture(groups).soundSlotCounts[1] != 0)
    {
        // 4D191A先捕获根A，第二次418480可更改A的Ref；B解址后才解析A。
        auto* const rootA = groups.CaptureAt418480();
        const auto& second = Capture(groups);
        const auto& first = Resolve(rootA);
        if (SignedLess(port, second.soundSlotCounts[1] + first.soundSlotCounts[3]))
        {
            const auto relative = absolute - Capture(groups).soundSlotCounts[3];
            ApplyScriptSecondSoundAt4D62A0(resourceId, relative, resourceNames_, groups, leaves, gate_);
            return;
        }
    }
    if (gate_.ShouldSkipRemainingOpeningAt4D196A()) return;
    const auto parameters = leaves.CaptureSoundParametersAt1C();
    const auto& second = Capture(groups);
    const auto& first = Capture(groups);
    const auto names = resourceNames_.CapturePatternNames();
    // 4D1A6D只捕获根，4188F0必须等fallback/计数/name读取完成后再解manager。
    const auto root = leaves.CaptureRootAfterEnsureAt413BE0();
    const auto fallback = parameters.NormalEffectValueOrThrow();
    const auto relative = absolute - first.soundSlotCounts[3] - second.soundSlotCounts[1];
    const char* const name = names.FindNameAt4AD1B0(resourceId);
    if (!Manager(root.Resolve()).OpenRemainingSlot(relative, name, fallback))
        throw 1;
}
}
