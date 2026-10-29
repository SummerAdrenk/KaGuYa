#include <Runtime/SoundCharacterGroupConfiguration.h>
#include <Runtime/SoundParamsEnvironmentArchive.h>
#include <Runtime/EnvironmentArchiveVectorRuntime.h>
#include <Runtime/RuntimeCStringVector.h>
#include <Runtime/ArchiveResourceNameRuntime.h>
#include "SoundSettingsTraversal.h"

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace SoundSettingsTraversal;
constexpr char Characters[] = "\x83\x4C\x83\x83\x83\x89\x83\x4E\x83\x5E\x95\xCA";
constexpr char Character[] = "\x83\x4C\x83\x83\x83\x89\x83\x4E\x83\x5E";
constexpr char Registration[] = "\x93\x6F\x98\x5E";
constexpr char Files[] = "\x83\x74\x83\x40\x83\x43\x83\x8B";
constexpr char Key[] = "\x83\x4C\x81\x5B";
constexpr char TestFiles[] = "\x83\x65\x83\x58\x83\x67\x83\x74\x83\x40\x83\x43\x83\x8B";

void ReadGroupFlag(SoundCharacterGroupOwner& owner, const Slot& section,
    std::uint8_t SoundCharacterGroupNative::* member, const char* name, ISettingTagErrorSource& errors)
{
    Body<SoundCharacterGroupNative>(owner.Native()).*member = 1;
    try
    {
        // 47218A/472204在getter之前缓存目标；getter改变根槽也不能改写另一正文。
        auto& target = Body<SoundCharacterGroupNative>(owner.Native());
        auto* error = errors.CaptureAt47BCF0();
        target.*member = ReadBool(error, section, name) ? 1 : 0;
    }
    catch (const MissingSettingsRoot&) { throw; }
    catch (...) {} // 已经写入的默认值/回调修改保持原样，不二次写默认。
}
void ReadCharacterFlag(Slot& local, const Slot& tag,
    std::uint8_t SoundCharacterNative::* member, const char* name, ISettingTagErrorSource& errors)
{
    Body<SoundCharacterNative>(local).*member = 1;
    try
    {
        const bool value = ReadBool(errors.CaptureAt47BCF0(), tag, name);
        // 47263A/4727DC：与组开关不同，内联布尔叶完成后才重新解析临时Ref。
        Body<SoundCharacterNative>(local).*member = value ? 1 : 0;
    }
    catch (const MissingSettingsRoot&) { throw; }
    catch (...) {}
}
void ReadRegistrationScalars(Slot& local, const Slot& tag, ISettingTagErrorSource& errors)
{
    Body<SoundSeExNative>(local).valueAt08 = 100; // 472C8C，在查询Volume之前写正文。
    try
    {
        const auto volume = ReadDword(errors.CaptureAt47BCF0(), tag, "Volume");
        Body<SoundSeExNative>(local).valueAt08 = volume;
    }
    catch (const MissingSettingsRoot&) { throw; }
    catch (...) {}
    Body<SoundSeExNative>(local).valueAt04 = 0;
    try
    {
        const bool usingSound = ReadBool(errors.CaptureAt47BCF0(), tag, "Using");
        Body<SoundSeExNative>(local).valueAt04 = usingSound ? 0 : 1;
    }
    catch (const MissingSettingsRoot&) { throw; }
    catch (...) {}
}
void ReadRegistrationFiles(Slot& local, const Slot& registration, ISettingTagErrorSource& errors,
    IRuntimeStringManagerSource& strings, IEnvironmentRootMemory& memory)
{
    {
        auto files = CaptureSettingChildAt477E90(errors.CaptureAt47BCF0(), registration, Files);
        SetRuntimeCStringAt437680(Body<SoundSeExNative>(local).textAt10, "", 0); // 473106在try外。
        try
        {
            auto file = CaptureSettingValueAt47D470(errors.CaptureAt47BCF0(), files.Native(), "File");
            AssignRuntimeCStringAt410220(Body<SoundSeExNative>(local).textAt10, file.Value());
        }
        catch (const MissingSettingsRoot&) { throw; }
        catch (...) {} // 4732CE不重清已经赋值的File。

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
            catch (...) {}
            const char* const extensionCharacters = extension.Value().GetString();

            ArchiveRawResourceNameRuntime file(ArchiveRawResourceNameRuntime::ProducerTag{},
                [&](RuntimeCString* out) { InitializeEmptyRuntimeCString(*out, strings); });
            SetRuntimeCStringAt437680(file.Value(), "", 0);
            try
            {
                auto text = CaptureSettingValueAt47D470(errors.CaptureAt47BCF0(), key.Native(), "File");
                AssignRuntimeCStringAt410220(file.Value(), text.Value());
            }
            catch (const MissingSettingsRoot&) { throw; }
            catch (...) {}
            const char* const fileCharacters = file.Value().GetString();
            auto* body = CreateSoundFileKeyAt47391A(value.Value().GetString(), extensionCharacters,
                fileCharacters, strings, memory);
            // 4739CB解析SE，再捕获其+14；不能把push 0误读成接收者为空。
            auto& keys = Body<SoundSeExNative>(local).fileKeysAt14;
            Slot fileRef;
            AdoptSoundNativeReference(fileRef, body, memory);
            SoundNativeTemporary fileUnwind(fileRef);
            AppendEnvironmentArchiveReferenceAt49852D(keys, fileRef, memory);
            fileUnwind.Finish(fileRef.controlAt08);
            AdvanceChild(key, files.Native(), cursor, Key, errors);
            // 473BF3/473C0B/473C2A：File的缓存字符、Ext的缓存字符、Value当前槽。
            file.ReleaseNormalCached(fileCharacters);
            extension.ReleaseNormalCached(extensionCharacters);
        }
    }
    Body<SoundSeExNative>(local).stateAt20 = 0;
    {
        auto tests = CaptureSettingChildAt477E90(errors.CaptureAt47BCF0(), registration, TestFiles);
        Slot* cursor = nullptr;
        auto test = FirstChild(errors.CaptureAt47BCF0(), tests.Native(), cursor, Files);
        while (test.Native().baseAt00)
        {
            {
                auto name = CaptureSettingValueAt47D470(errors.CaptureAt47BCF0(), test.Native(), "Name");
                AppendRuntimeCStringAt41D5E9(Body<SoundSeExNative>(local).stringsAt24, name.Value(), memory);
            }
            AdvanceChild(test, tests.Native(), cursor, Files, errors);
        }
    }
}
void AppendComplete(EnvironmentVectorStorage<Slot>& destination, const Slot& local,
    IEnvironmentRootMemory& memory)
{
    // 4741CA/4742B7：完整正文完成后复制Ref，再由vector增持。
    Slot copied = local;
    if (copied.controlAt08) ++copied.controlAt08->referenceCountAt04;
    SoundNativeTemporary copiedUnwind(copied);
    AppendEnvironmentArchiveReferenceAt49852D(destination, copied, memory);
    copiedUnwind.Finish(copied.controlAt08);
}
}

SoundCharacterGroupOwner::~SoundCharacterGroupOwner() noexcept { Reset(); }
void SoundCharacterGroupOwner::Reset() noexcept { ReleaseYtlReference(slot_); }
SoundCharacterGroupNative* SoundCharacterGroupOwner::Get() const noexcept
{ return ResolveEnvironmentObjectReference<SoundCharacterGroupNative>(slot_); }
void SoundCharacterGroupOwner::Serialize(SerializeBuffer& serializer)
{ Body<SoundCharacterGroupNative>(slot_).Serialize(serializer); } // 明确旧宿主协议，不跳过null。

void ConfigureSoundCharacterGroupAt471FB0(SoundCharacterGroupOwner& destination,
    ISoundParamsSettingsSource& source, ISettingTagErrorSource& errors,
    IRuntimeStringManagerSource& strings, IEnvironmentRootMemory& memory)
{
    SettingTagNodeCapture section;
    try
    {
        const auto& game = source.ReadGameSystem(SoundParamsSettingsReadPoint::Characters);
        section = CaptureSettingChildAt477E90(errors.CaptureAt47BCF0(), game.settingTags[0].Native(), Characters);
    }
    catch (const MissingSettingsRoot&) { throw; }
    catch (...) { return; } // FuncInfo5E7924仅states1..3允许跳过整个section。
    if (!section.Native().baseAt00) return;

    auto* group = CreateSoundCharacterGroupAt47209F(memory);
    ReplaceSoundGroupReference(destination.Native(), group, memory);
    ReadGroupFlag(destination, section.Native(), &SoundCharacterGroupNative::valueAt04, "Enabled", errors);
    ReadGroupFlag(destination, section.Native(), &SoundCharacterGroupNative::valueAt05, "Using", errors);
    auto* character = RequireNode(section.Native()).childrenAt10.begin;
    for (; character != RequireNode(section.Native()).childrenAt10.end; ++character)
    {
        if (!Same(RequireNode(*character).nameAt00, Character)) continue;
        auto* body = CreateSoundCharacterForSettingsAt4722D1(strings, memory);
        Slot local;
        AdoptSoundCharacterAt47230E(local, body, memory);
        SoundNativeTemporary unwind(local);
        {
            auto name = CaptureSettingValueAt47D470(errors.CaptureAt47BCF0(), *character, "Name");
            AssignRuntimeCStringAt410220(Body<SoundCharacterNative>(local).keyAt08, name.Value());
        }
        ReadCharacterFlag(local, *character, &SoundCharacterNative::valueAt04, "Using", errors);
        ReadCharacterFlag(local, *character, &SoundCharacterNative::valueAt05, "Display", errors);
        auto* registration = RequireNode(*character).childrenAt10.begin;
        for (; registration != RequireNode(*character).childrenAt10.end; ++registration)
        {
            if (!Same(RequireNode(*registration).nameAt00, Registration)) continue;
            auto* se = CreateSoundSeExForSettingsAt4728B3(strings, memory);
            Slot seRef;
            AdoptSoundSeExAt472916(seRef, se, memory);
            SoundNativeTemporary seUnwind(seRef);
            {
                auto name = CaptureSettingValueAt47D470(errors.CaptureAt47BCF0(), *registration, "Name");
                AssignRuntimeCStringAt410220(Body<SoundSeExNative>(seRef).keyAt0C, name.Value());
            }
            {
                auto master = CaptureSettingValueAt47D470(errors.CaptureAt47BCF0(), *registration, "Master");
                AssignRuntimeCStringAt410220(Body<SoundSeExNative>(seRef).textAt30, master.Value());
            }
            ReadRegistrationScalars(seRef, *registration, errors);
            ReadRegistrationFiles(seRef, *registration, errors, strings, memory);
            AppendComplete(Body<SoundCharacterNative>(local).parametersAt0C, seRef, memory);
            seUnwind.Finish(seRef.controlAt08);
        }
        AppendComplete(Body<SoundCharacterGroupNative>(destination.Native()).charactersAt08, local, memory);
        unwind.Finish(local.controlAt08);
    }
    section.ReleaseCached();
}
}
