#pragma once
#include <Runtime/ScriptSoundLeafSource.h>

#include <Runtime/ResourceCatalog.h>
#include <Runtime/RuntimeCStringArray.h>
#include <Runtime/ScrVmMusicHandler.h>
#include <Runtime/ScrVmSoundHandler.h>
#include <Runtime/ScrVmVoiceHandler.h>
#include <Runtime/SoundRuntimeApi.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace KaGuYa::Start::Runtime
{
class IArchiveResourceSource;

class ISoundPlaybackControl
{
public:
    virtual ~ISoundPlaybackControl() = default;
    [[nodiscard]] virtual std::int32_t PlaybackParameter() const = 0;
    [[nodiscard]] virtual bool Active() const = 0;
};

struct SoundEffectSlotPreparation final
{
    bool shouldOpen = false;
    std::int32_t playbackParameter = 0;
};

struct SoundFirstEffectSlotPreparation final
{
    bool shouldOpen = false;
    std::int32_t resolvedParameter = 0;
    std::string relation;
    bool useAlternateParameter = false;
};

struct SoundFirstEffectLowerVolumeState final
{
    std::size_t absoluteSlotIndex = 0;
    std::int32_t alternateParameter = 0;
    // 45D3C3..45D3CB 未写入 +08；保持未定义，仅实际读取时明确失败。
    std::optional<std::int32_t> resolvedParameter;
    std::string relation;
};

// 对应 Environment::CSoundParams 的声音参数查询。三个音效组必须保留各自的调用顺序。
class ISoundEffectPlaybackControl
{
public:
    virtual ~ISoundEffectPlaybackControl() = default;
    [[nodiscard]] virtual SoundFirstEffectSlotPreparation PrepareFirstEffectSlotAt50(
        std::size_t groupIndex,
        std::string_view resourceName,
        std::string_view currentSlotResourceName,
        bool restorePersistedValue) = 0;
    [[nodiscard]] virtual SoundEffectSlotPreparation PrepareSecondEffectSlotAt48(
        std::size_t groupIndex,
        std::string_view resourceName,
        std::string_view currentSlotResourceName) = 0;
    [[nodiscard]] virtual SoundEffectSlotPreparation
    PrepareSecondEffectSlotAt48WithFallback(
        std::size_t groupIndex,
        std::string_view resourceName,
        std::string_view currentSlotResourceName,
        std::int32_t fallbackValue) = 0;
    [[nodiscard]] virtual SoundEffectSlotPreparation PrepareRemainingEffectSlot(
        std::size_t groupIndex,
        std::string_view resourceName,
        std::string_view currentSlotResourceName) = 0;
    // 4D1AD7/+58、4D1B0A/+5C；fallback 已由另一 Env.slot1 receiver 提前读取。
    [[nodiscard]] virtual SoundEffectSlotPreparation PrepareRemainingEffectSlotWithFallback(
        const char* resourceName, std::int32_t fallbackValue) = 0;
};

// 46D62E..46DB6B 只读取 CUnSaveData 的映射并恢复专用槽，
// 不经过脚本命令使用的 TextWin gate，也不反向写入映射表。
class ISoundMappingRestoreSlotBoundary
{
public:
    virtual ~ISoundMappingRestoreSlotBoundary() = default;
    [[nodiscard]] virtual std::size_t FirstMappedEffectCount() const = 0;
    [[nodiscard]] virtual bool AnyVoiceSlotPlaying() const = 0;
    [[nodiscard]] virtual ScrVmSoundOpenResult RestoreFirstMappedEffect(
        std::size_t groupIndex,
        std::string_view resourceName,
        bool anyVoiceSlotPlaying) = 0;
    [[nodiscard]] virtual ScrVmSoundOpenResult RestoreSecondMappedEffect(
        std::size_t groupIndex,
        std::string_view resourceName,
        std::int32_t fallbackValue) = 0;
};

// 4D3B20 属于 TextWin 与 Environment 状态，不属于 CSoundParams。
class ISoundEffectOpenGate
{
public:
    virtual ~ISoundEffectOpenGate() = default;
    [[nodiscard]] virtual bool ShouldSuppressRemainingOpen() const = 0;
    [[nodiscard]] virtual bool ShouldSuppressOpen(
        ScrVmSoundSlotGroup group,
        std::size_t groupIndex) const = 0;
};

// 4196C0 的结果决定第一组音效是否恢复槽内持久化值。
class ISoundFirstEffectOpenModeSource
{
public:
    virtual ~ISoundFirstEffectOpenModeSource() = default;
    [[nodiscard]] virtual bool RestorePersistedValue(
        std::size_t groupIndex) const = 0;
};

// 4D6640/4D62A0 使用的资源名映射实际属于 Environment::CUnSaveData+0x30，
// 不属于 CSoundManager。单独抽出该所有权，后续状态恢复可复用同一实例。
class ISoundEffectMappingState
{
public:
    virtual ~ISoundEffectMappingState() = default;
    [[nodiscard]] virtual std::size_t Size() const = 0;
    [[nodiscard]] virtual std::string_view ResourceNameAt(std::size_t port) const = 0;
    virtual void ClearAll() = 0;
    virtual void Clear(std::size_t port) = 0;
    virtual void Store(std::size_t port, std::string_view resourceName) = 0;
    // 固定映射对象的实现；晚取适配器须先捕获实际对象，再调用此方法。
    virtual void StorePatternName(std::size_t port,
        ScrResourceNameReceiver receiver, std::int32_t resourceId);
};

class UnsavedSoundEffectMappingState final : public ISoundEffectMappingState
{
public:
    explicit UnsavedSoundEffectMappingState(std::size_t slotCount);

    [[nodiscard]] std::size_t Size() const noexcept override;
    [[nodiscard]] std::string_view ResourceNameAt(std::size_t port) const override;
    void ClearAll() override;
    void Clear(std::size_t port) override;
    void Store(std::size_t port, std::string_view resourceName) override;

private:
    std::vector<std::string> resourceNames_;
};

// 对应 42C4F0 使用的控制对象 +A4/+A8/+AC/+6C 虚调用。
class IVoicePlaybackControl
{
public:
    virtual ~IVoicePlaybackControl() = default;
    [[nodiscard]] virtual bool IsBatchObjectReadyAtA4() const = 0;
    [[nodiscard]] virtual bool PrepareResourceAtA8(std::string_view resourceName) = 0;
    [[nodiscard]] virtual std::int32_t ReadResourceValueAtAC(
        std::string_view resourceName) = 0;
    [[nodiscard]] virtual std::int32_t ReadFallbackValueAt6C() = 0;
};

// 419470 调用 +B4 时先压声音名、再压关系串；现代参数次序为关系、声音名。
class IVoiceRelationLookup
{
public:
    virtual ~IVoiceRelationLookup() = default;
    [[nodiscard]] virtual bool MatchesVoiceRelationAt46DD20(
        std::string_view relation,
        std::string_view voiceName) = 0;
};

struct SoundSlotLayout final
{
    std::size_t currentMusicSlot = 0;
    std::size_t firstBoundary = 0;
    std::size_t secondBoundary = 0;
    std::size_t thirdBoundary = 0;
    std::size_t totalCount = 0;
};

using SoundDefaultDeviceId = std::array<std::byte, 16>;

// 42C573 -> 418480后读取当前GameSystem+4Ch，不是manager构造时的物理槽边界。
class ISoundVoiceBatchCountSource
{
public:
    virtual ~ISoundVoiceBatchCountSource() = default;
    [[nodiscard]] virtual std::uint32_t ReadVoiceBatchCountAt42C599() const = 0;
};

// 与 manager 在 45D1DA 固定的 +24 分离；捕获不读值、不延长原对象寿命。
class ISoundEffectParameterSource
{
public:
    virtual ~ISoundEffectParameterSource() = default;
    [[nodiscard]] virtual SoundMappingParameterReceiver
        CaptureEffectParametersAt1C() const = 0;
};

struct SoundSlotManagerDependencies final
{
    // 非空 owner 用于保活 music/gate 等具体适配器，使其寿命不依赖构造 source。
    // 不要求拥有全部服务：Sound API、资源目录等进程服务可借用，由外层 root
    // 保证它们及适配器借用的其他对象活到 manager 析构结束。
    // 禁止为满足保活契约反向持有拥有此 manager 的 root，避免形成所有权环。
    std::shared_ptr<const void> lifetime;
    ISoundRuntimeApi& soundApi;
    const ResourceCatalog& resources;
    ISoundPlaybackControl& musicControl;
    ISoundEffectOpenGate& effectOpenGate;
    ISoundFirstEffectOpenModeSource& firstEffectOpenMode;
    ISoundEffectMappingState& effectMappingState;
    // 须由lifetime或进程根保活；不得借用仅在构造期间存在的source本体。
    ISoundVoiceBatchCountSource& voiceBatchCount;
    ISoundEffectParameterSource& effectParameters;
};

class ISoundSlotManagerConstructionSource
{
public:
    virtual ~ISoundSlotManagerConstructionSource() = default;
    // 仅绑定依赖；不得在此提前读取 GameSystem、设置 focus 或查询策略、设备 ID。
    [[nodiscard]] virtual SoundSlotManagerDependencies Dependencies() = 0;
    // 45D1DA：持有实际 +24 SoundParams 及其底层 archive，可使用别名共享 owner。
    // 同一实际对象必须同时实现 IVoicePlaybackControl，不能另配一份语音状态。
    [[nodiscard]] virtual std::shared_ptr<ISoundEffectPlaybackControl>
        AcquireSoundParamsAt45D1DA() = 0;
    // 45D1F4..45D28C：晚取 Sound root；仅引用不存在时跳过，不以 !Initialized() 门控。
    // 引用存在但缺少导出时必须失败；调用后忽略导出返回值。
    virtual void SetGlobalFocusAt45D28C(std::uint8_t enabled) = 0;
    // 每次调用均独立解析当前 Params/GameSystem，包括 42CA60 语音循环的终止检查。
    // 索引为 [0..3]，对应 +44..+50 的四个 DWORD。
    [[nodiscard]] virtual std::uint32_t ReadSlotCountAt45D120(std::size_t index) = 0;
    // 每次 AddSound 前独立解析当前 Sound；nullptr 仅表示原 root 引用不存在（句柄为 0）。
    // 缺少导出必须报错；返回的 API 必须存活至紧接着的调用结束。
    [[nodiscard]] virtual ISoundRuntimeApi* ReadSoundApiAt42CA60() = 0;
    // 再次解析当前 Params/GameSystem：+54 为 DWORD，不能截成 bool 或单字节。
    [[nodiscard]] virtual std::uint32_t ReadPolicyAt45D4DC() = 0;
    // 晚取 Sound root 并调用 GetDefaultDeviceID，返回前复制完整 16 字节。
    // root 引用不存在、缺少导出或 GUID 指针为空时必须失败，不能返回全零兜底。
    [[nodiscard]] virtual SoundDefaultDeviceId ReadDefaultDeviceIdAt45D57A() = 0;
};

// Start.exe:45D120 的 CSoundManager 生产替代：同一槽数组承载
// BGM、语音、两组专用音效和公共音效，边界顺序严格按原构造函数建立。
class SoundSlotManagerRuntime final :
    public IScrVmMusicRuntimeBoundary,
    public IScrVmSoundRuntimeBoundary,
    public IScrVmVoiceRuntimeBoundary,
    public ISoundMappingRestoreSlotBoundary,
    public IScriptSoundLeafManager
{
public:
    explicit SoundSlotManagerRuntime(ISoundSlotManagerConstructionSource& source);
    // 受控固定输入入口：借用依赖，仅使用一次固定数量快照，不重新读取 root。
    // 不初始化全局 focus、策略或设备 ID；所有依赖须活到本对象析构结束。
    // 句柄由 Sound 拥有，本对象不负责回收。
    SoundSlotManagerRuntime(
        ISoundRuntimeApi& soundApi,
        const ResourceCatalog& resources,
        const std::array<std::uint32_t, 4>& slotCounts,
        ISoundPlaybackControl& musicControl,
        ISoundEffectPlaybackControl& effectControl,
        ISoundEffectOpenGate& effectOpenGate,
        ISoundFirstEffectOpenModeSource& firstEffectOpenMode,
        ISoundEffectMappingState& effectMappingState,
        IVoicePlaybackControl& voiceControl);
    ~SoundSlotManagerRuntime() override;

    SoundSlotManagerRuntime(const SoundSlotManagerRuntime&) = delete;
    SoundSlotManagerRuntime& operator=(const SoundSlotManagerRuntime&) = delete;

    [[nodiscard]] const SoundSlotLayout& Layout() const noexcept;
    // 固定输入入口未初始化这两个字段，通过这里读取时明确失败。
    [[nodiscard]] std::uint32_t PolicyAt3C() const;
    [[nodiscard]] IVoiceRelationLookup& VoiceRelationsAt24();
    [[nodiscard]] const SoundDefaultDeviceId& DefaultDeviceIdAt40() const;
    [[nodiscard]] const SoundFirstEffectLowerVolumeState& FirstEffectLowerVolumeStateAt(
        std::size_t groupIndex) const;
    void SetFirstEffectAlternateParameter(
        std::size_t groupIndex,
        std::int32_t value);

    [[nodiscard]] std::int32_t ReadPlaybackParameter() const override;
    [[nodiscard]] bool HasPlaybackSlots() const override;
    [[nodiscard]] bool IsCurrentSlotActive() const override;
    [[nodiscard]] const char* CurrentSlotResourceName() const override;
    void SwitchCurrentSlot(
        std::string_view resourceName,
        std::int32_t playbackParameter) override;
    void StopMusicSlots() override;

    // 绝对槽号，不是映射端口；查询真实 SoundIsPlay，停止后只清槽内名称。
    [[nodiscard]] bool IsAbsoluteSlotPlayingAt419370(std::uint32_t absoluteSlot) const;
    void StopAbsoluteSlotAt4383B0(std::uint32_t absoluteSlot);

    // 生产 Demo 的实际 CString 输出与原始名称；caller 自己按4BF5A0顺序捕获环境及参数。
    void StopDemoMusicAt4C3500(RuntimeCString& environmentMusicName);
    [[nodiscard]] bool PlayDemoMusicAt4BCCC0(RuntimeCString& environmentMusicName,
        const char* resourceName, std::int32_t playbackParameter, std::int32_t loop,
        IArchiveResourceSource& resources);
    [[nodiscard]] bool PlayDemoVoiceAt4BCE10(const char* resourceName,
        IArchiveResourceSource& resources);

    // 显式现代工具入口，不代表原 Demo 环境/root 取值链。
    void ExecuteDemoSound(
        std::uint8_t mode,
        const std::string& resourceName,
        std::string& environmentMusicName);

    [[nodiscard]] std::size_t FirstSlotGroupCountAt50() const override;
    [[nodiscard]] std::size_t SecondSlotGroupCountAt48() const override;
    void StopAndClearAllEffectSlots() override;
    void StopEffectsKeepingMappingsAt413270() override;
    void ClearMappedResourceNameAtPort(std::size_t port) override;
    void StopAndClearMappedSlotAtPort(std::size_t port) override;
    void StopAndClearRemainingSlot(std::size_t groupIndex) override;
    void StoreMappedResourceNameAtPort(
        std::size_t port,
        std::string_view resourceName) override;
    void StoreMappedPatternNameAtPort(std::size_t port,
        ScrResourceNameReceiver receiver, std::int32_t resourceId) override;
    [[nodiscard]] ScrVmSoundOpenResult OpenMappedSlotAtPort(
        ScrVmSoundSlotGroup group, std::size_t groupIndex, std::size_t port) override;
    [[nodiscard]] SoundMappingParameterReceiver
        CaptureEffectParametersAt1C() const override;
    [[nodiscard]] ScrVmSoundOpenResult OpenRemainingSlot(
        std::size_t groupIndex, const char* resourceName, std::int32_t fallbackValue) override;
    [[nodiscard]] bool ShouldSkipRemainingOpeningAt4D196A() const override;
    [[nodiscard]] bool ShouldSkipOpening(
        ScrVmSoundSlotGroup group,
        std::size_t groupIndex) const override;
    [[nodiscard]] ScrVmSoundOpenResult OpenAndPlaySlot(
        ScrVmSoundSlotGroup group,
        std::size_t groupIndex,
        std::string_view resourceName) override;

    [[nodiscard]] std::size_t FirstMappedEffectCount() const override;
    [[nodiscard]] bool AnyVoiceSlotPlaying() const override;
    // 返回 0：无播放；1：有播放但关系未命中；3：首个关系命中后立即返回。
    [[nodiscard]] std::uint32_t ReadVoiceRelationMaskAt419470(
        std::string_view relation,
        IVoiceRelationLookup& relations) const;
    // 显式策略重载保留给受控调用；source 构造的实例也可使用无显式策略的重载，
    // 后者读取 45D4DC 保存的完整 DWORD。
    void AdjustFirstEffectsAt4C2410(std::uint32_t policy, IVoiceRelationLookup& relations);
    void RefreshFirstEffectsAt433F60(std::uint32_t policy, IVoiceRelationLookup& relations);
    void AdjustFirstEffectsAt4C2410(IVoiceRelationLookup& relations);
    void RefreshFirstEffectsAt433F60(IVoiceRelationLookup& relations);
    [[nodiscard]] ScrVmSoundOpenResult RestoreFirstMappedEffectRaw(
        std::size_t groupIndex, const char* resourceName, bool anyVoiceSlotPlaying) override;
    [[nodiscard]] ScrVmSoundOpenResult RestoreSecondMappedEffectRaw(
        std::size_t groupIndex, const char* resourceName, std::int32_t fallbackValue) override;
    [[nodiscard]] ScrVmSoundOpenResult RestoreFirstMappedEffect(
        std::size_t groupIndex,
        std::string_view resourceName,
        bool anyVoiceSlotPlaying) override;
    [[nodiscard]] ScrVmSoundOpenResult RestoreSecondMappedEffect(
        std::size_t groupIndex,
        std::string_view resourceName,
        std::int32_t fallbackValue) override;

    [[nodiscard]] bool IsBatchObjectReadyAtA4() const override;
    // 状态数组先于 Stop 分配；两遍提交，只有 +6C 查询有局部 catch。
    [[nodiscard]] bool PlayVoiceBatchAt42C4F0(const UnSaveDataStringArrayReference& names);
    [[nodiscard]] std::int32_t VoiceBatchCapacityAt42C599() const override;
    // 仅用于物理索引的现代安全校验，不代替42C599当前配置门槛。
    [[nodiscard]] std::size_t VoiceSlotCapacity() const override;
    void StopAndClearAllVoiceSlots() override;
    [[nodiscard]] bool PrepareSlotAtA8(
        std::size_t slotIndex,
        std::string_view resourceName) override;
    [[nodiscard]] std::int32_t ReadSlotValueAtAC(
        std::size_t slotIndex,
        std::string_view resourceName) override;
    [[nodiscard]] std::int32_t ReadFallbackValueAt6C(
        std::size_t slotIndex,
        std::string_view resourceName) override;
    [[nodiscard]] bool OpenSlot(
        std::size_t slotIndex,
        std::string_view resourceName,
        std::int32_t resolvedValue) override;
    void PlayVoiceSlot(std::size_t slotIndex) override;
    void StoreVoiceSlotResourceName(
        std::size_t slotIndex,
        std::string_view resourceName) override;

private:
    SoundSlotManagerRuntime(ISoundSlotManagerConstructionSource& source,
        SoundSlotManagerDependencies dependencies);
    struct Slot final
    {
        SoundRuntimeHandle handle = 0;
        std::string resourceName;
    };

    struct LowerVolumeTable final
    {
        ~LowerVolumeTable();
        void Clear() noexcept;
        std::vector<std::shared_ptr<SoundFirstEffectLowerVolumeState>> entries;
    };
    void ConstructLowerVolumeTable();

    [[nodiscard]] static SoundSlotLayout BuildLayout(
        const std::array<std::uint32_t, 4>& counts);
    [[nodiscard]] static std::wstring ToWide(std::string_view value);
    [[nodiscard]] std::size_t ResolveEffectSlot(
        ScrVmSoundSlotGroup group,
        std::size_t groupIndex) const;
    [[nodiscard]] std::size_t ResolveMappedSlot(std::size_t port) const;
    [[nodiscard]] std::size_t ResolveVoiceSlot(std::size_t slotIndex) const;
    [[nodiscard]] ScrVmSoundOpenResult RestoreFirstMappedEffectImpl(
        std::size_t groupIndex, std::string_view resourceName, bool anyVoiceSlotPlaying,
        std::optional<const char*> rawResourceName);
    [[nodiscard]] ScrVmSoundOpenResult RestoreSecondMappedEffectImpl(
        std::size_t groupIndex, std::string_view resourceName, std::int32_t fallbackValue,
        std::optional<const char*> rawResourceName);
    [[nodiscard]] bool OpenResolvedSlot(
        std::size_t absoluteIndex,
        std::string_view resourceName,
        std::int32_t playbackParameter,
        std::uint8_t loop,
        bool commitImmediately,
        std::optional<const char*> rawResourceName = std::nullopt,
        IArchiveResourceSource* source = nullptr);
    void StopAndClearSlot(std::size_t absoluteIndex);
    [[nodiscard]] bool PlayDemoVoice(const std::string& resourceName);

    std::shared_ptr<const void> dependenciesLifetime_;
    ISoundRuntimeApi& soundApi_;
    const ResourceCatalog& resources_;
    ISoundPlaybackControl& musicControl_;
    ISoundEffectOpenGate& effectOpenGate_;
    ISoundFirstEffectOpenModeSource& firstEffectOpenMode_;
    ISoundEffectMappingState& effectMappingState_;
    // 生产构造绑定必需source；受控固定入口单独发布fixed值，不以layout兜底。
    const ISoundVoiceBatchCountSource* voiceBatchCountSource_ = nullptr;
    const ISoundEffectParameterSource* effectParameterSource_ = nullptr;
    std::optional<std::uint32_t> fixedVoiceBatchCount_;
    SoundSlotLayout layout_;
    std::vector<Slot> slots_;
    // 45D120 EH 状态 2 -> 1 -> 0：依次清理 lowerVolume、SoundParams、槽数组。
    std::shared_ptr<ISoundEffectPlaybackControl> soundParamsLifetime_;
    ISoundEffectPlaybackControl& effectControl_;
    IVoicePlaybackControl& voiceControl_;
    LowerVolumeTable firstEffectLowerVolumeStates_;
    std::optional<std::uint32_t> policy_;
    std::optional<SoundDefaultDeviceId> defaultDeviceId_;
};
}
