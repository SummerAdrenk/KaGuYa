#pragma once

#include <Runtime/ModalDialogRunnerRuntime.h>

namespace KaGuYa::Start::Runtime
{
class IGraphicsPresentNativeApi;
class RootFrameControlRuntime;
class WindowInputUpdateRuntime;
class IRootLogicalRectangleSynchronization;
class RootRuntimeOwner;

// 507398..5073EB的系统叶；窗口过滤和A版入口由Win32实现固定，不由调用方猜测。
class IModalDialogMessageApi
{
public:
    virtual ~IModalDialogMessageApi() = default;
    [[nodiscard]] virtual bool Peek(MSG& message) = 0;
    [[nodiscard]] virtual int Get(MSG& message) = 0;
    virtual void Translate(const MSG& message) = 0;
    virtual void Dispatch(const MSG& message) = 0;
};

class Win32ModalDialogMessageApi final : public IModalDialogMessageApi
{
public:
    [[nodiscard]] bool Peek(MSG& message) override;
    [[nodiscard]] int Get(MSG& message) override;
    void Translate(const MSG& message) override;
    void Dispatch(const MSG& message) override;
};

// 只借用生产根的同一计时器、Graphics和输入适配器；不建立第二套模态运行状态。
// 413BE0同步由实际owner提供，不能传入空操作来冒充完整运行根。
class ModalDialogServicesRuntime final : public IModalDialogRunnerServices
{
public:
    ModalDialogServicesRuntime(IGraphicsPresentNativeApi& graphics, RootFrameControlRuntime& frame,
        IRootLogicalRectangleSynchronization& root, WindowInputUpdateRuntime& input,
        IModalDialogMessageApi& messages) noexcept;
    ModalDialogServicesRuntime(IGraphicsPresentNativeApi& graphics, RootRuntimeOwner& root,
        WindowInputUpdateRuntime& input, IModalDialogMessageApi& messages) noexcept;

    void RenderScreenIfGraphicsLoaded() override;
    void SynchronizeRuntimeAt413BE0() override;
    [[nodiscard]] bool RootOwnerUpdateEnabledAt1B0() const override;
    void UpdateRootOwnerAt6CAtVslot18() override;
    [[nodiscard]] bool PeekMessageNoRemove(MSG& message) override;
    [[nodiscard]] int GetMessageAt5073B0(MSG& message) override;
    void TranslateMessageAt5073C9(const MSG& message) override;
    void DispatchMessageAt5073D3(const MSG& message) override;
    [[nodiscard]] bool ExitRequestedAt1BC() const override;
    void UpdateInputAt450AA0() override;

private:
    IGraphicsPresentNativeApi& graphics_;
    [[nodiscard]] RootFrameControlRuntime& Frame() const;
    RootFrameControlRuntime* frame_ = nullptr;
    RootRuntimeOwner* owner_ = nullptr;
    IRootLogicalRectangleSynchronization& root_;
    WindowInputUpdateRuntime& input_;
    IModalDialogMessageApi& messages_;
};
}
