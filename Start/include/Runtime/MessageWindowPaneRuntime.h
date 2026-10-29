#pragma once

#include <Runtime/ColorTransWindowRuntime.h>
#include <Runtime/MessageWindowConfigRuntime.h>
#include <Runtime/MessageWindowLayerRuntime.h>

#include <array>
#include <memory>
#include <vector>

namespace KaGuYa::Start::Runtime
{
// 45F2E0 的三个 DWORD 描述符直接采用 ColorTransWindowDescriptor。
class MessageWindowNormalPaneRuntime final : public IMessageWindowPane
{
public:
    MessageWindowNormalPaneRuntime(IGraphicsSurfaceApi& surfaces,
        IGraphicsWindowSurfaceApi& graphics, IColorTransWindowResourceLoader& resources,
        const ColorTransWindowDescriptor& descriptor,
        std::shared_ptr<const IMessageWindowColorConfig> config);
    ~MessageWindowNormalPaneRuntime() override;

    [[nodiscard]] std::int32_t TypeAtVslot00() const override;
    [[nodiscard]] RECT BoundsAtVslot04() const override;
    [[nodiscard]] SIZE BaseTileSizeAt47DFE0() const override;
    void DrawAtVslot0C(const RECT& dirty) override;
    void CompositeAtVslot10(GraphicsSurfaceDestination destination, const RECT* clip) override;

    [[nodiscard]] bool RefreshColorsAt4874C0();
    void RefreshColorsAtVslot08() override { (void)RefreshColorsAt4874C0(); }
    [[nodiscard]] RuntimeGraphicsSurface* Processed() noexcept;
    [[nodiscard]] const RuntimeGraphicsSurface* Processed() const noexcept;

private:
    IGraphicsWindowSurfaceApi& graphics_;
    RECT bounds_{};
    // 465570：config 引用先于 ColorTrans 释放，构造失败也保持该顺序。
    std::unique_ptr<ColorTransWindowRuntime> window_;
    std::shared_ptr<const IMessageWindowColorConfig> config_;
};

// 45F720：中心描述符、最大列/行数、四个可空边缘描述符；只在构造期借用。
struct MessageWindowVariablePaneDescriptor final
{
    const ColorTransWindowDescriptor* center = nullptr;
    std::int32_t maximumColumns = 0;
    std::int32_t maximumRows = 0;
    std::array<const ColorTransWindowDescriptor*, 4> edges{};
};

class MessageWindowVariablePaneRuntime final : public IMessageWindowPane
{
public:
    MessageWindowVariablePaneRuntime(IGraphicsSurfaceApi& surfaces,
        IGraphicsWindowSurfaceApi& graphics, IColorTransWindowResourceLoader& resources,
        const MessageWindowVariablePaneDescriptor& descriptor,
        std::shared_ptr<const IMessageWindowColorConfig> config);
    ~MessageWindowVariablePaneRuntime() override;

    [[nodiscard]] std::int32_t TypeAtVslot00() const override;
    [[nodiscard]] RECT BoundsAtVslot04() const override;
    [[nodiscard]] SIZE BaseTileSizeAt47DFE0() const override;
    void DrawAtVslot0C(const RECT& dirty) override;
    void CompositeAtVslot10(GraphicsSurfaceDestination destination, const RECT* clip) override;

    // 487540 丢弃子项布尔结果，但不吞异常或合并 config Read。
    void RefreshColorsAt487540();
    void RefreshColorsAtVslot08() override { RefreshColorsAt487540(); }
    [[nodiscard]] const MessageWindowNormalPaneRuntime& Center() const noexcept;
    [[nodiscard]] std::size_t EdgeCount() const noexcept;

private:
    struct Edge final
    {
        std::uint32_t index = 0;
        std::unique_ptr<MessageWindowNormalPaneRuntime> pane;
    };

    void Render(GraphicsSurfaceDestination destination, const RECT* clip, bool composite);
    void Release() noexcept;

    IGraphicsWindowSurfaceApi& graphics_;
    RECT bounds_{};
    std::vector<Edge> edges_;
    std::unique_ptr<MessageWindowNormalPaneRuntime> center_;
};
}
