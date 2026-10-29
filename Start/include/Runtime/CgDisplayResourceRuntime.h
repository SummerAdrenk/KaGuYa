#pragma once

#include <Runtime/CgDisplayResourceExecutor.h>
#include <Runtime/CgDisplayWindowRuntime.h>
#include <Runtime/SpriteArchiveResourceLoader.h>

#include <cstddef>
#include <cstdint>
#include <string>

namespace KaGuYa::Start::Runtime
{
// CGOperationDialog::vslot +0x44（0x50BFD0）的外部可见边界。
// 实现负责激活 Dialog/子控件，并把已写入 +0x7C 的 opaque 选择值提交给按钮组。
class ICgOperationDialogResourceControl
{
public:
    virtual ~ICgOperationDialogResourceControl() = default;

    virtual void SetSelectionValueAt7C(std::int32_t value) noexcept = 0;
    virtual void OpenAtVslot44() = 0;
};

class ICgDisplaySpriteResourceLoader
{
public:
    virtual ~ICgDisplaySpriteResourceLoader() = default;

    virtual void ExecuteLoad(const PatternResourceLoadRequest& request,
        const IPatternResourceFailureName& failureName, IPatternResourceLoadAction& action,
        IRuntimeStringManagerSource& strings) = 0;
};

// 将 CG 专用资源边界接到已审计的 archive reader 与 SpriteRuntime。
class CgDisplayArchiveSpriteResourceLoader final
    : public ICgDisplaySpriteResourceLoader
{
public:
    explicit CgDisplayArchiveSpriteResourceLoader(const PatternResourceLoadServices& services) noexcept;
    void ExecuteLoad(const PatternResourceLoadRequest& request,
        const IPatternResourceFailureName& failureName, IPatternResourceLoadAction& action,
        IRuntimeStringManagerSource& strings) override;

private:
    PatternResourceLoadServices services_;
};

// CCGDispWin 与 CCGOpeWin 单一 CGOperationDialog 之间的生产资源协调器。
// Sprite 数组由 CgDisplayWindowRuntime 直接拥有；本对象只按 0x4A84E0
// 协调窗口、loader 与 Dialog，不能再次持有一份平行资源状态。
class CgDisplayResourceRuntime final
    : public ICgDisplayResourceExecutionTarget
{
public:
    CgDisplayResourceRuntime(
        const CgDisplayWindowRef& displayWindowSlot,
        ICgOperationWindow& operationWindow,
        ICgDisplaySpriteResourceLoader& resourceLoader,
        ICgOperationDialogResourceControl& operationDialog) noexcept;

    [[nodiscard]] SpriteRuntime& SpriteAt(std::size_t slotIndex);
    [[nodiscard]] const SpriteRuntime& SpriteAt(std::size_t slotIndex) const;
    [[nodiscard]] bool CgDisplayWindowActiveAt0C() const;
    [[nodiscard]] bool CgOperationWindowActiveAt0C() const noexcept;

    [[nodiscard]] std::size_t PrimarySpriteSlotCount() const override;
    [[nodiscard]] const char* PrimaryLogicalNameCharacters(
        std::size_t slotIndex) const override;
    void ClearPrimarySprite(std::size_t slotIndex) override;
    [[nodiscard]] PatternSpriteReceiver CaptureSpriteReceiver(std::uint32_t slotIndex) const override;
    void LoadResource(std::uint32_t slotIndex, PatternSpriteReceiver capturedReceiver,
        const PatternResourceLoadRequest& request, const IPatternResourceFailureName& failureName,
        IPatternResourceLoadCommit& commit, IRuntimeStringManagerSource& strings) override;
    [[nodiscard]] const char* ReceiverPrimaryNameCharacters(PatternSpriteReceiver receiver) const override;
    [[nodiscard]] PatternResourceNamesBorrow ReceiverAuxiliaryNames(
        PatternSpriteReceiver receiver) const override;
    void ClearReceiverFileGroup(PatternSpriteReceiver receiver) override;
    void SetReceiverLogicalName(PatternSpriteReceiver receiver, const RuntimeCString& source) override;
    void SetReceiverActive(PatternSpriteReceiver receiver, bool active) override;
    void SetReceiverOpaqueValuesAt2CAnd30(PatternSpriteReceiver receiver,
        std::uint32_t valueAt2C, std::uint32_t valueAt30) override;
    void SetCgDisplayWindowActiveAt0C(bool active) override;
    void SetCgOperationWindowActiveAt0C(bool active) noexcept override;
    void SetCgOperationDialogSelectionValueAt7C(
        std::int32_t value) noexcept override;
    void OpenCgOperationDialogAtVslot44() override;

private:
    [[nodiscard]] CgDisplayWindowRuntime& RequireCurrentDisplayWindow() const;
    // 借用 Scene 的实际成员槽；不复制 shared_ptr，不在 target 构造时解析正文。
    const CgDisplayWindowRef& displayWindowSlot_;
    ICgOperationWindow* operationWindow_ = nullptr;
    ICgDisplaySpriteResourceLoader* resourceLoader_ = nullptr;
    ICgOperationDialogResourceControl* operationDialog_ = nullptr;
};
}
