#pragma once

#include <Runtime/ByteArrayReferenceRuntime.h>

#include <cstddef>
#include <initializer_list>
#include <span>
#include <vector>

namespace KaGuYa::Start::Runtime
{
// 原memmove入口的现代有界视图，不增持/复制Ref；范围来自control正文和stride/count。
// 不可证实的区间明确抛out_of_range，不把元素count误当作偏移后仍可写的字节数。
[[nodiscard]] std::span<std::uint8_t> BorrowBoundedByteArrayCopySpan(const ByteArrayReferenceRuntime& bytes);

// GameSystem的uchar[]唯一Ref12。原生共享引用仍由storage表达，现代profile复制另作深快照。
class ParamsByteArray final
{
public:
    ParamsByteArray() noexcept = default;
    ~ParamsByteArray() noexcept = default;
    ParamsByteArray(const ParamsByteArray& other);
    ParamsByteArray& operator=(const ParamsByteArray& other);
    ParamsByteArray(ParamsByteArray&& other) noexcept;
    ParamsByteArray& operator=(ParamsByteArray&& other);

    // 仅strict-parser/fixture/profile导入，不为生产数组保留vector镜像或补零尾部。
    ParamsByteArray(std::initializer_list<std::byte> values);
    ParamsByteArray& operator=(std::initializer_list<std::byte> values);
    void AssignSnapshot(std::span<const std::byte> values);
    [[nodiscard]] std::vector<std::byte> SnapshotBytes() const;

    [[nodiscard]] std::size_t size() const noexcept { return storage.Size(); }
    [[nodiscard]] bool empty() const noexcept { return size() == 0; }
    [[nodiscard]] std::byte* Resolve(std::uint32_t index) noexcept;
    [[nodiscard]] const std::byte* Resolve(std::uint32_t index) const noexcept;
    // 引用便利口不冒充原空目标写入；生产reader在原写入时点处理nullable Resolve。
    [[nodiscard]] std::byte& operator[](std::size_t index);
    [[nodiscard]] const std::byte& operator[](std::size_t index) const;
    [[nodiscard]] bool operator==(const ParamsByteArray& other) const;

    ByteArrayReferenceRuntime storage;
};
static_assert(sizeof(ParamsByteArray) == 12);
static_assert(offsetof(ParamsByteArray, storage) == 0);
}
