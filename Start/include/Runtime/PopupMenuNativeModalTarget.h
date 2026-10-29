#pragma once
#include <Runtime/PopupMenuNativeBody.h>
#include <Runtime/ModalDialogRunnerRuntime.h>

namespace KaGuYa::Start::Runtime
{
// 5071D0现代宿主的非拥有视图。每次Resolve读真实Ref，返回值捕获本次接收者；
// 不创建Popup、不增加Ref、不另造State，也不把现代C++虚表放进A4h。
class PopupMenuNativeModalTarget final : public IModalDialogRuntimeTargetReference,
    private IModalDialogRuntimeTarget
{
public:
    PopupMenuNativeModalTarget(YtlReferenceSlotRuntime& slot, PopupMenuNativeDependencies& dependencies) noexcept
        : slot_(slot), dependencies_(dependencies) {}
    IModalDialogRuntimeTarget* ResolveAt4A5330() override;
private:
    void BeginAtVslot44() override;
    bool PollAtVslot48() override;
    bool EndAtVslot4C() override;
    std::int32_t ResultAtVslot50() const override;
    YtlReferenceSlotRuntime& slot_;
    PopupMenuNativeDependencies& dependencies_;
    GuiDialogNativeBody5Ch* selected_ = nullptr;
};
}
