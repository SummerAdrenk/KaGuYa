#pragma once

#include <Runtime/MusicAppreciationSceneRuntime.h>

#include <Windows.h>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <span>
#include <string_view>

namespace KaGuYa::Start::Runtime
{
struct MusicAppreciationItemStaticData final
{
    std::int32_t controlId = -1;
    POINT position{};
    std::string_view soundResource;
};

struct MusicAppreciationNavigationPair final
{
    std::int32_t sourceControlId = -1;
    std::int32_t destinationControlId = -1;
};

struct MusicAppreciationOperationKeyStaticData final
{
    std::int32_t actionAt04 = 0;
    std::span<const MusicAppreciationNavigationPair> navigationPairs;
    std::int32_t extendedValueAt1C = -1;
    std::int32_t extendedValueAt20 = 0;
};

struct MusicAppreciationCheckboxStaticData final
{
    std::int32_t firstControlId = 2;
    std::string_view off;
    std::string_view offTargetFirst;
    std::string_view offTargetSecond;
    std::string_view offClick;
    std::string_view selectionRange;
    std::string_view on;
    std::string_view onTargetFirst;
    std::string_view onTargetSecond;
    std::string_view onClick;
    std::int32_t valueAt40 = 6;
    std::int32_t valueAt44 = -1;
};

struct MusicAppreciationButtonStaticData final
{
    std::span<const std::string_view> dialogBaseResources;
    MusicAppreciationCheckboxStaticData checkbox;
    std::span<const MusicAppreciationItemStaticData> items;
    std::span<const MusicAppreciationOperationKeyStaticData> operationKeys;
    std::int32_t skippedGuiChildAt40 = 2;
};

// 仅对应 18#ママトモ Start.exe 中 0x609278..0x60956F 的固定表。
[[nodiscard]] const MusicAppreciationButtonStaticData&
Sample18MusicAppreciationButtonStaticData() noexcept;

class IMusicAppreciationButtonDirtyBatch
{
public:
    virtual ~IMusicAppreciationButtonDirtyBatch() = default;

    virtual void PrepareAtVslot04() = 0;
    virtual void AddAtVslot08(const RECT& rect) = 0;
    [[nodiscard]] virtual std::span<const RECT> RectsAtVslots18And1C() const = 0;
    virtual void ClearAtVslot00() noexcept = 0;
};

// ModeSoundDialog 尚未完成独立生产实现。这里仅声明 CSoundBtn 已证明会调用的虚槽，
// 由外层注入真实对象；不得在按钮核心内伪造资源、控件或事件结果。
class IMusicAppreciationModeSoundDialog
{
public:
    virtual ~IMusicAppreciationModeSoundDialog() = default;

    virtual void InitializeAtVslot44() = 0;
    [[nodiscard]] virtual std::int32_t UpdateAtVslot48() = 0;
    [[nodiscard]] virtual std::int32_t SelectionAtVslot50() = 0;
    [[nodiscard]] virtual std::span<const RECT>
        EventDirtyRectsAtVslots00_18_1C() const = 0;
    virtual void DrawAtVslot60(const RECT* clip) = 0;

    [[nodiscard]] virtual std::int32_t ItemCount() const noexcept = 0;
    virtual void ClearItemStateAt04And20(std::int32_t itemIndex) = 0;
};

// 0x4B4920 通过接收者自身的 +0x14 子项表、+0x20 邻接表和 +0x30
// operation-key 表工作；它调用的 +0x18/+0x1C/+0x28/+0x2C 也都属于
// CSoundBtn 的 IBtnProc/IModeBtnProc 基类虚槽，而不是 +0x50 的 ModeSoundDialog。
class IMusicAppreciationButtonNavigationState
{
public:
    virtual ~IMusicAppreciationButtonNavigationState() = default;

    [[nodiscard]] virtual std::int32_t CurrentItemAtVslot2C() const = 0;
    [[nodiscard]] virtual bool IsItemNavigableAt4B4920(
        std::int32_t itemIndex) const = 0;
    virtual void ResetItemAtVslot18(std::int32_t itemIndex) = 0;
    [[nodiscard]] virtual std::int32_t FocusItemAtVslot1C(
        std::int32_t itemIndex) = 0;
    virtual void WriteCurrentItemAtVslot28(std::int32_t itemIndex) = 0;
};

class IMusicAppreciationGuiChildren
{
public:
    virtual ~IMusicAppreciationGuiChildren() = default;

    [[nodiscard]] virtual std::size_t CountAtPlus10() const noexcept = 0;
    [[nodiscard]] virtual std::int32_t StateAt04(std::size_t index) const = 0;
    [[nodiscard]] virtual std::int32_t ProcessAt4A98E0(
        std::size_t index,
        bool forced) = 0;
    [[nodiscard]] virtual RECT VisibleRect(std::size_t index) const = 0;
    virtual void DrawCurrentStateAt441980(
        std::size_t index,
        const RECT& clip) = 0;
};

class IMusicAppreciationButtonServices
{
public:
    virtual ~IMusicAppreciationButtonServices() = default;

    [[nodiscard]] virtual std::unique_ptr<IMusicAppreciationButtonDirtyBatch>
        CreateDirtyBatchAt4A2E60() = 0;
    [[nodiscard]] virtual IMusicAppreciationButtonNavigationState&
        NavigationStateAt4A2E60() noexcept = 0;
    [[nodiscard]] virtual std::shared_ptr<IMusicAppreciationModeSoundDialog>
        CreateModeSoundDialogAt4E3900(
            const MusicAppreciationButtonStaticData& staticData) = 0;
    [[nodiscard]] virtual std::shared_ptr<IMusicAppreciationGuiChildren>
        AcquireGuiChildrenAt47B7B0() = 0;
    [[nodiscard]] virtual RECT ReadGraphicsTargetRectAt4180C0() const = 0;

    [[nodiscard]] virtual std::int32_t PollBaseSelectionAt4AA200() = 0;
    virtual void ClearSoundMappingAt437680() = 0;
    virtual void StopCurrentSoundAt4383B0() = 0;
    virtual void BindAndPlayAt4BCCC0(std::string_view resourceName) = 0;
    virtual void EmitExitSoundAt4BCD70() = 0;
    [[nodiscard]] virtual std::string_view ReadExtensionAt45DA40(
        std::string_view resourceName) const = 0;
    virtual void SubmitInvalidatedRect(const RECT& rect) = 0;
};

// ModeSoundScene::CSoundBtn（0x4A1C70，原对象大小 0x70）的行为核心。
// 本类型不伪造原字节布局，只保持已审计的调用顺序、状态与失败传播语义。
class MusicAppreciationButtonRuntime final : public IMusicAppreciationButton
{
public:
    MusicAppreciationButtonRuntime(
        IMusicAppreciationButtonServices& services,
        const MusicAppreciationButtonStaticData& staticData);
    ~MusicAppreciationButtonRuntime() override;

    [[nodiscard]] std::int32_t ProcessSelectionAtVslot04() override;
    void InvalidateAtVslot08(RECT rect) override;
    [[nodiscard]] std::int32_t ItemCountAt3C() const override;
    [[nodiscard]] std::int32_t SelectedItemAt68() const override;
    void WriteSelectedItemAt68(std::int32_t value) override;
    [[nodiscard]] std::uint8_t MovieSelectionPendingAt6C() const override;
    void WriteMovieSelectionPendingAt6C(std::uint8_t value) override;
    void ClearSelectedItemStateAt04And20(std::int32_t index) override;

    [[nodiscard]] std::int32_t MoveSelectionAtVslot30(
        std::int32_t actionAt04);
    void SynchronizeAt4ABB40();

private:
    [[nodiscard]] std::int32_t ItemIndexFromControlId(
        std::int32_t controlId) const noexcept;
    [[nodiscard]] const MusicAppreciationOperationKeyStaticData&
        OperationKeyForAction(std::int32_t actionAt04) const;
    void SelectSoundAt4C1A20(std::int32_t itemIndex);

    IMusicAppreciationButtonServices* services_ = nullptr;
    const MusicAppreciationButtonStaticData* staticData_ = nullptr;
    std::unique_ptr<IMusicAppreciationButtonDirtyBatch> dirtyBatch_;
    IMusicAppreciationButtonNavigationState* navigationState_ = nullptr;
    std::shared_ptr<IMusicAppreciationGuiChildren> guiChildrenAt44_;
    std::shared_ptr<IMusicAppreciationModeSoundDialog> dialogAt50_;
    std::int32_t skippedGuiChildAt40_ = 2;
    std::int32_t itemCountAt3C_ = 0;
    std::int32_t selectedItemAt68_ = -1;
    std::uint8_t movieSelectionPendingAt6C_ = 0;
};
}
