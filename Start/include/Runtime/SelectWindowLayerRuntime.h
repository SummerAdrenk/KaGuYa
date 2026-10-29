#pragma once

#include <Runtime/DrawManagerLayerRuntime.h>
#include <Runtime/GraphicsTextRuntime.h>
#include <Runtime/MessageFormatSelectionRuntime.h>
#include <Runtime/MessageTextSelectionRuntime.h>

#include <array>
#include <optional>

namespace KaGuYa::Start::Runtime
{
class ISelectWindowDialog;
class ISelectWindowConstructionServices
{
public:
    virtual ~ISelectWindowConstructionServices() = default;
    virtual std::shared_ptr<const std::uint32_t> ReadColorOverride(std::uint32_t index) = 0;
    virtual std::shared_ptr<ISelectWindowDialog> CreateDialogAt4F17E0() = 0;
    virtual std::unique_ptr<RuntimeGraphicsSurface> AllocateSurface() = 0;
    virtual std::int32_t ReadProgramHeightAt1C() const = 0;
    virtual std::int32_t ReadProgramWidthAt18() const = 0;
};

class ISelectWindowStateServices
{
public:
    virtual ~ISelectWindowStateServices() = default;
    virtual float ReadMessageVersion() const = 0;
    virtual std::size_t MenuCountAt64() const = 0;
    virtual std::int32_t MenuChoiceAt64(std::size_t ordinal) const = 0;
    virtual std::int32_t ReadChoiceAt418700(std::int32_t id, MessageTextSelection& output) = 0;
    virtual std::int32_t ReadLegacyAt417C40(std::int32_t id, MessageFormatSelectionTarget output) = 0;
    virtual bool HasProgramChoiceFlagsAt28() const = 0;
    virtual bool ReadProgramChoiceFlag(std::int32_t ordinal) const = 0;
    virtual bool HighlightChosenAt27() const = 0;
    virtual std::string ReadFontFaceAt619824() const = 0;
    virtual std::uint32_t ReadSaveColorAt64() const = 0;
    virtual std::uint8_t ReadSaveByteAt60() const = 0;
    virtual std::uint8_t ReadSaveByteAt61() const = 0;
    virtual std::uint32_t MouseSecondaryCountdownAt2C() const = 0;
    virtual std::int32_t MouseWheelAt34() const = 0;
    virtual void SetUnSaveFlagAt1C() = 0;
    virtual void MarkChoiceAt4C29D0(std::int32_t id) = 0;
    virtual void RefreshEffectsAt433F60() = 0;
};

// 45CC30 / 5B2D0C；仅表达确认字段，不给 +24/+28 或 +46/+47 臆造用途。
class SelectWindowLayerRuntime final : public FrameLayerItem, public ILayerPrepareTarget, public ILayerResetTarget
{
public:
    SelectWindowLayerRuntime(LayerList* parent, ISelectWindowConstructionServices& construction,
        IGraphicsWindowSurfaceApi& graphics);
    ~SelectWindowLayerRuntime() override;
    std::uint8_t SetRequestedEnabledAtVslot10(std::uint8_t value, std::int32_t unused) override;
    void ResetAt47EE70() noexcept;
    void ResetOwnedLayerAtVslot18(IGraphicsWindowSurfaceApi&) override { ResetAt47EE70(); }
    void Draw(const RECT& dirty) override;
    void CompositeToSurface(GraphicsSurfaceDestination target, const RECT* clip) override;
    void AdvanceOwnedFrame() override;
    void RebuildAt4369E0(ISelectWindowStateServices& services,
        IGraphicsTextApi& text, IGraphicsTextGlyphApi& glyphs);
    [[nodiscard]] std::int32_t TickAt4BBFD0(std::uint8_t inputBlocked,
        ISelectWindowStateServices& services);
    [[nodiscard]] RECT BoundsAt479DB0() const noexcept { return bounds_; }
    [[nodiscard]] const RuntimeGraphicsSurface* CanvasAt2C() const noexcept { return canvas_.get(); }
    [[nodiscard]] const std::shared_ptr<ISelectWindowDialog>& DialogAt38() const noexcept { return dialog_; }
    [[nodiscard]] const std::uint32_t* ColorOverride(std::size_t index) const { return colors_.at(index).get(); }
    [[nodiscard]] std::optional<std::uint8_t> SavedByteAt44() const noexcept { return savedByteAt44_; }
    [[nodiscard]] std::optional<std::uint8_t> SavedByteAt45() const noexcept { return savedByteAt45_; }

private:
    void ReleaseOwnedResources() noexcept;
    RuntimeGraphicsSurface& RequireCanvas() const;
    ISelectWindowDialog& RequireDialog() const;
    LayerList& RequireParent() const;
    std::uint32_t NormalColor(ISelectWindowStateServices& services) const;
    RECT bounds_{};
    std::unique_ptr<RuntimeGraphicsSurface> canvas_;
    std::shared_ptr<ISelectWindowDialog> dialog_;
    std::array<std::shared_ptr<const std::uint32_t>, 3> colors_;
    std::optional<std::uint8_t> savedByteAt44_, savedByteAt45_;
    IGraphicsWindowSurfaceApi& graphics_;
};
}
