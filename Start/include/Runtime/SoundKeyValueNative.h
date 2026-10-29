#pragma once
#include <Runtime/SoundParameterNative.h>

namespace KaGuYa::Start::Runtime
{
// Sound::ISoundParams的基类对象族，与Environment::CSoundParams::se_params不是同一布局。
class SoundBasicValueNative : public IEnvironmentArchiveRuntime
{
public:
    explicit SoundBasicValueNative(SoundNativeShellTag) noexcept {}
    void Serialize(SerializeBuffer& serializer);
    void SerializeNative18(void* serializer18) override;
    void* DeleteAtVslot04(std::uint32_t flags) noexcept override;
    std::uint8_t mutedAt04;
    std::uint32_t volumeAt08;
protected:
    ~SoundBasicValueNative() noexcept {}
};
class SoundBgmValueNative final : public SoundBasicValueNative
{
public:
    explicit SoundBgmValueNative(SoundNativeShellTag tag) noexcept : SoundBasicValueNative(tag) {}
    void* DeleteAtVslot04(std::uint32_t flags) noexcept override;
    std::uint32_t runtimeAt0C; // 48BE9A仅建立vptr；该字段不进入48C9C0存档。
private:
    ~SoundBgmValueNative() noexcept {}
};
class SoundChildValueNative final : public SoundBasicValueNative
{
public:
    explicit SoundChildValueNative(SoundNativeShellTag tag) noexcept : SoundBasicValueNative(tag) {}
    void Serialize(SerializeBuffer& serializer);
    void SerializeNative18(void* serializer18) override;
    void* DeleteAtVslot04(std::uint32_t flags) noexcept override;
    std::uint8_t valueAt0C;
    union { RuntimeCString textAt10; };
    union { RuntimeCString textAt14; };
    EnvironmentVectorStorage<RuntimeCString> stringsAt18;
    std::uint32_t runtimeAt24;
    EnvironmentVectorStorage<RuntimeCString> stringsAt28;
private:
    ~SoundChildValueNative() noexcept {}
};
class SoundEffectValueNative final : public SoundBasicValueNative
{
public:
    explicit SoundEffectValueNative(SoundNativeShellTag tag) noexcept : SoundBasicValueNative(tag) {}
    void Serialize(SerializeBuffer& serializer);
    void SerializeNative18(void* serializer18) override;
    void SerializeWithSources(SerializeBuffer& serializer, IRuntimeStringManagerSource& strings);
    void* DeleteAtVslot04(std::uint32_t flags) noexcept override;
    EnvironmentVectorStorage<YtlReferenceSlotRuntime> childrenAt0C;
    std::uint32_t runtimeAt18;
    EnvironmentVectorStorage<RuntimeCString> stringsAt1C;
private:
    ~SoundEffectValueNative() noexcept {}
};
class SoundKeyValueNative final : public IEnvironmentArchiveRuntime
{
public:
    explicit SoundKeyValueNative(SoundNativeShellTag) noexcept {}
    void Serialize(SerializeBuffer& serializer);
    void SerializeNative18(void* serializer18) override;
    void SerializeWithMemory(SerializeBuffer& serializer, IEnvironmentRootMemory& memory);
    using LegacyValueSerialize = void(*)(void*, std::uint32_t, SerializeBuffer&);
    void SerializeWithLegacyValue(SerializeBuffer&, IEnvironmentRootMemory&, LegacyValueSerialize);
    template<class Value> void SerializeWithLegacyValue(SerializeBuffer& serializer, IEnvironmentRootMemory& memory)
    {
        SerializeWithLegacyValue(serializer, memory, [](void* body, std::uint32_t, SerializeBuffer& output)
            { static_cast<Value*>(body)->Serialize(output); });
    }
    void* DeleteAtVslot04(std::uint32_t flags) noexcept override;
    union { RuntimeCString keyAt04; };
    std::uint32_t kindAt08;
    union { YtlReferenceSlotRuntime valueAt0C; };
private:
    ~SoundKeyValueNative() noexcept {}
};
static_assert(sizeof(SoundBasicValueNative) == 0x0C && offsetof(SoundBasicValueNative, volumeAt08) == 8);
static_assert(sizeof(SoundBgmValueNative) == 0x10);
static_assert(sizeof(SoundChildValueNative) == 0x34 && offsetof(SoundChildValueNative, textAt10) == 0x10);
static_assert(offsetof(SoundChildValueNative, stringsAt28) == 0x28);
static_assert(sizeof(SoundEffectValueNative) == 0x28 && offsetof(SoundEffectValueNative, childrenAt0C) == 0x0C);
static_assert(sizeof(SoundKeyValueNative) == 0x18 && offsetof(SoundKeyValueNative, valueAt0C) == 0x0C);

// 48BD00读到不匹配的kind时只消费对应记录，不将其替换进当前KeyValue。
void ConstructSoundValueReferenceAt48BE18(YtlReferenceSlotRuntime& fresh, std::uint32_t kind,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
void AdoptSoundChildValueReference(YtlReferenceSlotRuntime& fresh, SoundChildValueNative* value,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
// 46A650/+08返回追加后的count，不是从0开始的index；key不参加48BD00文件记录。
std::int32_t AppendSoundKeyValueAt46A650(EnvironmentVectorStorage<YtlReferenceSlotRuntime>& table,
    const char* key, std::uint32_t kind, const YtlReferenceSlotRuntime& value,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());

class SoundKeyValueTableNative final
{
public:
    SoundKeyValueTableNative() noexcept = default;
    ~SoundKeyValueTableNative() noexcept;
    SoundKeyValueTableNative(const SoundKeyValueTableNative&) = delete;
    SoundKeyValueTableNative& operator=(const SoundKeyValueTableNative&) = delete;
    [[nodiscard]] const EnvironmentVectorStorage<YtlReferenceSlotRuntime>& Native() const noexcept { return storage_; }
    std::int32_t AddAt46A650(const char* key, std::uint32_t kind, const YtlReferenceSlotRuntime& value);
private:
    EnvironmentVectorStorage<YtlReferenceSlotRuntime> storage_{};
};
static_assert(sizeof(SoundKeyValueTableNative) == 12);
}
