#pragma once
#include <Runtime/SystemDataEnvironmentArchive.h>
#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/EnvironmentRootRuntime.h>

namespace KaGuYa::Start::Runtime
{
class EnvironmentSoundParamsArchive;
// 4787E0/+84：原组/元素Ref逐点解析，无end/index保护；空角色正文返回地址5。
std::uint8_t* BorrowSoundCharacterDisplayAt4787E0(EnvironmentSoundParamsArchive&,std::int32_t) noexcept;
// 只建立vptr，字段由原调用点按顺序初始化；不得普通new或值初始化后当原构造。
struct SoundNativeShellTag {};
class SoundTypeNative : public IEnvironmentArchiveRuntime
{
public:
    explicit SoundTypeNative(SoundNativeShellTag) noexcept {}
    SoundTypeNative(const SoundTypeNative&) = delete;
    SoundTypeNative& operator=(const SoundTypeNative&) = delete;
    void Serialize(SerializeBuffer& serializer);
    void SerializeNative18(void* serializer18) override;
    void* DeleteAtVslot04(std::uint32_t flags) noexcept override;
    std::uint8_t valueAt04;
    std::uint32_t valueAt08;
    union { RuntimeCString keyAt0C; };
protected:
    ~SoundTypeNative() noexcept {}
};

class SoundSeNative : public SoundTypeNative
{
public:
    explicit SoundSeNative(SoundNativeShellTag tag) noexcept : SoundTypeNative(tag) {}
    void* DeleteAtVslot04(std::uint32_t flags) noexcept override;
    union { RuntimeCString textAt10; };
    EnvironmentVectorStorage<YtlReferenceSlotRuntime> fileKeysAt14;
    std::uint32_t stateAt20;
    EnvironmentVectorStorage<RuntimeCString> stringsAt24;
protected:
    ~SoundSeNative() noexcept {}
};

class SoundSeExNative final : public SoundSeNative
{
public:
    explicit SoundSeExNative(SoundNativeShellTag tag) noexcept : SoundSeNative(tag) {}
    void* DeleteAtVslot04(std::uint32_t flags) noexcept override;
    union { RuntimeCString textAt30; };
private:
    ~SoundSeExNative() noexcept {}
};

class SoundCharacterNative final : public IEnvironmentArchiveRuntime
{
public:
    explicit SoundCharacterNative(SoundNativeShellTag) noexcept {}
    SoundCharacterNative(const SoundCharacterNative&) = delete;
    SoundCharacterNative& operator=(const SoundCharacterNative&) = delete;
    void Serialize(SerializeBuffer& serializer);
    void SerializeNative18(void* serializer18) override;
    void SerializeWithSources(SerializeBuffer& serializer, IRuntimeStringManagerSource& strings,
        IEnvironmentRootMemory& memory);
    void MergeIntoAt48EE20(YtlReferenceSlotRuntime& destination) const;
    void* DeleteAtVslot04(std::uint32_t flags) noexcept override;
    std::uint8_t valueAt04;
    std::uint8_t valueAt05;
    union { RuntimeCString keyAt08; };
    EnvironmentVectorStorage<YtlReferenceSlotRuntime> parametersAt0C;
private:
    ~SoundCharacterNative() noexcept {}
};
static_assert(sizeof(SoundTypeNative) == 0x10 && offsetof(SoundTypeNative, keyAt0C) == 0x0C);
static_assert(offsetof(SoundTypeNative, valueAt04) == 4 && offsetof(SoundTypeNative, valueAt08) == 8);
static_assert(sizeof(SoundSeNative) == 0x30 && offsetof(SoundSeNative, fileKeysAt14) == 0x14);
static_assert(offsetof(SoundSeNative, stateAt20) == 0x20 && offsetof(SoundSeNative, stringsAt24) == 0x24);
static_assert(sizeof(SoundSeExNative) == 0x34 && offsetof(SoundSeExNative, textAt30) == 0x30);
static_assert(sizeof(SoundCharacterNative) == 0x18 && offsetof(SoundCharacterNative, keyAt08) == 8);
static_assert(offsetof(SoundCharacterNative, parametersAt0C) == 0x0C);

// 5B2480仅有469BA0一个删除槽，不能继承两槽IArchive或添加虚析构。
class SoundFileKeyNative
{
public:
    explicit SoundFileKeyNative(SoundNativeShellTag) noexcept {}
    SoundFileKeyNative(const SoundFileKeyNative&) = delete;
    SoundFileKeyNative& operator=(const SoundFileKeyNative&) = delete;
    virtual void* DeleteAtVslot00(std::uint32_t flags) noexcept;
    union { RuntimeCString keyAt04; };
    union { RuntimeCString extensionAt08; };
    union { RuntimeCString fileAt0C; };
protected:
    ~SoundFileKeyNative() noexcept {}
};
static_assert(sizeof(SoundFileKeyNative) == 0x10 && offsetof(SoundFileKeyNative, keyAt04) == 4);

class SoundTypeGroupNative final : public IEnvironmentArchiveRuntime
{
public:
    explicit SoundTypeGroupNative(SoundNativeShellTag) noexcept {}
    SoundTypeGroupNative(const SoundTypeGroupNative&) = delete;
    SoundTypeGroupNative& operator=(const SoundTypeGroupNative&) = delete;
    void Serialize(SerializeBuffer& serializer);
    void SerializeNative18(void* serializer18) override;
    void SerializeWithSources(SerializeBuffer& serializer, IRuntimeStringManagerSource& strings,
        IEnvironmentRootMemory& memory);
    void* DeleteAtVslot04(std::uint32_t flags) noexcept override;
    union { RuntimeCString masterAt04; };
    EnvironmentVectorStorage<YtlReferenceSlotRuntime> parametersAt08;
private:
    ~SoundTypeGroupNative() noexcept {}
};

class SoundCharacterGroupNative final : public IEnvironmentArchiveRuntime
{
public:
    explicit SoundCharacterGroupNative(SoundNativeShellTag) noexcept {}
    SoundCharacterGroupNative(const SoundCharacterGroupNative&) = delete;
    SoundCharacterGroupNative& operator=(const SoundCharacterGroupNative&) = delete;
    void Serialize(SerializeBuffer& serializer);
    void SerializeNative18(void* serializer18) override;
    void SerializeWithSources(SerializeBuffer& serializer, IRuntimeStringManagerSource& strings,
        IEnvironmentRootMemory& memory);
    void* DeleteAtVslot04(std::uint32_t flags) noexcept override;
    std::uint8_t valueAt04, valueAt05;
    EnvironmentVectorStorage<YtlReferenceSlotRuntime> charactersAt08;
private:
    ~SoundCharacterGroupNative() noexcept {}
};
static_assert(sizeof(SoundTypeGroupNative) == 0x14 && offsetof(SoundTypeGroupNative, parametersAt08) == 8);
static_assert(sizeof(SoundCharacterGroupNative) == 0x14 && offsetof(SoundCharacterGroupNative, charactersAt08) == 8);

// 47391A..4739C2：包括raw分配和三个字段构造/赋值；成功后尚无Ref所有权。
[[nodiscard]] SoundFileKeyNative* CreateSoundFileKeyAt47391A(const char* key, const char* extension,
    const char* file, IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
// 474877..4748E6只负责正文内层构造。外层必须先分配，再读取Master；失败释放raw仍属外层。
[[nodiscard]] SoundTypeGroupNative* InitializeSoundTypeGroupAt474877(void* raw, const char* master,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
[[nodiscard]] SoundCharacterGroupNative* CreateSoundCharacterGroupAt47209F(
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());

// 原读取临时对象的分配/构造阶段尚无清理资格；失败不得自动回收body。
[[nodiscard]] SoundTypeNative* CreateSoundTypeAt46FEBD(
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
[[nodiscard]] SoundSeNative* CreateSoundSeForFileAt48C7C0(
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
// 474A2C：配置入口的独立45F670构造有终止边界，不能复用可抛的读档内联构造。
[[nodiscard]] SoundSeNative* CreateSoundSeForSettingsAt474A2C(
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
void AdoptSoundSeAt474A47(YtlReferenceSlotRuntime& fresh, SoundSeNative* body,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
[[nodiscard]] SoundCharacterNative* CreateSoundCharacterForSettingsAt4722D1(
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
[[nodiscard]] SoundSeExNative* CreateSoundSeExForSettingsAt4728B3(
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
void AdoptSoundCharacterAt47230E(YtlReferenceSlotRuntime& fresh, SoundCharacterNative* body,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
void AdoptSoundSeExAt472916(YtlReferenceSlotRuntime& fresh, SoundSeExNative* body,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
[[nodiscard]] SoundSeExNative* CreateSoundSeExForFileAt48C310(
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
[[nodiscard]] SoundCharacterNative* CreateSoundCharacterForFileAt48C570(
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());

// 与析构相同：原生typed18h控制块，不用shared_ptr接管正文。
void AdoptSoundNativeReference(YtlReferenceSlotRuntime& fresh, SoundTypeNative* body,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
void AdoptSoundNativeReference(YtlReferenceSlotRuntime& fresh, SoundSeNative* body,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
void AdoptSoundNativeReference(YtlReferenceSlotRuntime& fresh, SoundSeExNative* body,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
void AdoptSoundNativeReference(YtlReferenceSlotRuntime& fresh, SoundCharacterNative* body,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
void AdoptSoundNativeReference(YtlReferenceSlotRuntime& fresh, SoundFileKeyNative* body,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
// 4748F7/4720D4：旧槽释放后正文先发布；控制块发布早于其count/stride初始化。
void ReplaceSoundGroupReference(YtlReferenceSlotRuntime& slot, SoundTypeGroupNative* body,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
void ReplaceSoundGroupReference(YtlReferenceSlotRuntime& slot, SoundCharacterGroupNative* body,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());

// 正常尾声使用缓存control，展开则从Ref槽重新读取；二者不能混成一条路径。
class SoundNativeTemporary final
{
public:
    explicit SoundNativeTemporary(YtlReferenceSlotRuntime& slot) noexcept : slot_(&slot) {}
    ~SoundNativeTemporary() noexcept;
    void Finish(YtlReferenceControlRuntime* cached) noexcept;
    SoundNativeTemporary(const SoundNativeTemporary&) = delete;
    SoundNativeTemporary& operator=(const SoundNativeTemporary&) = delete;
private:
    YtlReferenceSlotRuntime* slot_;
};
}
