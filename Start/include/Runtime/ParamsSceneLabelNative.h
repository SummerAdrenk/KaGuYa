#pragma once

#include <Runtime/EnvironmentRootRuntime.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

namespace KaGuYa::Start::Runtime
{
struct ParamsSceneLabel;
class ParamsRuntimeReadCursor;

// 41F934：只构造首个 CString；后两个 DWORD 保留分配所得字节。
struct ParamsSceneLabelEntryRuntime final
{
    // 仅供原生分配后的逐字段构造；43C7E0 手动 raw Free 后删除正文，禁止普通 C++ 构析。
    ParamsSceneLabelEntryRuntime() = delete;
    ~ParamsSceneLabelEntryRuntime() = delete;
    union { RuntimeCString nameAt00; };
    std::uint32_t value1At04;
    std::uint32_t value2At08;
};
static_assert(sizeof(ParamsSceneLabelEntryRuntime) == 0x0C);
static_assert(offsetof(ParamsSceneLabelEntryRuntime, value1At04) == 4);
static_assert(offsetof(ParamsSceneLabelEntryRuntime, value2At08) == 8);

// 434ECE：首 DWORD 不写；+04 才是三指针 Ref12 vector，无正文 vptr。
class ParamsSceneLabelNative final
{
public:
    std::uint8_t legacyByteAt00;
    std::array<std::byte, 3> unwrittenAt01;
    EnvironmentVectorStorage<YtlReferenceSlotRuntime> labelsAt04;

    ParamsSceneLabelNative() noexcept;
    ~ParamsSceneLabelNative() noexcept;
    ParamsSceneLabelNative(const ParamsSceneLabelNative& other);
    ParamsSceneLabelNative& operator=(const ParamsSceneLabelNative& other);
    ParamsSceneLabelNative(ParamsSceneLabelNative&& other) noexcept;
    ParamsSceneLabelNative& operator=(ParamsSceneLabelNative&& other) noexcept;

    [[nodiscard]] std::size_t size() const noexcept;
    [[nodiscard]] bool empty() const noexcept { return labelsAt04.begin == labelsAt04.end; }
    [[nodiscard]] ParamsSceneLabelEntryRuntime* Resolve(std::uint32_t index) noexcept;
    [[nodiscard]] const ParamsSceneLabelEntryRuntime* Resolve(std::uint32_t index) const noexcept;
    [[nodiscard]] ParamsSceneLabelEntryRuntime& operator[](std::uint32_t index);
    [[nodiscard]] const ParamsSceneLabelEntryRuntime& operator[](std::uint32_t index) const;
    [[nodiscard]] ParamsSceneLabelEntryRuntime& at(std::size_t index);
    [[nodiscard]] const ParamsSceneLabelEntryRuntime& at(std::size_t index) const;
    void clear() noexcept;

    // 仅现代工具/fixture 深快照；不读取或复制未定义的 legacy/padding。
    void AssignSnapshot(std::span<const ParamsSceneLabel> source);
    [[nodiscard]] bool operator==(const ParamsSceneLabelNative& other) const;
};
static_assert(sizeof(ParamsSceneLabelNative) == 0x10);
static_assert(offsetof(ParamsSceneLabelNative, labelsAt04) == 4);

// 离线工具导出：按 CString 显式长度保留字节，UTF-16 由当前线程 ACP 重建，不保留原 wire 镜像。
[[nodiscard]] std::vector<ParamsSceneLabel> ExportParamsSceneLabelSnapshot(
    const ParamsSceneLabelNative& native);

void RebuildParamsSceneLabelAt434ECE(YtlReferenceSlotRuntime& out,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
[[nodiscard]] ParamsSceneLabelNative* ResolveParamsSceneLabelOwner(
    const YtlReferenceSlotRuntime& slot) noexcept;
[[nodiscard]] ParamsSceneLabelEntryRuntime* ResolveParamsSceneLabelEntry(
    const YtlReferenceSlotRuntime& slot) noexcept;

// 原 reader 的新局部 Ref 工厂：返回缓存正文，不从分配回调可改写的 out.base 反取。
[[nodiscard]] ParamsSceneLabelEntryRuntime* CreateParamsSceneLabelEntryAt41F934(
    YtlReferenceSlotRuntime& out,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
void ReleaseParamsSceneLabelAt40CFE0(YtlReferenceSlotRuntime& slot) noexcept;

// 41F8A0：<5窄串/XOR或>=5宽串；成功提交局部游标，失败保留已完成表项。
void ReadParamsSceneLabelsNativeAt41F8A0(ParamsRuntimeReadCursor& cursor,
    ParamsSceneLabelNative& native, double version,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());

// 唯一 Ref12。与 GS 接口一样，复制浅增持；深快照在 owner 显式执行。
class ParamsSceneLabelReference final
{
public:
    ParamsSceneLabelReference() noexcept = default;
    ~ParamsSceneLabelReference() noexcept;
    ParamsSceneLabelReference(const ParamsSceneLabelReference& other) noexcept;
    ParamsSceneLabelReference& operator=(const ParamsSceneLabelReference& other);
    ParamsSceneLabelReference(ParamsSceneLabelReference&& other) noexcept;
    ParamsSceneLabelReference& operator=(ParamsSceneLabelReference&& other);
    void reset();
    [[nodiscard]] ParamsSceneLabelNative* get() const noexcept
    { return ResolveParamsSceneLabelOwner(slot_); }
    [[nodiscard]] ParamsSceneLabelNative* operator->() const noexcept { return get(); }
    [[nodiscard]] ParamsSceneLabelNative& operator*() const { return *get(); }
    [[nodiscard]] explicit operator bool() const noexcept { return get() != nullptr; }
    [[nodiscard]] bool operator==(const ParamsSceneLabelReference& other) const noexcept
    { return get() == other.get(); }
    [[nodiscard]] std::uint32_t use_count() const noexcept
    { return slot_.controlAt08 ? slot_.controlAt08->referenceCountAt04 : 0; }
    [[nodiscard]] YtlReferenceSlotRuntime& Native() noexcept { return slot_; }
    [[nodiscard]] const YtlReferenceSlotRuntime& Native() const noexcept { return slot_; }
private:
    YtlReferenceSlotRuntime slot_{};
};
static_assert(sizeof(ParamsSceneLabelReference) == 12);
}
