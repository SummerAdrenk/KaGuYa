#pragma once

#include <Runtime/RuntimeStringBorrow.h>
#include <Runtime/RuntimeCString.h>
#include <Runtime/EnvironmentObjectReference.h>

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace KaGuYa::Start::Runtime
{
class RuntimeCStringArrayReference final
{
public:
    RuntimeCStringArrayReference() noexcept = default;
    RuntimeCStringArrayReference(std::nullptr_t) noexcept {}
    explicit RuntimeCStringArrayReference(const YtlReferenceSlotRuntime& source) noexcept;
    RuntimeCStringArrayReference(const RuntimeCStringArrayReference& source) noexcept;
    RuntimeCStringArrayReference(RuntimeCStringArrayReference&& source) noexcept;
    RuntimeCStringArrayReference& operator=(const RuntimeCStringArrayReference& source);
    RuntimeCStringArrayReference& operator=(RuntimeCStringArrayReference&& source);
    RuntimeCStringArrayReference& operator=(std::nullptr_t) { Reset(); return *this; }
    ~RuntimeCStringArrayReference() noexcept;

    void Reset();
    void reset() { Reset(); }
    void RebuildAt412870(std::int32_t count,
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
    void Assign(const RuntimeCStringArrayReference& source, std::int32_t bias);

    // 数量来自 control.count，不扣除 bias，也不读取 cookie。
    [[nodiscard]] std::size_t Size() const noexcept;
    [[nodiscard]] std::size_t size() const noexcept { return Size(); }
    [[nodiscard]] bool Empty() const noexcept { return Size() == 0; }
    [[nodiscard]] bool empty() const noexcept { return Empty(); }
    explicit operator bool() const noexcept { return reference_.baseAt00 != 0; }
    // 借用未应用 bias 的原始存储；部分发布时可以没有 control。
    [[nodiscard]] RuntimeCString* StorageAt3C() noexcept;
    [[nodiscard]] const RuntimeCString* StorageAt3C() const noexcept;
    [[nodiscard]] std::int32_t IndexBiasAt40() const noexcept;
    // 可空的原始目标；无 control 时 stride 为零，不按 count 检查索引。
    [[nodiscard]] RuntimeCString* Resolve(std::uint32_t index) noexcept;
    [[nodiscard]] const RuntimeCString* Resolve(std::uint32_t index) const noexcept;
    // 按原 DWORD 回绕规则解址，不根据 count 检查索引范围。
    // 安全差异：base 为空时抛出异常，不解引用空指针。
    [[nodiscard]] RuntimeCString& At(std::uint32_t index);
    [[nodiscard]] const RuntimeCString& At(std::uint32_t index) const;
    [[nodiscard]] std::string_view ResourceNameAt(std::size_t index) const;
    [[nodiscard]] YtlReferenceSlotRuntime& RawSlot() noexcept { return reference_; }
    [[nodiscard]] const YtlReferenceSlotRuntime& RawSlot() const noexcept { return reference_; }

    // 比较 DWORD 回绕的 base+bias*stride 解址结果，不比较拥有关系或字符串值。
    // 与 nullptr 比较也观察同一解址结果。
    friend bool operator==(const RuntimeCStringArrayReference& a,
        const RuntimeCStringArrayReference& b) noexcept
    {
        return a.Resolve(0) == b.Resolve(0);
    }
    friend bool operator==(const RuntimeCStringArrayReference& a, std::nullptr_t) noexcept
        { return a.Resolve(0) == nullptr; }

private:
    YtlReferenceSlotRuntime reference_{};
};
static_assert(sizeof(RuntimeCStringArrayReference) == 12);
using UnSaveDataStringArrayReference = RuntimeCStringArrayReference;
}
