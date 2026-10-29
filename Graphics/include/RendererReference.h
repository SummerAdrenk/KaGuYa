#pragma once

#include <cstddef>
#include "NativeObjectReference.h"

namespace KaGuYa::Graphics
{
class SurfaceInfo;
class SurfaceLocker;
struct Surface;
struct DirtyRectState;
struct SurfaceManager;
struct TextState;
template<class T> struct RendererNativeStride;
template<> struct RendererNativeStride<SurfaceInfo> { static constexpr unsigned value = 0x34; };
template<> struct RendererNativeStride<SurfaceLocker> { static constexpr unsigned value = 4; };
template<> struct RendererNativeStride<Surface> { static constexpr unsigned value = 0x34; };
template<> struct RendererNativeStride<DirtyRectState> { static constexpr unsigned value = 0x80; };
template<> struct RendererNativeStride<SurfaceManager> { static constexpr unsigned value = 0x2C; };
template<> struct RendererNativeStride<TextState> { static constexpr unsigned value = 0xD8; };

// 成员槽的保护范围由所属原析构/构造函数决定，不能由C++成员析构统一补退休。
// Capture另返回自动退休的临时Ref，供原有锁调用的局部保护使用。
template<class T, class Deleter = NativePayloadDelete<T>>
class RendererReference final
{
public:
    using Captured = NativeObjectReference<T, 0, Deleter>;
    RendererReference() noexcept { ::new (storage_) Captured; }
    RendererReference(const RendererReference&) = delete;
    RendererReference& operator=(const RendererReference&) = delete;
    void Publish(T* body) { Value().Publish(body, RendererNativeStride<T>::value); }
    void Publish(T* body, std::uint32_t stride) { Value().Publish(body, stride); }
    void Assign(const Captured& other) { Value() = other; }
    void reset() { Value().RetireOwned(); }
    void RetireOwned() { Value().RetireOwned(); }
    T* get() const noexcept { return Value().get(); }
    bool HasOwner() const noexcept { return Value().HasOwner(); }
    Captured Capture() const noexcept { return Value(); }
    T* operator->() const noexcept { return get(); }
    T& operator*() const noexcept { return *get(); }
    explicit operator bool() const noexcept { return get() != nullptr; }
    bool operator==(std::nullptr_t) const noexcept { return get() == nullptr; }
    bool operator!=(std::nullptr_t) const noexcept { return get() != nullptr; }

private:
    Captured& Value() noexcept { return *std::launder(reinterpret_cast<Captured*>(storage_)); }
    const Captured& Value() const noexcept { return *std::launder(reinterpret_cast<const Captured*>(storage_)); }
    alignas(Captured) std::byte storage_[sizeof(Captured)];
};

// 原析构展开：先建立后继槽保护，在显式退休前撤销该槽，避免抛出后再次退休它。
template<class Reference> class ReferenceRetirement final
{
public:
    explicit ReferenceRetirement(Reference& value) noexcept : value_(&value) {}
    ~ReferenceRetirement() noexcept(false) { if (value_) value_->RetireOwned(); }
    void Dismiss() noexcept { value_ = nullptr; }
    void Retire()
    {
        auto* value = value_;
        value_ = nullptr;
        value->RetireOwned();
    }
private:
    Reference* value_;
};
}
