#pragma once

#include <Runtime/CountDownWindowResources.h>
#include <Runtime/CountDownWindowFiveFrameProfile.h>
#include <Runtime/DrawManagerLayerRuntime.h>
#include <Runtime/ScrVmExecutableProfile.h>
#include <Runtime/ScrVmValueTable.h>

#include <memory>
#include <optional>
#include <vector>

namespace KaGuYa::Start::Runtime
{
struct CountDownWindowDependencies final
{
    const GlobalParamsArchive& globalParams;
    const ScrVmValueTable& values;
    const ScrVmExecutableProfile& executableProfile;
    IGraphicsSurfaceApi& surfaces;
    IGraphicsWindowSurfaceApi& drawing;
    CountDownWindowResources& resources;
};

// 458210 / 5B2D7C默认60个数字和12个后缀；显式五帧重载仅拥有5个数字Sprite。
// 名称中的 CountDown 不代表 owner 内含秒表；+3C 由显示请求读取 VM。
class CountDownWindowRuntime final : public FrameLayerItem, public ILayerPrepareTarget, public ILayerResetTarget
{
public:
    CountDownWindowRuntime(LayerList* parent, CountDownWindowDependencies dependencies,
        CountDownWindowResourceProfile resourceProfile);
    CountDownWindowRuntime(LayerList* parent, CountDownWindowDependencies dependencies,
        const CountDownWindowFiveFrameResourceProfile& resourceProfile);
    ~CountDownWindowRuntime() override;
    CountDownWindowRuntime(const CountDownWindowRuntime&) = delete;
    CountDownWindowRuntime& operator=(const CountDownWindowRuntime&) = delete;

    void Draw(const RECT& dirtyRect) override;
    void CompositeToSurface(GraphicsSurfaceDestination destination, const RECT* clip) override;
    void AdvanceOwnedFrame() override;
    [[nodiscard]] std::uint8_t SetRequestedEnabledAtVslot10(
        std::uint8_t requested, std::int32_t secondArgument) override;
    [[nodiscard]] RECT BoundsAt479CE0() const;
    void ResetAt47EC20() noexcept;
    void ResetOwnedLayerAtVslot18(IGraphicsWindowSurfaceApi&) override { ResetAt47EC20(); }
    [[nodiscard]] std::int32_t SuffixVisibleAt47C7A0() const;
    void ClearNumberAt46DC00() noexcept;

    [[nodiscard]] std::shared_ptr<SpriteRuntime> NumberAt47BE60(std::int32_t mode);
    [[nodiscard]] std::shared_ptr<SpriteRuntime> SuffixAt47C7B0(std::int32_t mode);
    [[nodiscard]] std::int32_t NumberIndexAt3C() const;
    [[nodiscard]] const RECT& LocalBoundsAt14() const noexcept { return bounds_; }
    [[nodiscard]] std::size_t NumberCount() const noexcept { return numbers_.size(); }
    [[nodiscard]] std::size_t SuffixCount() const noexcept { return suffixes_.size(); }
    [[nodiscard]] const SpriteRuntime& NumberSpriteAt(std::size_t index) const;
    [[nodiscard]] const SpriteRuntime& SuffixSpriteAt(std::size_t index) const;

private:
    void Initialize(const CountDownWindowFiveFrameResourceProfile* fiveFrames);
    [[nodiscard]] std::int32_t DisplayMode() const noexcept;
    void InvalidateBounds();
    void DrawSprite(const SpriteRuntime& sprite, const RECT* clip,
        const std::optional<GraphicsSurfaceDestination>& destination);
    static void ReleaseForward(std::vector<std::shared_ptr<SpriteRuntime>>& sprites) noexcept;

    CountDownWindowDependencies dependencies_;
    CountDownWindowResourceProfile profile_;
    bool fiveFrames_ = false;
    RECT bounds_{};
    std::vector<std::shared_ptr<SpriteRuntime>> suffixes_;
    std::vector<std::shared_ptr<SpriteRuntime>> numbers_;
    // 原构造在正常/异常收束点才初始化；不为重入观察伪造早期值。
    std::optional<std::int32_t> numberIndex_;
    std::optional<std::int32_t> suffixVisible_;
};
}
