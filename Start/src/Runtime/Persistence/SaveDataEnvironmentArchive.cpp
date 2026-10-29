#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/GameBuildProfile.h>
#include <Runtime/SoundParamsReference.h>
#include <Runtime/RuntimeCStringSerialize.h>
#include <Runtime/SoundParamsEnvironmentArchive.h>
#include <bit>

namespace KaGuYa::Start::Runtime
{
EnvironmentSoundParamsArchive& SaveDataEnvironmentArchive::BorrowSoundParamsAt1C() const
{
    auto* body = ResolveEnvironmentObjectReference<EnvironmentSoundParamsArchive>(soundParamsAt1C);
    if (!body) throw std::logic_error("SaveData +1C has no SoundParams payload (safe boundary)");
    return *body;
}

namespace
{
template <std::size_t Size>
void SerializeFixedBytes(SerializeBuffer& serializer, std::array<std::uint8_t, Size>& bytes)
{
    for (std::uint8_t& value : bytes)
    {
        serializer.SerializeSlot50Byte(value);
    }
}

template <std::size_t Size>
void StoreDword(
    std::array<std::uint8_t, Size>& bytes,
    const std::size_t offset,
    const std::uint32_t value)
{
    bytes[offset] = static_cast<std::uint8_t>(value);
    bytes[offset + 1u] = static_cast<std::uint8_t>(value >> 8u);
    bytes[offset + 2u] = static_cast<std::uint8_t>(value >> 16u);
    bytes[offset + 3u] = static_cast<std::uint8_t>(value >> 24u);
}
}

SaveDataEnvironmentArchive::SaveDataEnvironmentArchive(
    IEnvironmentSoundParamsArchive& soundParams)
    : bytesAt04{}, bytesAt34{}, bytesAt40{}, bytesAt48{}, valueAt50(0), valueAt54(0),
      bytesAt60{}, valueAt6C(0), bytesAt6D{}, valueAt6F(0), valueAt70(0),
      bytesAt74{}, bytesAt77{}, valueAt81(0), bytesAt84{}, bytesAt8C{}
{
    // 显式夹具入口借用声音，不构造两套配置，也不伪造owning控制块。
    InitializeEmptyRuntimeCString(stringAt58);
    try { InitializeEmptyRuntimeCString(stringAt5C); }
    catch (...) { DestroyRuntimeCStringAt40CD40(&stringAt58); throw; }
    soundParamsAt1C.baseAt00 = reinterpret_cast<std::uintptr_t>(&soundParams);
}

SaveDataEnvironmentArchive::SaveDataEnvironmentArchive(
    IEnvironmentSoundParamsArchive& soundParams,
    const SaveDataConstructionInputs& inputs)
    : SaveDataEnvironmentArchive(soundParams)
{
    ApplySnapshot(inputs);
}

void SaveDataEnvironmentArchive::ApplySnapshot(
    const SaveDataConstructionInputs& inputs)
{
    // Start.exe:45B410。未初始化字节由 inputs 保留，不能以零值代替原存储内容。
    bytesAt04 = {0, 1, inputs.storageBytesAt06[0], inputs.storageBytesAt06[1],
        1, 0, 0, 0, 1, 0, 0, 0};
    recordsAt10.clear();
    bytesAt34 = {2, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0};

    StoreDword(bytesAt40, 0, inputs.paramsValueAt40);
    StoreDword(bytesAt40, 4, inputs.paramsValueAt44);
    StoreDword(bytesAt48, 0, inputs.paramsValueAt48);
    StoreDword(bytesAt48, 4, inputs.paramsValueAt4C);
    valueAt50 = 0;
    valueAt54 = 0;
    SetRuntimeCStringAt437680(stringAt58, inputs.runtimeStringAt58.data(), static_cast<int>(inputs.runtimeStringAt58.size()));
    SetRuntimeCStringAt437680(stringAt5C, inputs.runtimeStringAt5C.data(), static_cast<int>(inputs.runtimeStringAt5C.size()));

    bytesAt60 = {1, 1, inputs.storageBytesAt62[0], inputs.storageBytesAt62[1],
        0, 0, 0, 0, 0, 0, 0, 0};
    StoreDword(bytesAt60, 4, inputs.paramsValueAt64);
    StoreDword(bytesAt60, 8, inputs.paramsValueAt68);
    valueAt6C = 1;
    bytesAt6D = {1, 1};
    valueAt6F = 1;
    valueAt70 = 2;
    bytesAt74 = {1, 1, 1};
    bytesAt77 = {1, 1, 0, 0, 1, 1, 1, 1, 1, 0};
    valueAt81 = 0;
    bytesAt84 = {0, 0, inputs.storageBytesAt86[0], inputs.storageBytesAt86[1],
        50, 0, 0, 0};
    bytesAt8C = {1, 0, 0, 0};
}

SaveDataEnvironmentArchive::SaveDataEnvironmentArchive(ISaveDataConstructionSource& source,
    ISoundParamsSettingsSource& sounds, ISettingTagErrorSource& errors,
    IRuntimeStringManagerSource& strings, IEnvironmentRootMemory& memory)
{
    bool namesCompleted = false;
    try
    {
        InitializeEmptyRuntimeCString(stringAt58, strings);
        InitializeEmptyRuntimeCString(stringAt5C, strings);
        // 45B4C1..45B4DA：第二个GetNil完成前没有第一串的独立展开资格。
        namesCompleted = true;
        bytesAt04[0] = 0; bytesAt04[1] = 1;
        StoreDword(bytesAt04, 4, 1); StoreDword(bytesAt04, 8, 1);
        recordsAt10.clear();
        ReplaceSoundParamsAt451800(soundParamsAt1C, CreateSoundParamsAt45B4D8(sounds, errors, strings, memory), memory);
        ReplaceSoundParamsAt451800(soundParamsAt28, CreateSoundParamsAt45B4D8(sounds, errors, strings, memory), memory);
        bytesAt34 = {2, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0};
        valueAt54 = 0;
        AssignRuntimeCStringAt410220(stringAt58, source.ReadName(SaveDataName::Given));
        AssignRuntimeCStringAt410220(stringAt5C, source.ReadName(SaveDataName::Family));
        StoreDword(bytesAt40, 0, source.ReadSystemValue(SaveDataSystemField::At40));
        StoreDword(bytesAt40, 4, source.ReadSystemValue(SaveDataSystemField::At44));
        StoreDword(bytesAt48, 0, source.ReadSystemValue(SaveDataSystemField::At50));
        StoreDword(bytesAt48, 4, source.ReadSystemValue(SaveDataSystemField::At54));
        bytesAt60[0] = 1; bytesAt60[1] = 1;
        StoreDword(bytesAt60, 4, source.ReadSystemValue(SaveDataSystemField::At38));
        StoreDword(bytesAt60, 8, source.ReadSystemValue(SaveDataSystemField::At3C));
        valueAt6C = 1; bytesAt6D = {1, 1}; valueAt6F = 1;
        valueAt70 = 2;
        bytesAt74 = {1, 1, 1}; bytesAt77 = {1, 1, 0, 0, 1, 1, 1, 1, 1, 0}; valueAt81 = 0;
        valueAt50 = 0;
        bytesAt84[0] = 0; bytesAt84[1] = 0; StoreDword(bytesAt84, 4, 50);
        StoreDword(bytesAt8C, 0, 1);
    }
    catch (...)
    {
        if (namesCompleted) ReleaseNames();
        ReleaseSounds();
        // records成员最后自动释放，然后进入IArchive；不回收control失败留下的孤立正文。
        throw;
    }
}
void SaveDataEnvironmentArchive::ReleaseNames() noexcept
{ DestroyRuntimeCStringAt40CD40(&stringAt5C); DestroyRuntimeCStringAt40CD40(&stringAt58); }
void SaveDataEnvironmentArchive::ReleaseSounds() noexcept
{ ReleaseYtlReference(soundParamsAt28); ReleaseYtlReference(soundParamsAt1C); }
SaveDataEnvironmentArchive::~SaveDataEnvironmentArchive() noexcept
{ ReleaseNames(); ReleaseSounds(); }
void* SaveDataEnvironmentArchive::DeleteAtVslot04(std::uint32_t flags) noexcept
{
    this->~SaveDataEnvironmentArchive();
    if (flags & 1u) ::operator delete(this, std::size_t{0x90});
    return this;
}
SaveDataEnvironmentArchive* CreateSaveDataAt47E2B0(ISaveDataConstructionSource& source,
    ISoundParamsSettingsSource& sounds, ISettingTagErrorSource& errors,
    IRuntimeStringManagerSource& strings, IEnvironmentRootMemory& memory)
{
    void* raw = memory.AllocateBody(0x90);
    if (!raw) return nullptr;
    try { return ::new (raw) SaveDataEnvironmentArchive(source, sounds, errors, strings, memory); }
    catch (...) { memory.DeleteBody(raw, 0x90); throw; }
}

void SaveDataEnvironmentArchive::Serialize(SerializeBuffer& serializer)
{
    SerializeWithLegacySound(serializer, [](void* sound, SerializeBuffer& output)
        { static_cast<EnvironmentSoundParamsArchive*>(sound)->Serialize(output); });
}
void SaveDataEnvironmentArchive::SerializeWithLegacySound(SerializeBuffer& serializer,
    LegacySoundSerialize serializeSound)
{
    SerializeWithSoundContext(serializer,[](void* sound,SerializeBuffer& output,void* context)
        {(*static_cast<LegacySoundSerialize*>(context))(sound,output);},&serializeSound);
}

void SaveDataEnvironmentArchive::SerializeWithSoundContext(SerializeBuffer& serializer,
    ContextSoundSerialize serializeSound,void* context)
{
    // Start.exe:48B150。这里严格保留原调用顺序，不能按字段 offset 排序。
    SerializeFixedBytes(serializer, bytesAt04);

    std::uint32_t recordCount = static_cast<std::uint32_t>(recordsAt10.size());
    serializer.SerializeSlot58Dword(recordCount);
    if (!serializer.IsWriting())
    {
        recordsAt10.resize(recordCount);
    }
    for (std::int32_t index = 0; index < std::bit_cast<std::int32_t>(recordCount); ++index)
    {
        // 48B270..48B2A6每个DWORD前都重新读取三指针中的begin。
        serializer.SerializeSlot3CDword(recordsAt10[index].valueAt00);
        serializer.SerializeSlot3CDword(recordsAt10[index].valueAt04);
        serializer.SerializeSlot3CDword(recordsAt10[index].valueAt08);
        serializer.SerializeSlot3CDword(recordsAt10[index].valueAt0C);
    }

    // +1C 引用对象已由 RTTI 确认为 Environment::CSoundParams。
    auto* sound = ResolveEnvironmentObjectReference<void>(soundParamsAt1C);
    if (!sound) _invalid_parameter_noinfo_noreturn();
    serializeSound(sound, serializer, context);
    SerializeFixedBytes(serializer, bytesAt34);
    serializer.SerializeSlot54Byte(valueAt54);
    SerializeRuntimeCStringAt465ED0(serializer, stringAt58);
    SerializeRuntimeCStringAt465ED0(serializer, stringAt5C);
    SerializeFixedBytes(serializer, bytesAt40);
    SerializeFixedBytes(serializer, bytesAt48);
    SerializeFixedBytes(serializer, bytesAt60);
    // 01:45C160/02:460D00/03:460310没有该落盘字节；旧wire读取不改现代字段原值。
    if (CompiledGameProfile().envReadColorBytePresent)
        serializer.SerializeSlot54Byte(valueAt6C);
    SerializeFixedBytes(serializer, bytesAt6D);
    serializer.SerializeSlot54Byte(valueAt6F);
    serializer.SerializeSlot58Dword(valueAt70);
    serializer.SerializeSlot54Byte(valueAt81);
    SerializeFixedBytes(serializer, bytesAt74);
    SerializeFixedBytes(serializer, bytesAt77);
    serializer.SerializeSlot50Byte(valueAt50);
    SerializeFixedBytes(serializer, bytesAt84);
    SerializeFixedBytes(serializer, bytesAt8C);
}
}
