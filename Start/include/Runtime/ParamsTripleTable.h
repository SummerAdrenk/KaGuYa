#pragma once

#include <Runtime/EnvironmentRootRuntime.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

namespace KaGuYa::Start::Runtime
{
// GS+A0：三个 DWORD 为一项，唯一三指针存储，不含 Ref/control 或 raw-count 元数据。
class ParamsTripleTable final
{
public:
    using value_type = std::array<std::uint32_t, 3>;

    ParamsTripleTable() noexcept = default;
    ~ParamsTripleTable() noexcept;
    ParamsTripleTable(const ParamsTripleTable& other);
    ParamsTripleTable& operator=(const ParamsTripleTable& other);
    ParamsTripleTable(ParamsTripleTable&& other) noexcept;
    ParamsTripleTable& operator=(ParamsTripleTable&& other) noexcept;

    // 原函数无 SEH：分配后晚取旧范围，旧删除成功后发布；失败不补新块回滚。
    void ResizeAt40ACB0(std::uint32_t count,
        IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
    // 独立 helper 可抛；完整 GS 的终止边界来自外层 40F370，不在这里补 noexcept。
    void DestroyAt40E6C0(IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
    void clear() noexcept { storage.end = storage.begin; }
    [[nodiscard]] std::size_t size() const noexcept;
    [[nodiscard]] bool empty() const noexcept { return storage.begin == storage.end; }
    [[nodiscard]] value_type& operator[](std::size_t index) noexcept;
    [[nodiscard]] const value_type& operator[](std::size_t index) const noexcept;

    // 现代 profile/strict/fixture 的独立深快照，不宣称执行原 resize 的异常轨迹。
    void AssignSnapshot(std::span<const value_type> values);
    [[nodiscard]] bool operator==(const ParamsTripleTable& other) const noexcept;

    // 不保留 allocator；非默认内存须在 wrapper 析构前显式 DestroyAt40E6C0(memory)。
    EnvironmentVectorStorage<value_type> storage{nullptr, nullptr, nullptr};
};
static_assert(sizeof(ParamsTripleTable::value_type) == 12);
static_assert(sizeof(ParamsTripleTable) == 12);
static_assert(offsetof(ParamsTripleTable, storage) == 0);
}
