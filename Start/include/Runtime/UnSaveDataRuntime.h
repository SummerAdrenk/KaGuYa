#pragma once

#include <Runtime/SerializeBuffer.h>
#include <Runtime/SoundSlotManagerRuntime.h>
#include <Runtime/RuntimeCStringArray.h>
#include <Runtime/EnvironmentRootRuntime.h>
#include <Runtime/SystemDataEnvironmentArchive.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include <span>

namespace KaGuYa::Start::Runtime
{
class ParamsRuntimeRootOwner;
class IUnSaveDataGameSystemSource
{
public:
    virtual ~IUnSaveDataGameSystemSource() = default;
    // 45E370/45E398只捕获解址后的对象，不提前读计数，也不增持。
    [[nodiscard]] virtual const ParamsGameSystem* CaptureGameSystemAt418480() = 0;
};

class UnSaveDataParamsSource final : public IUnSaveDataGameSystemSource
{
public:
    explicit UnSaveDataParamsSource(ParamsRuntimeRootOwner& params) noexcept : params_(params) {}
    const ParamsGameSystem* CaptureGameSystemAt418480() override;
private:
    ParamsRuntimeRootOwner& params_;
};

// 45E2D0：原4Ch正文只含IArchive两槽；声音适配器和配置计数不属于正文。
class UnSaveDataRuntime final : public IEnvironmentArchiveRuntime
{
public:
    static constexpr std::size_t OriginalObjectSize = 0x4C;
    static constexpr std::size_t OriginalOffsetMode04 = 0x04;
    static constexpr std::size_t OriginalOffsetBytes08 = 0x08;
    static constexpr std::size_t OriginalOffsetDword0C = 0x0C;
    static constexpr std::size_t OriginalOffsetDword10 = 0x10;
    static constexpr std::size_t OriginalOffsetFlags14 = 0x14;
    static constexpr std::size_t OriginalOffsetFlags18 = 0x18;
    static constexpr std::size_t OriginalOffsetFlags1C = 0x1C;
    static constexpr std::size_t OriginalOffsetFlags20 = 0x20;
    static constexpr std::size_t OriginalOffsetBytes24 = 0x24;
    static constexpr std::size_t OriginalOffsetString2C = 0x2C;
    static constexpr std::size_t OriginalOffsetMappings30 = 0x30;
    static constexpr std::size_t OriginalOffsetArrayReference3C = 0x3C;
    static constexpr std::size_t OriginalOffsetDword48 = 0x48;
    static constexpr std::uint32_t OriginalVtableAddress = 0x5B28E4;
    static constexpr std::uint32_t OriginalSerializeAddress = 0x48BA10;
    static constexpr std::uint32_t OriginalDeletingDestructorAddress = 0x4695C0;

    explicit UnSaveDataRuntime(IUnSaveDataGameSystemSource& source,
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
        IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
    // 固定对象/计数入口仅供旧值模型装配与受控夹具，不冒充两次根捕获。
    explicit UnSaveDataRuntime(const ParamsGameSystem& gameSystem);
    UnSaveDataRuntime(
        std::uint32_t firstEffectCountAtGameSystem50,
        std::uint32_t secondEffectCountAtGameSystem48);
    ~UnSaveDataRuntime() noexcept;
    UnSaveDataRuntime(const UnSaveDataRuntime&) = delete;
    UnSaveDataRuntime& operator=(const UnSaveDataRuntime&) = delete;

    [[nodiscard]] std::size_t Size() const noexcept;
    [[nodiscard]] std::string_view ResourceNameAt(std::size_t port) const;
    void ClearAll();
    void Clear(std::size_t port);
    void Store(std::size_t port, std::string_view resourceName);
    void StorePatternName(std::size_t port,
        ScrResourceNameReceiver receiver, std::int32_t resourceId);

    // 原虚表首槽 0x48BA10 不读写归档，也不推进序列化游标。
    void Serialize(SerializeBuffer& serializer) noexcept;
    void SerializeNative18(void* serializer18) noexcept override;
    void* DeleteAtVslot04(std::uint32_t flags) noexcept override;

    [[nodiscard]] std::uint32_t& ModeAt04() noexcept;
    [[nodiscard]] const std::uint32_t& ModeAt04() const noexcept;
    [[nodiscard]] std::array<std::uint8_t, 4>& BytesAt08() noexcept;
    [[nodiscard]] const std::array<std::uint8_t, 4>& BytesAt08() const noexcept;
    [[nodiscard]] std::uint32_t& DwordAt0C() noexcept;
    [[nodiscard]] const std::uint32_t& DwordAt0C() const noexcept;
    [[nodiscard]] std::uint32_t& DwordAt10() noexcept;
    [[nodiscard]] const std::uint32_t& DwordAt10() const noexcept;
    [[nodiscard]] std::uint32_t& FlagsAt14() noexcept;
    [[nodiscard]] const std::uint32_t& FlagsAt14() const noexcept;
    [[nodiscard]] std::uint32_t& FlagsAt18() noexcept;
    [[nodiscard]] const std::uint32_t& FlagsAt18() const noexcept;
    [[nodiscard]] std::uint32_t& FlagsAt1C() noexcept;
    [[nodiscard]] const std::uint32_t& FlagsAt1C() const noexcept;
    [[nodiscard]] std::uint32_t& FlagsAt20() noexcept;
    [[nodiscard]] const std::uint32_t& FlagsAt20() const noexcept;
    [[nodiscard]] std::array<std::uint8_t, 8>& BytesAt24() noexcept;
    [[nodiscard]] const std::array<std::uint8_t, 8>& BytesAt24() const noexcept;
    [[nodiscard]] RuntimeCString& StringAt2C() noexcept;
    [[nodiscard]] const RuntimeCString& StringAt2C() const noexcept;
    [[nodiscard]] std::span<const RuntimeCString> MappingsAt30() const noexcept;
    [[nodiscard]] UnSaveDataStringArrayReference& ArrayReferenceAt3C() noexcept;
    [[nodiscard]] const UnSaveDataStringArrayReference& ArrayReferenceAt3C() const noexcept;
    [[nodiscard]] std::uint32_t& DwordAt48() noexcept;
    [[nodiscard]] const std::uint32_t& DwordAt48() const noexcept;

    // union由原EH阶段逐项发布/清理，不能让ATL或成员析构再次减持。
    std::uint32_t modeAt04_;
    std::array<std::uint8_t, 4> bytesAt08_;
    std::uint32_t dwordAt0C_;
    std::uint32_t dwordAt10_;
    std::uint32_t flagsAt14_;
    std::uint32_t flagsAt18_;
    std::uint32_t flagsAt1C_;
    std::uint32_t flagsAt20_;
    std::array<std::uint8_t, 8> bytesAt24_;
    union { RuntimeCString stringAt2C_; };
    EnvironmentVectorStorage<RuntimeCString> mappingsAt30_;
    union { UnSaveDataStringArrayReference arrayReferenceAt3C_; };
    std::uint32_t dwordAt48_;
};
static_assert(sizeof(UnSaveDataRuntime) == 0x4C);
static_assert(alignof(UnSaveDataRuntime) == 4);
static_assert(offsetof(UnSaveDataRuntime, modeAt04_) == 0x04);
static_assert(offsetof(UnSaveDataRuntime, bytesAt08_) == 0x08);
static_assert(offsetof(UnSaveDataRuntime, dwordAt0C_) == 0x0C);
static_assert(offsetof(UnSaveDataRuntime, dwordAt10_) == 0x10);
static_assert(offsetof(UnSaveDataRuntime, flagsAt14_) == 0x14);
static_assert(offsetof(UnSaveDataRuntime, flagsAt18_) == 0x18);
static_assert(offsetof(UnSaveDataRuntime, flagsAt1C_) == 0x1C);
static_assert(offsetof(UnSaveDataRuntime, flagsAt20_) == 0x20);
static_assert(offsetof(UnSaveDataRuntime, bytesAt24_) == 0x24);
static_assert(offsetof(UnSaveDataRuntime, stringAt2C_) == 0x2C);
static_assert(offsetof(UnSaveDataRuntime, mappingsAt30_) == 0x30);
static_assert(offsetof(UnSaveDataRuntime, arrayReferenceAt3C_) == 0x3C);
static_assert(offsetof(UnSaveDataRuntime, dwordAt48_) == 0x48);

// 固定正文的外置借用适配器；正式晚取来源由TextWindowSoundSource/ScriptSoundLeafSource处理。
class BorrowedUnSaveDataSoundMappings final : public ISoundEffectMappingState
{
public:
    explicit BorrowedUnSaveDataSoundMappings(UnSaveDataRuntime& value) noexcept : value_(value) {}
    std::size_t Size() const noexcept override { return value_.Size(); }
    std::string_view ResourceNameAt(std::size_t port) const override { return value_.ResourceNameAt(port); }
    void ClearAll() override { value_.ClearAll(); }
    void Clear(std::size_t port) override { value_.Clear(port); }
    void Store(std::size_t port, std::string_view name) override { value_.Store(port, name); }
    void StorePatternName(std::size_t port, ScrResourceNameReceiver receiver, std::int32_t id) override
        { value_.StorePatternName(port, receiver, id); }
private:
    UnSaveDataRuntime& value_;
};
}
