#pragma once

#include <Runtime/SaveFileContainerRuntime.h>
#include <Runtime/ScrVmParamsGenerationBinding.h>

namespace KaGuYa::Start::Runtime
{
// live来源与VM共用根槽；catalog/rolling临时对象直接借栈上70h正文，不创建外层Ref。
class ScrSaveFileArchiveStateRuntime final : public ISaveFileArchiveState
{
public:
    explicit ScrSaveFileArchiveStateRuntime(
        ScrVmParamsGenerationBinding generation);
    explicit ScrSaveFileArchiveStateRuntime(ScriptParamsNativeBody& receiver) noexcept;

    [[nodiscard]] ScriptParamsNativeBody& CaptureParamsAtEntry() override;
    void SetLastWriteTimeAt00(ScriptParamsNativeBody& receiver, std::uint64_t value) override;
    void SerializeSaveParamsAt48B720(ScriptParamsNativeBody& receiver, SerializeBuffer& archive) override;
    void SerializeSaveParams2At48B6B0(ScriptParamsNativeBody& receiver, SerializeBuffer& archive) override;
    [[nodiscard]] ScriptDwordVectorStorage& FormatHistoryAt3C(ScriptParamsNativeBody& receiver) override;
    [[nodiscard]] std::int32_t& CurrentFormatAt60(ScriptParamsNativeBody& receiver) override;

    // 独立叶调用也捕获一次；容器使用上面的显式receiver接口贯穿整次操作。
    void SetLastWriteTimeAt00(std::uint64_t value);
    void SerializeSaveParamsAt48B720(SerializeBuffer& archive);
    void SerializeSaveParams2At48B6B0(SerializeBuffer& archive);
    [[nodiscard]] ScriptDwordVectorStorage& FormatHistoryAt3C();
    [[nodiscard]] std::int32_t CurrentFormatAt60() const;

private:
    [[nodiscard]] ScriptParamsNativeBody& BorrowParams() const;
    ScrVmParamsGenerationBinding generation_;
    ScriptParamsNativeBody* receiver_ = nullptr;
};
}
