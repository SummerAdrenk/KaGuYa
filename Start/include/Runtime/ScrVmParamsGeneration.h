#pragma once

#include <Runtime/ScriptParamsNativeBody.h>

#include <memory>

namespace KaGuYa::Start::Runtime
{
using ScrVmParamsGeneration = ScriptParamsNativeBody;
using ScrSaveParams2RuntimeState = ScrSaveParams2Native;

// 显式工具值，不是CParams+18的存储；真实表保存ScriptStackFrameNative Ref。
struct ScrVmFileCallFrame final
{
    std::int32_t scriptId = -1;
    std::uint32_t returnProgramCounter = 0;
    bool operator==(const ScrVmFileCallFrame&) const = default;
};

// 根+1C0的唯一Ref与外部服务适配；不把memory/strings放入70h正文或18h control。
// 分配必须兼容原全局sized delete；服务借用须长于此适配器。
class ScrVmParamsGenerationSlot final
{
public:
    struct DeferredTag final {};
    struct RootManagedTag final {};

    // 独立fixture入口立即构造；生产root使用DeferredTag，只建立空Ref。
    ScrVmParamsGenerationSlot();
    explicit ScrVmParamsGenerationSlot(IEnvironmentRootMemory& memory,
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
    explicit ScrVmParamsGenerationSlot(DeferredTag,
        IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory(),
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource()) noexcept;
    // 464263仅释放一次；后续析构回调重填槽不触发现代成员的第二遍清理。
    explicit ScrVmParamsGenerationSlot(RootManagedTag, IEnvironmentRootMemory& memory,
        IRuntimeStringManagerSource& strings) noexcept;
    ~ScrVmParamsGenerationSlot() noexcept;
    ScrVmParamsGenerationSlot(const ScrVmParamsGenerationSlot&) = delete;
    ScrVmParamsGenerationSlot& operator=(const ScrVmParamsGenerationSlot&) = delete;

    [[nodiscard]] ScrVmParamsGeneration& Current();
    [[nodiscard]] const ScrVmParamsGeneration& Current() const;
    [[nodiscard]] const ScrVmParamsGeneration* Published() const noexcept;
    [[nodiscard]] YtlReferenceSlotRuntime& ReferenceAt1C0() noexcept { return reference_; }
    [[nodiscard]] const YtlReferenceSlotRuntime& ReferenceAt1C0() const noexcept { return reference_; }
    [[nodiscard]] IEnvironmentRootMemory& Memory() const noexcept { return memory_; }
    [[nodiscard]] IRuntimeStringManagerSource& Strings() const noexcept { return strings_; }
    void Replace();
    void Release();

private:
    IEnvironmentRootMemory& memory_;
    IRuntimeStringManagerSource& strings_;
    mutable YtlReferenceSlotRuntime reference_;
    bool automaticRelease_ = true;
};

// 显式独立fixture/工具的slot生命期；生产root及binding不借此保活正文。
[[nodiscard]] std::shared_ptr<ScrVmParamsGenerationSlot> CreateScrVmParamsGenerationSlot(
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory(),
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
}
