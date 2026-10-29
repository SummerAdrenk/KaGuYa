#pragma once

#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/EnvironmentRootRuntime.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

namespace KaGuYa::Start::Runtime
{
struct ParamsDemo; // strict/fixture 的显式 wire DTO，不是原生正文。
enum class ParamsDemoCommandType : std::uint32_t
{
    End, Next, Wait, Sound, Load, Transit, Disp, Update, Move, Pos,
};

// 正文首字是 type，不是虚表。只供 raw 分配及 reader 分阶段构造，不值初始化整块。
struct ParamsDemoCmdEnd final { std::uint32_t typeAt00; };
struct ParamsDemoCmdNext final { std::uint32_t typeAt00; };
struct ParamsDemoCmdWait final
{
    std::uint32_t typeAt00;
    std::uint8_t modeAt04;
    std::array<std::byte, 3> unwrittenAt05;
    std::uint32_t ticksAt08;
};
struct ParamsDemoCmdSound final
{
    std::uint32_t typeAt00;
    std::uint8_t modeAt04;
    std::uint8_t valueAt05;
    std::array<std::byte, 2> unwrittenAt06;
    RuntimeCString nameAt08;
};
struct ParamsDemoCmdLoad final
{
    std::uint32_t typeAt00;
    std::uint32_t slotAt04;
    RuntimeCString nameAt08;
};
struct ParamsDemoCmdTransit final
{
    std::uint32_t typeAt00;
    RuntimeCString effectAt04;
    std::uint32_t stepAt08;
    RuntimeCString maskAt0C;
};
struct ParamsDemoCmdDisp final
{
    std::uint32_t typeAt00;
    std::uint32_t slotAt04;
    std::uint8_t visibleAt08;
    std::array<std::byte, 3> unwrittenAt09;
};
struct ParamsDemoCmdUpdate final { std::uint32_t typeAt00; };
struct ParamsDemoCmdMove final
{
    std::uint32_t typeAt00;
    std::uint32_t slotAt04;
    std::uint32_t durationAt08;
    std::uint32_t startXAt0C;
    std::uint32_t startYAt10;
    std::uint32_t endXAt14;
    std::uint32_t endYAt18;
};
struct ParamsDemoCmdPos final
{
    std::uint32_t typeAt00;
    std::uint32_t slotAt04;
    std::uint32_t xAt08;
    std::uint32_t yAt0C;
};
static_assert(sizeof(ParamsDemoCmdEnd) == 4 && sizeof(ParamsDemoCmdNext) == 4);
static_assert(sizeof(ParamsDemoCmdWait) == 0x0C && offsetof(ParamsDemoCmdWait, ticksAt08) == 8);
static_assert(sizeof(ParamsDemoCmdSound) == 0x0C && offsetof(ParamsDemoCmdSound, nameAt08) == 8);
static_assert(sizeof(ParamsDemoCmdLoad) == 0x0C && offsetof(ParamsDemoCmdLoad, nameAt08) == 8);
static_assert(sizeof(ParamsDemoCmdTransit) == 0x10);
static_assert(offsetof(ParamsDemoCmdTransit, effectAt04) == 4 && offsetof(ParamsDemoCmdTransit, maskAt0C) == 0x0C);
static_assert(sizeof(ParamsDemoCmdDisp) == 0x0C && offsetof(ParamsDemoCmdDisp, visibleAt08) == 8);
static_assert(sizeof(ParamsDemoCmdUpdate) == 4);
static_assert(sizeof(ParamsDemoCmdMove) == 0x1C && offsetof(ParamsDemoCmdMove, endYAt18) == 0x18);
static_assert(sizeof(ParamsDemoCmdPos) == 0x10 && offsetof(ParamsDemoCmdPos, yAt0C) == 0x0C);

using ParamsDemoCommandVector = EnvironmentVectorStorage<YtlReferenceSlotRuntime>;
struct ParamsDemoDataRuntime final
{
    ParamsDemoCommandVector commandsAt00;
};
struct ParamsDemoEntryRuntime final
{
    RuntimeCString nameAt00;
    YtlReferenceSlotRuntime dataAt04;
};
static_assert(sizeof(ParamsDemoDataRuntime) == 0x0C);
static_assert(sizeof(ParamsDemoEntryRuntime) == 0x10 && offsetof(ParamsDemoEntryRuntime, dataAt04) == 4);

[[nodiscard]] std::uint32_t ParamsDemoCommandBodySize(ParamsDemoCommandType type);
// 只分配对应大小，不写 type/数值/padding，也不取得 CString 清理资格。
[[nodiscard]] void* AllocateParamsDemoCommandBody(ParamsDemoCommandType type,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
// 对已完成正文执行原 typed payload 删除；不适用于 reader 尚未完成的含串正文。
// CString Free 失败不继续释放后串或正文；入口自身可抛。
void DestroyParamsDemoCommandBody(ParamsDemoCommandType type, void* body);

// 接收 reader 已完整构造的正文，重置的是新 local Ref，绝不隐式释放旧拥有者。
// 先 base，再 new control；refs/owning/stride 在发布前，count 在发布后写缓存 control。
// body/control 失败不补强回滚；返回值供原快路径缓存 control，不能恢复回调改过的 base。
[[nodiscard]] EnvironmentObjectReferenceControl* PublishParamsDemoCommandReference(
    YtlReferenceSlotRuntime& local, ParamsDemoCommandType type, void* constructedBody,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
void ReleaseParamsDemoCommandAt40D3D0(YtlReferenceSlotRuntime& local) noexcept;
void ReleaseParamsDemoCommandCached(YtlReferenceSlotRuntime& local,
    YtlReferenceControlRuntime* capturedControl) noexcept;

// 408B70 总是重分配，可用于中插；position/source 由调用者保证合法且回调期间存活。
[[nodiscard]] YtlReferenceSlotRuntime* InsertParamsDemoCommandAt408B70(
    ParamsDemoCommandVector& storage, YtlReferenceSlotRuntime* position,
    const YtlReferenceSlotRuntime& source,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
void AppendParamsDemoCommandAt43D350(ParamsDemoCommandVector& storage,
    const YtlReferenceSlotRuntime& source,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
void ClearParamsDemoCommandsAt419F71(ParamsDemoCommandVector& storage) noexcept;
void DestroyParamsDemoDataAt40DF10(ParamsDemoDataRuntime& body,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory()) noexcept;

// 可抛的显式 Ref 释放；与命令 40D3D0 的整体终止边界不同。
void ReleaseParamsDemoDataAt43B7D0(YtlReferenceSlotRuntime& reference);
void ReleaseParamsDemoEntriesAt43B890(YtlReferenceSlotRuntime& reference);
[[nodiscard]] ParamsDemoDataRuntime* RebuildParamsDemoDataAt41CF13(
    YtlReferenceSlotRuntime& reference,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
void ConstructParamsDemoEntryAt40C810(ParamsDemoEntryRuntime& entry,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
void DestroyParamsDemoEntryAt40FB50(ParamsDemoEntryRuntime& entry) noexcept;
// 从 41CDCC 开始：先释放旧 Ref，count=0 不再分配。wire 零计数留旧由 reader 在调用前判断。
// new[] cookie 使用原数组分配口；memory 只承接 scalar control 分配。
[[nodiscard]] ParamsDemoEntryRuntime* RebuildParamsDemoEntriesAt41CDCC(
    YtlReferenceSlotRuntime& reference, std::uint32_t count,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory(),
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());

[[nodiscard]] void* ResolveParamsDemoCommand(const YtlReferenceSlotRuntime& reference) noexcept;
[[nodiscard]] ParamsDemoDataRuntime* ResolveParamsDemoDataAt4A52A0(
    const YtlReferenceSlotRuntime& reference) noexcept;
[[nodiscard]] ParamsDemoEntryRuntime* ResolveParamsDemoEntry(
    const YtlReferenceSlotRuntime& reference, std::uint32_t index) noexcept;
// 返回 control+10 的元素数，不是 control+04 的引用计数；Resolve 均不做 count 检查。
[[nodiscard]] std::uint32_t ParamsDemoReferenceCount(const YtlReferenceSlotRuntime& reference) noexcept;

// GS+B8 的唯一 owning Ref12；不持有 DTO、预解码 program 或同步名称表。
// 现代 profile 复制走独立深快照，不代表原 Ref 增持复制。
class ParamsDemoStorage final
{
public:
    ParamsDemoStorage() noexcept = default;
    ~ParamsDemoStorage() noexcept;
    ParamsDemoStorage(const ParamsDemoStorage& other);
    ParamsDemoStorage& operator=(const ParamsDemoStorage& other);
    ParamsDemoStorage(ParamsDemoStorage&& other) noexcept;
    ParamsDemoStorage& operator=(ParamsDemoStorage&& other);
    // 工具导入使用 DTO 的 CP932 名字及严格 payload 解码，不保留 wire/decoded 镜像。
    void AssignSnapshot(std::span<const ParamsDemo> values);
    // 按原生字段深复制，不经 wire 回转；保留 DWORD、原始 byte 和内嵌 NUL。
    void AssignSnapshot(const ParamsDemoStorage& source);
    [[nodiscard]] bool operator==(const ParamsDemoStorage& other) const;
    void Release();
    [[nodiscard]] std::uint32_t size() const noexcept;
    [[nodiscard]] bool empty() const noexcept { return size() == 0; }
    [[nodiscard]] ParamsDemoEntryRuntime* Resolve(std::uint32_t index) noexcept;
    [[nodiscard]] const ParamsDemoEntryRuntime* Resolve(std::uint32_t index) const noexcept;
    YtlReferenceSlotRuntime storage{};
};
static_assert(sizeof(ParamsDemoStorage) == 12 && offsetof(ParamsDemoStorage, storage) == 0);
}
