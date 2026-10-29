#include <Runtime/MessageWindowConstructionRuntime.h>

namespace KaGuYa::Start::Runtime
{
MessageWindowConstructionRuntime::MessageWindowConstructionRuntime(
    const ParamsGameSystem& params, const SaveDataEnvironmentArchive& save,
    IMessageWindowConfigExpressionParser& parser, IGraphicsSurfaceApi& surfaces,
    IGraphicsWindowSurfaceApi& graphics, IColorTransWindowResourceLoader& resources)
    : params_(&params), save_(save), parser_(parser), surfaces_(surfaces),
      graphics_(graphics), resources_(resources) {}

MessageWindowConstructionRuntime::MessageWindowConstructionRuntime(
    ParamsRuntimeRootOwner& params, const SaveDataEnvironmentArchive& save,
    IMessageWindowConfigExpressionParser& parser, IGraphicsSurfaceApi& surfaces,
    IGraphicsWindowSurfaceApi& graphics, IColorTransWindowResourceLoader& resources)
    : params_(&params), save_(save), parser_(parser), surfaces_(surfaces),
      graphics_(graphics), resources_(resources) {}

std::shared_ptr<MessageWindowConfigRuntime> MessageWindowConstructionRuntime::AllocateConfig(
    MessageWindowConfigType type)
{
    // 现代共享所有权不模拟原 Ref12 控制块分配失败后的悬空/无控制块状态。
    return std::make_shared<MessageWindowConfigRuntime>(save_, type);
}

void MessageWindowConstructionRuntime::ApplyConfigOverrides(
    MessageWindowConfigRuntime& type1, MessageWindowConfigRuntime& type2)
{
    std::visit([&](auto* params) {
        ApplyMessageWindowConfigOverrides(*params, type1, type2, parser_);
    }, params_);
}

std::unique_ptr<MessageWindowStyleRuntime> MessageWindowConstructionRuntime::CreateStyle(
    std::uint32_t index, const std::shared_ptr<MessageWindowConfigRuntime>& type1,
    const std::shared_ptr<MessageWindowConfigRuntime>& type2)
{
    // 临时工厂不能活到 CMesWin 析构之后，否则会延后原 468D00 的 config 释放。
    MessageWindowStyleFactoryRuntime factory(surfaces_, graphics_, resources_,
        MessageWindowStylesSample18At601408(), type1, type2);
    return factory.CreateStyle(index);
}
}
