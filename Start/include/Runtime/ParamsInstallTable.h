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
// 工具/测试值夹具；导入 native 表时只消费 cp932，不保存或重建 utf16。
struct ParamsInstallEntry final
{
    bool operator==(const ParamsInstallEntry&) const = default;
    ParamsText file;
    ParamsText media;
};

// 40C860 / 40FBE0：原 GameSystem::_install 正文，非 SystemData 的安装项类型。
struct ParamsInstalledFile final
{
    RuntimeCString file;
    RuntimeCString media;
};
static_assert(sizeof(ParamsInstalledFile) == 8);
static_assert(offsetof(ParamsInstalledFile, file) == 0);
static_assert(offsetof(ParamsInstalledFile, media) == 4);

// 41AFF2..41B0D6：仅还原 GameSystem+38 的安装表 Ref，不声明整个 GameSystem 为 native。
class ParamsInstallTable final
{
public:
    ParamsInstallTable() noexcept = default;
    ~ParamsInstallTable() noexcept;

    // 显式 profile 深快照便利口，非原生产复制语义；CString 按完整长度 SetString，不 share。
    ParamsInstallTable(std::initializer_list<ParamsInstallEntry> values);
    ParamsInstallTable& operator=(std::initializer_list<ParamsInstallEntry> values);
    void AssignSnapshot(std::span<const ParamsInstallEntry> values);
    ParamsInstallTable(const ParamsInstallTable& other);
    ParamsInstallTable& operator=(const ParamsInstallTable& other);
    ParamsInstallTable(ParamsInstallTable&& other) noexcept;
    ParamsInstallTable& operator=(ParamsInstallTable&& other);

    // 先释放旧 Ref；count==0 不分配。reader 须在 count!=0 时才调用，以保留零计数旧表。
    // 元素构造 noexcept；control 分配失败保留已发布 base/bias，不追加回滚。
    void RebuildAt41B025(std::uint32_t count,
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
    // 回调后重读 control；无 control 时不清 base/bias。显式释放允许异常外传。
    void Reset();
    void clear() { Reset(); }

    [[nodiscard]] std::size_t size() const noexcept;
    [[nodiscard]] bool empty() const noexcept { return size() == 0; }
    // 41B10A..41B125：DWORD base+(bias+index)*stride；不以 count 做边界检查。
    // 无 control 时 stride=0；仅对最终 null 结果显式报错，保留部分发布的寻址语义。
    [[nodiscard]] ParamsInstalledFile& At(std::uint32_t index);
    [[nodiscard]] const ParamsInstalledFile& At(std::uint32_t index) const;
    [[nodiscard]] ParamsInstalledFile& operator[](std::size_t index)
        { return At(static_cast<std::uint32_t>(index)); }
    [[nodiscard]] const ParamsInstalledFile& operator[](std::size_t index) const
        { return At(static_cast<std::uint32_t>(index)); }
    [[nodiscard]] bool operator==(const ParamsInstallTable& other) const;

    YtlReferenceSlotRuntime reference{};
};
static_assert(sizeof(ParamsInstallTable) == 12);
static_assert(offsetof(ParamsInstallTable, reference) == 0);
}
