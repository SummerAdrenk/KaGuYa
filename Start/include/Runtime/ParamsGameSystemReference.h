#pragma once

#include <Runtime/EnvironmentRootRuntime.h>
#include <Runtime/ParamsGameSystemRuntime.h>

namespace KaGuYa::Start::Runtime
{
// 434C60..434D16：先退旧槽，构造11Ch正文，再发布18h control。
// memory只注入申请和构造失败的原始块删除；成功control不附加allocator或weak字段。
void RebuildParamsGameSystemAt434C60(YtlReferenceSlotRuntime& slot,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory(),
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
[[nodiscard]] ParamsGameSystem* ResolveParamsGameSystem(
    const YtlReferenceSlotRuntime& slot) noexcept;

// 唯一Ref12；复制是浅增持，不是ParamsGameSystem的工具深快照。
class ParamsGameSystemReference final
{
public:
    ParamsGameSystemReference() noexcept = default;
    ~ParamsGameSystemReference() noexcept;
    ParamsGameSystemReference(const ParamsGameSystemReference& other) noexcept;
    ParamsGameSystemReference& operator=(const ParamsGameSystemReference& other);
    ParamsGameSystemReference(ParamsGameSystemReference&& other) noexcept;
    ParamsGameSystemReference& operator=(ParamsGameSystemReference&& other);

    void reset();
    // 496D86正常尾声用缓存ESI；异常展开仍由析构按当前Ref释放。
    void ReleaseCached(YtlReferenceControlRuntime* captured) noexcept;
    [[nodiscard]] ParamsGameSystem* get() const noexcept { return ResolveParamsGameSystem(slot_); }
    [[nodiscard]] ParamsGameSystem* operator->() const noexcept { return get(); }
    [[nodiscard]] ParamsGameSystem& operator*() const { return *get(); }
    [[nodiscard]] explicit operator bool() const noexcept { return get() != nullptr; }
    [[nodiscard]] bool operator==(const ParamsGameSystemReference& other) const noexcept
    { return get() == other.get(); }
    [[nodiscard]] std::uint32_t use_count() const noexcept
    { return slot_.controlAt08 ? slot_.controlAt08->referenceCountAt04 : 0; }
    [[nodiscard]] YtlReferenceSlotRuntime& Native() noexcept { return slot_; }
    [[nodiscard]] const YtlReferenceSlotRuntime& Native() const noexcept { return slot_; }

    // 仅现代夹具/工具入口：定义原未写标量，分配失败回收完整正文；生产root不调用。
    [[nodiscard]] static ParamsGameSystemReference CreateFixture();
private:
    YtlReferenceSlotRuntime slot_;
};
static_assert(sizeof(ParamsGameSystemReference) == 12);
}
