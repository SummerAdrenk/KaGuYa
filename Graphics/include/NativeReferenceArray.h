#pragma once

#include <algorithm>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <new>
#include <stdexcept>

namespace KaGuYa::Graphics
{
// 原manager的两张表共用ECD0/EEF0/F000/F1C0；只有槽数组分配，没有哈希节点或表owner。
// 成员自身不自动清理，由原manager析构保护域决定哪些后继表要展开。
template<class Reference> class NativeReferenceArray final
{
public:
    NativeReferenceArray() = default;
    NativeReferenceArray(const NativeReferenceArray&) = delete;
    NativeReferenceArray& operator=(const NativeReferenceArray&) = delete;

    std::uint32_t size() const noexcept { return Count(begin_, end_); }
    std::uint32_t Capacity() const noexcept { return Count(begin_, capacity_); }
    bool empty() const noexcept { return begin_ == end_; }
    const Reference* BeginReferences() const noexcept { return begin_; }
    const Reference* EndReferences() const noexcept { return end_; }

    template<class T> bool Contains(const T* body) const noexcept
    {
        // 原D970/D9C0按Ref解析body，线性查找第一项；空句柄也不能跳过原查找规则。
        const auto* const last = end_;
        for (const auto* slot = begin_; slot != last; ++slot)
            if (slot->get() == body) return true;
        return false;
    }

    void Append(const Reference& reference)
    {
        alignas(Reference) std::byte storage[sizeof(Reference)];
        auto* temporary = ::new (storage) Reference(reference);
        try { AppendReference(*temporary); }
        catch (...) { temporary->~Reference(); throw; }
        // 原ED32先撤局部EH，正常尾使用缓存control；异常尾才回读槽。
        temporary->RetireCachedControl();
    }

    template<class T> bool Erase(const T* body)
    {
        auto* const last = end_;
        for (auto* slot = begin_; slot != last; ++slot)
        {
            if (slot->get() != body) continue;
            auto* output = slot;
            for (auto* input = slot + 1; input != last; ++input, ++output)
                *output = *input;
            // 原DCFC/E89C先逐项赋值，后重读end退尾槽，成功才缩短；失败保留前缀。
            (end_ - 1)->RetireOwned();
            --end_;
            return true;
        }
        return false;
    }

    void Clear()
    {
        auto* const last = end_;
        for (auto* slot = begin_; slot != last; ++slot) slot->RetireOwned();
        end_ = begin_;
    }

    void RetireOwned()
    {
        // 原F580：只有整段退休正常返回才释放表并清三指针，不给失败表补回收。
        if (begin_)
        {
            auto* const last = end_;
            for (auto* slot = begin_; slot != last; ++slot) slot->RetireOwned();
            ::operator delete(begin_);
        }
        begin_ = end_ = capacity_ = nullptr;
    }

private:
    static constexpr std::uint32_t MaximumCount = 0x15555555;
    static_assert(sizeof(Reference) == 12);
    static std::uint32_t Count(const Reference* first, const Reference* last) noexcept
    {
        const auto bytes = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(last) -
            reinterpret_cast<std::uintptr_t>(first));
        return static_cast<std::uint32_t>(std::bit_cast<std::int32_t>(bytes) / 12);
    }

    void Reserve(std::uint32_t requested)
    {
        if (requested > MaximumCount) throw std::length_error("vector<T> too long");
        if (requested <= Capacity()) return;
        auto* replacement = static_cast<Reference*>(::operator new(requested * sizeof(Reference)));
        try
        {
            auto* output = replacement;
            const auto* const last = end_;
            for (const auto* input = begin_; input != last; ++input, ++output)
                ::new (output) Reference(*input);
        }
        catch (...) { ::operator delete(replacement); throw; }
        // 原F07A撤新存储保护，再退旧表；不能在旧析构异常时恢复旧状态或删除新表。
        const auto count = size();
        auto* const last = end_;
        if (begin_)
        {
            for (auto* slot = begin_; slot != last; ++slot) slot->RetireOwned();
            ::operator delete(begin_);
        }
        capacity_ = replacement + requested;
        end_ = replacement + count;
        begin_ = replacement;
    }

    void AppendReference(const Reference& reference)
    {
        const auto source = reinterpret_cast<std::uintptr_t>(&reference);
        const bool internal = source >= reinterpret_cast<std::uintptr_t>(begin_) &&
            source < reinterpret_cast<std::uintptr_t>(end_);
        const auto index = internal ? Count(begin_, &reference) : 0;
        if (end_ == capacity_)
        {
            const auto count = size();
            if (count > MaximumCount - 1) throw std::length_error("vector<T> too long");
            const auto old = Capacity();
            const auto half = old >> 1;
            const auto grown = old > MaximumCount - half ? 0u : old + half;
            Reserve((std::max)(count + 1, grown));
        }
        ::new (end_) Reference(internal ? begin_[index] : reference);
        ++end_;
    }

    Reference* begin_ = nullptr;
    Reference* end_ = nullptr;
    Reference* capacity_ = nullptr;
};
}
