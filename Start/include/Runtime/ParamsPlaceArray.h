#pragma once

#include <Runtime/ParamsText.h>
#include <Runtime/RuntimeCString.h>
#include <Runtime/YtlReferenceArrayRuntime.h>

#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <span>

namespace KaGuYa::Start::Runtime
{
// 现代工具/测试夹具；只导入 name.cp932，不作为原生数组的第二份存储。
struct ParamsNamedValue final
{
    bool operator==(const ParamsNamedValue&) const = default;
    ParamsText name;
    std::uint32_t value = 0;
};

// 40C930 只构造 name；value 在 reader 写入前保持原分配字节，不能补零。
struct ParamsPlaceEntry final
{
    RuntimeCString nameAt00;
    std::uint32_t valueAt04;
};
static_assert(sizeof(ParamsPlaceEntry) == 8);
static_assert(offsetof(ParamsPlaceEntry, nameAt00) == 0);
static_assert(offsetof(ParamsPlaceEntry, valueAt04) == 4);

// GS+D0：唯一 Ref12，指向带 cookie 的八字节元素数组，不是 vector。
class ParamsPlaceArray final
{
public:
    using value_type = ParamsPlaceEntry;

    ParamsPlaceArray() noexcept = default;
    ~ParamsPlaceArray() noexcept;
    ParamsPlaceArray(std::initializer_list<ParamsNamedValue> values);
    ParamsPlaceArray& operator=(std::initializer_list<ParamsNamedValue> values);
    void AssignSnapshot(std::span<const ParamsNamedValue> values);
    ParamsPlaceArray(const ParamsPlaceArray& other);
    ParamsPlaceArray& operator=(const ParamsPlaceArray& other);
    ParamsPlaceArray(ParamsPlaceArray&& other) noexcept;
    // 旧 Ref 的释放可抛，不给赋值增加 noexcept 边界。
    ParamsPlaceArray& operator=(ParamsPlaceArray&& other);

    // 先按 43B910 释放旧 Ref；负数/零不分配。reader 对线上零计数须直接跳过。
    // 元素构造整体 noexcept；control 分配失败保留已发布正文，不额外回滚。
    void RebuildAt41D0B2(std::int32_t count,
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());

    [[nodiscard]] std::size_t size() const noexcept;
    [[nodiscard]] bool empty() const noexcept { return size() == 0; }
    // DWORD base+(bias+index)*stride；空 base 返回 null，无 control 时 stride=0。
    // reader 在 name 返回后须重新 Resolve value 目标，不能缓存整项引用。
    [[nodiscard]] ParamsPlaceEntry* Resolve(std::uint32_t index) noexcept;
    [[nodiscard]] const ParamsPlaceEntry* Resolve(std::uint32_t index) const noexcept;
    // [] 不检查 count，仅拒绝空解址；at 另加现代显式越界保护。
    [[nodiscard]] ParamsPlaceEntry& operator[](std::size_t index);
    [[nodiscard]] const ParamsPlaceEntry& operator[](std::size_t index) const;
    [[nodiscard]] ParamsPlaceEntry& at(std::size_t index);
    [[nodiscard]] const ParamsPlaceEntry& at(std::size_t index) const;
    [[nodiscard]] bool operator==(const ParamsPlaceArray& other) const;

    YtlReferenceSlotRuntime storage{};
};
static_assert(sizeof(ParamsPlaceArray) == 12);
static_assert(offsetof(ParamsPlaceArray, storage) == 0);
}
