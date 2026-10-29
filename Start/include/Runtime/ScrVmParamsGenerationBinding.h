#pragma once

#include <Runtime/ScrVmParamsGeneration.h>

#include <cstddef>
#include <memory>
#include <utility>

namespace KaGuYa::Start::Runtime
{
class ScrVmParamsGenerationBinding;
[[nodiscard]] bool operator==(const ScrVmParamsGenerationBinding&,
    const ScrVmParamsGenerationBinding&) noexcept;

// 区分显式fixture的slot拥有适配与root+1C0借用，不分配正文或触发47BDB0。
// 借用必须短于对应payload；跨payload的服务仍须在原调用位置重新解根。
class ScrVmParamsGenerationBinding final
{
public:
    ScrVmParamsGenerationBinding() noexcept = default;
    // 保留旧shared槽入口的nullptr调用；消费者仍显式拒绝空绑定，不自动创建。
    ScrVmParamsGenerationBinding(std::nullptr_t) noexcept {}

    // 此shared_ptr仅管理外部slot适配器；两个子正文及CParams只有原生Ref拥有链。
    ScrVmParamsGenerationBinding(std::shared_ptr<ScrVmParamsGenerationSlot> owner) noexcept
        : owner_(std::move(owner)), slot_(owner_.get()) {}

    [[nodiscard]] static ScrVmParamsGenerationBinding Borrowed(ScrVmParamsGenerationSlot& slot) noexcept
    {
        ScrVmParamsGenerationBinding binding;
        binding.slot_ = &slot;
        return binding;
    }

    ScrVmParamsGenerationBinding(const ScrVmParamsGenerationBinding&) noexcept = default;
    ScrVmParamsGenerationBinding(ScrVmParamsGenerationBinding&& other) noexcept
        : owner_(std::move(other.owner_)), slot_(std::exchange(other.slot_, nullptr)) {}

    ScrVmParamsGenerationBinding& operator=(ScrVmParamsGenerationBinding other) noexcept
    {
        // 先发布新绑定，再由临时对象释放旧owning；移动源不得留下隐式借用。
        swap(other);
        return *this;
    }

    void swap(ScrVmParamsGenerationBinding& other) noexcept
    {
        owner_.swap(other.owner_);
        std::swap(slot_, other.slot_);
    }

    // 只撤销这一份绑定；不调用Slot::Release，也不改其他视图的绑定。
    void reset() noexcept { ScrVmParamsGenerationBinding{}.swap(*this); }

    [[nodiscard]] ScrVmParamsGenerationSlot* get() const noexcept { return slot_; }
    [[nodiscard]] ScrVmParamsGenerationSlot* operator->() const noexcept { return slot_; }
    [[nodiscard]] ScrVmParamsGenerationSlot& operator*() const noexcept { return *slot_; }
    [[nodiscard]] explicit operator bool() const noexcept { return slot_ != nullptr; }

    friend bool operator==(const ScrVmParamsGenerationBinding& left,
        const ScrVmParamsGenerationBinding& right) noexcept
    {
        return left.slot_ == right.slot_;
    }

private:
    std::shared_ptr<ScrVmParamsGenerationSlot> owner_;
    ScrVmParamsGenerationSlot* slot_ = nullptr;
};
}
