#pragma once

#include <Runtime/EnvironmentRootRuntime.h>

#include <cstddef>
#include <cstdint>

namespace KaGuYa::Start::Runtime
{
using ParamsPatternReferenceVector = EnvironmentVectorStorage<YtlReferenceSlotRuntime>;

// 43A760 无整体终止帧；范围释放终止，但最终外块删除仍可抛。
void DestroyIntegersAt43A760(ParamsPatternReferenceVector& storage,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());

struct ParamsPatternNative final
{
    std::uint8_t legacyByte00;
    std::uint8_t unwrittenAt01[3];
    ParamsPatternReferenceVector itemsAt04;
    ParamsPatternReferenceVector integersAt10;
    ParamsPatternReferenceVector firstGroupsAt1C;
    ParamsPatternReferenceVector secondGroupsAt28;

    // 434DA4..434DF1 只清四组三指针，首四字节保持申请所得内容。
    ParamsPatternNative() noexcept;
    ~ParamsPatternNative() noexcept;
    ParamsPatternNative(const ParamsPatternNative&) = delete;
    ParamsPatternNative& operator=(const ParamsPatternNative&) = delete;

    // 43CB30 的成员清理部分；不删除 this。具体 control 另删缓存的34h正文。
    void DestroyStorage(IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory()) noexcept;
};
static_assert(sizeof(ParamsPatternNative) == 0x34);
static_assert(offsetof(ParamsPatternNative, legacyByte00) == 0);
static_assert(offsetof(ParamsPatternNative, itemsAt04) == 0x04);
static_assert(offsetof(ParamsPatternNative, integersAt10) == 0x10);
static_assert(offsetof(ParamsPatternNative, firstGroupsAt1C) == 0x1C);
static_assert(offsetof(ParamsPatternNative, secondGroupsAt28) == 0x28);

// 434D61..434E50：退当前Ref，先发布正文，再申请/发布control；不调用reader。
// 注入块须兼容global delete；成功control不附加allocator或旁路拥有表。
void RebuildParamsPatternAt434D61(YtlReferenceSlotRuntime& slot,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
[[nodiscard]] ParamsPatternNative* ResolveParamsPattern(const YtlReferenceSlotRuntime& slot) noexcept;

// 唯一Ref12；复制是浅增持，和未来工具深快照严格区分。
class ParamsPatternReference final
{
public:
    ParamsPatternReference() noexcept = default;
    ~ParamsPatternReference() noexcept;
    ParamsPatternReference(const ParamsPatternReference& other) noexcept;
    ParamsPatternReference& operator=(const ParamsPatternReference& other);
    ParamsPatternReference(ParamsPatternReference&& other) noexcept;
    ParamsPatternReference& operator=(ParamsPatternReference&& other);

    void reset();
    [[nodiscard]] ParamsPatternNative* get() const noexcept { return ResolveParamsPattern(slot_); }
    [[nodiscard]] ParamsPatternNative* operator->() const noexcept { return get(); }
    [[nodiscard]] ParamsPatternNative& operator*() const { return *get(); }
    [[nodiscard]] explicit operator bool() const noexcept { return get() != nullptr; }
    [[nodiscard]] bool operator==(const ParamsPatternReference& other) const noexcept
    { return get() == other.get(); }
    [[nodiscard]] std::uint32_t use_count() const noexcept
    { return slot_.controlAt08 ? slot_.controlAt08->referenceCountAt04 : 0; }
    [[nodiscard]] YtlReferenceSlotRuntime& Native() noexcept { return slot_; }
    [[nodiscard]] const YtlReferenceSlotRuntime& Native() const noexcept { return slot_; }

    // 仅工具/夹具：显式定义legacy byte，失败回收正文；不是434D61的失败协议。
    [[nodiscard]] static ParamsPatternReference CreateFixture(std::uint8_t legacyByte = 0);
private:
    YtlReferenceSlotRuntime slot_;
};
static_assert(sizeof(ParamsPatternReference) == 12);
}
