#include <Runtime/CategorizedSoundSettings.h>
#include <Runtime/SoundTypeGroupConfiguration.h>
#include "SoundSettingsTraversal.h"
#include <cstring>

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace SoundSettingsTraversal;
constexpr char Voice[] = "\x89\xB9\x90\xBA";
constexpr char Effects[] = "\x8C\xF8\x89\xCA\x89\xB9";
constexpr char Loop[] = "\x83\x8B\x81\x5B\x83\x76\x8C\xF8\x89\xCA\x89\xB9";
constexpr char PerFile[] = "\x83\x74\x83\x40\x83\x43\x83\x8B\x95\xCA";
constexpr char Setting[] = "\x90\xDD\x92\xE8";
constexpr char RegisteredName[] = "\x93\x6F\x98\x5E\x96\xBC";
constexpr char FileKeyNames[] = "\x83\x74\x83\x40\x83\x43\x83\x8B\x83\x4C\x81\x5B\x96\xBC";
constexpr char Registration[] = "\x93\x6F\x98\x5E";
constexpr char Files[] = "\x83\x74\x83\x40\x83\x43\x83\x8B";
constexpr char TestFiles[] = "\x83\x65\x83\x58\x83\x67\x83\x74\x83\x40\x83\x43\x83\x8B";
constexpr char ModernKey[] = "\x83\x4C\x81\x5B";
constexpr char Type[] = "\x83\x5E\x83\x43\x83\x76";
constexpr char Character[] = "\x83\x4C\x83\x83\x83\x89\x83\x4E\x83\x5E";

ParamsSettingTag Node(std::string name)
{
    ParamsSettingTag result;
    result.name.cp932 = std::move(name);
    return result;
}
void Attribute(ParamsSettingTag& node, std::string key, std::string value)
{
    node.pairs.push_back({{{}, std::move(key)}, {{}, std::move(value)}});
}
std::string Text(const RuntimeCString& value)
{ return {value.GetString(), static_cast<std::size_t>(value.GetLength())}; }
const Slot* Child(const Slot& node, const char* name)
{
    const auto& parent = RequireNode(node);
    for (auto* it = parent.childrenAt10.begin; it != parent.childrenAt10.end; ++it)
        if (Same(RequireNode(*it).nameAt00, name)) return it;
    return nullptr;
}
SettingTagNodeCapture RequiredChild(const Slot& node, const char* name, ISettingTagErrorSource& errors)
{ return CaptureSettingChildAt477E90(errors.CaptureAt47BCF0(), node, name); }
std::string Value(const Slot& node, const char* name, ISettingTagErrorSource& errors)
{
    auto value = CaptureSettingValueAt47D470(errors.CaptureAt47BCF0(), node, name);
    return Text(value.Value());
}
std::uint32_t Volume(const Slot& node, ISettingTagErrorSource& errors)
{
    try { return ReadDword(errors.CaptureAt47BCF0(), node, "Value"); }
    catch (const MissingSettingsRoot&) { throw; }
    catch (...) { return 100; } // 45A0E0：分类及子项 Value 缺失时为100。
}
const char* Boolean(bool value) { return value ? "true" : "false"; }

ParamsSettingTag AdaptEntry(const Slot& entry, const char* kind, const char* category,
    ISettingTagErrorSource& errors, bool voice)
{
    auto name = RequiredChild(entry, RegisteredName, errors);
    auto result = Node(kind);
    Attribute(result, "Name", voice ? category : Value(name.Native(), "Name", errors));
    Attribute(result, "Volume", std::to_string(Volume(name.Native(), errors)));
    const bool mute = ReadSoundBoolOrDefault(errors, name.Native(), "IsMute", false);
    Attribute(result, voice ? "Using" : "IsMute", Boolean(voice ? !mute : mute));
    if (voice) Attribute(result, "Master", category);

    std::string file;
    try { file = Value(name.Native(), "File", errors); }
    catch (const MissingSettingsRoot&) { throw; }
    catch (...) {} // 原注册名 File 是可选属性。
    // 45A0E0：File是旧扩展名替换表达式，原样交给分类试听适配器，不能当路径前缀。
    auto files = Node(Files);
    Attribute(files, "File", file);
    auto keys = RequiredChild(entry, FileKeyNames, errors);
    for (auto* it = RequireNode(keys.Native()).childrenAt10.begin;
         it != RequireNode(keys.Native()).childrenAt10.end; ++it)
    {
        if (!Same(RequireNode(*it).nameAt00, Registration)) continue;
        auto key = Node(ModernKey);
        Attribute(key, "Value", Value(*it, "Key", errors));
        files.children.push_back(std::move(key));
    }
    result.children.push_back(std::move(files));
    auto tests = Node(TestFiles);
    auto oldTests = RequiredChild(entry, TestFiles, errors);
    for (auto* it = RequireNode(oldTests.Native()).childrenAt10.begin;
         it != RequireNode(oldTests.Native()).childrenAt10.end; ++it)
    {
        if (!Same(RequireNode(*it).nameAt00, Files)) continue;
        auto test = Node(Files);
        Attribute(test, "Name", Value(*it, "Name", errors));
        tests.children.push_back(std::move(test));
    }
    result.children.push_back(std::move(tests));
    return result;
}
}

ParamsSettingTag AdaptCategorizedSoundSettings(const ParamsGameSystem& game, ISettingTagErrorSource& errors)
{
    const auto& root = game.settingTags[0].Native();
    auto result = Node(Text(RequireNode(root).nameAt00));
    auto master = Node("\x83\x7D\x83\x58\x83\x5E\x81\x5B\x89\xB9\x97\xCA");
    auto characters = Node("\x83\x4C\x83\x83\x83\x89\x83\x4E\x83\x5E\x95\xCA");
    auto types = Node("\x83\x5E\x83\x43\x83\x76\x95\xCA\x8C\xF8\x89\xCA\x89\xB9");
    Attribute(characters, "Enabled", Boolean(Child(root, Voice) != nullptr));
    Attribute(characters, "Using", "true");
    Attribute(types, "Master", Effects);
    for (const auto* category : {"BGM", Effects, Loop, Voice})
    {
        const auto* section = Child(root, category);
        if (!section) continue; // 原4583C0/45A0E0允许缺分类，不制造Loop条目。
        auto parameter = Node(category);
        const bool music = std::strcmp(category, "BGM") == 0;
        if (music && ReadSoundBoolOrDefault(errors, *section, "IsMidi", false))
            throw std::invalid_argument("categorized MIDI playback is not implemented");
        Attribute(parameter, "Value", std::to_string(music
            ? ReadDword(errors.CaptureAt47BCF0(), *section, "Value") : Volume(*section, errors)));
        Attribute(parameter, "IsMute", Boolean(ReadSoundBoolOrDefault(errors, *section, "IsMute", false)));
        master.children.push_back(std::move(parameter));
        if (music) continue;
        const auto* perFile = Child(*section, PerFile);
        if (!perFile) continue;
        const bool voice = std::strcmp(category, Voice) == 0;
        if (!voice && std::strcmp(category, Effects) != 0)
            throw std::invalid_argument("categorized loop-effect registrations require a separate master binding");
        for (auto* it = RequireNode(*perFile).childrenAt10.begin;
             it != RequireNode(*perFile).childrenAt10.end; ++it)
        {
            if (!Same(RequireNode(*it).nameAt00, Setting)) continue;
            auto entry = AdaptEntry(*it, voice ? Registration : Type, category, errors, voice);
            if (voice)
            {
                auto name = RequiredChild(*it, RegisteredName, errors);
                auto character = Node(Character);
                Attribute(character, "Name", Value(name.Native(), "Name", errors));
                Attribute(character, "Using", "true");
                Attribute(character, "Display", Boolean(ReadSoundBoolOrDefault(errors, name.Native(), "IsDisp", true)));
                character.children.push_back(std::move(entry));
                characters.children.push_back(std::move(character));
            }
            else types.children.push_back(std::move(entry));
        }
    }
    result.children.push_back(std::move(master));
    result.children.push_back(std::move(characters));
    result.children.push_back(std::move(types));
    return result;
}
}
