#pragma once

#include <Runtime/SerializeBuffer.h>
#include <Runtime/SoundParamsBaseNative.h>
#include <Runtime/SaveDataRecordVector.h>
#include <Runtime/SaveDataConstructionSource.h>

#include <array>
#include <cstdint>
#include <string>

namespace KaGuYa::Start::Runtime
{
class ISoundParamsSettingsSource;
class ISettingTagErrorSource;
class EnvironmentSoundParamsArchive;
using IEnvironmentSoundParamsArchive = SoundParamsBaseNative;

struct SaveDataConstructionInputs final
{
    // 45B410 未写这六个字节；原 operator new 也不清零，调用方必须提供原始存储值。
    std::array<std::uint8_t, 2> storageBytesAt06{};
    std::array<std::uint8_t, 2> storageBytesAt62{};
    std::array<std::uint8_t, 2> storageBytesAt86{};

    std::string runtimeStringAt58;
    std::string runtimeStringAt5C;
    // 历史paramsValue命名保留以避免改动消费者；45B600..45B787的实际来源均为
    // SysData slot0：+40/+44、+50/+54、+38/+3C，不是GameSystem的同offset字段。
    std::uint32_t paramsValueAt40 = 0;
    std::uint32_t paramsValueAt44 = 0;
    std::uint32_t paramsValueAt48 = 0;
    std::uint32_t paramsValueAt4C = 0;
    std::uint32_t paramsValueAt64 = 0;
    std::uint32_t paramsValueAt68 = 0;
};

class SaveDataEnvironmentArchive final : public IEnvironmentArchiveRuntime
{
public:
    // 仅构造空白序列化模型；不等价于 Environment::CSaveData 构造函数。
    explicit SaveDataEnvironmentArchive(IEnvironmentSoundParamsArchive& soundParams);
    SaveDataEnvironmentArchive(
        IEnvironmentSoundParamsArchive& soundParams,
        const SaveDataConstructionInputs& inputs);

    // 正式45B410：未写字节保持调用方raw存储，不整体值初始化。
    SaveDataEnvironmentArchive(ISaveDataConstructionSource& source, ISoundParamsSettingsSource& sounds,
        ISettingTagErrorSource& errors,
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
        IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
    ~SaveDataEnvironmentArchive() noexcept;
    SaveDataEnvironmentArchive(const SaveDataEnvironmentArchive&) = delete;
    SaveDataEnvironmentArchive& operator=(const SaveDataEnvironmentArchive&) = delete;
    void Serialize(SerializeBuffer& serializer);
    void SerializeNative18(void* serializer18) override;
    // 显式旧宿主域：借用声音模型须由调用者给出静态协议，不按vptr或名字猜类型。
    using LegacySoundSerialize = void(*)(void*, SerializeBuffer&);
    void SerializeWithLegacySound(SerializeBuffer&, LegacySoundSerialize);
    using ContextSoundSerialize = void(*)(void*,SerializeBuffer&,void*);
    void SerializeWithSoundContext(SerializeBuffer&,ContextSoundSerialize,void* context);
    template<class Sound> void SerializeWithLegacySound(SerializeBuffer& serializer)
    {
        SerializeWithLegacySound(serializer, [](void* sound, SerializeBuffer& output)
            { static_cast<Sound*>(sound)->Serialize(output); });
    }
    void* DeleteAtVslot04(std::uint32_t flags) noexcept override;
    // 419326/4D19D5只解+1C的原Ref；返回正文，不借现代owner、不增加引用。
    [[nodiscard]] EnvironmentSoundParamsArchive& BorrowSoundParamsAt1C() const;

    std::array<std::uint8_t, 12> bytesAt04;
    SaveDataRecordVector recordsAt10;
    YtlReferenceSlotRuntime soundParamsAt1C;
    YtlReferenceSlotRuntime soundParamsAt28;
    std::array<std::uint8_t, 12> bytesAt34;
    std::array<std::uint8_t, 8> bytesAt40;
    std::array<std::uint8_t, 8> bytesAt48;
    std::uint8_t valueAt50;
    std::uint8_t unwrittenAt51[3];
    std::uint8_t valueAt54;
    std::uint8_t unwrittenAt55[3];
    union { RuntimeCString stringAt58; };
    union { RuntimeCString stringAt5C; };
    std::array<std::uint8_t, 12> bytesAt60;
    std::uint8_t valueAt6C;
    std::array<std::uint8_t, 2> bytesAt6D;
    std::uint8_t valueAt6F;
    std::uint32_t valueAt70;
    std::array<std::uint8_t, 3> bytesAt74;
    std::array<std::uint8_t, 10> bytesAt77;
    std::uint8_t valueAt81;
    std::uint8_t unwrittenAt82[2];
    std::array<std::uint8_t, 8> bytesAt84;
    std::array<std::uint8_t, 4> bytesAt8C;

private:
    void ApplySnapshot(const SaveDataConstructionInputs& inputs);
    void ReleaseNames() noexcept;
    void ReleaseSounds() noexcept;
};
static_assert(sizeof(SaveDataEnvironmentArchive) == 0x90);
static_assert(offsetof(SaveDataEnvironmentArchive, recordsAt10) == 0x10);
static_assert(offsetof(SaveDataEnvironmentArchive, soundParamsAt1C) == 0x1C);
static_assert(offsetof(SaveDataEnvironmentArchive, soundParamsAt28) == 0x28);
static_assert(offsetof(SaveDataEnvironmentArchive, stringAt58) == 0x58);
static_assert(offsetof(SaveDataEnvironmentArchive, stringAt5C) == 0x5C);
static_assert(offsetof(SaveDataEnvironmentArchive, bytesAt84) == 0x84);

SaveDataEnvironmentArchive* CreateSaveDataAt47E2B0(ISaveDataConstructionSource& source,
    ISoundParamsSettingsSource& sounds, ISettingTagErrorSource& errors,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
}
