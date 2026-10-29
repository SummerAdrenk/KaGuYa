#include <Runtime/PostWindowNativeDrawScope.h>

namespace KaGuYa::Start::Runtime
{
PostWindowNativeDrawScope::PostWindowNativeDrawScope(PostWindowNativeLayerServices& layers,
    LayerWindowNativeServices& layerRuntime, IGraphicsScreenCalls* graphics, DrawGuiNativeServices* gui) noexcept
    : gui_(gui), layer_(layerRuntime), message_(layers.message), tool_(layers.tool), text_(layers.text),
      wait_(layers.wait), select_(layers.select), countdown_(layers.countdown),
      configuration_(layers.text.environment), surfaces_(layers.surfaces),
      registration_(layers.memory, graphics)
{}

PostWindowNativeDrawScope::~PostWindowNativeDrawScope() noexcept = default;
}
