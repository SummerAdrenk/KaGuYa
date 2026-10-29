#include <Runtime/SoundSlotManagerRuntime.h>
#include <Runtime/VoiceBatchNative.h>
#include <Runtime/RootArchiveResourceSource.h>
#include <Runtime/SoundParamsEnvironmentArchive.h>
#include <Runtime/UnSaveDataRuntime.h>
#include <Runtime/ArchiveResourceNameRuntime.h>
#include <Runtime/RuntimeCStringResult.h>

#include <Windows.h>
#include <mbstring.h>

#include <bit>
#include <cstring>
#include <limits>
#include <optional>
#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
[[nodiscard]] std::size_t CheckedAdd(const std::size_t first, const std::uint32_t second)
{
    if (second > std::numeric_limits<std::size_t>::max() - first)
        throw std::overflow_error("sound slot count overflow");
    return first + second;
}

[[nodiscard]] std::int32_t ArithmeticHalf(const std::int32_t value) noexcept
{
    // 等价于 x86 SAR 1：负奇数向负无穷取整，且 INT32_MIN 不发生取负溢出。
    return value / 2 - (value % 2 < 0 ? 1 : 0);
}

[[nodiscard]] ISoundEffectPlaybackControl& RequireSoundParams(
    const std::shared_ptr<ISoundEffectPlaybackControl>& token)
{
    if (!token)
        throw std::invalid_argument("45D1DA requires an owning SoundParams reference");
    return *token;
}

[[nodiscard]] std::int32_t ReadLowerVolumeParameter(
    const SoundFirstEffectLowerVolumeState& state)
{
    if (!state.resolvedParameter)
        throw std::logic_error("sound lowerVolume +08 is uninitialized");
    return *state.resolvedParameter;
}

[[nodiscard]] std::int32_t SignedDword(const std::uint32_t value) noexcept
{
    return std::bit_cast<std::int32_t>(value);
}

[[nodiscard]] std::string_view ReadSoundName(const std::string_view bounded,
    const std::optional<const char*> raw)
{
    // 未提供 raw 的旧入口始终有界；engaged-null 与有效空 CString 保持不同 data()。
    if (!raw) return bounded;
    return *raw ? std::string_view(*raw) : std::string_view{};
}
}

void ISoundEffectMappingState::StorePatternName(const std::size_t port,
    const ScrResourceNameReceiver receiver, const std::int32_t resourceId)
{
    // 4D66F1/4D63D6 查询后，437680 才按首 NUL 长度写入；null 在此明确表示空串。
    const char* const name = receiver.FindNameAt4AD1B0(resourceId);
    Store(port, name ? std::string_view(name) : std::string_view{});
}

UnsavedSoundEffectMappingState::UnsavedSoundEffectMappingState(
    const std::size_t slotCount)
    : resourceNames_(slotCount)
{
}

std::size_t UnsavedSoundEffectMappingState::Size() const noexcept
{
    return resourceNames_.size();
}

std::string_view UnsavedSoundEffectMappingState::ResourceNameAt(
    const std::size_t port) const
{
    if (port >= resourceNames_.size())
        throw std::out_of_range("sound effect mapping port is out of range");
    return resourceNames_[port];
}

void UnsavedSoundEffectMappingState::ClearAll()
{
    for (std::string& name : resourceNames_)
        name.clear();
}

void UnsavedSoundEffectMappingState::Clear(const std::size_t port)
{
    if (port >= resourceNames_.size())
        throw std::out_of_range("sound effect mapping port is out of range");
    resourceNames_[port].clear();
}

void UnsavedSoundEffectMappingState::Store(
    const std::size_t port,
    const std::string_view resourceName)
{
    if (port >= resourceNames_.size())
        throw std::out_of_range("sound effect mapping port is out of range");
    resourceNames_[port].assign(resourceName);
}

SoundSlotManagerRuntime::SoundSlotManagerRuntime(
    ISoundSlotManagerConstructionSource& source)
    : SoundSlotManagerRuntime(source, source.Dependencies())
{
}

SoundSlotManagerRuntime::SoundSlotManagerRuntime(
    ISoundSlotManagerConstructionSource& source,
    SoundSlotManagerDependencies dependencies)
    : dependenciesLifetime_(std::move(dependencies.lifetime)),
      soundApi_(dependencies.soundApi),
      resources_(dependencies.resources),
      musicControl_(dependencies.musicControl),
      effectOpenGate_(dependencies.effectOpenGate),
      firstEffectOpenMode_(dependencies.firstEffectOpenMode),
      effectMappingState_(dependencies.effectMappingState),
      voiceBatchCountSource_(&dependencies.voiceBatchCount),
      effectParameterSource_(&dependencies.effectParameters),
      soundParamsLifetime_(source.AcquireSoundParamsAt45D1DA()),
      effectControl_(RequireSoundParams(soundParamsLifetime_)),
      voiceControl_(dynamic_cast<IVoicePlaybackControl&>(effectControl_))
{
    if (!dependenciesLifetime_)
        throw std::invalid_argument("sound manager dependencies require an owning token");

    source.SetGlobalFocusAt45D28C(1);
    // 45D2A2/45D2CF/45D2FB/45D327: independent late reads, x86 DWORD adds.
    std::uint32_t boundary = 1u + source.ReadSlotCountAt45D120(2);
    layout_.firstBoundary = boundary;
    boundary += source.ReadSlotCountAt45D120(3);
    layout_.secondBoundary = boundary;
    boundary += source.ReadSlotCountAt45D120(1);
    layout_.thirdBoundary = boundary;
    boundary += source.ReadSlotCountAt45D120(0);
    layout_.totalCount = boundary;
    // 45750F / 466870: allocate and zero every slot before the first AddSound.
    if (boundary > 0x1FFFFFFFu)
        throw std::length_error("4574C0 sound slot vector exceeds x86 maximum");
    slots_.resize(boundary);

    const auto add = [&source, this](const std::uint32_t index, const std::uint8_t mode)
    {
        ISoundRuntimeApi* const api = source.ReadSoundApiAt42CA60();
        const auto handle = api ? api->AddSound(mode) : SoundRuntimeHandle{0};
        // The original stores after AddSound. Reject an invalid store explicitly,
        // preserving that call's side effects, including a returned zero handle.
        slots_.at(index).handle = handle;
    };
    add(0u, 1);
    for (std::uint32_t index = 1u;; ++index)
    {
        const std::uint32_t end = 1u + source.ReadSlotCountAt45D120(2);
        if (SignedDword(index) >= SignedDword(end))
            break;
        add(index, 0);
    }
    // 42CCC1/42CE05: signed byte-distance SAR 3, not an unsigned size compare.
    const auto slotEnd = ArithmeticHalf(ArithmeticHalf(ArithmeticHalf(
        SignedDword(static_cast<std::uint32_t>(slots_.size()) * 8u))));
    for (auto index = static_cast<std::uint32_t>(layout_.firstBoundary);
         SignedDword(index) < slotEnd; ++index)
    {
        add(index, 0);
    }
    ConstructLowerVolumeTable();
    policy_ = source.ReadPolicyAt45D4DC();
    defaultDeviceId_ = source.ReadDefaultDeviceIdAt45D57A();
    // EH 5E9A44 has no catch: partial raw Sound handles are NOT removed.
}

SoundSlotManagerRuntime::SoundSlotManagerRuntime(
    ISoundRuntimeApi& soundApi,
    const ResourceCatalog& resources,
    const std::array<std::uint32_t, 4>& slotCounts,
    ISoundPlaybackControl& musicControl,
    ISoundEffectPlaybackControl& effectControl,
    ISoundEffectOpenGate& effectOpenGate,
    ISoundFirstEffectOpenModeSource& firstEffectOpenMode,
    ISoundEffectMappingState& effectMappingState,
    IVoicePlaybackControl& voiceControl)
    : soundApi_(soundApi),
      resources_(resources),
      musicControl_(musicControl),
      effectOpenGate_(effectOpenGate),
      firstEffectOpenMode_(firstEffectOpenMode),
      effectMappingState_(effectMappingState),
      fixedVoiceBatchCount_(slotCounts[2]),
      layout_(BuildLayout(slotCounts)),
      slots_(layout_.totalCount),
      effectControl_(effectControl),
      voiceControl_(voiceControl)
{
    if (effectMappingState_.Size() != layout_.thirdBoundary - layout_.firstBoundary)
        throw std::invalid_argument("sound effect mapping size does not match slot layout");
    // Fixed counts collapse the two stable non-music ranges; no root sampling.
    slots_[0].handle = soundApi_.AddSound(1);
    for (std::size_t index = 1; index < slots_.size(); ++index)
        slots_[index].handle = soundApi_.AddSound(0);
    ConstructLowerVolumeTable();
}

SoundSlotManagerRuntime::~SoundSlotManagerRuntime()
{
    // 463ED0: names forward, lowerVolume forward/storage, +24 ref, slot storage.
    // Raw handles are owned by Sound's shutdown, NOT by this vector (4627C0).
    for (Slot& slot : slots_)
        std::string{}.swap(slot.resourceName);
    firstEffectLowerVolumeStates_.Clear();
    soundParamsLifetime_.reset();
    std::vector<Slot>{}.swap(slots_);
}

SoundSlotManagerRuntime::LowerVolumeTable::~LowerVolumeTable()
{
    Clear();
}

void SoundSlotManagerRuntime::LowerVolumeTable::Clear() noexcept
{
    // 462240 -> 4529E0 also applies during construction unwind.
    for (auto& entry : entries)
        entry.reset();
    decltype(entries){}.swap(entries);
}

void SoundSlotManagerRuntime::ConstructLowerVolumeTable()
{
    // 45D37B..45D4B6: no reserve/whole-table allocation ahead of AddSound.
    const auto count = SignedDword(static_cast<std::uint32_t>(layout_.secondBoundary) -
        static_cast<std::uint32_t>(layout_.firstBoundary));
    for (std::int32_t index = 0; index < count; ++index)
    {
        // 45D394 / 45D3E7: object, then independent shared control, then append.
        std::shared_ptr<SoundFirstEffectLowerVolumeState> state(
            new SoundFirstEffectLowerVolumeState{
                static_cast<std::uint32_t>(layout_.firstBoundary) +
                    static_cast<std::uint32_t>(index),
                0, std::nullopt, {}});
        firstEffectLowerVolumeStates_.entries.push_back(std::move(state));
    }
}

std::uint32_t SoundSlotManagerRuntime::PolicyAt3C() const
{
    if (!policy_)
        throw std::logic_error("fixed-input sound manager has no +3C policy");
    return *policy_;
}
IVoiceRelationLookup& SoundSlotManagerRuntime::VoiceRelationsAt24()
{
    // 旧producer保留已在45D1DA获得的参数，而不是重新借当前Environment参数。
    return dynamic_cast<IVoiceRelationLookup&>(effectControl_);
}

const SoundDefaultDeviceId& SoundSlotManagerRuntime::DefaultDeviceIdAt40() const
{
    if (!defaultDeviceId_)
        throw std::logic_error("fixed-input sound manager has no +40 device ID");
    return *defaultDeviceId_;
}

const SoundSlotLayout& SoundSlotManagerRuntime::Layout() const noexcept
{
    return layout_;
}

const SoundFirstEffectLowerVolumeState&
SoundSlotManagerRuntime::FirstEffectLowerVolumeStateAt(
    const std::size_t groupIndex) const
{
    if (groupIndex >= firstEffectLowerVolumeStates_.entries.size())
        throw ScrVmSoundCommandException({});
    return *firstEffectLowerVolumeStates_.entries[groupIndex];
}

void SoundSlotManagerRuntime::SetFirstEffectAlternateParameter(
    const std::size_t groupIndex,
    const std::int32_t value)
{
    if (groupIndex >= firstEffectLowerVolumeStates_.entries.size())
        throw ScrVmSoundCommandException({});
    firstEffectLowerVolumeStates_.entries[groupIndex]->alternateParameter = value;
}

std::int32_t SoundSlotManagerRuntime::ReadPlaybackParameter() const
{
    return musicControl_.PlaybackParameter();
}

bool SoundSlotManagerRuntime::HasPlaybackSlots() const
{
    return !slots_.empty();
}

bool SoundSlotManagerRuntime::IsCurrentSlotActive() const
{
    return musicControl_.Active();
}

const char* SoundSlotManagerRuntime::CurrentSlotResourceName() const
{
    return slots_[layout_.currentMusicSlot].resourceName.c_str();
}

void SoundSlotManagerRuntime::SwitchCurrentSlot(
    const std::string_view resourceName,
    const std::int32_t playbackParameter)
{
    (void)OpenResolvedSlot(
        layout_.currentMusicSlot, resourceName, playbackParameter, true, true);
}

void SoundSlotManagerRuntime::StopMusicSlots()
{
    StopAndClearSlot(layout_.currentMusicSlot);
}

bool SoundSlotManagerRuntime::IsAbsoluteSlotPlayingAt419370(
    const std::uint32_t absoluteSlot) const
{
    // 原槽数组没有边界保护；现代显式拒绝越界，不映射到空槽或 Active 配置。
    return soundApi_.IsSlotPlaying(slots_.at(absoluteSlot).handle);
}

void SoundSlotManagerRuntime::StopAbsoluteSlotAt4383B0(
    const std::uint32_t absoluteSlot)
{
    StopAndClearSlot(absoluteSlot);
}

void SoundSlotManagerRuntime::StopDemoMusicAt4C3500(RuntimeCString& environmentMusicName)
{
    SetRuntimeCStringAt437680(environmentMusicName, "", 0);
    StopMusicSlots();
}

bool SoundSlotManagerRuntime::PlayDemoMusicAt4BCCC0(RuntimeCString& environmentMusicName,
    const char* const resourceName, const std::int32_t playbackParameter, const std::int32_t loop,
    IArchiveResourceSource& resources)
{
    // 写环境名早于空槽/Active/同名判断；该副作用不因后续失败回滚。
    const int length = resourceName ? static_cast<int>(std::strlen(resourceName)) : 0;
    SetRuntimeCStringAt437680(environmentMusicName, resourceName, length);
    if (slots_.empty() || !musicControl_.Active()) return true;
    if (!resourceName) throw ATL::CAtlException(E_FAIL); // 4BCD2C/4BCD61，不提前至输出前。
    if (_mbsicmp(reinterpret_cast<const unsigned char*>(slots_.at(layout_.currentMusicSlot).resourceName.c_str()),
            reinterpret_cast<const unsigned char*>(resourceName)) == 0) return true;
    return OpenResolvedSlot(layout_.currentMusicSlot, {}, playbackParameter,
        static_cast<std::uint8_t>(loop), true, resourceName, &resources);
}

bool SoundSlotManagerRuntime::PlayDemoVoiceAt4BCE10(const char* const resourceName,
    IArchiveResourceSource& resources)
{
    if (slots_.empty() || !voiceControl_.IsBatchObjectReadyAtA4()) return true;
    // 每个参数叶才读取本次首 NUL；前一个虚调用允许改写同一合法源缓冲。
    const auto readName = [&] { return ReadSoundName({}, std::optional<const char*>{resourceName}); };
    if (!voiceControl_.PrepareResourceAtA8(readName())) return true;
    auto value = voiceControl_.ReadResourceValueAtAC(readName());
    if (value < 0)
    {
        // PE 5EE0A4 try[0,0] / 5EE094 catch-all，仅覆盖 +6C；+AC 异常外传。
        try { value = voiceControl_.ReadFallbackValueAt6C(); }
        catch (...) { return false; }
        if (value < 0) return false;
    }
    // 4BCF0A 的40B530在音量判定后复制；Open回调不再借用原命令里的名字。
    RuntimeCStringResult name([&](RuntimeCString* result)
        { ConstructArchiveResourceNameAt40B530(result, resourceName); });
    return OpenResolvedSlot(1u, {}, value, 0, true, name.Value().GetString(), &resources);
}

void SoundSlotManagerRuntime::ExecuteDemoSound(
    const std::uint8_t mode,
    const std::string& resourceName,
    std::string& environmentMusicName)
{
    if (mode == 0)
    {
        if (resourceName.empty())
        {
            // 4C3500：先清环境名称，再进入 4383B0 停止实际槽。
            environmentMusicName.clear();
            StopMusicSlots();
            return;
        }
        const std::int32_t parameter = musicControl_.PlaybackParameter();
        // 4BCCC0：禁用、同名及打开失败都不能撤销这次环境名称写入。
        environmentMusicName = resourceName;
        if (slots_.empty() || !musicControl_.Active())
            return;
        if (_mbsicmp(
                reinterpret_cast<const unsigned char*>(
                    slots_[layout_.currentMusicSlot].resourceName.c_str()),
                reinterpret_cast<const unsigned char*>(resourceName.c_str())) == 0)
            return;
        // 4BF5A0 传 loop=0；不同于脚本音乐入口的 loop=1。
        (void)OpenResolvedSlot(
            layout_.currentMusicSlot, resourceName, parameter, false, true);
        return;
    }
    if (mode == 1)
    {
        if (resourceName.empty())
            StopAndClearAllVoiceSlots();
        else
            (void)PlayDemoVoice(resourceName);
        return;
    }
    // 413270(manager,nullptr)：停三个音效组，不修改环境映射；非空名称无操作。
    if (resourceName.empty())
        StopEffectsKeepingMappingsAt413270();
}

bool SoundSlotManagerRuntime::PlayDemoVoice(const std::string& resourceName)
{
    FixedArchiveResourceSource fixture(resources_);
    return PlayDemoVoiceAt4BCE10(resourceName.c_str(), fixture);
}

std::size_t SoundSlotManagerRuntime::FirstSlotGroupCountAt50() const
{
    return layout_.secondBoundary - layout_.firstBoundary;
}

std::size_t SoundSlotManagerRuntime::SecondSlotGroupCountAt48() const
{
    return layout_.thirdBoundary - layout_.secondBoundary;
}

void SoundSlotManagerRuntime::StopAndClearAllEffectSlots()
{
    // 413270：只清理 firstBoundary 起始的三组音效，不能触碰 BGM 与语音槽。
    effectMappingState_.ClearAll();
    StopEffectsKeepingMappingsAt413270();
}

void SoundSlotManagerRuntime::StopEffectsKeepingMappingsAt413270()
{
    for (std::size_t index = layout_.firstBoundary; index < slots_.size(); ++index)
        StopAndClearSlot(index);
}

void SoundSlotManagerRuntime::ClearMappedResourceNameAtPort(const std::size_t port)
{
    if (port >= effectMappingState_.Size())
        throw ScrVmSoundCommandException({});
    effectMappingState_.Clear(port);
}

void SoundSlotManagerRuntime::StopAndClearMappedSlotAtPort(const std::size_t port)
{
    StopAndClearSlot(ResolveMappedSlot(port));
}

void SoundSlotManagerRuntime::StopAndClearRemainingSlot(const std::size_t groupIndex)
{
    StopAndClearSlot(ResolveEffectSlot(ScrVmSoundSlotGroup::Remaining, groupIndex));
}

void SoundSlotManagerRuntime::StoreMappedResourceNameAtPort(
    const std::size_t port,
    const std::string_view resourceName)
{
    if (port >= effectMappingState_.Size())
        throw ScrVmSoundCommandException({});
    effectMappingState_.Store(port, resourceName);
}

bool SoundSlotManagerRuntime::ShouldSkipRemainingOpeningAt4D196A() const
{
    return effectOpenGate_.ShouldSuppressRemainingOpen();
}

bool SoundSlotManagerRuntime::ShouldSkipOpening(
    const ScrVmSoundSlotGroup group,
    const std::size_t groupIndex) const
{
    return effectOpenGate_.ShouldSuppressOpen(group, groupIndex);
}

void SoundSlotManagerRuntime::StoreMappedPatternNameAtPort(const std::size_t port,
    const ScrResourceNameReceiver receiver, const std::int32_t resourceId)
{
    effectMappingState_.StorePatternName(port, receiver, resourceId);
}

SoundMappingParameterReceiver
SoundSlotManagerRuntime::CaptureEffectParametersAt1C() const
{
    if (effectParameterSource_)
        return effectParameterSource_->CaptureEffectParametersAt1C();
    // 旧固定输入入口显式借 effectControl；不支持所需参数接口时抛 bad_cast，不伪造值。
    return dynamic_cast<const ISoundMappingRestoreParameterSource&>(effectControl_);
}

ScrVmSoundOpenResult SoundSlotManagerRuntime::OpenMappedSlotAtPort(
    const ScrVmSoundSlotGroup group, const std::size_t groupIndex, const std::size_t port)
{
    if (group == ScrVmSoundSlotGroup::FirstAt50)
    {
        // 4D67C7 捕获映射字符地址，4D67F8 查询语音，4D6807 播放；不复制字符串。
        const char* const name = effectMappingState_.ResourceNameAt(port).data();
        const bool anyVoice = AnyVoiceSlotPlaying();
        return RestoreFirstMappedEffectImpl(groupIndex, {}, anyVoice, name);
    }
    if (group != ScrVmSoundSlotGroup::SecondAt48)
        throw std::invalid_argument("remaining sound has no mapped port");

    // 4D6491 查询 +28 后再取 Env.slot1；映射字符地址早于 +30/+24 的值查询。
    const bool loop = CaptureEffectParametersAt1C().HasLoopEffectType();
    const auto& parameters = CaptureEffectParametersAt1C();
    const char* const name = effectMappingState_.ResourceNameAt(port).data();
    const std::int32_t fallback = loop
        ? parameters.LoopEffectValueOrThrow() : parameters.NormalEffectValueOrThrow();
    return RestoreSecondMappedEffectImpl(groupIndex, {}, fallback, name);
}

ScrVmSoundOpenResult SoundSlotManagerRuntime::OpenRemainingSlot(
    const std::size_t groupIndex, const char* const resourceName,
    const std::int32_t fallbackValue)
{
    // 4D1AA8 检查整个槽数组；不能提前按第三组容量拒绝或读取 nullable 名称。
    if (slots_.empty())
        return ScrVmSoundOpenResult::Skipped;
    const auto preparation = effectControl_.PrepareRemainingEffectSlotWithFallback(
        resourceName, fallbackValue);
    if (!preparation.shouldOpen)
        return ScrVmSoundOpenResult::Skipped;
    // 42BC90 先查资源，随后才访问物理槽；不先调用 ResolveEffectSlot。
    if (groupIndex > (std::numeric_limits<std::size_t>::max)() - layout_.thirdBoundary)
        throw std::overflow_error("remaining sound absolute slot overflow");
    return OpenResolvedSlot(layout_.thirdBoundary + groupIndex, {},
        preparation.playbackParameter, false, true, std::optional<const char*>{resourceName})
        ? ScrVmSoundOpenResult::Opened : ScrVmSoundOpenResult::Failed;
}

ScrVmSoundOpenResult SoundSlotManagerRuntime::OpenAndPlaySlot(
    const ScrVmSoundSlotGroup group,
    const std::size_t groupIndex,
    const std::string_view resourceName)
{
    const std::size_t absoluteIndex = ResolveEffectSlot(group, groupIndex);
    bool shouldOpen = false;
    std::int32_t playbackParameter = 0;
    switch (group)
    {
    case ScrVmSoundSlotGroup::FirstAt50:
    {
        const SoundFirstEffectSlotPreparation preparation =
            effectControl_.PrepareFirstEffectSlotAt50(
                groupIndex,
                resourceName,
                slots_[absoluteIndex].resourceName,
                firstEffectOpenMode_.RestorePersistedValue(groupIndex));
        shouldOpen = preparation.shouldOpen;
        if (!shouldOpen)
            break;

        SoundFirstEffectLowerVolumeState& lowerVolume =
            *firstEffectLowerVolumeStates_.entries[groupIndex];
        playbackParameter = preparation.resolvedParameter;
        if (!preparation.useAlternateParameter || lowerVolume.alternateParameter >= 0)
        {
            lowerVolume.resolvedParameter = preparation.resolvedParameter;
            lowerVolume.relation = preparation.relation;
            if (preparation.useAlternateParameter)
                playbackParameter = lowerVolume.alternateParameter;
        }
        break;
    }
    case ScrVmSoundSlotGroup::SecondAt48:
    {
        const SoundEffectSlotPreparation preparation =
            effectControl_.PrepareSecondEffectSlotAt48(
            groupIndex, resourceName, slots_[absoluteIndex].resourceName);
        shouldOpen = preparation.shouldOpen;
        playbackParameter = preparation.playbackParameter;
        break;
    }
    case ScrVmSoundSlotGroup::Remaining:
    {
        const SoundEffectSlotPreparation preparation =
            effectControl_.PrepareRemainingEffectSlot(
            groupIndex, resourceName, slots_[absoluteIndex].resourceName);
        shouldOpen = preparation.shouldOpen;
        playbackParameter = preparation.playbackParameter;
        break;
    }
    default:
        throw ScrVmSoundCommandException({});
    }
    if (!shouldOpen)
        return ScrVmSoundOpenResult::Skipped;

    const bool loop = group != ScrVmSoundSlotGroup::Remaining;
    return OpenResolvedSlot(
        absoluteIndex, resourceName, playbackParameter, loop, true)
        ? ScrVmSoundOpenResult::Opened
        : ScrVmSoundOpenResult::Failed;
}

std::size_t SoundSlotManagerRuntime::FirstMappedEffectCount() const
{
    return FirstSlotGroupCountAt50();
}

bool SoundSlotManagerRuntime::AnyVoiceSlotPlaying() const
{
    bool anyPlaying = false;
    // 4196C0 不在首次命中后提前退出；每个 Voice 槽都调用一次 SoundIsPlay。
    for (std::size_t index = 1; index < layout_.firstBoundary; ++index)
    {
        if (soundApi_.IsSlotPlaying(slots_[index].handle))
            anyPlaying = true;
    }
    return anyPlaying;
}

std::uint32_t SoundSlotManagerRuntime::ReadVoiceRelationMaskAt419470(
    const std::string_view relation,
    IVoiceRelationLookup& relations) const
{
    std::uint32_t result = 0;
    for (std::size_t index = 1; index < layout_.firstBoundary; ++index)
    {
        const Slot& slot = slots_[index];
        if (!soundApi_.IsSlotPlaying(slot.handle))
            continue;
        result = 1;
        if (relations.MatchesVoiceRelationAt46DD20(relation, slot.resourceName))
            return 3;
    }
    return result;
}

void SoundSlotManagerRuntime::AdjustFirstEffectsAt4C2410(
    IVoiceRelationLookup& relations)
{
    AdjustFirstEffectsAt4C2410(PolicyAt3C(), relations);
}

void SoundSlotManagerRuntime::RefreshFirstEffectsAt433F60(
    IVoiceRelationLookup& relations)
{
    RefreshFirstEffectsAt433F60(PolicyAt3C(), relations);
}

void SoundSlotManagerRuntime::AdjustFirstEffectsAt4C2410(
    const std::uint32_t policy,
    IVoiceRelationLookup& relations)
{
    for (const auto& entry : firstEffectLowerVolumeStates_.entries)
    {
        const SoundFirstEffectLowerVolumeState& state = *entry;
        const auto handle = slots_[state.absoluteSlotIndex].handle;
        if (!soundApi_.IsSlotPlaying(handle))
            continue;
        std::int32_t value;
        if (policy == 1u)
        {
            // 4C253F：每个活动音效都重新全扫 Voice，不能跨记录缓存。
            if (!AnyVoiceSlotPlaying())
                continue;
            value = state.alternateParameter;
        }
        else if (policy == 0u || policy == 2u)
        {
            const auto mask = ReadVoiceRelationMaskAt419470(state.relation, relations);
            if ((mask & 1u) == 0u)
                continue;
            if ((mask & 2u) != 0u)
                value = state.alternateParameter;
            else if (policy == 2u)
                value = ArithmeticHalf(ReadLowerVolumeParameter(state));
            else
                continue;
        }
        else
        {
            continue;
        }
        soundApi_.SetSoundVolume(handle, 100, value);
    }
}

void SoundSlotManagerRuntime::RefreshFirstEffectsAt433F60(
    const std::uint32_t policy,
    IVoiceRelationLookup& relations)
{
    // 433F8D：表为空也必须先扫描；有 Voice 且非策略 2 时不查询任何音效。
    const bool anyVoicePlaying = AnyVoiceSlotPlaying();
    if (anyVoicePlaying && policy != 2u)
        return;
    for (const auto& entry : firstEffectLowerVolumeStates_.entries)
    {
        const SoundFirstEffectLowerVolumeState& state = *entry;
        const auto handle = slots_[state.absoluteSlotIndex].handle;
        if (!soundApi_.IsSlotPlaying(handle))
            continue;
        if (anyVoicePlaying &&
            (ReadVoiceRelationMaskAt419470(state.relation, relations) & 3u) != 1u)
            continue;
        // 434251 在关系查询返回后才读取 +08，不能提前缓存解析音量。
        const std::int32_t parameter = ReadLowerVolumeParameter(state);
        const std::int32_t value = anyVoicePlaying ? ArithmeticHalf(parameter) : parameter;
        soundApi_.SetSoundVolume(handle, 100, value);
    }
}

ScrVmSoundOpenResult SoundSlotManagerRuntime::RestoreFirstMappedEffectRaw(
    std::size_t groupIndex, const char* name, bool anyVoice)
{
    return RestoreFirstMappedEffectImpl(groupIndex, {}, anyVoice, name);
}

ScrVmSoundOpenResult SoundSlotManagerRuntime::RestoreSecondMappedEffectRaw(
    std::size_t groupIndex, const char* name, std::int32_t volume)
{
    return RestoreSecondMappedEffectImpl(groupIndex, {}, volume, name);
}

ScrVmSoundOpenResult SoundSlotManagerRuntime::RestoreFirstMappedEffect(
    const std::size_t groupIndex,
    const std::string_view resourceName,
    const bool anyVoiceSlotPlaying)
{
    return RestoreFirstMappedEffectImpl(groupIndex, resourceName, anyVoiceSlotPlaying, std::nullopt);
}

ScrVmSoundOpenResult SoundSlotManagerRuntime::RestoreFirstMappedEffectImpl(
    const std::size_t groupIndex, const std::string_view resourceName,
    const bool anyVoiceSlotPlaying, const std::optional<const char*> rawResourceName)
{
    const std::size_t absoluteIndex = ResolveEffectSlot(
        ScrVmSoundSlotGroup::FirstAt50, groupIndex);
    const SoundFirstEffectSlotPreparation preparation =
        effectControl_.PrepareFirstEffectSlotAt50(
            groupIndex,
            ReadSoundName(resourceName, rawResourceName),
            slots_[absoluteIndex].resourceName,
            anyVoiceSlotPlaying);
    if (!preparation.shouldOpen)
        return ScrVmSoundOpenResult::Skipped;

    SoundFirstEffectLowerVolumeState& lowerVolume =
        *firstEffectLowerVolumeStates_.entries[groupIndex];
    std::int32_t playbackParameter = preparation.resolvedParameter;
    if (!preparation.useAlternateParameter || lowerVolume.alternateParameter >= 0)
    {
        lowerVolume.resolvedParameter = preparation.resolvedParameter;
        lowerVolume.relation = preparation.relation;
        if (preparation.useAlternateParameter)
            playbackParameter = lowerVolume.alternateParameter;
    }

    return OpenResolvedSlot(
        absoluteIndex, resourceName, playbackParameter, true, true, rawResourceName)
        ? ScrVmSoundOpenResult::Opened
        : ScrVmSoundOpenResult::Failed;
}

ScrVmSoundOpenResult SoundSlotManagerRuntime::RestoreSecondMappedEffect(
    const std::size_t groupIndex,
    const std::string_view resourceName,
    const std::int32_t fallbackValue)
{
    return RestoreSecondMappedEffectImpl(groupIndex, resourceName, fallbackValue, std::nullopt);
}

ScrVmSoundOpenResult SoundSlotManagerRuntime::RestoreSecondMappedEffectImpl(
    const std::size_t groupIndex, const std::string_view resourceName,
    const std::int32_t fallbackValue, const std::optional<const char*> rawResourceName)
{
    const std::size_t absoluteIndex = ResolveEffectSlot(
        ScrVmSoundSlotGroup::SecondAt48, groupIndex);
    const SoundEffectSlotPreparation preparation =
        effectControl_.PrepareSecondEffectSlotAt48WithFallback(
            groupIndex,
            ReadSoundName(resourceName, rawResourceName),
            slots_[absoluteIndex].resourceName,
            fallbackValue);
    if (!preparation.shouldOpen)
        return ScrVmSoundOpenResult::Skipped;

    return OpenResolvedSlot(
        absoluteIndex,
        resourceName,
        preparation.playbackParameter,
        true,
        true,
        rawResourceName)
        ? ScrVmSoundOpenResult::Opened
        : ScrVmSoundOpenResult::Failed;
}

bool SoundSlotManagerRuntime::IsBatchObjectReadyAtA4() const
{
    return voiceControl_.IsBatchObjectReadyAtA4();
}

bool SoundSlotManagerRuntime::PlayVoiceBatchAt42C4F0(
    const UnSaveDataStringArrayReference& names)
{
    return Runtime::PlayVoiceBatchAt42C4F0(*this, names);
}

std::int32_t SoundSlotManagerRuntime::VoiceBatchCapacityAt42C599() const
{
    // 生产不缓存当前GameSystem；固定入口只使用显式slotCounts[2]，未发布则拒绝。
    const std::uint32_t rawCount = voiceBatchCountSource_
        ? voiceBatchCountSource_->ReadVoiceBatchCountAt42C599()
        : fixedVoiceBatchCount_.value();
    return SignedDword(rawCount);
}

std::size_t SoundSlotManagerRuntime::VoiceSlotCapacity() const
{
    return layout_.firstBoundary - 1u;
}

void SoundSlotManagerRuntime::StopAndClearAllVoiceSlots()
{
    // 4384D0：语音只占 [1, firstBoundary)，与 opcode 9 的音效区间互斥。
    for (std::size_t index = 1; index < layout_.firstBoundary; ++index)
        StopAndClearSlot(index);
}

bool SoundSlotManagerRuntime::PrepareSlotAtA8(
    const std::size_t slotIndex,
    const std::string_view resourceName)
{
    (void)ResolveVoiceSlot(slotIndex);
    return voiceControl_.PrepareResourceAtA8(resourceName);
}

std::int32_t SoundSlotManagerRuntime::ReadSlotValueAtAC(
    const std::size_t slotIndex,
    const std::string_view resourceName)
{
    (void)ResolveVoiceSlot(slotIndex);
    return voiceControl_.ReadResourceValueAtAC(resourceName);
}

std::int32_t SoundSlotManagerRuntime::ReadFallbackValueAt6C(
    const std::size_t slotIndex,
    const std::string_view resourceName)
{
    // 42C6EE..42C6F7仅调用全局+6C，无slot/name参数；不能把物理校验异常混入局部catch。
    (void)slotIndex;
    (void)resourceName;
    return voiceControl_.ReadFallbackValueAt6C();
}

bool SoundSlotManagerRuntime::OpenSlot(
    const std::size_t slotIndex,
    const std::string_view resourceName,
    const std::int32_t resolvedValue)
{
    // 42C4F0 第一遍只完成 Open/Volume/Loop，Play 与名称提交留给第二遍。
    return OpenResolvedSlot(
        ResolveVoiceSlot(slotIndex), resourceName, resolvedValue, false, false);
}

void SoundSlotManagerRuntime::PlayVoiceSlot(const std::size_t slotIndex)
{
    const Slot& slot = slots_[ResolveVoiceSlot(slotIndex)];
    soundApi_.PlaySlot(slot.handle);
}

void SoundSlotManagerRuntime::StoreVoiceSlotResourceName(
    const std::size_t slotIndex,
    const std::string_view resourceName)
{
    slots_[ResolveVoiceSlot(slotIndex)].resourceName.assign(resourceName);
}

SoundSlotLayout SoundSlotManagerRuntime::BuildLayout(
    const std::array<std::uint32_t, 4>& counts)
{
    SoundSlotLayout layout;
    // 45D120：固定 BGM 槽后依次累加 Params GameSystem +76、+80、+72、+68。
    layout.firstBoundary = CheckedAdd(1u, counts[2]);
    layout.secondBoundary = CheckedAdd(layout.firstBoundary, counts[3]);
    layout.thirdBoundary = CheckedAdd(layout.secondBoundary, counts[1]);
    layout.totalCount = CheckedAdd(layout.thirdBoundary, counts[0]);
    return layout;
}

std::size_t SoundSlotManagerRuntime::ResolveEffectSlot(
    const ScrVmSoundSlotGroup group,
    const std::size_t groupIndex) const
{
    std::size_t base = 0;
    std::size_t count = 0;
    switch (group)
    {
    case ScrVmSoundSlotGroup::FirstAt50:
        base = layout_.firstBoundary;
        count = FirstSlotGroupCountAt50();
        break;
    case ScrVmSoundSlotGroup::SecondAt48:
        base = layout_.secondBoundary;
        count = SecondSlotGroupCountAt48();
        break;
    case ScrVmSoundSlotGroup::Remaining:
        base = layout_.thirdBoundary;
        count = layout_.totalCount - layout_.thirdBoundary;
        break;
    default:
        throw ScrVmSoundCommandException({});
    }
    if (groupIndex >= count)
        throw ScrVmSoundCommandException({});
    return base + groupIndex;
}

std::size_t SoundSlotManagerRuntime::ResolveMappedSlot(const std::size_t port) const
{
    if (port >= effectMappingState_.Size())
        throw ScrVmSoundCommandException({});
    return layout_.firstBoundary + port;
}

std::size_t SoundSlotManagerRuntime::ResolveVoiceSlot(const std::size_t slotIndex) const
{
    if (slotIndex >= VoiceSlotCapacity())
        throw ScrVmVoiceCommandException({});
    return 1u + slotIndex;
}

bool SoundSlotManagerRuntime::OpenResolvedSlot(
    const std::size_t absoluteIndex,
    const std::string_view resourceName,
    const std::int32_t playbackParameter,
    const std::uint8_t loop,
    const bool commitImmediately,
    const std::optional<const char*> rawResourceName,
    IArchiveResourceSource* source)
{
    // 第三组保存 4D1A8C 返回的原指针；各叶调用独立读首 NUL，回调后不沿用旧长度。
    // 416C08/40B530 与 410290 自身接受 null；仅在这些 CString 消费点转换为空串。
    const auto readName = [&] { return ReadSoundName(resourceName, rawResourceName); };
    // 原 Demo 已提供实际 source；getter/查询异常外传，不回退构造时固定的 resources_。
    // 有界旧接口仍属于既有现代适配，不替它伪造额外 root getter。
    const auto entry = [&]() -> std::optional<ResourceReadQuery>
    {
        if (source)
            return source->CaptureCatalogAt413BE0().ResolveForReadBorrowed(rawResourceName.value());
        auto fixed = resources_.Resolve(std::string(readName()));
        if (!fixed) return std::nullopt;
        return ResourceReadQuery(std::move(*fixed));
    }();
    // 42BC90 只接受 flags(+0x1C) 为零的原始 VFS 条目。
    if (!entry || entry->Flags() != 0)
        return false;
    // 42BD02..42BD23：VFS 拒绝发生在首次解引用槽之前。
    if (absoluteIndex >= slots_.size())
        throw std::out_of_range("sound slot index exceeds constructed layout");
    // 42BD29..42BD37：先缓存长度、偏移、句柄及物理路径，再允许名字转换回调。
    const auto length = entry->Length();
    const auto offset = entry->Offset();
    if (offset > std::numeric_limits<std::uint32_t>::max())
        throw std::overflow_error("sound archive offset exceeds LINK DWORD layout");

    Slot& slot = slots_[absoluteIndex];
    const auto handle = slot.handle;
    const char* const path = entry->Path();
    const std::wstring logicalName = ToWide(readName());
    const std::wstring physicalName = ToWide(path);
    if (!soundApi_.OpenSound(
            handle,
            logicalName.c_str(),
            physicalName.c_str(),
            static_cast<std::int32_t>(static_cast<std::uint32_t>(offset)),
            static_cast<std::int32_t>(length)))
    {
        return false;
    }

    soundApi_.SetSoundVolume(slot.handle, 100, playbackParameter);
    soundApi_.SetSoundLoop(slot.handle, loop);
    if (commitImmediately)
    {
        soundApi_.PlaySlot(slot.handle);
        slot.resourceName.assign(readName());
    }
    return true;
}

void SoundSlotManagerRuntime::StopAndClearSlot(const std::size_t absoluteIndex)
{
    if (absoluteIndex >= slots_.size())
        throw std::out_of_range("sound slot index exceeds constructed layout");
    Slot& slot = slots_[absoluteIndex];
    soundApi_.StopSlot(slot.handle);
    slot.resourceName.clear();
}

std::wstring SoundSlotManagerRuntime::ToWide(const std::string_view value)
{
    if (value.empty())
        return {};
    if (value.size() > static_cast<std::size_t>(std::numeric_limits<int>::max()))
        throw std::length_error("sound resource name exceeds Win32 conversion ABI");

    const int sourceLength = static_cast<int>(value.size());
    // 40B3C0 固定使用 CP_THREAD_ACP(3) 与 flags=0；LE 日区由线程代码页生效。
    const int required = MultiByteToWideChar(
        CP_THREAD_ACP, 0, value.data(), sourceLength, nullptr, 0);
    if (required <= 0)
        throw std::runtime_error("sound resource name conversion failed");

    std::wstring result(static_cast<std::size_t>(required), L'\0');
    if (MultiByteToWideChar(
            CP_THREAD_ACP, 0, value.data(), sourceLength,
            result.data(), required) != required)
    {
        throw std::runtime_error("sound resource name conversion failed");
    }
    return result;
}
}
