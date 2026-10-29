#pragma once

#include <Runtime/YtlReferenceArrayRuntime.h>
#include <new>

namespace KaGuYa::Start::Runtime
{
// 4983AE/49846E：非数组Ref控制块，不在24-byte原布局内附加分配器或现代owner。
class EnvironmentObjectReferenceControl : public YtlReferenceControlRuntime
{
public:
    std::uint32_t strideAt0C;
    std::uint32_t countAt10;
    void* payloadAt14;
protected:
    explicit EnvironmentObjectReferenceControl(void* payload) noexcept : payloadAt14(payload) {}
    EnvironmentObjectReferenceControl(void* payload, UninitializedFieldsTag tag) noexcept
        : YtlReferenceControlRuntime(tag), payloadAt14(payload) {}
};
static_assert(sizeof(EnvironmentObjectReferenceControl) == 0x18);

void RetireEnvironmentObjectControl(EnvironmentObjectReferenceControl* control, std::uint32_t flags) noexcept;

template<class T>
class EnvironmentObjectControl final : public EnvironmentObjectReferenceControl
{
public:
    using EnvironmentObjectReferenceControl::UninitializedFieldsTag;
    explicit EnvironmentObjectControl(T* payload) noexcept : EnvironmentObjectReferenceControl(payload) {}
    // 只在显式分阶段发布入口使用；其它现有control构造口不改变其默认合同。
    EnvironmentObjectControl(T* payload, UninitializedFieldsTag tag) noexcept
        : EnvironmentObjectReferenceControl(payload, tag) {}
    void DestroyPayloadAtVslot00() override { delete static_cast<T*>(payloadAt14); }
    void DeleteControlAtVslot04(std::uint32_t flags) override { RetireEnvironmentObjectControl(this, flags); }
};

// 调用者已完成新正文构造；498348/498408此时才释放旧槽。失败不能用unique_ptr回滚新正文。
template<class T>
void ReplaceEnvironmentObjectReference(YtlReferenceSlotRuntime& slot, T* created,
    std::uint32_t originalStride)
{
    static_assert(sizeof(EnvironmentObjectControl<T>) == 0x18);
    ReleaseYtlReference(slot);
    if (!created) return;
    // 498388/498448保留第二次检查；只删控制块，不再减计数或释放正文。
    if (slot.controlAt08) slot.controlAt08->DeleteControlAtVslot04(1);
    slot.biasAt04 = 0;
    slot.baseAt00 = reinterpret_cast<std::uintptr_t>(created);
    // 4983A3/498463：只构造已知字段，不让/sdl的普通new初始化改写+09..+0B。
    void* storage = ::operator new(sizeof(EnvironmentObjectControl<T>));
    auto* control = storage ? ::new (storage) EnvironmentObjectControl<T>(
        created, typename EnvironmentObjectControl<T>::UninitializedFieldsTag{}) : nullptr;
    slot.controlAt08 = control;
    control->referenceCountAt04 = 1;
    // 4983C5/4983CC/4983D6以及498485/49848C/498496每次从已发布槽重读。
    slot.controlAt08->owningAt08 = 1;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->strideAt0C = originalStride;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->countAt10 = 1;
}

template<class T>
[[nodiscard]] T* ResolveEnvironmentObjectReference(const YtlReferenceSlotRuntime& slot) noexcept
{
    // 43D230(0)：base为空才返回null；control为空时stride为0，不拒绝部分发布正文。
    if (!slot.baseAt00) return nullptr;
    const auto stride = slot.controlAt08 ?
        static_cast<const EnvironmentObjectReferenceControl*>(slot.controlAt08)->strideAt0C : 0u;
    return reinterpret_cast<T*>(slot.baseAt00 + slot.biasAt04 * stride);
}
}
