#pragma once

#include <Runtime/EnvironmentInstalledFiles.h>

#include <cstddef>
#include <cstdint>

namespace KaGuYa::Start::Runtime
{
// 495E51..495E5F：CSysData+08 的实际数组 Ref，不保存安装项的现代值副本。
class SystemDataInstalledFiles final
{
public:
    SystemDataInstalledFiles() noexcept = default;
    ~SystemDataInstalledFiles() noexcept;
    SystemDataInstalledFiles(const SystemDataInstalledFiles&) = delete;
    SystemDataInstalledFiles& operator=(const SystemDataInstalledFiles&) = delete;
    // 仅转移三字槽，不复制元素或增加引用计数；移动赋值先释放目标旧槽。
    SystemDataInstalledFiles(SystemDataInstalledFiles&& other) noexcept;
    SystemDataInstalledFiles& operator=(SystemDataInstalledFiles&& other);

    // 4960E7 先释放旧 Ref；signed count<=0 不分配，也不额外清除保留的 bias。
    // 元素构造失败直接terminate；control分配失败不撤销已发布的base/bias。
    void RebuildAt49611A(std::int32_t count,
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
    // 正文回调后重读槽内 control；无 control 时保留 base/bias，不猜测所有权。
    // 显式 Reset/移动赋值允许回调异常外传；析构中的异常会终止。
    void Reset();

    [[nodiscard]] std::size_t size() const noexcept;
    [[nodiscard]] bool empty() const noexcept { return size() == 0; }
    // 496247..496250 / 47E7C7：32 位 base+(bias+index)*stride，不做 count 边界检查。
    // base 为零返回 nullptr；无 control 时 stride=0，非空 base 仍可解析。
    [[nodiscard]] EnvironmentInstalledFile* ResolveAt(std::uint32_t index) noexcept;
    [[nodiscard]] const EnvironmentInstalledFile* ResolveAt(std::uint32_t index) const noexcept;
    // 仅对解析结果为 null 显式抛 logic_error；不以二次取根的 count 拒绝原索引。
    [[nodiscard]] EnvironmentInstalledFile& At(std::size_t index);
    [[nodiscard]] const EnvironmentInstalledFile& At(std::size_t index) const;

    YtlReferenceSlotRuntime reference{};
};
static_assert(sizeof(SystemDataInstalledFiles) == 12);
static_assert(offsetof(SystemDataInstalledFiles, reference) == 0);

struct ParamsGameSystem;
// 4960D5..49638E：目标先按源 count 建表，随后逐字段重取源/目标 Ref 并赋值共享 CString。
void InitializeSystemDataInstallFilesAt4960D5(const ParamsGameSystem& source,
    SystemDataInstalledFiles& destination,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
}
