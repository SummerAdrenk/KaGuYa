#pragma once

#include <Runtime/MessageWindowConfigRuntime.h>
#include <Runtime/MessageWindowStyleFactoryRuntime.h>
#include <variant>

namespace KaGuYa::Start::Runtime
{
// 459B67/459C2B 分别分配并发布配置，然后 459CED 才访问 params。
// 服务只在构造期间借用，不能提前创建配置或代替 CMesWin 持有它们。
class IMessageWindowConstructionServices
{
public:
    virtual ~IMessageWindowConstructionServices() = default;
    [[nodiscard]] virtual std::shared_ptr<MessageWindowConfigRuntime> AllocateConfig(
        MessageWindowConfigType type) = 0;
    virtual void ApplyConfigOverrides(MessageWindowConfigRuntime& type1,
        MessageWindowConfigRuntime& type2) = 0;
    [[nodiscard]] virtual std::unique_ptr<MessageWindowStyleRuntime> CreateStyle(
        std::uint32_t index, const std::shared_ptr<MessageWindowConfigRuntime>& type1,
        const std::shared_ptr<MessageWindowConfigRuntime>& type2) = 0;
};

// 正式资源/config/pane 装配服务，不拥有任何 config 引用；save 需长寿于图层。
class MessageWindowConstructionRuntime final : public IMessageWindowConstructionServices
{
public:
    // 固定输入，不随 ParamsRuntimeRootOwner 的 GameSystem 替换。
    MessageWindowConstructionRuntime(const ParamsGameSystem& params,
        const SaveDataEnvironmentArchive& save, IMessageWindowConfigExpressionParser& parser,
        IGraphicsSurfaceApi& surfaces, IGraphicsWindowSurfaceApi& graphics,
        IColorTransWindowResourceLoader& resources);
    // 仅借 root；构造服务及 AllocateConfig 均不取源，459CED 才开始读。
    MessageWindowConstructionRuntime(ParamsRuntimeRootOwner& params,
        const SaveDataEnvironmentArchive& save, IMessageWindowConfigExpressionParser& parser,
        IGraphicsSurfaceApi& surfaces, IGraphicsWindowSurfaceApi& graphics,
        IColorTransWindowResourceLoader& resources);
    [[nodiscard]] std::shared_ptr<MessageWindowConfigRuntime> AllocateConfig(
        MessageWindowConfigType type) override;
    void ApplyConfigOverrides(MessageWindowConfigRuntime& type1,
        MessageWindowConfigRuntime& type2) override;
    [[nodiscard]] std::unique_ptr<MessageWindowStyleRuntime> CreateStyle(
        std::uint32_t index, const std::shared_ptr<MessageWindowConfigRuntime>& type1,
        const std::shared_ptr<MessageWindowConfigRuntime>& type2) override;

private:
    std::variant<const ParamsGameSystem*, ParamsRuntimeRootOwner*> params_;
    const SaveDataEnvironmentArchive& save_;
    IMessageWindowConfigExpressionParser& parser_;
    IGraphicsSurfaceApi& surfaces_;
    IGraphicsWindowSurfaceApi& graphics_;
    IColorTransWindowResourceLoader& resources_;
};
}
