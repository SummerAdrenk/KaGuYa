#pragma once

#include <Runtime/EnvironmentRootRuntime.h>
#include <Runtime/RuntimeCStringArray.h>

#include <array>
#include <cstddef>
#include <cstdint>

namespace KaGuYa::Start::Runtime
{
// 只含原始正文。各 union 在 name 构造成功后才开始内 Ref 生命周期，不提前清零。
struct ParamsLegacyVoiceRuntime final
{
    explicit ParamsLegacyVoiceRuntime(
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
    ~ParamsLegacyVoiceRuntime() noexcept;
    ParamsLegacyVoiceRuntime(const ParamsLegacyVoiceRuntime&) = delete;
    ParamsLegacyVoiceRuntime& operator=(const ParamsLegacyVoiceRuntime&) = delete;

    std::uint8_t valueAt00;
    std::array<std::byte, 3> unwrittenAt01;
    union { RuntimeCString nameAt04; };
    union { RuntimeCStringArrayReference stringsAt08; };
    union { RuntimeCStringArrayReference stringsAt14; };
};

struct ParamsLegacyMultiRuntime final
{
    explicit ParamsLegacyMultiRuntime(
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
    ~ParamsLegacyMultiRuntime() noexcept;
    ParamsLegacyMultiRuntime(const ParamsLegacyMultiRuntime&) = delete;
    ParamsLegacyMultiRuntime& operator=(const ParamsLegacyMultiRuntime&) = delete;

    union { RuntimeCString nameAt00; };
    union { YtlReferenceSlotRuntime integersAt04; };
};

struct ParamsLegacySoundRuntime final
{
    explicit ParamsLegacySoundRuntime(
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
    ~ParamsLegacySoundRuntime() noexcept;
    ParamsLegacySoundRuntime(const ParamsLegacySoundRuntime&) = delete;
    ParamsLegacySoundRuntime& operator=(const ParamsLegacySoundRuntime&) = delete;

    union { RuntimeCString nameAt00; };
    union { RuntimeCStringArrayReference stringsAt04; };
    union { RuntimeCStringArrayReference stringsAt10; };
};

static_assert(sizeof(ParamsLegacyVoiceRuntime) == 0x20);
static_assert(offsetof(ParamsLegacyVoiceRuntime, valueAt00) == 0);
static_assert(offsetof(ParamsLegacyVoiceRuntime, nameAt04) == 4);
static_assert(offsetof(ParamsLegacyVoiceRuntime, stringsAt08) == 8);
static_assert(offsetof(ParamsLegacyVoiceRuntime, stringsAt14) == 0x14);
static_assert(sizeof(ParamsLegacyMultiRuntime) == 0x10);
static_assert(offsetof(ParamsLegacyMultiRuntime, nameAt00) == 0);
static_assert(offsetof(ParamsLegacyMultiRuntime, integersAt04) == 4);
static_assert(sizeof(ParamsLegacySoundRuntime) == 0x1C);
static_assert(offsetof(ParamsLegacySoundRuntime, nameAt00) == 0);
static_assert(offsetof(ParamsLegacySoundRuntime, stringsAt04) == 4);
static_assert(offsetof(ParamsLegacySoundRuntime, stringsAt10) == 0x10);

// 原位构造，storage 必须是足够大且对齐的未构造存储；失败项没有析构资格。
[[nodiscard]] ParamsLegacyVoiceRuntime* ConstructParamsLegacyVoiceAt40CA60(
    ParamsLegacyVoiceRuntime* storage,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
[[nodiscard]] ParamsLegacyMultiRuntime* ConstructParamsLegacyMultiAt40C8E0(
    ParamsLegacyMultiRuntime* storage,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
[[nodiscard]] ParamsLegacySoundRuntime* ConstructParamsLegacySoundAt40CA00(
    ParamsLegacySoundRuntime* storage,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
void DestroyParamsLegacyVoiceAt40FF30(ParamsLegacyVoiceRuntime& body) noexcept;
void DestroyParamsLegacyMultiAt40FC50(ParamsLegacyMultiRuntime& body) noexcept;
void DestroyParamsLegacySoundAt40FDF0(ParamsLegacySoundRuntime& body) noexcept;

// 从原释放起点开始：count=0 也先释放旧 Ref，不分配；wire 零计数留旧由后续 reader 判断。
// new[] 使用全局数组入口，memory 只分配 scalar control；返回缓存 created body，不恢复回调改过的槽。
[[nodiscard]] ParamsLegacyVoiceRuntime* RebuildParamsLegacyVoiceAt41B581(
    YtlReferenceSlotRuntime& reference, std::uint32_t count,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory(),
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
[[nodiscard]] ParamsLegacyMultiRuntime* RebuildParamsLegacyMultiAt41C0A6(
    YtlReferenceSlotRuntime& reference, std::uint32_t count,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory(),
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
[[nodiscard]] ParamsLegacySoundRuntime* RebuildParamsLegacySoundAt41C420(
    YtlReferenceSlotRuntime& reference, std::uint32_t count,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory(),
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());

// multi+04：无 cookie、无元素构造、不预清零；异常不增加正文回收资格。
[[nodiscard]] std::int32_t* RebuildParamsLegacyIntegersAt41C2DE(
    YtlReferenceSlotRuntime& reference, std::uint32_t count,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());

// 只分配18h并构造原整数数组 control 的 vptr/payload；不发布槽、不写引用字段。
// 分配空返回仍返回空，由各原调用点决定发布和故障顺序。
[[nodiscard]] EnvironmentObjectReferenceControl* AllocateParamsLegacyIntegerControl(
    std::int32_t* elements,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());

// 显式释放可抛；元素析构与 GS 析构的终止边界由各自调用者承担。
void ReleaseParamsLegacyTable(YtlReferenceSlotRuntime& reference);
// 只读当前 control+10，不扣 bias；Resolve 不检查 count，null control 的 stride 为0。
[[nodiscard]] std::uint32_t ParamsLegacyTableCount(const YtlReferenceSlotRuntime& reference) noexcept;
[[nodiscard]] ParamsLegacyVoiceRuntime* ResolveParamsLegacyVoice(
    YtlReferenceSlotRuntime& reference, std::uint32_t index) noexcept;
[[nodiscard]] const ParamsLegacyVoiceRuntime* ResolveParamsLegacyVoice(
    const YtlReferenceSlotRuntime& reference, std::uint32_t index) noexcept;
[[nodiscard]] ParamsLegacyMultiRuntime* ResolveParamsLegacyMulti(
    YtlReferenceSlotRuntime& reference, std::uint32_t index) noexcept;
[[nodiscard]] const ParamsLegacyMultiRuntime* ResolveParamsLegacyMulti(
    const YtlReferenceSlotRuntime& reference, std::uint32_t index) noexcept;
[[nodiscard]] ParamsLegacySoundRuntime* ResolveParamsLegacySound(
    YtlReferenceSlotRuntime& reference, std::uint32_t index) noexcept;
[[nodiscard]] const ParamsLegacySoundRuntime* ResolveParamsLegacySound(
    const YtlReferenceSlotRuntime& reference, std::uint32_t index) noexcept;
[[nodiscard]] std::int32_t* ResolveParamsLegacyInteger(
    YtlReferenceSlotRuntime& reference, std::uint32_t index) noexcept;
[[nodiscard]] const std::int32_t* ResolveParamsLegacyInteger(
    const YtlReferenceSlotRuntime& reference, std::uint32_t index) noexcept;

// 本模块不提供正文浅复制或现代深快照；GS 暂时仍需拒绝非空旧表的工具复制/比较。
}
