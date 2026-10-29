#include <Runtime/DrawTransitionView.h>
#include <Runtime/DrawManagerRuntime.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
// 显式现代适配只借完整manager；Graphics继续由该manager的既有合同提供。
const DrawTransitionViewMethods methods{
    [](const void* body, const TransitOperation& operation, IGraphicsTransitApi*)
    { return static_cast<const DrawManagerRuntime*>(body)->RenderTransitionAt4C39E0(operation); }};
}

DrawTransitionView::DrawTransitionView(const DrawManagerRuntime& body) noexcept
    : body_(&body), methods_(&methods)
{
}
}
