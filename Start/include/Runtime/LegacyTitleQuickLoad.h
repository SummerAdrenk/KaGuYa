#pragma once

#include <Runtime/LegacyTitleWorkflow.h>
#include <Runtime/QuickOperationsNative.h>

namespace KaGuYa::Start::Runtime
{
// Baku 4199C0/501870：root 的唯一确认状态，不是原对象布局，也不由 quickload 缓存。
// root 的旧 GUI 调度、Prepare/Poll 和消息框查询必须借同一实例。
struct LegacyTitleQuickLoadModalState final
{
    std::int32_t busy = 0;
    std::int32_t kind = -1;
    std::int32_t mode = 0;
    std::int32_t result = -1;
    void Reset() noexcept;
};

// 旧消息框真实拥有者的借用视图；不能以现代同步 RunAt5071D0 代替 Begin/Poll。
class ILegacyTitleQuickLoadMessage
{
public:
    virtual ~ILegacyTitleQuickLoadMessage() = default;
    virtual void Begin() = 0;                // Baku vslot40，4DC253
    virtual bool Poll() = 0;                 // Baku vslot44，4DC2C5
    virtual std::int32_t Result() = 0;        // Baku vslot4C，4DC2D6
    virtual bool End() = 0;                  // Baku vslot48，4DC2F3
    // 4C9960 的唯一背景 Ref；允许槽为空，禁止返回第二份 Ref 或裸 handle。
    virtual YtlReferenceSlotRuntime& BackgroundSlot() = 0;
};

class ILegacyTitleQuickLoadSource
{
public:
    virtual ~ILegacyTitleQuickLoadSource() = default;
    // 416AD0 -> 4169A0：每次选择当前 root 的旧 GUI owner；不存在才返回 nullptr。
    // 不得把现代 SystemInformationDisplayNativeBody14h 强转成此状态。
    virtual LegacyTitleQuickLoadModalState* CaptureModal() = 0;
    // 46A5D0/46A5B0：仅查找已存在的消息框，kind=0 按 mode 选槽。
    // 本适配只发起 (0,4)；root 若复用 Tick 到其它种类，须提供其真实对应消息框。
    virtual ILegacyTitleQuickLoadMessage* FindMessage(LegacyTitleQuickLoadModalState&) = 0;
    // 4CC220：已选统一 CParams，读入真实旧存档，mode 低 byte=1；AL 表示成功。
    // 不能未经旧归档协议核实就别名到现代 ReadContainerAt4C7420。
    virtual std::uint32_t ReadLegacyContainer(void* selectedParams,
        const char* nameCp932, std::uint32_t mode) = 0;
    // 4CD230：重新选取的 CParams；真实重建历史临时文件/索引，并保留局部失败提示。
    // 必填真实实现，禁止默认 return 1，也不能以 AppendScriptParamsHistory 代替。
    virtual std::uint32_t RebuildLegacyHistory(void* selectedParams) = 0;
};

// 4DC2A0：由 root 在旧 modal 调度点驱动，不在 Title::Poll 内同步运行。
// 不自行判断 busy；调用者保持原调度门控。End()==true 才 Reset，异常不补 Reset。
void TickLegacyTitleQuickLoadModal(LegacyTitleQuickLoadModalState&,
    ILegacyTitleQuickLoadSource&);

class LegacyTitleQuickLoad final : public ILegacyTitleQuickLoad
{
public:
    // shared 可直接借 RootQuickOperationsNativeServicesRuntime::Source()。
    // 只用环境/Registry/字符串/Root-CParams选择/MessageBox叶，不调用现代 QuickLoad。
    // 两个源均须长于本对象；legacy 的四个纯虚端口必须真实绑定。
    LegacyTitleQuickLoad(IQuickOperationsNativeSource& shared,
        ILegacyTitleQuickLoadSource& legacy) noexcept : shared_(shared), legacy_(legacy) {}
    LegacyTitleQuickLoad(const LegacyTitleQuickLoad&) = delete;
    LegacyTitleQuickLoad& operator=(const LegacyTitleQuickLoad&) = delete;
    std::int32_t Prepare() override;
    std::int32_t Poll() override;
    bool Load() override;
    YtlReferenceSlotRuntime* MessageBackgroundSlot() override;

private:
    bool ConfirmationEnabled();
    std::int32_t LatestQuickItem();
    void* CaptureParams();
    int MessageBox(const char* textCp932, UINT type);
    IQuickOperationsNativeSource& shared_;
    ILegacyTitleQuickLoadSource& legacy_;
};

const char* LegacyTitleQuickLoadConfirmationText() noexcept; // Baku 529E9C，CP932
}
