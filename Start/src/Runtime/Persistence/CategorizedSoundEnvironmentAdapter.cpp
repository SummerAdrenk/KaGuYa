#include <Runtime/CategorizedSoundEnvironmentAdapter.h>
#include <Runtime/SoundParamsEnvironmentArchive.h>
#include "SoundSettingsTraversal.h"

#include <stdexcept>
#include <string_view>
#include <typeinfo>

namespace KaGuYa::Start::Runtime
{
namespace
{
namespace Wire = CategorizedSoundEnvironmentWire;
using namespace SoundSettingsTraversal;
constexpr char Effects[] = "\x8C\xF8\x89\xCA\x89\xB9";
constexpr char Loop[] = "\x83\x8B\x81\x5B\x83\x76\x8C\xF8\x89\xCA\x89\xB9";
constexpr char Voice[] = "\x89\xB9\x90\xBA";
constexpr char PerFile[] = "\x83\x74\x83\x40\x83\x43\x83\x8B\x95\xCA";
constexpr char Setting[] = "\x90\xDD\x92\xE8";
constexpr char RegisteredName[] = "\x93\x6F\x98\x5E\x96\xBC";

[[noreturn]] void Reject(const std::string& reason)
{ throw std::invalid_argument("categorized sound environment: " + reason); }

std::string_view Bytes(const RuntimeCString& text)
{ return {text.GetString(), static_cast<std::size_t>(text.GetLength())}; }

const Slot* Child(const Slot& parent, const char* name, std::size_t& index)
{
    index = 0;
    const auto& children = RequireNode(parent).childrenAt10;
    for (auto* it = children.begin; it != children.end; ++it, ++index)
        if (Same(RequireNode(*it).nameAt00, name)) return it;
    return nullptr;
}

std::size_t Count(const EnvironmentVectorStorage<Slot>& table)
{
    // 合法容器只检查已构造形状；空/残缺表明确拒绝，不触发原空正文危险域。
    if (!table.begin)
    {
        if (table.end || table.capacity) Reject("incomplete reference table");
        return 0;
    }
    return static_cast<std::size_t>(table.end - table.begin);
}

template<class T> T& Exact(const Slot& slot, const std::string& location)
{
    auto* base = ResolveEnvironmentObjectReference<IEnvironmentArchiveRuntime>(slot);
    if (!base || typeid(*base) != typeid(T)) Reject(location + ": missing or wrong native type");
    return *static_cast<T*>(base);
}

void Name(const RuntimeCString& actual, std::string_view expected, const std::string& location)
{
    if (Bytes(actual) != expected) Reject(location + ": CP932 name/master mismatch");
}

void CheckWire(Wire::Result result, const char* operation)
{
    if (!result)
        throw std::runtime_error(std::string("categorized sound environment ") + operation +
            ": error=" + std::to_string(static_cast<int>(result.error)) +
            " cursor=" + std::to_string(result.cursor) + " offset=" + std::to_string(result.offset) +
            " entry=" + std::to_string(result.entry) + " child=" + std::to_string(result.child) +
            " field=" + std::to_string(static_cast<int>(result.field)));
}
}

CategorizedSoundEnvironmentAdapter::CategorizedSoundEnvironmentAdapter(
    const ParamsGameSystem& game, EnvironmentSoundParamsArchive& archive, ISettingTagErrorSource& errors)
{
    const auto& root = game.settingTags[0].Native();
    // 01:4583C0/458680/45A0E0：按固定分类调用顺序选存在项，子项按原迭代顺序。
    // kind0 有独立 wire 正文，但当前构造没有注册来源，不能从现代 master 猜出来。
    for (const auto role : {Role::Bgm, Role::Effect, Role::Loop, Role::Voice})
    {
        const char* category = role == Role::Bgm ? "BGM" : role == Role::Effect ? Effects :
            role == Role::Loop ? Loop : Voice;
        Metadata metadata;
        metadata.role = role;
        const auto* section = Child(root, category, metadata.sourceIndex);
        if (!section) continue;
        metadata.name = std::string(Bytes(RequireNode(*section).nameAt00));
        metadata.masterIndex = metadata_.size();
        Wire::Entry entry;
        entry.kind = role == Role::Bgm ? Wire::Kind::Bgm : Wire::Kind::Categorized;
        if (role == Role::Bgm)
        {
            if (ReadSoundBoolOrDefault(errors, *section, "IsMidi", false))
                Reject("MIDI has no supported modern playback binding");
        }
        else
        {
            std::size_t ignored{};
            const auto* perFile = Child(*section, PerFile, ignored);
            // 与 AdaptCategorizedSoundSettings 相同：Loop per-file 分支尚无独立 master 投影。
            if (perFile && role == Role::Loop) Reject("loop per-file registrations have no supported binding");
            if (perFile)
            {
                std::size_t sourceIndex = 0;
                const auto& children = RequireNode(*perFile).childrenAt10;
                for (auto* it = children.begin; it != children.end; ++it, ++sourceIndex)
                {
                    if (!Same(RequireNode(*it).nameAt00, Setting)) continue;
                    auto name = CaptureSettingChildAt477E90(errors.CaptureAt47BCF0(), *it, RegisteredName);
                    auto text = CaptureSettingValueAt47D470(errors.CaptureAt47BCF0(), name.Native(), "Name");
                    ChildMetadata child;
                    child.sourceIndex = sourceIndex;
                    child.targetIndex = metadata.children.size();
                    child.name = std::string(Bytes(text.Value()));
                    // 45A58C/45A5AE：IsDisp 独立默认1；不能从无此字段的现代 SE 反推。
                    child.defaultIsDisp = ReadSoundBoolOrDefault(errors, name.Native(), "IsDisp", true) ? 1 : 0;
                    entry.children.push_back({{}, child.defaultIsDisp});
                    metadata.children.push_back(std::move(child));
                }
            }
        }
        metadata_.push_back(std::move(metadata));
        entries_.push_back(std::move(entry));
    }
    const auto bindings = Bind(archive, true);
    Gather(bindings);
}

std::vector<CategorizedSoundEnvironmentAdapter::Binding>
CategorizedSoundEnvironmentAdapter::Bind(EnvironmentSoundParamsArchive& archive, const bool captureIndices)
{
    const auto& masters = archive.typeParametersAt20.Native();
    if (Count(masters) != metadata_.size()) Reject("master count differs from original registrations");
    auto& effects = Exact<SoundTypeGroupNative>(archive.seByTypeAt2C.Native(), "effect group");
    auto& voices = Exact<SoundCharacterGroupNative>(archive.seByCharsAt38.Native(), "voice group");
    Name(effects.masterAt04, Effects, "effect group");
    std::size_t effectCount = 0, voiceCount = 0;
    for (const auto& item : metadata_)
    {
        if (item.role == Role::Effect) effectCount += item.children.size();
        if (item.role == Role::Voice) voiceCount += item.children.size();
    }
    if (Count(effects.parametersAt08) != effectCount || Count(voices.charactersAt08) != voiceCount)
        Reject("child count differs from original registrations");

    std::vector<Binding> bindings;
    const auto append = [&](Binding binding)
    {
        for (const auto& previous : bindings)
            if (previous.modern == binding.modern ||
                (binding.display && previous.display == binding.display))
                Reject("aliased scalar targets cannot represent independent occurrences");
        bindings.push_back(binding);
    };
    for (std::size_t i = 0; i != metadata_.size(); ++i)
    {
        auto& metadata = metadata_[i];
        auto& entry = entries_[i];
        const auto location = "entry " + std::to_string(i) + " (source " +
            std::to_string(metadata.sourceIndex) + ")";
        auto& master = Exact<SoundTypeNative>(masters.begin[metadata.masterIndex], location);
        Name(master.keyAt0C, metadata.name, location);
        append({&entry.value, nullptr, &master, nullptr});
        for (std::size_t j = 0; j != metadata.children.size(); ++j)
        {
            auto& child = metadata.children[j];
            auto& value = entry.children[j];
            const auto childLocation = location + " child " + std::to_string(j) +
                " (source " + std::to_string(child.sourceIndex) + ")";
            if (metadata.role == Role::Effect)
            {
                auto& effect = Exact<SoundSeNative>(effects.parametersAt08.begin[child.targetIndex], childLocation);
                Name(effect.keyAt0C, child.name, childLocation);
                append({&value.value, &value.isDisp, &effect, nullptr});
            }
            else if (metadata.role == Role::Voice)
            {
                auto& character = Exact<SoundCharacterNative>(voices.charactersAt08.begin[child.targetIndex], childLocation);
                Name(character.keyAt08, child.name, childLocation);
                // 当前 AdaptEntry 为每个旧出现项创建一个 character 和一个注册；先验证实际形状，
                // 再遍历记录实际注册序号，不按名字首查，也不把 registration=0 当隐含约定。
                const auto& registrations = character.parametersAt0C;
                if (Count(registrations) != 1) Reject(childLocation + ": unsupported voice registration shape");
                std::size_t registrationIndex = 0;
                for (auto* it = registrations.begin; it != registrations.end; ++it, ++registrationIndex)
                {
                    auto& registration = Exact<SoundSeExNative>(*it, childLocation);
                    Name(registration.keyAt0C, metadata.name, childLocation);
                    Name(registration.textAt30, metadata.name, childLocation);
                    if (captureIndices) child.registrationIndex = registrationIndex;
                    else if (child.registrationIndex != registrationIndex)
                        Reject(childLocation + ": registration occurrence changed");
                    append({&value.value, &value.isDisp, &registration, &character.valueAt05});
                }
            }
            else Reject(childLocation + ": no supported child projection");
        }
    }
    return bindings;
}

void CategorizedSoundEnvironmentAdapter::Gather(const std::span<const Binding> bindings) noexcept
{
    for (const auto& binding : bindings)
    {
        *binding.value = {binding.modern->valueAt04, binding.modern->valueAt08};
        if (binding.display) *binding.isDisp = *binding.display;
        // 普通 effect 没有 display 字段：保留 owner 已读入的原始 IsDisp byte。
    }
}

void CategorizedSoundEnvironmentAdapter::Scatter(const std::span<const Binding> bindings) noexcept
{
    for (const auto& binding : bindings)
    {
        binding.modern->valueAt04 = binding.value->mute;
        binding.modern->valueAt08 = binding.value->volume;
        if (binding.display) *binding.display = *binding.isDisp;
        // Voice mute 已在现代构造中还原，不反转，也不写 character/group 的 Using。
    }
}

CategorizedSoundEnvironmentAdapter::Result CategorizedSoundEnvironmentAdapter::Read(
    EnvironmentSoundParamsArchive& archive, const std::span<const std::byte> input, const std::size_t cursor)
{
    const auto bindings = Bind(archive, false);
    Gather(bindings);
    const auto result = Wire::Read(input, cursor, entries_);
    // 458780/458C40/458E00：失败前的完整标量已提交；这里无分配/回调/重新绑定，不能回滚。
    Scatter(bindings);
    return result;
}

std::vector<std::byte> CategorizedSoundEnvironmentAdapter::Write(EnvironmentSoundParamsArchive& archive)
{
    const auto bindings = Bind(archive, false);
    Gather(bindings);
    std::size_t size{};
    CheckWire(Wire::Measure(entries_, size), "Measure");
    std::vector<std::byte> output(size);
    CheckWire(Wire::Write(output, 0, entries_), "Write");
    return output;
}
}
