#pragma once

#include <Runtime/CgDisplaySceneRuntime.h>
#include <Runtime/LayerRuntime.h>
#include <Runtime/SpriteRuntime.h>

#include <Windows.h>

#include <cstdint>
#include <vector>

namespace KaGuYa::Start::Runtime
{
enum class CgDisplayOpaqueAxisSource : std::uint8_t
{
    At418210 = 0,
    At4AD740 = 1,
};

// CCGDispWin::4BD050 读取的最小外部边界。尚未由消费者定名的两个轴源
// 保留其取得函数地址，避免把键盘、鼠标或手柄方向擅自混为同一概念。
class ICgDisplayWindowServices
{
public:
    virtual ~ICgDisplayWindowServices() = default;

    virtual void ConfigurePointerAtInputVslot04(
        std::int32_t resourceId,
        std::int32_t first,
        std::int32_t second) = 0;
    virtual void BeginPointerAtInputVslot08() = 0;
    [[nodiscard]] virtual bool PrimaryPressedAt10() const = 0;
    [[nodiscard]] virtual bool PrimaryPreviousPressedAt30() const = 0;
    [[nodiscard]] virtual std::uint32_t PrimarySuppressionAt28() const = 0;
    [[nodiscard]] virtual POINT PointerPositionAt18() const = 0;
    [[nodiscard]] virtual bool PointerIsInsideClient() const = 0;

    [[nodiscard]] virtual std::uint32_t ReadRegistrySlot2DwordAt04() const = 0;
    virtual void WriteRegistrySlot2DwordAt04(std::uint32_t value) = 0;
    [[nodiscard]] virtual bool ReadInputEdgeAt47F4E0(
        std::int32_t action) const = 0;
    [[nodiscard]] virtual bool ReadInputHeldAt4198D0(
        std::int32_t action) const = 0;
    [[nodiscard]] virtual std::int32_t ReadOpaqueAxisStateAt44(
        CgDisplayOpaqueAxisSource source) const = 0;
    [[nodiscard]] virtual std::int32_t ReadOpaqueAxisValueAt52(
        CgDisplayOpaqueAxisSource source) const = 0;
    virtual void SubmitDirtyRect(const RECT& rect) = 0;
};

struct CgDisplayWindowPanState final
{
    std::uint32_t flagsAt30 = 0;
    std::int32_t stepAt34 = 0;
    std::int32_t maximumXAt38 = 0;
    std::int32_t maximumYAt3C = 0;
    std::int32_t minimumXAt40 = 0;
    std::int32_t minimumYAt44 = 0;
};

// CGDispWin::CCGDispWin 的窗口、Sprite 所有权、输入与动画 dirty-rect 子图。
// 0x499A20 先注册 ILayerItem，再直接构造 count 个 0xA0 字节 CSprite；
// 0x4A6030 先释放该数组，最后由 ILayerItem 析构从 LayerList 注销。
class CgDisplayWindowRuntime final : public LayerItem, public ICgDisplayWindow
{
public:
    CgDisplayWindowRuntime(
        std::size_t spriteSlotCount,
        IGraphicsSurfaceApi& surfaceApi,
        LayerList* layerList,
        ICgDisplayWindowServices& services,
        IGraphicsSurfaceCompositionApi& graphics,
        std::int32_t screenWidth,
        std::int32_t screenHeight);
    ~CgDisplayWindowRuntime() override = default;

    void Draw(const RECT& dirtyRect) override;
    void CompositeToSurface(
        GraphicsSurfaceDestination destination, const RECT* destinationClip) override;

    // 4B5300 / 4B5040：临时发布 Sprite 坐标，正常完成才恢复；映射每轮重读。
    [[nodiscard]] std::size_t DrawAtVslot00(
        std::int32_t argument) override;
    [[nodiscard]] std::size_t BlitAtVslot04(
        GraphicsSurfaceDestination destination,
        std::int32_t argument) override;
    [[nodiscard]] RECT* CopyClipRectAtVslot0C(RECT* output) const override;
    [[nodiscard]] bool QueryAtVslot10(
        std::uint32_t first,
        std::uint32_t second) const noexcept override;
    void DeactivateAtVslot18() noexcept override;
    void WritePositionAt04And08(
        std::int32_t x,
        std::int32_t y) override;
    [[nodiscard]] std::uint8_t ActiveByteAt54() const override;
    void BeginAt4AA860() override;
    void UpdateAt4BD050() override;
    void InvokeAt4C17F0(std::int32_t value) override;
    void InvokeAt4C13A0(std::int32_t value) override;
    void UpdateAnimationAndDirtyRectsAtVslot14() override;

    [[nodiscard]] std::size_t SpriteSlotCount() const noexcept;
    [[nodiscard]] SpriteRuntime& SpriteAt(std::size_t slotIndex);
    [[nodiscard]] const SpriteRuntime& SpriteAt(std::size_t slotIndex) const;
    void SetWindowActiveAt0C(bool active) noexcept;
    [[nodiscard]] bool WindowActiveAt0C() const noexcept;

    void ConfigurePanStateAt30(const CgDisplayWindowPanState& state) noexcept;
    [[nodiscard]] const CgDisplayWindowPanState& PanStateAt30() const noexcept;
    [[nodiscard]] std::int32_t XAt04() const noexcept;
    [[nodiscard]] std::int32_t YAt08() const noexcept;
    [[nodiscard]] std::int32_t RepeatDelayAt48() const noexcept;
    [[nodiscard]] std::int32_t RepeatCounterAt4C() const noexcept;
    [[nodiscard]] std::uint32_t SavedRegistryValueAt50() const noexcept;
    [[nodiscard]] POINT PreviousPointerAt58() const noexcept;

private:
    [[nodiscard]] std::int32_t ClampX(std::int32_t value) const noexcept;
    [[nodiscard]] std::int32_t ClampY(std::int32_t value) const noexcept;
    [[nodiscard]] std::size_t ComposeSprites(
        GraphicsSurfaceDestination destination,
        std::int32_t argument,
        bool drawToPrimary);
    void SubmitClippedSpriteRect(const SpriteRuntime& sprite);

    std::vector<SpriteRuntime> sprites_;
    ICgDisplayWindowServices* services_ = nullptr;
    IGraphicsSurfaceCompositionApi* graphics_ = nullptr;
    std::int32_t xAt04_ = 0;
    std::int32_t yAt08_ = 0;
    RECT clipRectAt14_{};
    CgDisplayWindowPanState panState_{};
    std::int32_t repeatDelayAt48_ = 0;
    std::int32_t repeatCounterAt4C_ = 0;
    std::uint32_t savedRegistryValueAt50_ = 0;
    std::uint8_t activeByteAt54_ = 0;
    POINT previousPointerAt58_{};
};
}
