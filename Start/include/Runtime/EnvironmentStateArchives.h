#pragma once

#include <Runtime/ByteArrayReferenceRuntime.h>
#include <Runtime/ParamsRawBlobInstaller.h>
#include <Runtime/SerializeBuffer.h>
#include <Runtime/SystemDataEnvironmentArchive.h>
#include <Runtime/RuntimeBitVector.h>
#include <Runtime/SecretCgNativeArchive.h>

#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace KaGuYa::Start::Runtime
{
class IEnvironmentKeyByteSource
{
public:
    virtual ~IEnvironmentKeyByteSource() = default;

    // 返回值必须与 Start.exe:47CB00 一致：params 原字节为零时返回 0xFF。
    [[nodiscard]] virtual std::uint8_t NormalizedByteAt(std::uint32_t index) const = 0;
};

// 长期消费者持有来源口，不固定借用某一代FileManager的key对象。
class IEnvironmentKeyTableSource : public IEnvironmentKeyByteSource
{
public:
    [[nodiscard]] virtual ByteArrayReferenceRuntime AcquireRawBytes() const = 0;
    [[nodiscard]] virtual std::size_t RawByteCount() const = 0;
};

class EnvironmentKeyByteTable final : public IEnvironmentKeyTableSource
{
public:
    explicit EnvironmentKeyByteTable(InstalledParamsRawBlob installedRawBlob);

    [[nodiscard]] std::uint8_t NormalizedByteAt(std::uint32_t index) const override;
    [[nodiscard]] std::span<const std::uint8_t> Bytes() const noexcept;
    // 480380 -> 465810：归档转换增持原字节，不调用47CB00的零字节归一化。
    // 465810只增加同一控制块的引用计数，不复制字节或分配现代共享控制块。
    [[nodiscard]] ByteArrayReferenceRuntime AcquireRawBytes() const noexcept override;
    [[nodiscard]] std::size_t RawByteCount() const noexcept override { return Bytes().size(); }
    [[nodiscard]] ParamsRawBlobStorageKind StorageKind() const noexcept;

private:
    ByteArrayReferenceRuntime bytes_;
    // 此类是工具/显式夹具的来源适配器；原manager的+1C只保存Ref，不保存压缩方式。
    ParamsRawBlobStorageKind storageKind_;
};

[[nodiscard]] std::uint8_t ReadNormalizedKeyByteAt47CB00(
    const ByteArrayReferenceRuntime& bytes, std::uint32_t index);

struct SecretCgFlagGroupSource
{
    std::string name;
    std::vector<std::uint32_t> flagIds;

    bool operator==(const SecretCgFlagGroupSource&) const = default;
};

struct SecretCgMappingSource
{
    std::string name;
    std::vector<std::string> flagListNames;

    bool operator==(const SecretCgMappingSource&) const = default;
};

struct SecretCgConstructionInputs
{
    // params +EC..+F0：决定哪些运行时 CG flag 组进入最终有序 flag 表。
    std::vector<std::string> selectedFlagGroupNamesAtEC;

    // 45BCA0 从另一运行时对象 +1C..+20 搜索的具名 flag 组。
    std::vector<SecretCgFlagGroupSource> availableFlagGroupsAt1C;

    // params +104..+108：每项包含名称以及若干需要解析的 flag-list 名称。
    std::vector<SecretCgMappingSource> mappingsAt104;

    // 4786A0 从其 this+1C..+20 搜索的名称到原始 flag ID 数组。
    std::vector<SecretCgFlagGroupSource> resolvableFlagListsAt1C;
};

struct SecretCgResolvedMapping
{
    std::string name;
    std::vector<std::vector<std::int32_t>> flagIndices;

    bool operator==(const SecretCgResolvedMapping&) const = default;
};

using SecretCgArchive = SecretCgNativeArchive;

class ISecretScCountSource
{
public:
    virtual ~ISecretScCountSource() = default;
    [[nodiscard]] virtual std::uint32_t CaptureSceneCountAt45CB5A() = 0;
};

class SecretScParamsSource final : public ISecretScCountSource
{
public:
    explicit SecretScParamsSource(ParamsRuntimeRootOwner& params) noexcept : params_(params) {}
    std::uint32_t CaptureSceneCountAt45CB5A() override;
private:
    ParamsRuntimeRootOwner& params_;
};

// CSecretSC是独立1Ch正文，不继承CG记录表，也不内嵌key服务或分配器。
// final限制C++继承，不取消5B2948的原业务虚槽。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunnecessary-virtual-specifier"
#endif
class SecretScArchive final : public IEnvironmentArchiveRuntime
{
public:
    static constexpr std::uint32_t OriginalObjectSize = 0x1C;
    static constexpr std::uint32_t OriginalVtableAddress = 0x5B2948;
    explicit SecretScArchive(ISecretScCountSource& source,
        IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
    // 显式固定计数入口供尚未迁移的值owner和夹具使用，不冒充全局根捕获。
    explicit SecretScArchive(std::uint32_t paramsDwordCountAtF8);
    ~SecretScArchive() noexcept;
    SecretScArchive(const SecretScArchive&) = delete;
    SecretScArchive& operator=(const SecretScArchive&) = delete;

    void Serialize(SerializeBuffer& serializer);
    void SerializeNative18(void* serializer18) override;
    void* DeleteAtVslot04(std::uint32_t flags) noexcept override;
    [[nodiscard]] virtual std::uint32_t CountAtVslot08() const noexcept;
    [[nodiscard]] virtual bool GetFlagAtVslot0C(std::uint32_t index) const noexcept;
    virtual void SetFlagAtVslot10(std::uint32_t index, std::uint8_t value) noexcept;
    virtual void SetKeyBaseAtVslot14(std::uint32_t base) noexcept;
    virtual void FillFlagsAtVslot18(std::uint8_t value) noexcept;

    // 原5B2948后三槽，Params借现有SecretCgParamsBinding，不驻留1Ch正文。
    [[nodiscard]] virtual const char* const* ThumbnailAtVslot1C(std::int32_t category, std::int32_t item);
    [[nodiscard]] virtual std::int32_t PercentAllAtVslot20() const;
    [[nodiscard]] virtual std::int32_t PercentCategoryAtVslot24(std::int32_t category) const;

    bool readyAt04;
    std::array<std::uint8_t, 3> unwrittenAt05;
    std::uint32_t baseKeyOffsetAt08;
    RuntimeBitVectorStorage bitsAt0C;
private:
    void InitializeAt45CAF0(ISecretScCountSource& source, IEnvironmentRootMemory& memory);
};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
static_assert(sizeof(SecretScArchive) == 0x1C);
static_assert(alignof(SecretScArchive) == 4);
static_assert(offsetof(SecretScArchive, readyAt04) == 0x04);
static_assert(offsetof(SecretScArchive, unwrittenAt05) == 0x05);
static_assert(offsetof(SecretScArchive, baseKeyOffsetAt08) == 0x08);
static_assert(offsetof(SecretScArchive, bitsAt0C) == 0x0C);

class ClickInfoArchive final : public IEnvironmentArchiveRuntime
{
public:
    // 18号样本：47E3B7分配、47E3C8写虚表；两槽+00/+04为48AFB0/468740。
    static constexpr std::uint32_t OriginalObjectSize = 0x3C;
    static constexpr std::uint32_t OriginalVtableAddress = 0x5B2974;
    static constexpr std::uint32_t OriginalSerializeAddress = 0x48AFB0;
    static constexpr std::uint32_t OriginalDeletingDestructorAddress = 0x468740;

    ClickInfoArchive() noexcept;
    ~ClickInfoArchive() noexcept = default;

    void Serialize(SerializeBuffer& serializer);
    void SerializeNative18(void* serializer18) override;
    void* DeleteAtVslot04(std::uint32_t flags) noexcept override;

    std::array<std::uint8_t, 8> bytesAt04{};
    std::array<std::uint8_t, 8> bytesAt0C{};
    std::array<std::uint8_t, 8> bytesAt14{};
    // 47E3F8..47E429 的运行时尾部，不属于48AFB0归档的前三组。
    std::array<std::uint8_t, 8> bytesAt1C{};
    std::array<std::uint8_t, 8> bytesAt24{};
    std::array<std::uint8_t, 8> bytesAt2C{};
    std::array<std::uint8_t, 8> bytesAt34{};
};
static_assert(sizeof(ClickInfoArchive) == ClickInfoArchive::OriginalObjectSize);
static_assert(alignof(ClickInfoArchive) == 4);
static_assert(offsetof(ClickInfoArchive, bytesAt04) == 0x04);
static_assert(offsetof(ClickInfoArchive, bytesAt0C) == 0x0C);
static_assert(offsetof(ClickInfoArchive, bytesAt14) == 0x14);
static_assert(offsetof(ClickInfoArchive, bytesAt1C) == 0x1C);
static_assert(offsetof(ClickInfoArchive, bytesAt24) == 0x24);
static_assert(offsetof(ClickInfoArchive, bytesAt2C) == 0x2C);
static_assert(offsetof(ClickInfoArchive, bytesAt34) == 0x34);

class GlobalParamsArchive final : public IEnvironmentArchiveRuntime
{
public:
    static constexpr std::uint32_t OriginalObjectSize = 0x4C;
    static constexpr std::uint32_t OriginalVtableAddress = 0x5B28D8;
    static constexpr std::uint32_t OriginalSerializeAddress = 0x48B020;
    static constexpr std::uint32_t OriginalDeletingDestructorAddress = 0x468BC0;

    // 47E4C0..47E4F1：唯一4Ch正文；key来源只借用本次serializer上下文。
    GlobalParamsArchive() noexcept;
    ~GlobalParamsArchive() noexcept = default;
    GlobalParamsArchive(const GlobalParamsArchive&) = delete;
    GlobalParamsArchive& operator=(const GlobalParamsArchive&) = delete;

    void Serialize(SerializeBuffer& serializer);
    void SerializeNative18(void* serializer18) override;
    void* DeleteAtVslot04(std::uint32_t flags) noexcept override;

    std::uint32_t lowKeyOffsetAt04;
    std::uint32_t highKeyOffsetAt08;
    std::array<std::uint16_t, 32> wordsAt0C;
};
static_assert(sizeof(GlobalParamsArchive) == 0x4C);
static_assert(alignof(GlobalParamsArchive) == 4);
static_assert(offsetof(GlobalParamsArchive, lowKeyOffsetAt04) == 0x04);
static_assert(offsetof(GlobalParamsArchive, highKeyOffsetAt08) == 0x08);
static_assert(offsetof(GlobalParamsArchive, wordsAt0C) == 0x0C);
}
