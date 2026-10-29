#pragma once

#include <Runtime/ReadScriptEffectNative.h>
#include <Runtime/TransitRuntime.h>

namespace KaGuYa::Start::Runtime
{
class IDrawTransitionViewSource;
class EnvironmentRootOwner;
struct EnvironmentRootStorage;
struct YtlReferenceSlotRuntime;

// 49BAC0/49BEE0/49D640/49D7F0：四类共用3Ch布局，但各有原四槽表。
// union阻止现代packet默认初始化提前发生；+05..07、+38在原写入点之前不动。
struct ReadScriptPacketNativeBody3Ch final
{
    ReadScriptEffectNativeBase8h baseAt00;
    union { TransitOperation packetAt08; };
    std::int32_t stepAt38;
    ReadScriptPacketNativeBody3Ch() noexcept {}
};
static_assert(std::is_standard_layout_v<ReadScriptPacketNativeBody3Ch>);
static_assert(sizeof(ReadScriptPacketNativeBody3Ch) == 0x3C);
static_assert(offsetof(ReadScriptPacketNativeBody3Ch, baseAt00) == 0);
static_assert(offsetof(ReadScriptPacketNativeBody3Ch, packetAt08) == 8);
static_assert(offsetof(ReadScriptPacketNativeBody3Ch, stepAt38) == 0x38);
static_assert(offsetof(TransitOperation, source) == 4);
static_assert(std::is_same_v<decltype(TransitOperation::source), GraphicsSurfaceNativeBody*>);

// 每次构造独立取根两次：先slot1固定Save指针，再slot0，最后读先前Save+70。
class IReadScriptPacketEnvironmentSource
{
public:
    virtual ~IReadScriptPacketEnvironmentSource() = default;
    virtual EnvironmentRootStorage* CaptureEnvironmentAt4180C0() = 0;
};
class RootReadScriptPacketEnvironmentSource final : public IReadScriptPacketEnvironmentSource
{
public:
    explicit RootReadScriptPacketEnvironmentSource(EnvironmentRootOwner& owner) noexcept : owner_(owner) {}
    EnvironmentRootStorage* CaptureEnvironmentAt4180C0() override;
private:
    EnvironmentRootOwner& owner_;
};

struct ReadScriptPacketNativeServices final
{
    IDrawTransitionViewSource& draw;
    IGraphicsScreenCalls& screen;
};
// 只在需要Render时借服务；Ready、空Draw、早退Tick、未ready删除无需scope。
class ReadScriptPacketNativeScope final
{
public:
    explicit ReadScriptPacketNativeScope(ReadScriptPacketNativeServices&) noexcept;
    ~ReadScriptPacketNativeScope() noexcept;
    ReadScriptPacketNativeScope(const ReadScriptPacketNativeScope&) = delete;
    ReadScriptPacketNativeScope& operator=(const ReadScriptPacketNativeScope&) = delete;
private:
    ReadScriptPacketNativeServices* previous_;
};

const ReadScriptEffectNativeVtable& ReadScriptBlendNativeMethods() noexcept;
const ReadScriptEffectNativeVtable& ReadScriptMosaicNativeMethods() noexcept;
const ReadScriptEffectNativeVtable& ReadScriptTransitNativeMethods() noexcept;
const ReadScriptEffectNativeVtable& ReadScriptUniversalNativeMethods() noexcept;

ReadScriptPacketNativeBody3Ch* ConstructReadScriptBlendNativeAt49BAC0(ReadScriptPacketNativeBody3Ch&,
    const YtlReferenceSlotRuntime& source, const YtlReferenceSlotRuntime& previous,
    IReadScriptPacketEnvironmentSource&);
ReadScriptPacketNativeBody3Ch* ConstructReadScriptMosaicNativeAt49BEE0(ReadScriptPacketNativeBody3Ch&,
    const YtlReferenceSlotRuntime& source, const YtlReferenceSlotRuntime& previous,
    const YtlReferenceSlotRuntime& work, IReadScriptPacketEnvironmentSource&);
ReadScriptPacketNativeBody3Ch* ConstructReadScriptTransitNativeAt49D640(ReadScriptPacketNativeBody3Ch&,
    const YtlReferenceSlotRuntime& source, std::int32_t opcode, IReadScriptPacketEnvironmentSource&);
ReadScriptPacketNativeBody3Ch* ConstructReadScriptUniversalNativeAt49D7F0(ReadScriptPacketNativeBody3Ch&,
    const YtlReferenceSlotRuntime& source, const YtlReferenceSlotRuntime& previous,
    const YtlReferenceSlotRuntime& mask, IReadScriptPacketEnvironmentSource&);

std::uint8_t TickReadScriptBlendNativeAt4BB750(ReadScriptPacketNativeBody3Ch&);
std::uint8_t TickReadScriptMosaicNativeAt4BB8D0(ReadScriptPacketNativeBody3Ch&);
std::uint8_t TickReadScriptTransitNativeAt4BBEA0(ReadScriptPacketNativeBody3Ch&);
std::uint8_t TickReadScriptUniversalNativeAt4BBF40(ReadScriptPacketNativeBody3Ch&);
// 原完成就是析构；无Finish槽或重复调用guard，末尾写回共同基类表且不清ready。
void DestroyReadScriptBlendNativeAt4A3FC0(ReadScriptPacketNativeBody3Ch&) noexcept;
void DestroyReadScriptMosaicNativeAt4A41C0(ReadScriptPacketNativeBody3Ch&) noexcept;
void DestroyReadScriptTransitNativeAt4A4590(ReadScriptPacketNativeBody3Ch&) noexcept;
void DestroyReadScriptUniversalNativeAt4A4650(ReadScriptPacketNativeBody3Ch&) noexcept;
ReadScriptPacketNativeBody3Ch* DeleteReadScriptBlendNativeAt4A63D0(ReadScriptPacketNativeBody3Ch&, std::uint32_t) noexcept;
ReadScriptPacketNativeBody3Ch* DeleteReadScriptMosaicNativeAt4A64E0(ReadScriptPacketNativeBody3Ch&, std::uint32_t) noexcept;
ReadScriptPacketNativeBody3Ch* DeleteReadScriptTransitNativeAt4A66D0(ReadScriptPacketNativeBody3Ch&, std::uint32_t) noexcept;
ReadScriptPacketNativeBody3Ch* DeleteReadScriptUniversalNativeAt4A67B0(ReadScriptPacketNativeBody3Ch&, std::uint32_t) noexcept;

// 只申请3Ch正文，不创建/接管Ref控制块；申请异常在原构造try之外。
[[nodiscard]] ReadScriptPacketNativeBody3Ch* NewReadScriptBlendNative(const YtlReferenceSlotRuntime& source,
    const YtlReferenceSlotRuntime& previous, IReadScriptPacketEnvironmentSource&);
[[nodiscard]] ReadScriptPacketNativeBody3Ch* NewReadScriptMosaicNative(const YtlReferenceSlotRuntime& source,
    const YtlReferenceSlotRuntime& previous, const YtlReferenceSlotRuntime& work, IReadScriptPacketEnvironmentSource&);
[[nodiscard]] ReadScriptPacketNativeBody3Ch* NewReadScriptTransitNative(const YtlReferenceSlotRuntime& source,
    std::int32_t opcode, IReadScriptPacketEnvironmentSource&);
[[nodiscard]] ReadScriptPacketNativeBody3Ch* NewReadScriptUniversalNative(const YtlReferenceSlotRuntime& source,
    const YtlReferenceSlotRuntime& previous, const YtlReferenceSlotRuntime& mask, IReadScriptPacketEnvironmentSource&);
}
