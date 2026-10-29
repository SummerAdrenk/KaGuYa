#pragma once

#include <Runtime/SaveFileCatalog.h>
#include <Runtime/SaveLoadSceneRuntime.h>

#include <Windows.h>

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace KaGuYa::Start::Runtime
{
class SaveLoadDialogProductionRuntime;

class ISaveLoadDialogFrame
{
public:
    virtual ~ISaveLoadDialogFrame() = default;
};

using SaveLoadDialogFrameRef = std::shared_ptr<ISaveLoadDialogFrame>;

class ISaveLoadDialogEventTarget
{
public:
    virtual ~ISaveLoadDialogEventTarget() = default;

    // 0x522350 对事件来源只访问 GuiControl 的重置与矩形虚槽。
    virtual void InvokeAtVslot08() = 0;
    [[nodiscard]] virtual RECT ReadRectAtVslot3C() const = 0;
};

using SaveLoadDialogEventTargetRef =
    std::shared_ptr<ISaveLoadDialogEventTarget>;

class ISaveLoadDialogControl : public ISaveLoadDialogEventTarget
{
public:
    virtual ~ISaveLoadDialogControl() = default;

    // 页码/槽位选择控件还会由 0x522350 以外的命令链访问这些派生字段。
    virtual void WriteAtVslot1C(std::int32_t value) = 0;
    virtual void WriteAtVslot24(std::int32_t value) = 0;
    virtual void OpenAtVslot44() = 0;
    [[nodiscard]] virtual std::int32_t ReadValueAt7C() const = 0;
    virtual void WriteValueAt7C(std::int32_t value) = 0;
    [[nodiscard]] virtual std::int32_t ReadValueAt84() const = 0;
    virtual void NotifySelectionAtVslot8C(
        std::int32_t previousValue,
        std::int32_t argument1,
        std::int32_t argument2) = 0;
};

using SaveLoadDialogControlRef = std::shared_ptr<ISaveLoadDialogControl>;

class ISaveLoadReturnTitleConfirmation
{
public:
    virtual ~ISaveLoadReturnTitleConfirmation() = default;

    [[nodiscard]] virtual std::int32_t RunAt5071D0() = 0;
};

using SaveLoadReturnTitleConfirmationRef =
    std::shared_ptr<ISaveLoadReturnTitleConfirmation>;

class ISaveLoadDeleteConfirmation
{
public:
    virtual ~ISaveLoadDeleteConfirmation() = default;

    // SaveLoadDialog+0x168 持有的确认窗口。+0x80 的模式与两个准备函数
    // 分别对应 0x51B8F0、0x502950 和 0x502B10。
    virtual void WriteModeAt80(std::int32_t value) = 0;
    virtual void PreparePageAt502950(std::int32_t pageNumber) = 0;
    virtual void PreparePageAndSlotAt502B10(
        std::int32_t pageNumber,
        std::int32_t slotNumber) = 0;
    [[nodiscard]] virtual std::int32_t RunAt5071D0() = 0;
};

using SaveLoadDeleteConfirmationRef =
    std::shared_ptr<ISaveLoadDeleteConfirmation>;

class ISaveLoadSlotConfirmation
{
public:
    virtual ~ISaveLoadSlotConfirmation() = default;

    // SaveLoadDialog+0x174 是保存与读取共用的确认窗口。+0x98 的模式
    // 0/1/2 分别代表读取、空槽保存和覆盖，0x538430 安装预览画面。
    virtual void WriteModeAtVslot98(std::int32_t value) = 0;
    virtual void ConfigureAt535FF0(
        const std::string& pageLabel,
        const std::string& slotLabel,
        const std::string& timestampLabel,
        const std::string& sceneLabel,
        bool isSaveMode) = 0;
    virtual void AssignPreviewFrameAt538430(
        const SaveLoadDialogFrameRef& frame) = 0;
    [[nodiscard]] virtual std::int32_t RunAt5071D0() = 0;
};

using SaveLoadSlotConfirmationRef =
    std::shared_ptr<ISaveLoadSlotConfirmation>;

class ISaveLoadFileFactory
{
public:
    virtual ~ISaveLoadFileFactory() = default;

    // 0x47AFA0 生成同一槽的 BMP 预览名；0x480830 读取 DAT 并返回
    // 0=成功、int 异常值或 -1 未知异常。
    [[nodiscard]] virtual std::string BuildPreviewFileNameAt47AFA0(
        std::int32_t slotNumber) = 0;
    [[nodiscard]] virtual std::int32_t LoadSlotAt480830(
        std::int32_t slotNumber) = 0;
};

using SaveLoadFileFactoryOwner = std::unique_ptr<ISaveLoadFileFactory>;

class ISaveLoadDeleteFactory
{
public:
    virtual ~ISaveLoadDeleteFactory() = default;

    virtual void DeleteAllAt5022E0() = 0;
    virtual void DeleteSlotAt476460(
        std::int32_t slotNumber,
        bool deleteDataFile) = 0;
};

using SaveLoadDeleteFactoryOwner =
    std::unique_ptr<ISaveLoadDeleteFactory>;

class ISaveLoadRegularSaveFactory : public ISaveLoadDeleteFactory
{
public:
    // 0x488CF0 返回零表示保存完成；非零分支由 0x532A90 显示错误并删除槽。
    [[nodiscard]] virtual std::int32_t SaveSlotAt488CF0(
        std::int32_t slotNumber,
        bool writeScreenshot) = 0;
};

using SaveLoadRegularSaveFactoryOwner =
    std::unique_ptr<ISaveLoadRegularSaveFactory>;

struct SaveLoadSceneLabelResolution final
{
    bool lookupSucceeded = false;
    bool labelObjectPresent = false;
    std::string label;
};

class ISaveLoadDialogChild
{
public:
    virtual ~ISaveLoadDialogChild() = default;

    // SaveLoadDialog+0x5C 容器的遍历顺序与 0x522350/0x508CF0 一致。
    [[nodiscard]] virtual std::int32_t StateAtVslot20() const = 0;
    virtual void WriteEnabledAtVslot24(std::int32_t value) = 0;
    [[nodiscard]] virtual std::int32_t GateAtVslot28() const = 0;
    virtual void OpenAtVslot44() = 0;
    virtual void UpdateAtVslot48() = 0;
    virtual void EndAtVslot4C() = 0;
    [[nodiscard]] virtual std::int32_t EventAtVslot50() const = 0;
    [[nodiscard]] virtual SaveLoadDialogEventTargetRef
        ResolveEventTargetAtVslot14(std::int32_t event) = 0;
};

using SaveLoadDialogChildRef = std::shared_ptr<ISaveLoadDialogChild>;

// SaveLoadDialog 的 +0x5C 派生 child 容器在 Begin(0x50E8B0) 内才由
// 0x5330A0 按需填充。生产运行时必须读取这个实时对象图，不能在外层构造时
// 复制当时仍为空的 vector。
class ISaveLoadDialogObjectGraph
{
public:
    virtual ~ISaveLoadDialogObjectGraph() = default;

    virtual void InitializePageAt5330A0(std::int32_t mode) = 0;
    virtual void OpenChildrenAt532560() = 0;
    [[nodiscard]] virtual const std::vector<SaveLoadDialogChildRef>&
        ChildrenInInsertionOrder() const noexcept = 0;
    [[nodiscard]] virtual SaveLoadDialogControlRef
        ResolveDialogControlAtVslot90(std::int32_t slot) = 0;
};

enum class SaveLoadDialogPhase : std::int32_t
{
    BeginTransition = 0,
    EnterInteractive = 1,
    ReturnInteractive = 2,
    ProcessChildren = 3,
    Complete = 4,
    RequestReturnToTitle = 5,
    ProcessDeleteCommand = 6,
    ProcessSaveCommand = 7,
    ProcessLoadCommand = 8,
    Finished = -1,
};

// SaveLoadDialog+0xAC 的 0x30 字节 operation。数值字段布局与
// sub_4C57F0/0x51CA80 一致；没有消费者证据的槽继续保留 offset 名。
struct SaveLoadDialogTransitionOperation final
{
    std::int32_t valueAt00 = 0;
    std::uint32_t sourceHandleAt04 = 0;
    std::int32_t valueAt08 = 0;
    std::int32_t valueAt0C = 0;
    std::int32_t valueAt10 = 0;
    std::int32_t valueAt14 = 0;
    std::int32_t valueAt18 = 0;
    std::int32_t selectorAt1C = -1;
    std::int32_t valueAt20 = 0;
    std::int32_t progressAt24 = 0;
    std::int32_t maximumAt28 = 255;
    std::int32_t valueAt2C = 0;

    void Reset() noexcept;
};

// 0x52F7A0 会把这一组字段交给多个内部过程共同修改。字段名保留原偏移，
// 尚未由其写入点证明的业务含义不提前命名。
struct SaveLoadDialogControlState final
{
    std::uint32_t completionResultAt68 = 0;
    SaveLoadDialogPhase phaseAt6C = SaveLoadDialogPhase::BeginTransition;
    std::int32_t transitionSubstateAt70 = 0;
    std::int32_t nextPhaseAt74 = 0;
    SaveLoadDialogTransitionOperation transitionAtAC;
    std::int32_t pageModeAtE4 = 0;
    std::int32_t sourceIndexAt160 = 0;
    std::int32_t slotIndexAt164 = 0;
};

class ISaveLoadDialogRuntimeServices
{
public:
    virtual ~ISaveLoadDialogRuntimeServices() = default;

    // SaveLoadDialog::Begin（0x50E8B0）的固定调用顺序。
    [[nodiscard]] virtual SaveLoadDialogFrameRef
        CapturePrimarySceneResourceAt4ACDD0() = 0;
    virtual void ClearClickedEventsAt532200() = 0;
    virtual void WriteDialogEnabledAtVslot24(std::int32_t value) = 0;
    virtual void ResetInputAtVslot3C() = 0;
    virtual void ResetChildrenAt522E90() = 0;
    virtual void InitializePageAt5330A0(std::int32_t pageIndex) = 0;
    virtual void RefreshPageAt532560() = 0;
    virtual void ClearSharedInputStateAt50EA19() = 0;

    // 0x51CA80 的直接依赖。转场状态机本身由 SaveLoadDialogRuntime 持有，
    // 不再把整个原函数委托给外部实现。
    virtual void CompositeTransitionFrameAtVslot5C(
        const SaveLoadDialogFrameRef& frame,
        std::int32_t argument) = 0;
    [[nodiscard]] virtual std::uint32_t ResolveTransitionSourceAt444770(
        const SaveLoadDialogFrameRef& frame,
        std::int32_t index) = 0;
    [[nodiscard]] virtual std::int32_t ResolveTransitionStepAt497900() = 0;
    [[nodiscard]] virtual bool PollTransitionAccelerationAt4C3430() = 0;
    [[nodiscard]] virtual bool PollTransitionAccelerationAt4AA970() = 0;
    virtual void RenderTransitionAt449DD0(
        SaveLoadDialogTransitionOperation& operation) = 0;
    virtual void CommitTransitionFrameAt4B02E0(std::int32_t argument) = 0;

    // 0x522350 仍需访问 SaveLoadDialog 外部的 slot 6 与控件/dirty 宿主；
    // 事件遍历和状态写回本身由 SaveLoadDialogRuntime 直接实现。
    [[nodiscard]] virtual std::int32_t ReadSlot6ValueAt04() const = 0;
    [[nodiscard]] virtual std::int32_t ReadSlot6ValueAt08() const = 0;
    [[nodiscard]] virtual std::int32_t ReadSlot6PageModeAt14() const = 0;
    virtual void WriteSlot6PageModeAt14(std::int32_t value) = 0;
    [[nodiscard]] virtual bool IsSlot6EntryAt20Present(
        std::int32_t index) const = 0;
    [[nodiscard]] virtual SaveLoadDialogControlRef
        ResolveDialogControlAtVslot90(std::int32_t index) = 0;
    virtual void SubmitControlRectAtVslot64(const RECT& rect) = 0;
    virtual void SubmitDialogBoundsAtVslot00Then08(const RECT& rect) = 0;

    // 0x51E320 的外部对象来源；控制流由 SaveLoadDialogRuntime 直接实现。
    [[nodiscard]] virtual bool
        ReadReturnTitleConfirmationFlagAtRootIndex1Offset7D() const = 0;
    [[nodiscard]] virtual SaveLoadReturnTitleConfirmationRef
        CreateReturnTitleConfirmationAt4E3330() = 0;
    virtual void SynchronizeRuntimeAt413BE0() = 0;
    [[nodiscard]] virtual HWND MainWindowAtRootOffset2C() const = 0;
    virtual void WriteSlot2ValueAt04(std::int32_t value) = 0;

    // 0x51B8F0/0x506CE0 的外部对象来源和文件系统副作用；确认控制流、
    // catalog 修改与控件失效由 SaveLoadDialogRuntime 直接实现。
    [[nodiscard]] virtual bool
        ReadDeleteConfirmationFlagAtRootIndex1Offset80() const = 0;
    [[nodiscard]] virtual bool
        ReadDeleteConfirmationFlagAtSlot1Offset7F() const = 0;
    [[nodiscard]] virtual SaveFileCatalog& MutableSaveFileCatalogAtSlot6() = 0;
    [[nodiscard]] virtual SaveLoadDeleteFactoryOwner
        CreateQuickSaveDeleteFactoryAt45B800() = 0;
    [[nodiscard]] virtual SaveLoadDeleteFactoryOwner
        CreateAutoSaveDeleteFactoryAt45B800() = 0;
    [[nodiscard]] virtual SaveLoadRegularSaveFactoryOwner
        CreateRegularSaveFactoryAt4DCDB0(
            std::int32_t pageNumber) = 0;
    virtual void RefreshSaveSelectionAt48DBD0() = 0;

    // 0x525480/0x532A90 的外部对象来源。确认、标签格式化、factory 生命周期、
    // catalog 刷新与控件失效由 SaveLoadDialogRuntime 直接实现。
    [[nodiscard]] virtual bool
        ReadSaveConfirmationFlagAtRootIndex1Offset77() const = 0;
    [[nodiscard]] virtual bool
        ReadOverwriteConfirmationFlagAtSlot1Offset7B() const = 0;
    [[nodiscard]] virtual bool
        ReadSaveScreenshotFlagAtSlot1Offset74() const = 0;
    [[nodiscard]] virtual std::int64_t ReadCurrentTimeAt56A321() const = 0;
    [[nodiscard]] virtual SaveLoadSceneLabelResolution
        ResolveCurrentSaveSceneLabelAt525480And532A90() = 0;
    [[nodiscard]] virtual SaveLoadDialogFrameRef
        CaptureSavePreviewFrameAt47D740() = 0;
    virtual void WriteCurrentSaveSceneLabelAtRootIndex0Offset04(
        const std::string& label) = 0;
    virtual void ShowSaveFailureMessageAt532A90() = 0;
    virtual void ReloadRegularSaveCatalogItemAt48EE00(
        std::int32_t pageIndex,
        std::int32_t slotIndex) = 0;

    // 0x520AB0/0x511FB0 的外部对象来源与文件系统副作用；确认控制流、
    // metadata 格式化、预览回退和 completion 写回由 runtime 直接实现。
    [[nodiscard]] virtual bool
        ReadLoadConfirmationFlagAtRootIndex1Offset78() const = 0;
    [[nodiscard]] virtual SaveLoadFileFactoryOwner
        CreateAutoLoadFactoryAt4DA870And511FB0() = 0;
    [[nodiscard]] virtual SaveLoadFileFactoryOwner
        CreateQuickLoadFactoryAt4DC5F0() = 0;
    [[nodiscard]] virtual SaveLoadFileFactoryOwner
        CreateRegularLoadFactoryAt4DCDB0(std::int32_t pageNumber) = 0;
    [[nodiscard]] virtual SaveLoadDialogFrameRef
        CreateSavePreviewSurfaceAt40C2E0() = 0;
    [[nodiscard]] virtual std::string BuildSavePreviewPathAt4C7210(
        const std::string& fileName) = 0;
    [[nodiscard]] virtual bool TryLoadSavePreviewAt511D90(
        const SaveLoadDialogFrameRef& destination,
        const std::string& path) = 0;
    virtual void ShowLoadFailureMessageAt511FB0() = 0;
    virtual void SubmitDirtyRectsAtVslot58() = 0;
};

// GuiControl::GuiSaveLoadDialog::SaveLoadDialog 虚表 0x5BE3C8 的
// +0x44/+0x48/+0x4C/+0x50 生产协调层。
class SaveLoadDialogRuntime final : public ISaveLoadDialog
{
public:
    SaveLoadDialogRuntime(
        ISaveLoadDialogRuntimeServices& services,
        std::int32_t initialPageIndex,
        SaveLoadDialogFrameRef transitionFrameAt94,
        SaveLoadDialogFrameRef backgroundFrameAtA0,
        SaveLoadDialogFrameRef savePreviewFrameAt144,
        SaveLoadDeleteConfirmationRef deleteConfirmationAt168,
        SaveLoadSlotConfirmationRef slotConfirmationAt174,
        const RECT& dialogBoundsAt78,
        std::vector<SaveLoadDialogChildRef> children);

    // 生产构造入口：children 由 0x5330A0 动态建立，后续查询始终指向
    // SaveLoadDialog 的当前八槽对象图。
    SaveLoadDialogRuntime(
        ISaveLoadDialogRuntimeServices& services,
        ISaveLoadDialogObjectGraph& objectGraph,
        std::int32_t initialPageIndex,
        SaveLoadDialogFrameRef transitionFrameAt94,
        SaveLoadDialogFrameRef backgroundFrameAtA0,
        SaveLoadDialogFrameRef savePreviewFrameAt144,
        SaveLoadDeleteConfirmationRef deleteConfirmationAt168,
        SaveLoadSlotConfirmationRef slotConfirmationAt174,
        const RECT& dialogBoundsAt78);

    void BeginAtVslot44() override;
    [[nodiscard]] bool PollAtVslot48() override;
    void EndAtVslot4C() override;
    [[nodiscard]] std::uint32_t CompletionResultAtVslot50() const override;

    [[nodiscard]] const SaveLoadDialogControlState& ControlState() const noexcept;
    [[nodiscard]] SaveLoadDialogControlState& MutableControlState() noexcept;
    [[nodiscard]] const SaveLoadDialogFrameRef& CapturedFrameAt88() const noexcept;
    [[nodiscard]] const SaveLoadDialogFrameRef& TransitionFrameAt94() const noexcept;
    [[nodiscard]] const SaveLoadDialogFrameRef& BackgroundFrameAtA0() const noexcept;
    [[nodiscard]] std::int32_t PageModeAtE4() const noexcept;

private:
    friend class SaveLoadDialogProductionRuntime;

    // 0x4F9030 的析构顺序跨越外层资源 owner，不能交给成员逆序隐式决定。
    void ReleaseConfirmationsAt4F9030() noexcept;
    void ReleasePreviewReferenceAt4F9030() noexcept;
    void ReleaseSurfaceFramesAt4F9030() noexcept;

    ISaveLoadDialogRuntimeServices* services_ = nullptr;
    ISaveLoadDialogObjectGraph* objectGraph_ = nullptr;
    std::vector<SaveLoadDialogChildRef> children_;
    SaveLoadDialogControlState state_;
    RECT dialogBoundsAt78_{};
    SaveLoadDialogFrameRef capturedFrameAt88_;
    SaveLoadDialogFrameRef transitionFrameAt94_;
    SaveLoadDialogFrameRef backgroundFrameAtA0_;
    SaveLoadDialogFrameRef savePreviewFrameAt144_;
    SaveLoadDeleteConfirmationRef deleteConfirmationAt168_;
    SaveLoadSlotConfirmationRef slotConfirmationAt174_;

    void AdvanceTransitionAt51CA80();
    void ProcessChildrenAt522350();
    [[nodiscard]] bool RequestReturnToTitleAt51E320();
    void ProcessDeleteCommandAt51B8F0();
    [[nodiscard]] bool ExecuteDeleteAt506CE0();
    void ProcessSaveCommandAt525480();
    [[nodiscard]] bool ExecuteSaveAt532A90();
    [[nodiscard]] bool ProcessLoadCommandAt520AB0();
    [[nodiscard]] bool ExecuteLoadAt511FB0();
    [[nodiscard]] SaveLoadFileFactoryOwner CreateLoadFactory(
        std::int32_t sourceIndex);
    [[nodiscard]] const std::vector<SaveLoadDialogChildRef>&
        CurrentChildren() const noexcept;
    void InitializeCurrentPage(std::int32_t mode);
    void OpenCurrentChildren();
    [[nodiscard]] SaveLoadDialogControlRef ResolveDialogControl(
        std::int32_t slot);
    void SubmitResolvedControlRect(
        const SaveLoadDialogEventTargetRef& control);
};
}
