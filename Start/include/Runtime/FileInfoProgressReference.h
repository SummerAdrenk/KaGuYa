#pragma once

#include <Runtime/IFileInfoProgress.h>
#include <Runtime/YtlReferenceArrayRuntime.h>

namespace KaGuYa::Start::Runtime
{
// 466520：每次按base+bias*stride解析；control为空时stride为0，不缓存正文地址。
[[nodiscard]] IFileInfoProgress* ResolveFileInfoProgressReference(
    const YtlReferenceSlotRuntime& reference) noexcept;

// 489DF5局部Ref及4590B6..4590CB的共享拥有链，唯一存储就是public reference。
// 本轮正文仍为现代FileInfoProgressRuntime/NativeDialog，不宣称原10h/24h正文ABI。
// title仍借std::string对象，dialog仍持现代State/thunk；环境晚取及原句柄语义尚未迁移。
class FileInfoProgressReference final
{
public:
    FileInfoProgressReference() noexcept = default;
    explicit FileInfoProgressReference(const YtlReferenceSlotRuntime& source) noexcept;
    FileInfoProgressReference(const FileInfoProgressReference& source) noexcept;
    FileInfoProgressReference(FileInfoProgressReference&& source) noexcept;
    FileInfoProgressReference& operator=(const FileInfoProgressReference& source) noexcept;
    FileInfoProgressReference& operator=(FileInfoProgressReference&& source) noexcept;
    ~FileInfoProgressReference() noexcept;

    // 调用前正文已构造完成且已脱离unique_ptr。先释放旧Ref，再初始化本轮局部槽；
    // base先发布，18h control分配失败不回收正文，reference保留base且control为空。
    void AdoptAt489DF5(IFileInfoProgress* created);
    [[nodiscard]] IFileInfoProgress* get() const noexcept;
    [[nodiscard]] IFileInfoProgress* operator->() const;
    [[nodiscard]] explicit operator bool() const noexcept;
    // 460730/EHFlags5：回调后清槽，control为空不清base/bias；释放异常终止。
    void reset() noexcept;

    YtlReferenceSlotRuntime reference;
};
static_assert(sizeof(FileInfoProgressReference) == 0x0C);

// 仅借用槽，不增持、不分配control、不缓存正文；调用期间槽及所指对象必须存活。
// 供现代业务入口消费archiver自身的+5C Ref，不是原IProgress正文。
class FileInfoProgressReferenceView final : public IFileInfoProgress
{
public:
    explicit FileInfoProgressReferenceView(const YtlReferenceSlotRuntime& reference) noexcept;
    [[nodiscard]] bool PollCancelAt47F0B0() override;
    LRESULT StepAt46E820() override;
    LRESULT AddRangeAt46A5A0(std::uint32_t delta) override;
    // 原空正文会故障；本入口保持noexcept，明确记录错误并终止，不返回默认HWND。
    [[nodiscard]] HWND WindowAt47ADF0() const noexcept override;

private:
    const YtlReferenceSlotRuntime* reference_;
};
}
