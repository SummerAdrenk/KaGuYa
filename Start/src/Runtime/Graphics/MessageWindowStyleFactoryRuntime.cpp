#include <Runtime/MessageWindowStyleFactoryRuntime.h>

#include <array>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
// 6013C0/6013F0；不要把相邻但未被两项表引用的资源当成第三套样式。
constexpr ColorTransWindowDescriptor Body1{
    nullptr, "\x83\x81\x83\x62\x83\x5A\x98\x67.vrm", 0};
constexpr ColorTransWindowDescriptor Body2{
    nullptr, "\x83\x81\x83\x62\x83\x5A\x98\x67" "2.vrm", 0};
constexpr std::array<MessageWindowStyleDescriptor, 2> Sample18Styles{{
    {&Body1, nullptr, nullptr, nullptr, {}, 0},
    {&Body2, nullptr, nullptr, nullptr, {}, 1},
}};
}

std::span<const MessageWindowStyleDescriptor> MessageWindowStylesSample18At601408() noexcept
    { return Sample18Styles; }

MessageWindowStyleFactoryRuntime::MessageWindowStyleFactoryRuntime(IGraphicsSurfaceApi& surfaces,
    IGraphicsWindowSurfaceApi& graphics, IColorTransWindowResourceLoader& resources,
    std::span<const MessageWindowStyleDescriptor> descriptors,
    std::shared_ptr<const IMessageWindowColorConfig> type1,
    std::shared_ptr<const IMessageWindowColorConfig> type2)
    : surfaces_(surfaces), graphics_(graphics), resources_(resources), descriptors_(descriptors),
      type1_(std::move(type1)), type2_(std::move(type2)) {}

std::unique_ptr<MessageWindowStyleRuntime> MessageWindowStyleFactoryRuntime::CreateStyle(std::uint32_t index)
{
    if (index >= descriptors_.size())
        throw std::out_of_range("CMesWin descriptor index exceeds the supplied PE table");
    const auto& descriptor = descriptors_[index];
    // 45A274..45A280：仅零/非零选择，不限制非零值为 1。
    const auto& config = descriptor.configDiscriminator == 0 ? type1_ : type2_;
    auto style = std::make_unique<MessageWindowStyleRuntime>();
    if (descriptor.normalBody)
        style->body = std::make_unique<MessageWindowNormalPaneRuntime>(
            surfaces_, graphics_, resources_, *descriptor.normalBody, config);
    else if (descriptor.variableBody)
        style->body = std::make_unique<MessageWindowVariablePaneRuntime>(
            surfaces_, graphics_, resources_, *descriptor.variableBody, config);
    if (descriptor.normalName)
        style->name = std::make_unique<MessageWindowNormalPaneRuntime>(
            surfaces_, graphics_, resources_, *descriptor.normalName, config);
    else if (descriptor.variableName)
    {
        style->name = std::make_unique<MessageWindowVariablePaneRuntime>(
            surfaces_, graphics_, resources_, *descriptor.variableName, config);
        style->nameBaseline = descriptor.nameBaseline;
    }
    // 45F236 之后才持有样式自身的 config；此前失败依次释放 name/body。
    style->config = config;
    return style;
}
}
