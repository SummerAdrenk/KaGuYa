#pragma once

#include <Windows.h>

#include <cstdint>

namespace KaGuYa::Start::Runtime
{
// 0x5071D0 只通过 +0x44/+0x48/+0x4C/+0x50 驱动对话框。
// 这里保留四个独立调用，不能把 Begin/Poll/End 折叠成单个回调。
class IModalDialogRuntimeTarget
{
public:
    virtual ~IModalDialogRuntimeTarget() = default;

    virtual void BeginAtVslot44() = 0;
    [[nodiscard]] virtual bool PollAtVslot48() = 0;
    [[nodiscard]] virtual bool EndAtVslot4C() = 0;
    [[nodiscard]] virtual std::int32_t ResultAtVslot50() const = 0;
};

// 0x4A5330 解析由三个 DWORD 组成的 YTL 引用。0x5071D0 在每次虚调用前都会
// 重新解析目标，因此运行器不能把首次得到的对象地址缓存到整个模态循环结束。
class IModalDialogRuntimeTargetReference
{
public:
    virtual ~IModalDialogRuntimeTargetReference() = default;

    [[nodiscard]] virtual IModalDialogRuntimeTarget*
        ResolveAt4A5330() = 0;
};

// 0x5071D0 访问的引擎单例与 Win32 消息队列边界。消息 API 也保持
// Peek/Get/Translate/Dispatch 四步，以便逐项验证 GetMessage=-1/0/>0。
class IModalDialogRunnerServices
{
public:
    virtual ~IModalDialogRunnerServices() = default;

    virtual void RenderScreenIfGraphicsLoaded() = 0;
    virtual void SynchronizeRuntimeAt413BE0() = 0;
    [[nodiscard]] virtual bool RootOwnerUpdateEnabledAt1B0() const = 0;
    virtual void UpdateRootOwnerAt6CAtVslot18() = 0;

    [[nodiscard]] virtual bool PeekMessageNoRemove(MSG& message) = 0;
    [[nodiscard]] virtual int GetMessageAt5073B0(MSG& message) = 0;
    virtual void TranslateMessageAt5073C9(const MSG& message) = 0;
    virtual void DispatchMessageAt5073D3(const MSG& message) = 0;

    [[nodiscard]] virtual bool ExitRequestedAt1BC() const = 0;
    virtual void UpdateInputAt450AA0() = 0;
};

class ModalDialogRunnerRuntime final
{
public:
    explicit ModalDialogRunnerRuntime(
        IModalDialogRunnerServices& services) noexcept;

    [[nodiscard]] std::int32_t RunAt5071D0(
        IModalDialogRuntimeTargetReference& targetReference);

private:
    IModalDialogRunnerServices* services_ = nullptr;
};
}
