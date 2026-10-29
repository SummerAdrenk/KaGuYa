#pragma once

#include <Runtime/GraphicsSurfaceRuntime.h>

#include <Windows.h>

#include <array>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace KaGuYa::Start::Runtime
{
struct CgUiButtonSurfaceView final
{
    GraphicsSurfaceHandle handle = 0;
    std::int32_t originX = 0;
    std::int32_t originY = 0;
    bool hasAlpha = false;
};

class ICgUiButtonRuntimeServices
{
public:
    virtual ~ICgUiButtonRuntimeServices() = default;

    [[nodiscard]] virtual bool HasTimelineActivityAt47F2A0(
        std::int32_t timelineIndexAt68) = 0;
    [[nodiscard]] virtual POINT ReadPointerAt47B850() = 0;
    [[nodiscard]] virtual bool CompletePressAt47F310() = 0;
    [[nodiscard]] virtual bool CompleteReleaseAt47F350() = 0;
    [[nodiscard]] virtual std::uint32_t ReadSurfacePixelAtGetSurfacePixel(
        GraphicsSurfaceHandle handle,
        std::int32_t x,
        std::int32_t y) = 0;
};

struct CgUiButtonConfiguration final
{
    bool pixelHitTestAt08 = true;
    RECT explicitHitRectAt0C{};
    std::int32_t inputModeAt1C = 0;
    POINT positionAt24{};
    bool usePositionAt30At2C = true;
    POINT positionAt30{};
    SIZE extentAt38{};
    std::array<std::int32_t, 7> stateSurfaceSlotIndicesAt40{
        -1, -1, -1, -1, -1, -1, -1};
    std::int32_t timelineIndexAt68 = -1;
};

// CGUIButton（0x459530，原对象大小 0x6C）的行为级内部重写。
// Start 内部对象不跨 DLL ABI，因此这里保留字段语义与调用顺序，不伪造原字节布局。
class CgUiButtonRuntime final
{
public:
    CgUiButtonRuntime(
        ICgUiButtonRuntimeServices& services,
        IGraphicsSurfaceCompositionApi& graphics) noexcept;

    // IGUIButton 公共状态机 0x4A98E0。
    [[nodiscard]] std::int32_t ProcessAt4A98E0(bool forced);
    // CGUIButton 绘制包装 0x4B5290。
    [[nodiscard]] std::int8_t DrawIntoAt4B5290(
        GraphicsSurfaceDestination destination,
        const RECT* destinationClip);
    [[nodiscard]] bool DrawToScreenAt441980(const RECT* destinationClip);

    void ResetInteractionState() noexcept;
    void SetStateAt04(std::int32_t state);
    void SetInputLatchAt20(bool value) noexcept;
    void SetPixelHitTestAt08(bool value) noexcept;
    void SetExplicitHitRectAt0C(const RECT& rect) noexcept;
    void SetInputModeAt1C(std::int32_t value) noexcept;
    void SetPositionAt24(const POINT& value) noexcept;
    void SetUsePositionAt30At2C(bool value) noexcept;
    void SetPositionAt30(const POINT& value) noexcept;
    void SetExtentAt38(const SIZE& value) noexcept;
    void SetStateSurfaceSlotAt40(
        std::size_t stateIndex,
        std::int32_t slotIndex);
    void SetTimelineIndexAt68(std::int32_t value) noexcept;
    [[nodiscard]] std::int32_t AppendOwnedSurfaceAt5C(
        std::shared_ptr<RuntimeGraphicsSurface> surface);
    [[nodiscard]] bool AttachNamedSurfaceAt4C1F20(
        std::size_t stateSurfaceSlot,
        std::string identifierCp932,
        std::shared_ptr<RuntimeGraphicsSurface> surface);

    [[nodiscard]] std::int32_t StateAt04() const noexcept;
    [[nodiscard]] bool InputLatchAt20() const noexcept;
    [[nodiscard]] RECT VisibleRect() const noexcept;
    [[nodiscard]] const CgUiButtonConfiguration& Configuration() const noexcept;
    [[nodiscard]] std::size_t OwnedSurfaceCountAt5C() const noexcept;

private:
    struct OwnedSurfaceRecord final
    {
        std::string identifierCp932;
        std::shared_ptr<RuntimeGraphicsSurface> surface;
    };

    [[nodiscard]] std::optional<CgUiButtonSurfaceView>
        ResolveSurfaceSlotAt4ADED0(std::int32_t slotIndex) const noexcept;
    [[nodiscard]] RECT NaturalHitRect(
        const CgUiButtonSurfaceView& surface) const noexcept;
    [[nodiscard]] POINT DrawPosition(
        const CgUiButtonSurfaceView& surface) const noexcept;

    ICgUiButtonRuntimeServices* services_ = nullptr;
    IGraphicsSurfaceCompositionApi* graphics_ = nullptr;
    CgUiButtonConfiguration configuration_{};
    std::vector<OwnedSurfaceRecord> ownedSurfaces_;
    std::int32_t stateAt04_ = -1;
    bool inputLatchAt20_ = false;
};
}
