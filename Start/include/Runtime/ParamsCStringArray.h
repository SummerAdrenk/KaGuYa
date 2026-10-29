#pragma once

#include <Runtime/ParamsText.h>
#include <Runtime/RuntimeCStringArray.h>

#include <cstddef>
#include <initializer_list>
#include <span>

namespace KaGuYa::Start::Runtime
{
// GS+C4 的唯一数组 Ref；元素为四字节 CString，不保存 ParamsText 镜像。
class ParamsCStringArray final
{
public:
    using value_type = RuntimeCString;

    ParamsCStringArray() noexcept = default;
    ~ParamsCStringArray() noexcept = default;

    // 仅供 fixture/profile 的深快照；生产 reader 直接调用 storage.RebuildAt412870。
    ParamsCStringArray(std::initializer_list<ParamsText> values);
    ParamsCStringArray& operator=(std::initializer_list<ParamsText> values);
    void AssignSnapshot(std::span<const ParamsText> values);
    ParamsCStringArray(const ParamsCStringArray& other);
    ParamsCStringArray& operator=(const ParamsCStringArray& other);
    ParamsCStringArray(ParamsCStringArray&& other) noexcept;
    // 底层先 Reset 旧 Ref，释放回调可抛；不能增加 noexcept 终止边界。
    ParamsCStringArray& operator=(ParamsCStringArray&& other);

    [[nodiscard]] std::size_t size() const noexcept;
    [[nodiscard]] bool empty() const noexcept { return size() == 0; }
    // [] 沿用底层 bias/stride 解址与空 base 检查，不按 count 检查范围。
    // at 是现代显式越界保护；原 reader 的 nullable 目标不通过这两个引用入口。
    [[nodiscard]] RuntimeCString& operator[](std::size_t index);
    [[nodiscard]] const RuntimeCString& operator[](std::size_t index) const;
    [[nodiscard]] RuntimeCString& at(std::size_t index);
    [[nodiscard]] const RuntimeCString& at(std::size_t index) const;
    [[nodiscard]] bool operator==(const ParamsCStringArray& other) const;

    RuntimeCStringArrayReference storage;
};
static_assert(sizeof(ParamsCStringArray) == 12);
static_assert(offsetof(ParamsCStringArray, storage) == 0);
}
