#pragma once

#include <Runtime/MessageWindowPaneRuntime.h>

#include <span>

namespace KaGuYa::Start::Runtime
{
struct MessageWindowStyleDescriptor final
{
    const ColorTransWindowDescriptor* normalBody = nullptr;
    const MessageWindowVariablePaneDescriptor* variableBody = nullptr;
    const ColorTransWindowDescriptor* normalName = nullptr;
    const MessageWindowVariablePaneDescriptor* variableName = nullptr;
    RECT nameBaseline{};
    std::int32_t configDiscriminator = 0;
};

[[nodiscard]] std::span<const MessageWindowStyleDescriptor> MessageWindowStylesSample18At601408() noexcept;

// 45F040 的真实 pane 工厂。借用描述符；config 引用由 style 和具体 pane 各自保留。
// CMesWin 前半段的 config 分配/四段 catch 与根 owner 仍由其构造器负责。
class MessageWindowStyleFactoryRuntime final : public IMessageWindowStyleFactory
{
public:
    MessageWindowStyleFactoryRuntime(IGraphicsSurfaceApi& surfaces, IGraphicsWindowSurfaceApi& graphics,
        IColorTransWindowResourceLoader& resources, std::span<const MessageWindowStyleDescriptor> descriptors,
        std::shared_ptr<const IMessageWindowColorConfig> type1,
        std::shared_ptr<const IMessageWindowColorConfig> type2);
    [[nodiscard]] std::unique_ptr<MessageWindowStyleRuntime> CreateStyle(std::uint32_t index) override;

private:
    IGraphicsSurfaceApi& surfaces_;
    IGraphicsWindowSurfaceApi& graphics_;
    IColorTransWindowResourceLoader& resources_;
    std::span<const MessageWindowStyleDescriptor> descriptors_;
    std::shared_ptr<const IMessageWindowColorConfig> type1_, type2_;
};
}
