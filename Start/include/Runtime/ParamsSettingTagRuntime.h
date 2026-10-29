#pragma once

#include <Runtime/EnvironmentRootRuntime.h>

#include <cstddef>

namespace KaGuYa::Start::Runtime
{
struct ParamsSettingTag; // Profile.h 中的现代 DTO，不是原生正文。

struct ParamsSettingPairRuntime final
{
    RuntimeCString keyAt00;
    RuntimeCString valueAt04;
};
static_assert(sizeof(ParamsSettingPairRuntime) == 8);
static_assert(offsetof(ParamsSettingPairRuntime, valueAt04) == 4);

// 仅由原生 helper 逐字段构造/销毁；不得普通 new/delete 此聚合。
struct ParamsSettingTagRuntime final
{
    RuntimeCString nameAt00;
    EnvironmentVectorStorage<ParamsSettingPairRuntime> pairsAt04;
    EnvironmentVectorStorage<YtlReferenceSlotRuntime> childrenAt10;
};
static_assert(sizeof(ParamsSettingTagRuntime) == 0x1C);
static_assert(offsetof(ParamsSettingTagRuntime, nameAt00) == 0);
static_assert(offsetof(ParamsSettingTagRuntime, pairsAt04) == 4);
static_assert(offsetof(ParamsSettingTagRuntime, childrenAt10) == 0x10);

// 包含先释放旧槽；GetNil/control 分配失败不回滚 body。注入分配须兼容普通 delete。
void RebuildParamsSettingTagAt41FE80(YtlReferenceSlotRuntime& out,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
[[nodiscard]] ParamsSettingTagRuntime* ResolveParamsSettingTagAt410560(
    const YtlReferenceSlotRuntime& slot) noexcept;
// 工厂显式释放可抛；40D910/Reference::Reset 则提供整个入口的终止边界。
void ReleaseParamsSettingTagAt41FEAE(YtlReferenceSlotRuntime& slot);

// destination 是未构造的八字节空间；source 是 reader 借用 EDI/ESI 两个 chars 的 raw pair。
// 入口缓存两个 header，不在第一次 Clone 回调后重读循环临时 CString 或 source 第二槽。
// 第二次 Clone 失败仅展开已构造的 key，不默认构造 nil，也不提前解析父节点。
void CloneParamsSettingPairAt42029C(ParamsSettingPairRuntime& destination,
    const ParamsSettingPairRuntime& source);
// 正常路径先撤回 pair 清理资格后调用，可抛；value Free 后才读取 key。
void ReleaseParamsSettingPairAt420324(ParamsSettingPairRuntime& pair);
// 展开路径的原 40CE90 整体 flags5；成功后不得再执行普通聚合析构。
void DestroyParamsSettingPairAt40CE90(ParamsSettingPairRuntime& pair) noexcept;
void AppendParamsSettingPairAt4202CF(
    EnvironmentVectorStorage<ParamsSettingPairRuntime>& storage,
    const ParamsSettingPairRuntime& source,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
void DestroyParamsSettingPairsAt40DC90(
    EnvironmentVectorStorage<ParamsSettingPairRuntime>& storage,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory()) noexcept;
// 独立 body 析构整体 noexcept；control payload43CBC0 不调用此入口。
void DestroyParamsSettingTagAt40FAF0(ParamsSettingTagRuntime& body,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory()) noexcept;

class ParamsSettingTagReference final
{
public:
    ParamsSettingTagReference() noexcept = default;
    ~ParamsSettingTagReference() noexcept;
    // 仅现代 profile 深快照，不代表原 Ref 增持复制；无第二份值树。
    ParamsSettingTagReference(const ParamsSettingTagReference& other);
    ParamsSettingTagReference& operator=(const ParamsSettingTagReference& other);
    ParamsSettingTagReference(ParamsSettingTagReference&& other) noexcept;
    ParamsSettingTagReference& operator=(ParamsSettingTagReference&& other) noexcept;
    void AssignSnapshot(const ParamsSettingTag& source);
    void AssignSnapshot(const ParamsSettingTagRuntime& source);

    [[nodiscard]] YtlReferenceSlotRuntime& Native() noexcept { return slot; }
    [[nodiscard]] const YtlReferenceSlotRuntime& Native() const noexcept { return slot; }
    [[nodiscard]] ParamsSettingTagRuntime* Get() noexcept;
    [[nodiscard]] const ParamsSettingTagRuntime* Get() const noexcept;
    void Reset() noexcept;
    [[nodiscard]] bool operator==(const ParamsSettingTagReference& other) const;

    YtlReferenceSlotRuntime slot{};
};
static_assert(sizeof(ParamsSettingTagReference) == 12);
static_assert(offsetof(ParamsSettingTagReference, slot) == 0);
}
