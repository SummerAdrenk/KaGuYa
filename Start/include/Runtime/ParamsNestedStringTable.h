#pragma once

#include <Runtime/ParamsText.h>
#include <Runtime/RuntimeCStringVector.h>

#include <cstddef>
#include <span>
#include <vector>

namespace KaGuYa::Start::Runtime
{
// GS+E0：唯一外层三指针；每个十二字节元素拥有独立的四字节 CString 元素区。
class ParamsNestedStringTable final
{
public:
    using Inner = EnvironmentVectorStorage<RuntimeCString>;
    using value_type = Inner;

    ParamsNestedStringTable() noexcept = default;
    ~ParamsNestedStringTable() noexcept;
    ParamsNestedStringTable(const ParamsNestedStringTable& other);
    ParamsNestedStringTable& operator=(const ParamsNestedStringTable& other);
    ParamsNestedStringTable(ParamsNestedStringTable&& other) noexcept;
    ParamsNestedStringTable& operator=(ParamsNestedStringTable&& other) noexcept;

    // 41D387：插入项调用 40B6E0 复制；40A640 扩容时仅搬移旧 inner 的三指针。
    void Append(const Inner& inner, IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
    // 40E5F0 整体 flags5：正序销毁 inner，回调后重读外层存储，成功后清三指针。
    void Destroy(IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory()) noexcept;

    // 仅 strict parser/fixture/profile 的深快照；只导入 cp932，不保存现代值镜像。
    void AssignSnapshot(std::span<const std::vector<ParamsText>> values);
    [[nodiscard]] std::size_t size() const noexcept;
    [[nodiscard]] bool empty() const noexcept { return storage.begin == storage.end; }
    [[nodiscard]] Inner& operator[](std::size_t index) noexcept;
    [[nodiscard]] const Inner& operator[](std::size_t index) const noexcept;
    [[nodiscard]] bool operator==(const ParamsNestedStringTable& other) const;

    // 不内嵌 allocator；注入分配须与默认销毁兼容，或在所属对象退出前显式 Destroy(memory)。
    EnvironmentVectorStorage<Inner> storage{nullptr, nullptr, nullptr};
};
static_assert(sizeof(ParamsNestedStringTable::Inner) == 12);
static_assert(sizeof(ParamsNestedStringTable) == 12);
static_assert(offsetof(ParamsNestedStringTable, storage) == 0);
}
