#pragma once

#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/EnvironmentRootRuntime.h>

#include <cstddef>
#include <span>

namespace KaGuYa::Start::Runtime
{
// 496E10及46A16C..46A232共用的原8-byte安装项正文；两者的构造异常边界不同。
struct EnvironmentInstalledFile final
{
    RuntimeCString stringAt00;
    RuntimeCString stringAt04;
};
static_assert(sizeof(EnvironmentInstalledFile) == 8);
static_assert(offsetof(EnvironmentInstalledFile, stringAt00) == 0);
static_assert(offsetof(EnvironmentInstalledFile, stringAt04) == 4);

// 5B2F28 = {492B60, 467890}：逆序析构 CString，随后执行控制块退休。
using EnvironmentInstalledFileControl = EnvironmentObjectControl<EnvironmentInstalledFile>;
static_assert(sizeof(EnvironmentInstalledFileControl) == 0x18);

// 构造失败不回滚：46A040 在 46A219 之前始终处于 state 0。
[[nodiscard]] EnvironmentInstalledFile* CreateEnvironmentInstalledFileAt46A16C(
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());

// 仅用于尚未持有引用的新槽；先发布 base，再分配控制块。
// 控制块分配失败时保留已发布的 base，不删除正文。
void InitializeEnvironmentInstalledFileReference(
    YtlReferenceSlotRuntime& reference, EnvironmentInstalledFile* payload);

// 复制构造而非赋值：目标槽不得已经持有 Ref。
void CopyEnvironmentInstalledFileReference(YtlReferenceSlotRuntime& destination,
    const YtlReferenceSlotRuntime& source) noexcept;
[[nodiscard]] EnvironmentInstalledFile* ResolveEnvironmentInstalledFile(
    const YtlReferenceSlotRuntime& reference) noexcept;

class EnvironmentInstalledFiles final
{
public:
    EnvironmentInstalledFiles() noexcept = default;
    ~EnvironmentInstalledFiles() noexcept;
    EnvironmentInstalledFiles(const EnvironmentInstalledFiles&) = delete;
    EnvironmentInstalledFiles& operator=(const EnvironmentInstalledFiles&) = delete;

    // 46A25A..46A285 / 453CB0：增加 source 的引用计数，不接管其现有引用。
    // memory 是逐次调用传入的分配叶接口，不在对象内增加字段。
    // 使用非默认分配器时，析构前须将其传入 DestroyStorage，
    // 或保证其分配结果能由默认释放器释放。
    void AppendCopy(const YtlReferenceSlotRuntime& source,
        IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
    // 便利操作：释放当前元素范围，保留已分配的存储。
    void clear() noexcept;
    // 496F80：4971B6..4971B9 在路径 CString 析构之前调用此操作。
    void DestroyStorage(IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory()) noexcept;
    [[nodiscard]] std::size_t size() const noexcept;
    [[nodiscard]] bool empty() const noexcept { return storage.begin == storage.end; }
    [[nodiscard]] std::span<YtlReferenceSlotRuntime> References() noexcept;
    [[nodiscard]] std::span<const YtlReferenceSlotRuntime> References() const noexcept;

    // 仅含三指针；与 EnvironmentRootStorage 一样公开给精确 ABI 的所有者。
    EnvironmentVectorStorage<YtlReferenceSlotRuntime> storage{nullptr, nullptr, nullptr};
};
static_assert(sizeof(EnvironmentInstalledFiles) == 12);
static_assert(offsetof(EnvironmentInstalledFiles, storage) == 0);
static_assert(offsetof(EnvironmentVectorStorage<YtlReferenceSlotRuntime>, begin) == 0);
static_assert(offsetof(EnvironmentVectorStorage<YtlReferenceSlotRuntime>, end) == 4);
static_assert(offsetof(EnvironmentVectorStorage<YtlReferenceSlotRuntime>, capacity) == 8);
}
