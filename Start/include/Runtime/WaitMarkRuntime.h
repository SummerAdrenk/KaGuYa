#pragma once

#include <Runtime/DrawManagerLayerRuntime.h>
#include <Runtime/SpriteArchiveResourceLoader.h>
#include <Runtime/SpriteRuntime.h>

#include <Windows.h>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace KaGuYa::Start::Runtime
{
class IWaitMarkResourceLoader
{
public:
    virtual ~IWaitMarkResourceLoader() = default;

    [[nodiscard]] virtual bool Load(
        SpriteRuntime& sprite,
        const std::string& resourceNameCp932,
        bool playAnimationFromStart) = 0;
};

class WaitMarkArchiveResourceLoader final : public IWaitMarkResourceLoader
{
public:
    explicit WaitMarkArchiveResourceLoader(
        SpriteArchiveResourceLoader& loader) noexcept;

    [[nodiscard]] bool Load(
        SpriteRuntime& sprite,
        const std::string& resourceNameCp932,
        bool playAnimationFromStart) override;

private:
    SpriteArchiveResourceLoader* loader_ = nullptr;
};

// 4873C0 与 47B030 读取的外部状态。字段名保留原对象布局含义，
// 在所属对象尚未还原前不提前赋予未经证实的业务名称。
class IWaitMarkStateSource
{
public:
    virtual ~IWaitMarkStateSource() = default;

    [[nodiscard]] virtual bool AutomaticMarkSelected() const = 0;
    [[nodiscard]] virtual bool AnyRuntimeTripletSecondByteSet() const = 0;
    [[nodiscard]] virtual bool SlotTwoField20BitOneSet() const = 0;
};

class IWaitMarkRenderer
{
public:
    virtual ~IWaitMarkRenderer() = default;

    [[nodiscard]] virtual std::int32_t DrawToScreen(
        const SpriteRuntime& sprite,
        const RECT& dirtyRect) = 0;
    [[nodiscard]] virtual std::int32_t DrawToSurface(
        const SpriteRuntime& sprite,
        GraphicsSurfaceDestination destination,
        const RECT* destinationClip) = 0;
};

// WaitMark::CWaitMark（45E450）的生产状态对象。
class WaitMarkRuntime final : public FrameLayerItem, public ILayerPrepareTarget, public ILayerResetTarget
{
public:
    [[nodiscard]] std::uint8_t SetRequestedEnabledAtVslot10(
        std::uint8_t requested, std::int32_t secondArgument) override
        { return SetRequestedEnabled(requested != 0, secondArgument); }
    WaitMarkRuntime(
        LayerList* layerList,
        IGraphicsSurfaceApi& graphicsApi,
        IWaitMarkResourceLoader& resourceLoader,
        IWaitMarkStateSource& stateSource,
        IWaitMarkRenderer& renderer);
    ~WaitMarkRuntime() override;

    WaitMarkRuntime(const WaitMarkRuntime&) = delete;
    WaitMarkRuntime& operator=(const WaitMarkRuntime&) = delete;

    [[nodiscard]] const RECT& Bounds() const noexcept;
    [[nodiscard]] std::size_t SpriteCount() const noexcept;
    [[nodiscard]] const std::optional<std::uint32_t>& CompletionFieldAt30() const noexcept;
    [[nodiscard]] const SpriteRuntime& NormalSprite() const;
    [[nodiscard]] const SpriteRuntime& AutomaticSprite() const;
    [[nodiscard]] const SpriteRuntime& SelectedSprite() const;

    // 4873C0：第二个整数参数在样本 18 中未被读取，仍保留 ABI 边界。
    [[nodiscard]] std::uint8_t SetRequestedEnabled(
        bool requestedEnabled,
        std::int32_t unusedSecondArgument);
    [[nodiscard]] std::int32_t DrawSelectedToSurface(
        GraphicsSurfaceDestination destination,
        const RECT* destinationClip);
    void Reset() noexcept;
    void ResetOwnedLayerAtVslot18(IGraphicsWindowSurfaceApi&) override { Reset(); }

    void Draw(const RECT& dirtyRect) override;
    void CompositeToSurface(
        GraphicsSurfaceDestination destination, const RECT* destinationClip) override;
    // 原 CWaitMark +14：独立帧更新，不执行 +04 合成。
    void AdvanceFrameAt46B1F0();
    void AdvanceOwnedFrame() override;

private:
    void InvalidateBounds();

    std::vector<std::unique_ptr<SpriteRuntime>> sprites_;
    RECT bounds_{};
    // 45E72A 仅正常路径写零；异常路径未初始化，不伪造默认业务状态。
    std::optional<std::uint32_t> completionFieldAt30_;
    IWaitMarkStateSource* stateSource_ = nullptr;
    IWaitMarkRenderer* renderer_ = nullptr;
};
}
