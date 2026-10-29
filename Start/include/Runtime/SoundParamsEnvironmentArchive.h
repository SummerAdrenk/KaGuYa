#pragma once

#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/ParamsRuntimeProfile.h>
#include <Runtime/SoundSlotManagerRuntime.h>
#include <Runtime/SettingTagErrorRuntime.h>
#include <Runtime/SoundMasterTableNative.h>
#include <Runtime/SoundTypeGroupConfiguration.h>
#include <Runtime/SoundCharacterGroupConfiguration.h>
#include <Runtime/SoundKeyValueNative.h>
#include <Runtime/SoundParamsNamesNative.h>

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace KaGuYa::Start::Runtime
{
class ISoundParamsSettingsSource;

class EnvironmentSoundParamsArchive : public IEnvironmentSoundParamsArchive
{
public:
    // 显式空表入口保留给夹具/读取模型；生产owner使用完整三阶段构造。
    explicit EnvironmentSoundParamsArchive(
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource()) : namesAt10(strings) {}
    EnvironmentSoundParamsArchive(ISoundParamsSettingsSource& source, ISettingTagErrorSource& errors,
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
    ~EnvironmentSoundParamsArchive() noexcept;
    void Serialize(SerializeBuffer& serializer);
    void SerializeNative18(void* serializer18) override;
    void* DeleteAtVslot04(std::uint32_t flags) noexcept override;

    // 5B2730：基类00/04/08之后严格按原地址次序追加43槽，无虚析构。
    virtual bool HasMusicAt0C();
    virtual std::uint8_t* MusicMuteAt10();
    virtual std::uint32_t* MusicVolumeAt14();
    virtual bool MusicActiveAt18();
    virtual bool HasNormalEffectAt1C();
    virtual std::uint8_t* NormalEffectMuteAt20();
    virtual std::uint32_t* NormalEffectVolumeAt24();
    virtual bool HasLoopEffectAt28();
    virtual std::uint8_t* LoopEffectMuteAt2C();
    virtual std::uint32_t* LoopEffectVolumeAt30();
    virtual bool HasTypeGroupAt34();
    virtual RuntimeCString* TypeMasterAt38(RuntimeCString* out);
    virtual std::int32_t TypeCountAt3C();
    virtual SoundSeNative* TypeAt40(std::int32_t index);
    virtual RuntimeCString* TypeNameAt44(RuntimeCString* out, std::int32_t index);
    virtual std::uint8_t* TypeMuteAt48(std::int32_t index);
    virtual std::uint32_t* TypeVolumeAt4C(std::int32_t index);
    virtual std::uint32_t* TypeStateAt50(std::int32_t index);
    virtual EnvironmentVectorStorage<RuntimeCString>* TypeFilesAt54(std::int32_t index);
    virtual bool TypeAllowedAt58(const char* name);
    virtual std::int32_t TypeResourceValueAt5C(const char* name);
    virtual bool VoiceArrayAllowedAt60(const UnSaveDataStringArrayReference& names);
    virtual bool HasVoiceAt64();
    virtual std::uint8_t* VoiceMuteAt68();
    virtual std::uint32_t* VoiceVolumeAt6C();
    virtual bool CharacterGroupEnabledAt70();
    virtual std::uint8_t* CharacterGroupUsingAt74();
    virtual std::int32_t CharacterCountAt78();
    virtual std::uint8_t* CharacterUsingAt7C(std::int32_t index);
    virtual RuntimeCString* CharacterNameAt80(RuntimeCString* out, std::int32_t index);
    virtual std::uint8_t* CharacterDisplayAt84(std::int32_t index);
    virtual SoundSeExNative* RegistrationAt88(std::int32_t index, const char* name);
    virtual std::uint32_t* RegistrationVolumeAt8C(std::int32_t index, const char* name);
    virtual std::uint8_t* RegistrationMuteAt90(std::int32_t index, const char* name);
    virtual std::uint32_t* RegistrationStateAt94(std::int32_t index, const char* name);
    virtual EnvironmentVectorStorage<RuntimeCString>* RegistrationFilesAt98(std::int32_t index, const char* name);
    virtual RuntimeCString* RegistrationMasterAt9C(RuntimeCString* out, std::int32_t index, const char* name);
    virtual RuntimeCString* MapResourceAtA0(RuntimeCString* out, const char* name);
    virtual bool CharacterGroupReadyAtA4();
    virtual bool CharacterResourceAllowedAtA8(const char* name);
    virtual std::int32_t CharacterResourceValueAtAC(const char* name);
    virtual RuntimeCString* ResourceCharacterNameAtB0(RuntimeCString* out, const char* name);
    virtual bool MatchesVoiceRelationAtB4(const char* relation, const char* name);

    // KeyValue由10h三槽基类拥有；48B750只序列化20/38/2C，不混入基类注册表。
    SoundParamsNamesNative namesAt10;
    SoundMasterTableNative typeParametersAt20;
    SoundTypeGroupOwner seByTypeAt2C;
    SoundCharacterGroupOwner seByCharsAt38;
};

// 44h唯一正文；业务适配器在正文之外，不改变46槽或偏移。
static_assert(sizeof(EnvironmentSoundParamsArchive) == 0x44);
static_assert(offsetof(EnvironmentSoundParamsArchive, namesAt10) == 0x10);
static_assert(offsetof(EnvironmentSoundParamsArchive, typeParametersAt20) == 0x20);
static_assert(offsetof(EnvironmentSoundParamsArchive, seByTypeAt2C) == 0x2C);
static_assert(offsetof(EnvironmentSoundParamsArchive, seByCharsAt38) == 0x38);

void ConfigureSoundParamsAt45D77F(EnvironmentSoundParamsArchive& archive,
    ISoundParamsSettingsSource& source, ISettingTagErrorSource& errors);


// 4D3B20 使用的 TextWin/Environment 状态。记录表保持惰性创建副作用。
class SoundEffectOpenGateState final : public ISoundEffectOpenGate
{
public:
    [[nodiscard]] bool ShouldSuppressRemainingOpen() const override;
    [[nodiscard]] bool ShouldSuppressOpen(
        ScrVmSoundSlotGroup group,
        std::size_t groupIndex) const override;

    std::uint8_t environmentFlagsAt38 = 0;
    std::uint8_t modeAt14 = 0;
    bool textWindowFlagAt9A = false;
    mutable bool activityRecordsCreated = false;
    std::vector<std::uint8_t> activityRecords;
};

class SoundFirstEffectOpenModeState final : public ISoundFirstEffectOpenModeSource
{
public:
    [[nodiscard]] bool RestorePersistedValue(std::size_t groupIndex) const override;
    bool restorePersistedValue = false;
};

enum class SoundParamsSettingsReadPoint : std::uint32_t
{
    MasterVolume = 0x46FDE7,
    TypeEffects = 0x474789,
    Characters = 0x471FF9,
};

class ISoundParamsSettingsSource
{
public:
    virtual ~ISoundParamsSettingsSource() = default;
    [[nodiscard]] virtual const ParamsGameSystem& ReadGameSystem(SoundParamsSettingsReadPoint point) = 0;
};

class ParamsRuntimeRootOwner;
class SoundParamsSettingsRootSource final : public ISoundParamsSettingsSource
{
public:
    explicit SoundParamsSettingsRootSource(ParamsRuntimeRootOwner& root) noexcept : root_(root) {}
    [[nodiscard]] const ParamsGameSystem& ReadGameSystem(SoundParamsSettingsReadPoint point) override;
private:
    ParamsRuntimeRootOwner& root_;
};

// Environment::CSoundParams 的生产行为对象。持久化字段仍由 archive 独立承载。
class EnvironmentSoundParamsRuntime final :
    public ISoundEffectPlaybackControl,
    public IVoicePlaybackControl,
    public ISoundMappingRestoreParameterSource,
    public IVoiceRelationLookup
{
public:
    struct BorrowConstructedTag {};
    EnvironmentSoundParamsRuntime(BorrowConstructedTag, EnvironmentSoundParamsArchive& archive) noexcept : archive_(archive) {}
    // 固定输入用于显式工具/夹具；生产使用下面的逐阶段取根入口。
    EnvironmentSoundParamsRuntime(
        const ParamsGameSystem& gameSystem,
        EnvironmentSoundParamsArchive& archive,
        ISettingTagErrorSource& errors = ProcessSettingTagErrorOwner());
    EnvironmentSoundParamsRuntime(
        ISoundParamsSettingsSource& source,
        EnvironmentSoundParamsArchive& archive,
        ISettingTagErrorSource& errors = ProcessSettingTagErrorOwner());

    // BGM 原 vslot +14/+18：读取 DWORD 原位值（缺项抛 int 1），存在且 mute==0 才 active。
    // 477D50 返回真实字段地址；调用方必须保证容器及所选条目在借用期间存活。
    [[nodiscard]] const std::uint32_t* BorrowPlaybackParameterAt477D50() const;
    // 供原调用点立即解引用的消费者使用，不能代替 Demo 跨 Environment getter 的借用。
    [[nodiscard]] std::int32_t ReadPlaybackParameterAt477D50() const;
    [[nodiscard]] bool IsMusicActiveAt47F4B0() const;

    // +A0/476A80：设置页试听使用，输出是尚未构造的CString隐藏返回槽。
    void MapPreviewResourceAtA0(RuntimeCString* result, const char* resourceName) const;
    std::int32_t AddKeyValueAt08(const char* key, std::uint32_t kind, const YtlReferenceSlotRuntime& value);

    [[nodiscard]] SoundFirstEffectSlotPreparation PrepareFirstEffectSlotAt50(
        std::size_t groupIndex,
        std::string_view resourceName,
        std::string_view currentSlotResourceName,
        bool restorePersistedValue) override;
    [[nodiscard]] SoundEffectSlotPreparation PrepareSecondEffectSlotAt48(
        std::size_t groupIndex,
        std::string_view resourceName,
        std::string_view currentSlotResourceName) override;
    [[nodiscard]] SoundEffectSlotPreparation PrepareSecondEffectSlotAt48WithFallback(
        std::size_t groupIndex,
        std::string_view resourceName,
        std::string_view currentSlotResourceName,
        std::int32_t fallbackValue) override;
    [[nodiscard]] SoundEffectSlotPreparation PrepareRemainingEffectSlot(
        std::size_t groupIndex,
        std::string_view resourceName,
        std::string_view currentSlotResourceName) override;
    [[nodiscard]] SoundEffectSlotPreparation PrepareRemainingEffectSlotWithFallback(
        const char* resourceName, std::int32_t fallbackValue) override;

    [[nodiscard]] bool IsBatchObjectReadyAtA4() const override;
    [[nodiscard]] bool PrepareResourceAtA8(std::string_view resourceName) override;
    [[nodiscard]] std::int32_t ReadResourceValueAtAC(
        std::string_view resourceName) override;
    [[nodiscard]] std::int32_t ReadFallbackValueAt6C() override;

    // +60：按原顺序检查声音数组及首匹配注册项；只读同一个实际参数 owner。
    [[nodiscard]] bool IsVoiceArrayAllowedAt46C830(
        const UnSaveDataStringArrayReference& names) const;

    // +B4：按原 prefix 顺序找首角色，再区分大小写比较角色 key/relation；不看静音开关。
    [[nodiscard]] bool MatchesVoiceRelationAt46DD20(
        std::string_view relation, std::string_view voiceName) override;

    [[nodiscard]] bool HasLoopEffectType() const override;
    [[nodiscard]] std::int32_t LoopEffectValueOrThrow() const override;
    [[nodiscard]] std::int32_t NormalEffectValueOrThrow() const override;

    // 419290 -> CSoundParams vslot +60/46C830 的只读状态源；返回实际 owner 的借用。
    [[nodiscard]] const EnvironmentSoundParamsArchive& ArchiveState() const noexcept;
    // 显式快照只供有界字符串调用方；正式查询直接借用namesAt10中的CString。
    [[nodiscard]] std::string VoiceTypeNameCp932() const;

private:
    void InitializeSettings(ISoundParamsSettingsSource& source, ISettingTagErrorSource& errors);

    [[nodiscard]] const SoundTypeNative& RequireType(const char* name) const;
    [[nodiscard]] const SoundTypeNative* FindType(const char* name) const;
    [[nodiscard]] std::int32_t ResolveTypeEffectValue(std::string_view resourceName) const;
    [[nodiscard]] bool IsTypeEffectAllowed(std::string_view resourceName) const;
    [[nodiscard]] bool PrepareCharacterResource(std::string_view resourceName) const;
    [[nodiscard]] std::int32_t ResolveCharacterResourceValue(
        std::string_view resourceName) const;

    EnvironmentSoundParamsArchive& archive_;
};
}
