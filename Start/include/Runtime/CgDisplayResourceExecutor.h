#pragma once

#include <Runtime/CgDisplaySceneRuntime.h>
#include <Runtime/ParamsPatternSource.h>
#include <Runtime/PatternResourceLoadTarget.h>
#include <Runtime/PatternResourceNameBorrow.h>
#include <Runtime/RuntimeCString.h>

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace KaGuYa::Start::Runtime
{
// 0x4A84E0 直接操作 CCGDispWin 与 CGOpeWin/CGOperationDialog 的资源边界。
// 该函数没有普通图层的 dirty-rect 提交，也不接受 Pattern 类型 3；
// 不能复用 0x4CB030 执行器。
class ICgDisplayResourceExecutionTarget
{
public:
    virtual ~ICgDisplayResourceExecutionTarget() = default;

    [[nodiscard]] virtual std::size_t PrimarySpriteSlotCount() const = 0;
    [[nodiscard]] virtual const char* PrimaryLogicalNameCharacters(
        std::size_t slotIndex) const = 0;
    virtual void ClearPrimarySprite(std::size_t slotIndex) = 0;
    [[nodiscard]] virtual PatternSpriteReceiver CaptureSpriteReceiver(std::uint32_t slotIndex) const = 0;
    virtual void LoadResource(std::uint32_t slotIndex, PatternSpriteReceiver capturedReceiver,
        const PatternResourceLoadRequest& request, const IPatternResourceFailureName& failureName,
        IPatternResourceLoadCommit& commit, IRuntimeStringManagerSource& strings) = 0;

    // 4A8848 固定 receiver；后续比较/清理/装载不得从 Scene 重新取得 Sprite。
    [[nodiscard]] virtual const char* ReceiverPrimaryNameCharacters(PatternSpriteReceiver receiver) const = 0;
    // 不读辅助表，4A889D处才由执行器CaptureRange取得本轮begin/count。
    [[nodiscard]] virtual PatternResourceNamesBorrow ReceiverAuxiliaryNames(
        PatternSpriteReceiver receiver) const = 0;
    virtual void ClearReceiverFileGroup(PatternSpriteReceiver receiver) = 0;
    virtual void SetReceiverLogicalName(PatternSpriteReceiver receiver, const RuntimeCString& source) = 0;
    virtual void SetReceiverActive(PatternSpriteReceiver receiver, bool active) = 0;
    virtual void SetReceiverOpaqueValuesAt2CAnd30(PatternSpriteReceiver receiver,
        std::uint32_t valueAt2C, std::uint32_t valueAt30) = 0;

    virtual void SetCgDisplayWindowActiveAt0C(bool active) = 0;
    virtual void SetCgOperationWindowActiveAt0C(bool active) noexcept = 0;
    virtual void SetCgOperationDialogSelectionValueAt7C(
        std::int32_t value) noexcept = 0;
    virtual void OpenCgOperationDialogAtVslot44() = 0;
};

// ModeCGdispScene::CModeCGdisp::0x4A84E0。全部 C++ 异常折叠为 false；
// 已经清空或装载的槽不回滚。selectedMappingIndexAt28 必须来自 0x4AD780
// 生成的有效记录，原函数本身不做范围检查。
[[nodiscard]] bool ExecuteCgDisplayResourceSelectionAt4A84E0(
    const IParamsPatternSource& source,
    const CgDisplaySelectionState& selection,
    ICgDisplayResourceExecutionTarget& target,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
}
