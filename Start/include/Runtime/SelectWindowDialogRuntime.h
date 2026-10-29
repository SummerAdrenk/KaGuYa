#pragma once
#include <Platform/InputViews.h>

#include <Runtime/CgOperationSurfaceResourceRuntime.h>
#include <Runtime/CgDisplayButtonGroupRuntime.h>
#include <Runtime/GuiInputStateRuntime.h>
#include <Runtime/OpacityDialogObjectGraphRuntime.h>

#include <optional>
#include <span>

namespace KaGuYa::Start::Runtime
{
class ScrVmMenuState;

// 图层与交互共用同一个 Dialog；现代析构槽不表示原 x86 虚表布局。
class ISelectWindowDialog
{
public:
    virtual ~ISelectWindowDialog() = default;
    virtual void ResetAt50F000() = 0;
    [[nodiscard]] virtual std::int32_t ProcessAt530270() = 0;
    [[nodiscard]] virtual std::int32_t ResultAt50ABD0() const = 0;
    [[nodiscard]] virtual RECT VisibleBoundsAt50AD40(std::int32_t ordinal) const = 0;
    [[nodiscard]] virtual ICgDisplayButtonGroupDirtyRects& OwnDirtyRectsAtVslot00() = 0;
    virtual void DrawAtVslot60(const RECT& dirty) = 0;
    // 对应原按值参数；调用方转交一次已增持引用，不在返回后再次释放。
    virtual void CompositeAtVslot5C(YtlReferenceSlotRuntime& consumed, const RECT* clip) = 0;
};

struct SelectWindowDialogDependencies final
{
    // 固定子对象入口：保留旧六参数花括号语法；子对象须长于所有使用它的Dialog。
    SelectWindowDialogDependencies(const ScrVmMenuState& menu,
        ICgOperationSurfaceFactory& surfaces, IGraphicsSurfaceCompositionApi& graphics,
        IGraphicsSurfacePixelApi& pixels, Platform::MouseView mouse,
        Platform::KeyboardView keyboard) noexcept;
    // 4F1B89..4F1BA7仅安装查询对象；不要求4507E0已经填充稳定aggregate。
    // aggregate必须长于Dialog；构造和依赖复制均不解析其Mouse/Keyboard。
    SelectWindowDialogDependencies(const ScrVmMenuState& menu,
        ICgOperationSurfaceFactory& surfaces, IGraphicsSurfaceCompositionApi& graphics,
        IGraphicsSurfacePixelApi& pixels, const Platform::IInputRuntime& input) noexcept;

    const ScrVmMenuState& menu;
    ICgOperationSurfaceFactory& surfaces;
    IGraphicsSurfaceCompositionApi& graphics;
    IGraphicsSurfacePixelApi& pixels;

private:
    friend class SelectWindowDialogRuntime;
    // 两种来源只能由构造重载选择，不能由调用者拼出混合或空来源。
    const Platform::IInputRuntime* input_ = nullptr;
    Platform::MouseView mouse_ = nullptr;
    Platform::KeyboardView keyboard_ = nullptr;
};

// 4F17E0 / 5BEAB4：父 Dialog 为空，五个 Button 与事件由本对象强拥有。
// menu/input/API 均借用真实根对象，必须长于本对象及被外部保活的 child。
class SelectWindowDialogRuntime final : public ISelectWindowDialog,
    private ICgOperationButtonInputServices
{
public:
    explicit SelectWindowDialogRuntime(SelectWindowDialogDependencies dependencies);
    ~SelectWindowDialogRuntime() override;
    SelectWindowDialogRuntime(const SelectWindowDialogRuntime&) = delete;
    SelectWindowDialogRuntime& operator=(const SelectWindowDialogRuntime&) = delete;

    void ResetAt50F000() override;
    [[nodiscard]] std::int32_t ProcessAt530270() override;
    [[nodiscard]] std::int32_t ResultAt50ABD0() const override;
    [[nodiscard]] RECT VisibleBoundsAt50AD40(std::int32_t ordinal) const override;
    [[nodiscard]] ICgDisplayButtonGroupDirtyRects& OwnDirtyRectsAtVslot00() override;
    void DrawAtVslot60(const RECT& dirty) override;
    void CompositeAtVslot5C(YtlReferenceSlotRuntime& consumed, const RECT* clip) override;

    void UpdateAtVslot54();
    void ResetInputAt510820();
    [[nodiscard]] std::int32_t ProcessConfirmAt511420();
    [[nodiscard]] std::int32_t ProcessNavigationAt51E740();
    void ResetChildrenAtVslot68();
    void SubmitActiveAt532170(ICgOperationChildControl* control);
    void SubmitRectAtVslot64(const RECT& rect);
    void WriteModeAtVslot1C(std::int32_t value) noexcept;
    void WriteEnabledAtVslot24(std::int32_t value) noexcept;
    [[nodiscard]] std::int32_t CloseAtVslot4C() noexcept;
    [[nodiscard]] std::int32_t ModeAt34() const noexcept;
    [[nodiscard]] std::int32_t EnabledAt38() const noexcept;
    [[nodiscard]] std::shared_ptr<CgOperationChildControlRuntime>
        FindControlAt509D20(std::int32_t id) const;
    [[nodiscard]] std::span<const CgOperationChildControlRef> Children() const noexcept;
    [[nodiscard]] std::span<const CgOperationClickedEventRef> EventsAt7C() const noexcept;
    [[nodiscard]] std::span<const GuiDialogNavigationGroupRef> NavigationAt24() const noexcept;
    [[nodiscard]] const ICgOperationChildControl* ActiveAt4C() const noexcept;
    [[nodiscard]] const RECT& BoundsAt6C() const noexcept;
    [[nodiscard]] const std::optional<std::int32_t>& ResultIdAt60() const noexcept;
    [[nodiscard]] const std::optional<std::int32_t>& PhaseAt64() const noexcept;
    [[nodiscard]] const std::optional<std::int32_t>& ConfirmLatchAt68() const noexcept;
    [[nodiscard]] std::int32_t NavigationLatchAt30() const noexcept;

private:
    void ReleaseOwnedAt4FE1E0() noexcept;
    void AppendNavigation(std::size_t count);
    void SetActiveControlAtVslot30(ICgOperationDialogEventTarget& control) override;
    bool ReadInputAtVslot6C(std::int32_t action) override;
    bool ReadInputAtVslot70(std::int32_t action) override;
    POINT ReadPointerAtVslot74() override;
    bool ReadPressAtVslot80() override;
    bool ReadReleaseAtVslot84() override;
    std::uint32_t ReadSurfacePixelAtGetSurfacePixel(GraphicsSurfaceHandle handle,
        std::int32_t x, std::int32_t y) override;

    SelectWindowDialogDependencies dependencies_;
    CgOperationDialogRenderHostRuntime renderHost_;
    std::vector<CgOperationChildControlRef> children_;
    std::unique_ptr<IGuiInputStateRuntime> inputAt18_;
    std::vector<GuiDialogNavigationGroupRef> navigationAt24_;
    std::int32_t navigationLatchAt30_ = 0;
    ICgOperationChildControl* activeAt4C_ = nullptr;
    std::unique_ptr<ICgDisplayButtonGroupDirtyRects> dirtyAt50_;
    std::optional<std::int32_t> resultIdAt60_;
    std::optional<std::int32_t> phaseAt64_;
    std::optional<std::int32_t> confirmLatchAt68_;
    RECT boundsAt6C_{};
    std::vector<CgOperationClickedEventRef> eventsAt7C_;
};
}
