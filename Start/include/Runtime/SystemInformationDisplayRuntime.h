#pragma once

#include <Runtime/SystemMenuDialogRuntime.h>

namespace KaGuYa::Start::Runtime
{
// 历史显式受控父模型，不是原14h正文；默认Root+3C使用SystemInformationDisplayNativeBody14h。
// 本模型的Suppressed值也不等于root+1B8或child+84，不能把本类raw读作原偏移。
// 现代 typed owning 槽不是 Ref12 内存覆盖；分配异常保证回收而不复刻原控制块失败泄漏。
class SystemInformationDisplayRuntime final : private IModalDialogRuntimeTargetReference
{
public:
    SystemInformationDisplayRuntime(GuiModalDialogDependencies dependencies, ISystemMenuDialogServices& services);
    ~SystemInformationDisplayRuntime() noexcept;
    SystemInformationDisplayRuntime(const SystemInformationDisplayRuntime&) = delete;
    SystemInformationDisplayRuntime& operator=(const SystemInformationDisplayRuntime&) = delete;

    bool CanOpenAt5105C0();
    std::int32_t RunAt5075B0();
    std::uint32_t& SuppressedAt10() noexcept { return suppressedAt10_; }
    // 仅借用成员槽；runner 每个虚调用重新解析，不延长首次取得的 child 生命周期。
    std::shared_ptr<SystemMenuDialogRuntime>& ChildSlotAt04() noexcept { return child_; }

private:
    IModalDialogRuntimeTarget* ResolveAt4A5330() override;
    IGuiModalRootSource& rootSource_;
    ModalDialogRunnerRuntime runner_;
    std::shared_ptr<SystemMenuDialogRuntime> child_;
    std::uint32_t suppressedAt10_;
};
}
