#pragma once

#include <cstdint>
#include <cstddef>
#include <new>
#include <type_traits>

namespace KaGuYa::Graphics
{
// 原06CD0/40A40的非数组Ref共用协议：正文、控制块分配独立，control0不取得所有权。
// 原deleting wrapper在析构正常返回后才delete；delete表达式会在析构抛出后补释放。
template<class T> struct NativePayloadDelete
{
    void operator()(T* value) const
    {
        if (!value) return;
        value->~T();
        ::operator delete(value);
    }
};

// 原EB00/EB80/EBF0/EC70临时数组只有标量字段，payload虚槽直接operator delete[]。
template<class T> struct NativeArrayStorageDelete
{
    void operator()(T* value) const
    {
        static_assert(std::is_trivially_destructible_v<T>);
        ::operator delete[](value);
    }
};

template<class T, std::uint32_t OriginalStride, class PayloadDeleter = NativePayloadDelete<T>>
class NativeObjectReference final
{
    // Ref可跨模块借用；保留原vslot派发，不能去虚化为当前宿主的payload删除器。
    class Control
    {
    public:
        explicit Control(T* value) noexcept : payload(value) {}
        virtual void DestroyPayload() { PayloadDeleter{}(payload); }
        virtual void DeleteControl(std::uint32_t flags)
        {
            if (!(flags & 1u)) return;
            // 本槽只销毁当前控制块；不能添加虚析构槽或把Control标final而去虚化跨模块调用。
            this->Control::~Control();
            ::operator delete(this);
        }
        std::uint32_t references;
        std::uint8_t owning;
        std::uint8_t unwritten[3];
        std::uint32_t stride;
        std::uint32_t count;
        T* payload;
    private:
        ~Control() = default;
    };
public:
    NativeObjectReference() = default;
    NativeObjectReference(const NativeObjectReference& other) noexcept
        : base_(other.base_), index_(other.index_), control_(other.control_)
    { if (control_) ++control_->references; }

    NativeObjectReference& operator=(const NativeObjectReference& other)
    {
        if (this == &other) return *this;
        RetireOwned();
        base_ = other.base_;
        index_ = other.index_;
        control_ = other.control_;
        if (control_) ++control_->references;
        return *this;
    }
    ~NativeObjectReference() noexcept(false) { RetireOwned(); }

    // 原ECD0正常尾退休的是缓存control，不重读槽且不清局部Ref。
    // 仅供手工生命周期的append临时对象使用，调用后不得再析构同一引用。
    void RetireCachedControl()
    {
        auto* control = control_;
        if (control && --control->references == 0)
        {
            if (control->owning) control->DestroyPayload();
            control->DeleteControl(1);
        }
    }

    // 原E659/E681缓存递减目标，但payload返回后E696重读槽再删control；正常尾不清死槽。
    // 调用者须先撤局部EH，之后不能再析构这个手工存储中的引用。
    void RetireLocalControl()
    {
        auto* control = control_;
        if (control && --control->references == 0)
        {
            if (control->owning) control->DestroyPayload();
            if (control_) control_->DeleteControl(1);
        }
    }

    void RetireOwned()
    {
        // 原05230：回调后重读槽内control，正常尾清base/control而保留index。
        if (!control_) return;
        if (--control_->references == 0)
        {
            if (control_->owning) control_->DestroyPayload();
            if (control_) control_->DeleteControl(1);
        }
        control_ = nullptr;
        base_ = nullptr;
    }
    void Publish(T* created, const std::uint32_t stride = OriginalStride, const std::uint32_t count = 1)
    {
        static_assert(sizeof(Control) == 0x18);
        RetireOwned();
        if (!created) return;
        if (control_) control_->DeleteControl(1);
        index_ = 0;
        base_ = created;
        void* storage = ::operator new(sizeof(Control));
        auto* control = ::new (storage) Control(created);
        control_ = control;
        control_->references = 1;
        control_->owning = 1;
        control_->stride = stride;
        control_->count = count;
    }
    T* get() const noexcept
    {
        if (!base_) return nullptr;
        const std::uint32_t stride = control_ ? control_->stride : 0u;
        return reinterpret_cast<T*>(reinterpret_cast<std::uintptr_t>(base_) + index_ * stride);
    }
    T* operator->() const noexcept { return get(); }
    explicit operator bool() const noexcept { return base_ != nullptr; }
    bool HasOwner() const noexcept { return control_ != nullptr; }
    std::uint32_t Count() const noexcept { return control_ ? control_->count : 0u; }
private:
    T* base_ = nullptr;
    std::uint32_t index_ = 0;
    Control* control_ = nullptr;
};
}
