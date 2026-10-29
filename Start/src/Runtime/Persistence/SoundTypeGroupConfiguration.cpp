#include <Runtime/SoundTypeGroupConfiguration.h>
#include <Runtime/SoundParamsEnvironmentArchive.h>
#include <Runtime/MessageWindowConfigExpressionParser.h>
#include <Runtime/EnvironmentArchiveVectorRuntime.h>
#include <Runtime/RuntimeCStringVector.h>
#include <Runtime/ArchiveResourceNameRuntime.h>
#include <mbstring.h>
#include "SoundSettingsTraversal.h"

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace SoundSettingsTraversal;
constexpr char TypeEffects[] = "\x83\x5E\x83\x43\x83\x76\x95\xCA\x8C\xF8\x89\xCA\x89\xB9";
constexpr char Type[] = "\x83\x5E\x83\x43\x83\x76";
constexpr char Files[] = "\x83\x74\x83\x40\x83\x43\x83\x8B";
constexpr char Key[] = "\x83\x4C\x81\x5B";
constexpr char TestFiles[] = "\x83\x65\x83\x58\x83\x67\x83\x74\x83\x40\x83\x43\x83\x8B";
struct RawGroupCleanup
{
    void* raw;
    IEnvironmentRootMemory& memory;
    ~RawGroupCleanup() noexcept { if (raw) memory.DeleteBody(raw, 0x14); }
};
}
bool SoundSettingsTraversal::ReadBool(RuntimeCString* errorCString, const YtlReferenceSlotRuntime& tag, const char* key)
{
    auto value = CaptureSettingValueAt47D470(errorCString, tag, key);
    LowerRuntimeCStringAt481690(value.Value());
    const auto* bytes = reinterpret_cast<const unsigned char*>(value.Value().GetString());
    if (_mbscmp(bytes, reinterpret_cast<const unsigned char*>("true")) == 0)
        return true;
    if (_mbscmp(bytes, reinterpret_cast<const unsigned char*>("false")) == 0)
        return false;
    // 470395/477E54：重新解tag并写入同一个error receiver，PE5B2524含两个%s。
    constexpr char NotBool[] = "%s \x83\x5E\x83\x4F\x82\xCC %s \x91\xAE\x90\xAB\x82\xCD"
        "Bool\x82\xC5\x82\xCD\x82\xA0\x82\xE8\x82\xDC\x82\xB9\x82\xF1\x81\x49";
    FormatRuntimeCStringAt417130(*errorCString, NotBool, RequireNode(tag).nameAt00.GetString(), key);
    throw const_cast<char*>("error !");
}

bool ReadSoundBoolOrDefault(ISettingTagErrorSource& errors,
    const YtlReferenceSlotRuntime& tag, const char* key, bool fallback)
{
    // 每个布尔字段各有一次原内联/47BCF0 Capture，且属于该字段的恢复范围。
    try
    {
        auto* errorCString = errors.CaptureAt47BCF0();
        return ReadBool(errorCString, tag, key);
    }
    catch (const MissingSettingsRoot&) { throw; }
    catch (...) { return fallback; }
}

std::uint32_t SoundSettingsTraversal::ReadDword(RuntimeCString* errorCString, const YtlReferenceSlotRuntime& tag, const char* key)
{
    MessageWindowConfigExpressionParser parser;
    return static_cast<std::uint32_t>(ReadSettingNodeIntegerAt47BBE0(errorCString, tag, key, parser));
}

std::uint32_t ReadSoundVolumeOrDefault(ISettingTagErrorSource& errors, const YtlReferenceSlotRuntime& tag)
{
    // 474C51/474DAF使用局部默认100；人物472C8C先写正文，不能套用此包装。
    try
    {
        auto* errorCString = errors.CaptureAt47BCF0();
        return ReadDword(errorCString, tag, "Volume");
    }
    catch (const MissingSettingsRoot&) { throw; }
    catch (...) { return 100; }
}


SoundTypeGroupOwner::~SoundTypeGroupOwner() noexcept { Reset(); }
void SoundTypeGroupOwner::Reset() noexcept { ReleaseYtlReference(slot_); }
SoundTypeGroupNative* SoundTypeGroupOwner::Get() const noexcept
{ return ResolveEnvironmentObjectReference<SoundTypeGroupNative>(slot_); }
void SoundTypeGroupOwner::Serialize(SerializeBuffer& serializer)
{
    // 48B9EC无null跳过分支；缺分组明确拒绝，不能伪造空表到存档中。
    Body<SoundTypeGroupNative>(slot_).Serialize(serializer);
}

void ConfigureSoundTypeGroupAt474740(SoundTypeGroupOwner& destination,
    ISoundParamsSettingsSource& source, ISettingTagErrorSource& errors,
    IRuntimeStringManagerSource& strings, IEnvironmentRootMemory& memory)
{
    SettingTagNodeCapture section;
    try
    {
        const auto& game = source.ReadGameSystem(SoundParamsSettingsReadPoint::TypeEffects);
        auto* error = errors.CaptureAt47BCF0();
        section = CaptureSettingChildAt477E90(error, game.settingTags[0].Native(), TypeEffects);
    }
    catch (const MissingSettingsRoot&) { throw; }
    catch (...) { return; } // 5E73D0: states1..3，仅捕获section查询。
    if (!section.Native().baseAt00) return;

    auto* raw = memory.AllocateBody(0x14); // 47482F在Master查询前分配。
    if (raw)
    {
        RawGroupCleanup cleanup{raw, memory};
        auto master = CaptureSettingValueAt47D470(errors.CaptureAt47BCF0(), section.Native(), "Master");
        auto* body = InitializeSoundTypeGroupAt474877(raw, master.Value().GetString(), strings);
        cleanup.raw = nullptr; // 正文构造完成，control失败没有raw回收资格。
        ReplaceSoundGroupReference(destination.Native(), body, memory);
        // 474988发布新group后才释放Master返回临时。
    }
    else ReplaceSoundGroupReference(destination.Native(), static_cast<SoundTypeGroupNative*>(nullptr), memory);

    auto* type = RequireNode(section.Native()).childrenAt10.begin;
    for (; type != RequireNode(section.Native()).childrenAt10.end; ++type)
    {
        if (!Same(RequireNode(*type).nameAt00, Type)) continue;
        auto* se = CreateSoundSeForSettingsAt474A2C(strings, memory);
        Slot local;
        AdoptSoundSeAt474A47(local, se, memory);
        SoundNativeTemporary unwind(local);
        {
            auto name = CaptureSettingValueAt47D470(errors.CaptureAt47BCF0(), *type, "Name");
            AssignRuntimeCStringAt410220(Body<SoundSeNative>(local).keyAt0C, name.Value());
        }
        Body<SoundSeNative>(local).valueAt08 = ReadSoundVolumeOrDefault(errors, *type);
        Body<SoundSeNative>(local).valueAt04 = ReadSoundBoolOrDefault(errors, *type, "IsMute", false) ? 1 : 0;

        {
            auto files = CaptureSettingChildAt477E90(errors.CaptureAt47BCF0(), *type, Files);
            try
            {
                auto file = CaptureSettingValueAt47D470(errors.CaptureAt47BCF0(), files.Native(), "File");
                AssignRuntimeCStringAt410220(Body<SoundSeNative>(local).textAt10, file.Value());
            }
            catch (const MissingSettingsRoot&) { throw; }
            catch (...) { SetRuntimeCStringAt437680(Body<SoundSeNative>(local).textAt10, "", 0); }

            Slot* cursor = nullptr;
            auto key = FirstChild(errors.CaptureAt47BCF0(), files.Native(), cursor, Key);
            while (key.Native().baseAt00)
            {
                auto value = CaptureSettingValueAt47D470(errors.CaptureAt47BCF0(), key.Native(), "Value");
                ArchiveRawResourceNameRuntime extension(ArchiveRawResourceNameRuntime::ProducerTag{},
                    [&](RuntimeCString* out) { InitializeEmptyRuntimeCString(*out, strings); });
                SetRuntimeCStringAt437680(extension.Value(), "", 0);
                try
                {
                    auto ext = CaptureSettingValueAt47D470(errors.CaptureAt47BCF0(), key.Native(), "Ext");
                    AssignRuntimeCStringAt410220(extension.Value(), ext.Value());
                }
                catch (const MissingSettingsRoot&) { throw; }
                catch (...) {} // 4754AC保留当前extension，不强制清除已赋值内容。

                const char* const extensionCharacters = extension.Value().GetString(); // 475486/4754B8的ESI。
                auto* fileKey = CreateSoundFileKeyAt47391A(value.Value().GetString(),
                    extensionCharacters, nullptr, strings, memory);
                // 4755F0在control分配前捕获目标vector，不能在回调后重新选择目标。
                auto& keys = Body<SoundSeNative>(local).fileKeysAt14;
                Slot fileReference;
                AdoptSoundNativeReference(fileReference, fileKey, memory);
                auto* cached = fileReference.controlAt08;
                SoundNativeTemporary fileUnwind(fileReference);
                AppendEnvironmentArchiveReferenceAt49852D(keys, fileReference, memory);
                fileUnwind.Finish(cached);
                AdvanceChild(key, files.Native(), cursor, Key, errors);
                // 47581E/475836：先释放extension，再释放Value；选中下一个key已完成。
                extension.ReleaseNormalCached(extensionCharacters);
            }
        }

        Body<SoundSeNative>(local).stateAt20 = 0;
        {
            auto tests = CaptureSettingChildAt477E90(errors.CaptureAt47BCF0(), *type, TestFiles);
            Slot* cursor = nullptr;
            auto test = FirstChild(errors.CaptureAt47BCF0(), tests.Native(), cursor, Files);
            while (test.Native().baseAt00)
            {
                {
                    auto name = CaptureSettingValueAt47D470(errors.CaptureAt47BCF0(), test.Native(), "Name");
                    // 456A10与4085C0追加分支同构：1.5倍扩容/Clone/COW/32-byte对齐。
                    AppendRuntimeCStringAt41D5E9(Body<SoundSeNative>(local).stringsAt24, name.Value(), memory);
                }
                AdvanceChild(test, tests.Native(), cursor, Files, errors);
            }
        }

        // 475CB8先捕获目的vector；两次增持与两次释放各自保留，完整SE才提交。
        auto& parameters = Body<SoundTypeGroupNative>(destination.Native()).parametersAt08;
        Slot copied = local;
        if (copied.controlAt08) ++copied.controlAt08->referenceCountAt04;
        SoundNativeTemporary copiedUnwind(copied);
        AppendEnvironmentArchiveReferenceAt49852D(parameters, copied, memory);
        copiedUnwind.Finish(copied.controlAt08);
        unwind.Finish(local.controlAt08);
    }
}

namespace
{
void NormalizeTypeResource(ArchiveRawResourceNameRuntime& owner)
{
    auto& text = owner.Value();
    LowerRuntimeCStringAt481690(text);
    if (text.GetLength() < 0) return;
    const auto* start = text.GetString();
    const auto* found = _mbsstr(reinterpret_cast<const unsigned char*>(start),
        reinterpret_cast<const unsigned char*>(":\\"));
    if (!found) return;
    const auto offset = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(found) -
        reinterpret_cast<std::uintptr_t>(start));
    if (offset < 0) return;
    auto tail = ArchiveResourceNameMidAt420C80(text, offset + 2, text.GetLength() - offset - 2);
    AssignRuntimeCStringAt410220(text, tail.Value());
    tail.ReleaseNormal();
}
const Slot* MatchTypeResource(SoundTypeGroupNative* group, RuntimeCString& text, bool prefixOnly)
{
    if (!group) throw std::invalid_argument("sound native group has no payload (safe boundary)");
    auto* chars = reinterpret_cast<const unsigned char*>(text.GetString());
    for (auto* type = group->parametersAt08.begin; type != group->parametersAt08.end; ++type)
    {
        auto* key = Body<SoundSeNative>(*type).fileKeysAt14.begin;
        for (; key != Body<SoundSeNative>(*type).fileKeysAt14.end; ++key)
        {
            const auto* prefix = Body<SoundFileKeyNative>(*key).keyAt04.GetString();
            if (!prefix || reinterpret_cast<const ATL::CStringData*>(chars)[-1].nDataLength < 0) continue;
            const auto* found = _mbsstr(chars, reinterpret_cast<const unsigned char*>(prefix));
            chars = reinterpret_cast<const unsigned char*>(text.GetString());
            if (!found) continue;
            const auto offset = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(found) -
                reinterpret_cast<std::uintptr_t>(chars));
            if (prefixOnly ? offset == 0 : offset >= 0) return type;
        }
    }
    return nullptr;
}
}
bool IsSoundTypeAllowedAt46C570(SoundTypeGroupNative* group, const char* resourceName)
{
    auto name = ArchiveResourceNameAt40B530(resourceName);
    NormalizeTypeResource(name);
    const auto* found = MatchTypeResource(group, name.Value(), false);
    const bool result = found && Body<SoundSeNative>(*found).valueAt04 == 0;
    name.ReleaseNormal();
    return result;
}
void FindSoundTypeAt489AC0(Slot& result, SoundTypeGroupNative* group, const char* resourceName)
{
    auto name = ArchiveResourceNameAt40B530(resourceName);
    NormalizeTypeResource(name);
    const auto* found = MatchTypeResource(group, name.Value(), true);
    result = found ? *found : Slot{};
    if (result.controlAt08) ++result.controlAt08->referenceCountAt04;
    name.ReleaseNormal();
}
}
