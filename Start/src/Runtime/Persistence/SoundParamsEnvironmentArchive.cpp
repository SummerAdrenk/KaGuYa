#include <Runtime/SoundParamsEnvironmentArchive.h>
#include <Runtime/MessageWindowConfigExpressionParser.h>
#include <Runtime/ParamsRuntimeRoot.h>
#include <Runtime/SystemDataEnvironmentArchive.h>
#include <Runtime/SoundParameterNative.h>
#include <Runtime/ArchiveResourceNameRuntime.h>
#include <Runtime/SoundResourcePathMapping.h>
#include <Runtime/CategorizedSoundSettings.h>
#include <Runtime/GameBuildProfile.h>

#include <algorithm>
#include <bit>
#include <locale.h>
#include <mbstring.h>
#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
constexpr char MasterVolume[] = "\x83\x7D\x83\x58\x83\x5E\x81\x5B\x89\xB9\x97\xCA";

const ParamsSettingTagRuntime& RequireNode(const YtlReferenceSlotRuntime& slot)
{
    const auto* node = ResolveParamsSettingTagAt410560(slot);
    if (!node) throw MissingSettingsRoot{};
    return *node;
}


class FixedSoundParamsSettingsSource final : public ISoundParamsSettingsSource
{
public:
    explicit FixedSoundParamsSettingsSource(const ParamsGameSystem& gameSystem) noexcept : gameSystem_(gameSystem) {}
    const ParamsGameSystem& ReadGameSystem(SoundParamsSettingsReadPoint) override { return gameSystem_; }
private:
    const ParamsGameSystem& gameSystem_;
};

std::uint32_t ReadDword(RuntimeCString* errorCString, const YtlReferenceSlotRuntime& tag, const char* key)
{
    MessageWindowConfigExpressionParser parser;
    return static_cast<std::uint32_t>(ReadSettingNodeIntegerAt47BBE0(errorCString, tag, key, parser));
}

bool EqualResourceName(
    const std::string_view left,
    const std::string_view right)
{
    // _mbsicmp 与原实现一致地按当前线程代码页比较，但输入必须显式以 NUL 结尾。
    const std::string leftString(left);
    const std::string rightString(right);
    return _mbsicmp(
        reinterpret_cast<const unsigned char*>(leftString.c_str()),
        reinterpret_cast<const unsigned char*>(rightString.c_str())) == 0;
}


std::int32_t ScalePercent(const std::int32_t value, const std::uint32_t factor)
{
    const std::uint32_t product =
        static_cast<std::uint32_t>(value) * factor;
    return static_cast<std::int32_t>(product) / 100;
}

}

EnvironmentSoundParamsArchive::EnvironmentSoundParamsArchive(ISoundParamsSettingsSource& source,
    ISettingTagErrorSource& errors, IRuntimeStringManagerSource& strings) : namesAt10(strings)
{ ConfigureSoundParamsAt45D77F(*this, source, errors); }

EnvironmentSoundParamsArchive::~EnvironmentSoundParamsArchive() noexcept
{
    // 463FC6先恢复本层46槽，再按声明逆序释放；基类阶段切回三槽。
}

void EnvironmentSoundParamsArchive::Serialize(SerializeBuffer& serializer)
{
    // Start.exe:48B750。外层 type_params 同样按键回填，不以文件内容重建运行时表。
    typeParametersAt20.SerializeAt48B750(serializer);

    // 原对象内存顺序为 seByType(+2C)、seByChars(+38)，文件顺序恰好相反。
    seByCharsAt38.Serialize(serializer);
    seByTypeAt2C.Serialize(serializer);
}

void EnvironmentSoundParamsRuntime::MapPreviewResourceAtA0(RuntimeCString* result, const char* resourceName) const
{
    (void)archive_.MapResourceAtA0(result, resourceName);
}
std::int32_t EnvironmentSoundParamsRuntime::AddKeyValueAt08(const char* key, std::uint32_t kind,
    const YtlReferenceSlotRuntime& value)
{
    return archive_.AddKeyValueAt08(key, kind, value);
}

const std::uint32_t* EnvironmentSoundParamsRuntime::BorrowPlaybackParameterAt477D50() const
{
    return archive_.MusicVolumeAt14();
}

std::int32_t EnvironmentSoundParamsRuntime::ReadPlaybackParameterAt477D50() const
{
    return std::bit_cast<std::int32_t>(*BorrowPlaybackParameterAt477D50());
}

bool EnvironmentSoundParamsRuntime::IsMusicActiveAt47F4B0() const
{
    // 47F4BA..47F4D2：缺项 false；否则只判断 mute byte，不检查 DWORD 音量。
    return archive_.MusicActiveAt18();
}

bool SoundEffectOpenGateState::ShouldSuppressOpen(
    const ScrVmSoundSlotGroup,
    const std::size_t) const
{
    return ShouldSuppressRemainingOpen();
}

bool SoundEffectOpenGateState::ShouldSuppressRemainingOpen() const
{
    activityRecordsCreated = true;
    const bool hasActiveRecord = std::any_of(
        activityRecords.begin(), activityRecords.end(),
        [](const std::uint8_t value) { return value != 0; });
    const bool environmentGate =
        (modeAt14 & 3u) == 3u &&
        ((environmentFlagsAt38 & 1u) == 0u || !textWindowFlagAt9A);
    return (hasActiveRecord || environmentGate) &&
           (environmentFlagsAt38 & 0x20u) != 0u;
}

bool SoundFirstEffectOpenModeState::RestorePersistedValue(const std::size_t) const
{
    return restorePersistedValue;
}

EnvironmentSoundParamsRuntime::EnvironmentSoundParamsRuntime(
    const ParamsGameSystem& gameSystem,
    EnvironmentSoundParamsArchive& archive, ISettingTagErrorSource& errors)
    : archive_(archive)
{
    FixedSoundParamsSettingsSource source(gameSystem);
    InitializeSettings(source, errors);
}

EnvironmentSoundParamsRuntime::EnvironmentSoundParamsRuntime(
    ISoundParamsSettingsSource& source,
    EnvironmentSoundParamsArchive& archive, ISettingTagErrorSource& errors)
    : archive_(archive)
{
    InitializeSettings(source, errors);
}

const ParamsGameSystem& SoundParamsSettingsRootSource::ReadGameSystem(SoundParamsSettingsReadPoint)
{
    const auto& game = root_.GetOrCreateAt418480().GameSystem();
    if (!game) throw MissingSettingsRoot{};
    return *game;
}

void EnvironmentSoundParamsRuntime::InitializeSettings(
    ISoundParamsSettingsSource& source, ISettingTagErrorSource& errors)
{ ConfigureSoundParamsAt45D77F(archive_, source, errors); }

static void ConfigureGroupedSoundParams(EnvironmentSoundParamsArchive& archive,
    ISoundParamsSettingsSource& source, ISettingTagErrorSource& errors)
{
    // 三阶段共用各自唯一原生正文；45E770表达式对象链仍保留现有适配边界。
    // 45D5C0的四串已在archive中构造，不重建平行std::string查询键。

    // 46FDB0..46FDCD 先清主音量表；原三个入口均固定 GS+88，不比较根名称。
    archive.typeParametersAt20.clear();
    // 46FE24 无空根返回分支；由查找在 477ECB 首次读取成员处安全拒绝空正文。

    // 45D77F/45D786/45D78D：主音量、类型、人物三阶段不可交换。
    {
        SettingTagNodeCapture master;
        try
        {
            const auto& params = source.ReadGameSystem(SoundParamsSettingsReadPoint::MasterVolume);
            auto* errorCString = errors.CaptureAt47BCF0(); // 46FE0D
            master = CaptureSettingChildAt477E90(errorCString, params.settingTags[0].Native(), MasterVolume);
        }
        catch (const MissingSettingsRoot&) { throw; }
        catch (...) {} // 470303：仅选择 section 允许恢复。
        if (master.Get())
        {
            auto* item = RequireNode(master.Native()).childrenAt10.begin;
            for (; item != RequireNode(master.Native()).childrenAt10.end; ++item)
            {
                auto* state = CreateSoundTypeAt46FEBD();
                YtlReferenceSlotRuntime local;
                AdoptSoundNativeReference(local, state);
                auto* cached = local.controlAt08;
                SoundNativeTemporary unwind(local); // 46FF50：control建立后才有清理资格。
                if (!state) _invalid_parameter_noinfo_noreturn();
                AssignRuntimeCStringAt410220(state->keyAt0C, RequireNode(*item).nameAt00);
                // 470063..470112 无局部 fallback；缺 Value 不得制造零值项目。
                auto* errorCString = errors.CaptureAt47BCF0(); // 47004B 内联 getter 的返回读点。
                state->valueAt08 = ReadDword(errorCString, *item, "Value");
                state->valueAt04 = ReadSoundBoolOrDefault(errors, *item, "IsMute", false) ? 1 : 0;
                // 470244：额外临时Ref先增持，插入再增持，正常尾声使用各自缓存control。
                YtlReferenceSlotRuntime copied{local.baseAt00, 0, cached};
                if (cached) ++cached->referenceCountAt04;
                SoundNativeTemporary copiedUnwind(copied);
                archive.typeParametersAt20.AppendReferenceAt47025E(copied);
                copiedUnwind.Finish(copied.controlAt08);
                unwind.Finish(cached);
            }
        }
        master.ReleaseCached();
    }

    ConfigureSoundTypeGroupAt474740(archive.seByTypeAt2C, source, errors);

    ConfigureSoundCharacterGroupAt471FB0(archive.seByCharsAt38, source, errors);
}

void ConfigureSoundParamsAt45D77F(EnvironmentSoundParamsArchive& archive,
    ISoundParamsSettingsSource& source, ISettingTagErrorSource& errors)
{
    if (CompiledGameProfile().soundSettings == GameBuildProfile::SoundSettings::Grouped)
    {
        ConfigureGroupedSoundParams(archive, source, errors);
        return;
    }
    // 旧分类仅在构造边界转成统一模型；原params树、查询器和播放/序列化器不分叉。
    ParamsGameSystem adapted;
    adapted.settingTags[0].AssignSnapshot(AdaptCategorizedSoundSettings(
        source.ReadGameSystem(SoundParamsSettingsReadPoint::MasterVolume), errors));
    FixedSoundParamsSettingsSource groupedSource(adapted);
    ConfigureGroupedSoundParams(archive, groupedSource, errors);
}

const EnvironmentSoundParamsArchive& EnvironmentSoundParamsRuntime::ArchiveState() const noexcept
{
    return archive_;
}

std::string EnvironmentSoundParamsRuntime::VoiceTypeNameCp932() const
{
    return std::string(RuntimeStringView(archive_.namesAt10.voiceAt0C));
}


const SoundTypeNative* EnvironmentSoundParamsRuntime::FindType(
    const char* name) const
{
    // 47CEA0/47AEB0/46C2B0也调用47B0C0，不能与BGM使用不同的键比较规则。
    return archive_.typeParametersAt20.FindAt47B0C0(name);
}

const SoundTypeNative& EnvironmentSoundParamsRuntime::RequireType(
    const char* name) const
{
    const SoundTypeNative* value = FindType(name);
    if (value == nullptr)
        throw std::out_of_range("required sound type is missing");
    return *value;
}

bool EnvironmentSoundParamsRuntime::IsTypeEffectAllowed(const std::string_view resourceName) const
{
    const std::string terminated(resourceName);
    return archive_.TypeAllowedAt58(terminated.c_str());
}
bool EnvironmentSoundParamsArchive::TypeAllowedAt58(const char* resourceName)
{
    if (!IsSoundTypeAllowedAt46C570(seByTypeAt2C.Get(), resourceName)) return false;
    // 46C7AA..46C803只保护master查询；实际匹配项直接读，不再按Name二次找首项。
    try
    {
        const auto* group = seByTypeAt2C.Get();
        if (!group) throw std::invalid_argument("sound type group has no payload (safe boundary)");
        const auto* master = typeParametersAt20.FindAt47B0C0(group->masterAt04.GetString());
        if (!master) throw 1;
        return master->valueAt04 == 0;
    }
    catch (...) { return false; }
}

std::int32_t EnvironmentSoundParamsRuntime::ResolveTypeEffectValue(const std::string_view resourceName) const
{
    const std::string terminated(resourceName);
    return archive_.TypeResourceValueAt5C(terminated.c_str());
}
std::int32_t EnvironmentSoundParamsArchive::TypeResourceValueAt5C(const char* resourceName)
{
    YtlReferenceSlotRuntime selected;
    FindSoundTypeAt489AC0(selected, seByTypeAt2C.Get(), resourceName);
    auto* cached = selected.controlAt08;
    SoundNativeTemporary unwind(selected);
    std::int32_t result = -1;
    if (selected.baseAt00 && ResolveEnvironmentObjectReference<SoundSeNative>(selected)->valueAt04 == 0)
    {
        // 47DEEA..47DF4C：只有master读取/缩放属于catch；返回Ref在函数尾正常减持。
        try
        {
            const auto* group = seByTypeAt2C.Get();
            if (!group) throw std::invalid_argument("sound type group has no payload (safe boundary)");
            const auto* master = typeParametersAt20.FindAt47B0C0(group->masterAt04.GetString());
            if (!master) throw 1;
            const auto volume = std::bit_cast<std::int32_t>(master->valueAt08);
            if (volume >= 0)
                result = ScalePercent(volume, ResolveEnvironmentObjectReference<SoundSeNative>(selected)->valueAt08);
        }
        catch (...) {}
    }
    unwind.Finish(cached);
    return result;
}

SoundFirstEffectSlotPreparation EnvironmentSoundParamsRuntime::PrepareFirstEffectSlotAt50(
    const std::size_t,
    const std::string_view resourceName,
    const std::string_view currentSlotResourceName,
    const bool restorePersistedValue)
{
    if (!IsBatchObjectReadyAtA4() || !PrepareCharacterResource(resourceName))
        return {};
    if (EqualResourceName(resourceName, currentSlotResourceName))
        return {};
    std::int32_t value = ResolveCharacterResourceValue(resourceName);
    if (value < 0)
    {
        const SoundTypeNative* fallback;
        // 42C264..277 仅保护 +24 查询；42C28E 捕获全部异常并返回 AL=0。
        try { fallback = &RequireType(archive_.namesAt10.normalEffectAt04.GetString()); }
        catch (...) { return {}; }
        // 42C27E 已离开 try，读取返回对象及其后的分支不能纳入 catch。
        value = static_cast<std::int32_t>(fallback->valueAt08);
    }
    if (value < 0)
        return {};
    const std::string terminated(resourceName);
    ArchiveRawResourceNameRuntime returned(ArchiveRawResourceNameRuntime::ProducerTag{}, [&](RuntimeCString* out) {
        (void)archive_.ResourceCharacterNameAtB0(out, terminated.c_str());
    });
    std::string character(RuntimeStringView(returned.Value()));
    returned.ReleaseNormal();
    return {true, value, std::move(character), restorePersistedValue};
}

SoundEffectSlotPreparation EnvironmentSoundParamsRuntime::PrepareSecondEffectSlotAt48(
    const std::size_t,
    const std::string_view resourceName,
    const std::string_view currentSlotResourceName)
{
    try
    {
        const SoundTypeNative* loop = FindType(archive_.namesAt10.loopEffectAt08.GetString());
        if (loop != nullptr)
        {
            if (loop->valueAt04 != 0)
                return {};
        }
        else if (!IsTypeEffectAllowed(resourceName))
        {
            return {};
        }
        // 与带显式 fallback 的入口一致：空但非 null 的有界字符串允许比较。
        if (resourceName.data() == nullptr)
            throw ScrVmSoundCommandException({});
        if (EqualResourceName(resourceName, currentSlotResourceName))
            return {};

        std::int32_t value = loop != nullptr
            ? static_cast<std::int32_t>(loop->valueAt08)
            : ResolveTypeEffectValue(resourceName);
        if (value < 0)
        {
            value = loop != nullptr
                ? static_cast<std::int32_t>(loop->valueAt08)
                : static_cast<std::int32_t>(RequireType(archive_.namesAt10.normalEffectAt04.GetString()).valueAt08);
        }
        return {value >= 0, value};
    }
    catch (const std::out_of_range&)
    {
        throw ScrVmSoundCommandException({});
    }
}

SoundEffectSlotPreparation
EnvironmentSoundParamsRuntime::PrepareSecondEffectSlotAt48WithFallback(
    const std::size_t,
    const std::string_view resourceName,
    const std::string_view currentSlotResourceName,
    const std::int32_t fallbackValue)
{
    // 42C070 没有局部 catch；+28/+2C/+58/+30/+5C 查询异常直接外逸。
    const SoundTypeNative* loop = FindType(archive_.namesAt10.loopEffectAt08.GetString());
    if (loop != nullptr)
    {
        if (loop->valueAt04 != 0)
            return {};
    }
    else if (!IsTypeEffectAllowed(resourceName))
    {
        return {};
    }
    // 42C11C/42C11E 只拒绝 null；有效空串仍进入 42C124 的比较。
    if (resourceName.data() == nullptr)
        throw ScrVmSoundCommandException({});
    if (EqualResourceName(resourceName, currentSlotResourceName))
        return {};

    std::int32_t value = loop != nullptr
        ? static_cast<std::int32_t>(loop->valueAt08)
        : ResolveTypeEffectValue(resourceName);
    if (value < 0)
        value = fallbackValue;
    // 42C17C CMOVS 选 fallback 后直接进入 42BC90，不再拒绝负值。
    return {true, value};
}

SoundEffectSlotPreparation EnvironmentSoundParamsRuntime::PrepareRemainingEffectSlotWithFallback(
    const char* const resourceName, const std::int32_t fallbackValue)
{
    // 46C750 -> 46C570/46C5A3：只有实际进入 +58 的 CString 叶才将 null 转空。
    // +5C -> 489AF3 会再次读取原指针；不能把先前规范化的字符串缓存给它。
    if (!IsTypeEffectAllowed(resourceName ? std::string_view(resourceName) : std::string_view{}))
        return {};
    std::int32_t value = ResolveTypeEffectValue(
        resourceName ? std::string_view(resourceName) : std::string_view{});
    if (value < 0)
        value = fallbackValue;
    // 4D1B14 CMOVS 后不再拒绝负参数，也不重新读取 manager 的 normal 参数。
    return {true, value};
}

SoundEffectSlotPreparation EnvironmentSoundParamsRuntime::PrepareRemainingEffectSlot(
    const std::size_t,
    const std::string_view resourceName,
    const std::string_view)
{
    try
    {
        if (!IsTypeEffectAllowed(resourceName))
            return {};
        std::int32_t value = ResolveTypeEffectValue(resourceName);
        if (value < 0)
            value = static_cast<std::int32_t>(RequireType(archive_.namesAt10.normalEffectAt04.GetString()).valueAt08);
        return {value >= 0, value};
    }
    catch (const std::out_of_range&)
    {
        throw ScrVmSoundCommandException({});
    }
}

bool EnvironmentSoundParamsRuntime::IsBatchObjectReadyAtA4() const
{ return archive_.CharacterGroupReadyAtA4(); }
bool EnvironmentSoundParamsArchive::CharacterGroupReadyAtA4()
{
    if (!seByCharsAt38.Native().baseAt00) return false; // 46C266 -> 46C2A1。
    const auto* group = seByCharsAt38.Get();
    if (!group) throw std::invalid_argument("sound character group has no payload (safe boundary)");
    return group->valueAt04 != 0 && group->valueAt05 != 0;
}

bool EnvironmentSoundParamsRuntime::MatchesVoiceRelationAt46DD20(
    const std::string_view relation, const std::string_view voiceName)
{
    const std::string terminated(voiceName);
    YtlReferenceSlotRuntime selected;
    FindSoundCharacterAt489630(selected, archive_.seByCharsAt38.Get(), terminated.c_str());
    auto* cached = selected.controlAt08;
    SoundNativeTemporary unwind(selected);
    bool result = false;
    if (selected.baseAt00)
    {
        if (!relation.data()) throw ATL::CAtlException(E_FAIL); // 46DD8F只在首角色命中后拒绝null。
        const std::string other(relation);
        result = _mbscmp(reinterpret_cast<const unsigned char*>(
            ResolveEnvironmentObjectReference<SoundCharacterNative>(selected)->keyAt08.GetString()),
            reinterpret_cast<const unsigned char*>(other.c_str())) == 0;
    }
    unwind.Finish(cached);
    return result;
}

bool EnvironmentSoundParamsRuntime::PrepareResourceAtA8(
    const std::string_view resourceName)
{
    return PrepareCharacterResource(resourceName);
}

std::int32_t EnvironmentSoundParamsRuntime::ReadResourceValueAtAC(
    const std::string_view resourceName)
{
    return ResolveCharacterResourceValue(resourceName);
}

bool EnvironmentSoundParamsRuntime::PrepareCharacterResource(
    const std::string_view resourceName) const
{
    const std::string terminated(resourceName);
    return archive_.CharacterResourceAllowedAtA8(terminated.c_str());
}
bool EnvironmentSoundParamsArchive::CharacterResourceAllowedAtA8(const char* resourceName)
{
    ArchiveRawResourceNameRuntime master(ArchiveRawResourceNameRuntime::ProducerTag{},
        [](RuntimeCString* out) { InitializeEmptyRuntimeCString(*out, DefaultRuntimeStringManagerSource()); });
    if (!IsSoundCharacterAllowedAt46C2D0(seByCharsAt38.Get(), resourceName, &master.Value()))
    {
        master.ReleaseNormal();
        return false;
    }
    const char* cached = master.Value().GetString();
    bool allowed = false;
    try
    {
        const auto* type = typeParametersAt20.FindAt47B0C0(cached);
        if (!type) throw 1;
        allowed = type->valueAt04 == 0;
    }
    catch (...) { cached = master.Value().GetString(); } // 46C1F7仅保护Master查询。
    master.ReleaseNormalCached(cached);
    return allowed;
}

std::int32_t EnvironmentSoundParamsRuntime::ResolveCharacterResourceValue(
    const std::string_view resourceName) const
{
    const std::string terminated(resourceName);
    return archive_.CharacterResourceValueAtAC(terminated.c_str());
}
std::int32_t EnvironmentSoundParamsArchive::CharacterResourceValueAtAC(const char* resourceName)
{
    YtlReferenceSlotRuntime selected;
    FindSoundRegistrationAt489880(selected, seByCharsAt38.Get(), resourceName);
    auto* cached = selected.controlAt08;
    SoundNativeTemporary unwind(selected);
    std::int32_t result = -1;
    if (selected.baseAt00 && ResolveEnvironmentObjectReference<SoundSeExNative>(selected)->valueAt04 == 0)
    {
        try
        {
            const auto* type = typeParametersAt20.FindAt47B0C0(
                ResolveEnvironmentObjectReference<SoundSeExNative>(selected)->textAt30.GetString());
            if (!type) throw 1;
            const auto volume = std::bit_cast<std::int32_t>(type->valueAt08);
            if (volume >= 0)
                result = ScalePercent(volume, ResolveEnvironmentObjectReference<SoundSeExNative>(selected)->valueAt08);
        }
        catch (...) {} // 478DAA恢复-1，不外抛或重选同名注册。
    }
    unwind.Finish(cached);
    return result;
}

std::int32_t EnvironmentSoundParamsRuntime::ReadFallbackValueAt6C()
{
    try
    {
        return static_cast<std::int32_t>(RequireType(archive_.namesAt10.voiceAt0C.GetString()).valueAt08);
    }
    catch (const std::out_of_range&)
    {
        throw ScrVmVoiceCommandException({});
    }
}

bool EnvironmentSoundParamsRuntime::HasLoopEffectType() const
{
    return archive_.HasLoopEffectAt28();
}

std::int32_t EnvironmentSoundParamsRuntime::LoopEffectValueOrThrow() const
{
    return static_cast<std::int32_t>(*archive_.LoopEffectVolumeAt30());
}

std::int32_t EnvironmentSoundParamsRuntime::NormalEffectValueOrThrow() const
{
    return static_cast<std::int32_t>(*archive_.NormalEffectVolumeAt24());
}



}
