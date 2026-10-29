#pragma once
#include <Runtime/FileInfoProgressNative.h>
#include <Runtime/YtlReferenceArrayRuntime.h>

namespace KaGuYa::Start::Runtime
{
// 原489D70的外层申请与state2展开；未成功构造时仅交还10h块，不替调用者持Ref。
FileInfoProgressNativeBody10h* NewFileInfoProgressNativeAt489D70(FileInfoProgressNativeServices&);
// 只用于caller本轮fresh局部Ref：原489DF5清三槽、base先发布、18h control后发布。
// control申请失败无正文回滚；不能经IFileInfoProgress的C++ delete释放原正文。
void ConstructFileInfoProgressNativeReferenceAt489DF5(YtlReferenceSlotRuntime&,
    FileInfoProgressNativeBody10h*, IEnvironmentRootMemory&);
void ReleaseFileInfoProgressNativeReferenceAt460730(YtlReferenceSlotRuntime&) noexcept;
// 每次调用重新解析同一Ref，不缓存body，不拥有引用；用于原局部progress的借用消费者。
class FileInfoProgressNativeReferenceView final : public IFileInfoProgress
{
public:
    FileInfoProgressNativeReferenceView(const YtlReferenceSlotRuntime&, FileInfoProgressNativeServices&) noexcept;
    bool PollCancelAt47F0B0() override;
    LRESULT StepAt46E820() override;
    LRESULT AddRangeAt46A5A0(std::uint32_t) override;
    HWND WindowAt47ADF0() const noexcept override;
private:
    const YtlReferenceSlotRuntime& reference_;
    FileInfoProgressNativeServices& services_;
};
}
