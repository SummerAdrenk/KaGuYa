#pragma once

#include <cstdint>

namespace KaGuYa::Start::Runtime
{
struct ScriptNativeBody;
class IDrawManagerNativeRootSource;
class ParamsRuntimeRootOwner;
class EnvironmentRootOwner;
struct PatternLayerNativeResourceServices;
class IRuntimeStringManagerSource;
class IEnvironmentRootMemory;

// 4CB030：服务全为调用期借用，不持有根、Result、Pattern或资源的第二份owner。
struct ScriptResultLayerNativeBindingServices final
{
    IDrawManagerNativeRootSource& draw;
    ParamsRuntimeRootOwner& params;
    EnvironmentRootOwner& environment;
    PatternLayerNativeResourceServices& resources;
    IRuntimeStringManagerSource& strings;
    IEnvironmentRootMemory& memory;
};

// 固定receiver.parserAtA0；各原取根点独立Capture，异常保留已发生的正文写入。
// Draw来源必须返回真实2Ch/4Ch/80h，不接受现代Root/DTO的布局强转。
void ApplyScriptResultLayersNativeAt4CB030(ScriptNativeBody& receiver,
    std::int32_t layerGroupIndex, ScriptResultLayerNativeBindingServices& services);
}
