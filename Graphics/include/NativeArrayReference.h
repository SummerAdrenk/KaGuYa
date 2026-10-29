#pragma once

#include "NativeObjectReference.h"
#include <algorithm>
#include <type_traits>

namespace KaGuYa::Graphics
{
// 原EB00/EB80/EBF0使用同一Ref协议；这里只界定临时数组的正常尾与展开状态。
template<class T> class NativeArrayReference final
{
    using Reference = NativeObjectReference<T, sizeof(T), NativeArrayStorageDelete<T>>;
public:
    NativeArrayReference() noexcept { ::new (storage_) Reference; }
    NativeArrayReference(const NativeArrayReference&) = delete;
    NativeArrayReference& operator=(const NativeArrayReference&) = delete;
    ~NativeArrayReference() noexcept(false) { if (active_) Value().RetireOwned(); }

    void Allocate(const std::uint32_t count, const bool clear)
    {
        static_assert(std::is_trivial_v<T> && std::is_standard_layout_v<T>);
        // 原ED8A/EE4A先发布payload再申请18h control，control失败不追加raw回收guard。
        T* raw = new T[count];
        if (clear) std::fill_n(raw, count, T{});
        Value().Publish(raw, sizeof(T), count);
    }
    T& operator[](const std::size_t index) noexcept { return Value().get()[index]; }
    void Retire()
    {
        // 原BB21/C7BE/C7EF先撤该层EH，正常尾使用缓存control且不清死槽。
        active_ = false;
        Value().RetireCachedControl();
    }
private:
    Reference& Value() noexcept { return *std::launder(reinterpret_cast<Reference*>(storage_)); }
    alignas(Reference) std::byte storage_[sizeof(Reference)];
    bool active_ = true;
};
}
