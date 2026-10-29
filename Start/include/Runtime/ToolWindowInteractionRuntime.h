#pragma once

#include <cstdint>
#include <memory>

namespace KaGuYa::Start::Runtime
{
class ICgDisplayButtonGroupDirtyRects;
class IToolWindowDialog;
class ToolWindowLayerRuntime;

// 与六项图层接口由同一个实际 Dialog 实现，不创建第二份控件或事件状态。
class IToolWindowDialogInteraction
{
public:
    virtual ~IToolWindowDialogInteraction() = default;
    virtual void UpdateAtVslot54() = 0;
    [[nodiscard]] virtual std::int32_t ResultAtVslot50() = 0;
    [[nodiscard]] virtual std::int32_t ScanControlStateAt50FD20() = 0;
    virtual void SetControlEnabledAt508470(std::int32_t id, std::int32_t enabled) = 0;
    virtual void SetControlCheckedAt501FA0(std::int32_t id, std::int32_t checked) = 0;
    virtual void ResetControlAt5312B0(std::int32_t id) = 0;
    virtual void InvalidateAllControlsAt530DC0() = 0;
    virtual void ReconcileControlsAt527720() = 0;
    virtual void ClearActiveControlAtVslot30() = 0;
    virtual void SetInputByteAtVslot1C(std::uint8_t value) = 0;
    [[nodiscard]] virtual ICgDisplayButtonGroupDirtyRects& OwnDirtyRectsAtVslot00() = 0;
};

// 每次调用读取同一生产对象图的当前值；不能用构造快照、缺失默认值或影子状态实现。
class IToolWindowInteractionServices
{
public:
    virtual ~IToolWindowInteractionServices() = default;
    [[nodiscard]] virtual std::uint32_t ReadUnSaveModeAt04() = 0;
    [[nodiscard]] virtual std::uint8_t ReadUnSaveByteAt08() = 0;
    [[nodiscard]] virtual std::uint8_t ReadUnSaveByteAt09() = 0;
    [[nodiscard]] virtual std::uint8_t ReadUnSaveByteAt0C() = 0;
    [[nodiscard]] virtual std::uint32_t ReadUnSaveFlagsAt14() = 0;
    [[nodiscard]] virtual std::uint32_t ReadUnSaveFlagsAt18() = 0;
    [[nodiscard]] virtual std::uint32_t ReadUnSaveFlagsAt1C() = 0;
    [[nodiscard]] virtual std::uint32_t ReadUnSaveFlagsAt20() = 0;
    // 原 419AC0 仅比较 +3C 指针，不改成数组长度或 46C830 声音许可扫描。
    [[nodiscard]] virtual bool HasVoiceArrayStorageAt3C() = 0;
    [[nodiscard]] virtual std::int32_t ReadTextGlyphCursorAt50() = 0;
    [[nodiscard]] virtual std::uint8_t ReadMessageWindowEnabledAt0C() = 0;
    // 对应真实 Keyboard action6 当前 held，不是新按下或 configured mouse 模式。
    [[nodiscard]] virtual bool IsSkipKeyHeldAt4198D0() = 0;
    [[nodiscard]] virtual std::int32_t ReadReplayValueAt4C73A0() = 0;
    // 经同一 profile/VM 读取 5AE270 的 CP932 名称，必须保留原全角空格。
    [[nodiscard]] virtual std::int32_t ReadAffectionDisplayFlagAt4C73A0() = 0;
};

class ToolWindowInteractionRuntime final
{
public:
    ToolWindowInteractionRuntime(ToolWindowLayerRuntime& window,
        IToolWindowInteractionServices& services) noexcept;

    void SetModeAt434FB0(std::int32_t mode);
    [[nodiscard]] std::uint32_t TickAt4BE200();
    // 沿用外层命令名称；原函数重置交互/提交 dirty，不保存或恢复状态快照。
    void CaptureAt4C01F0();
    void RestoreAt4BF4E0();
    void SetCaptureModeAt4C2300(std::uint8_t value);
    void PropagateDirtyRectsAt415910();
    void ResetSkipLatchAt4AB910(std::uint8_t value);

private:
    struct SelectedInteraction final
    {
        std::shared_ptr<IToolWindowDialog> owner;
        IToolWindowDialogInteraction* interaction;
    };
    [[nodiscard]] SelectedInteraction SelectInteraction();
    [[nodiscard]] bool CommonGate(bool checkHeld);
    [[nodiscard]] bool WindowGateAt419BF0(bool checkHeld);
    [[nodiscard]] bool VoiceGateAt419AC0(bool checkHeld);
    [[nodiscard]] bool AffectionGateAt419910(bool checkHeld);
    [[nodiscard]] bool SaveLoadGate(bool checkHeld, bool load);
    [[nodiscard]] std::int32_t SkipStatus(bool checkHeld);
    [[nodiscard]] std::int32_t AutoStatus(bool checkHeld);
    [[nodiscard]] std::uint32_t CurrentEventMask() const;
    void SetControlEnabled(std::int32_t id, bool value);
    void SetControlChecked(std::int32_t id, bool value);

    ToolWindowLayerRuntime& window_;
    IToolWindowInteractionServices& services_;
};
}
