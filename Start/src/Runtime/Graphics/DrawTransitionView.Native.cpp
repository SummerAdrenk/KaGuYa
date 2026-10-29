#include <Runtime/DrawTransitionView.h>
#include <Runtime/DrawManagerNative.h>

#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
const DrawTransitionViewMethods methods{
    [](const void* body, const TransitOperation& operation, IGraphicsTransitApi* graphics)
    {
        // 原正文没有Graphics字段；缺少显式服务不能改走Controlled或隐式根。
        if (!graphics)
            throw std::logic_error("native Draw transition graphics services were not bound");
        // IDA 4C39E3直接读[ecx+20h]；不提前拒绝显式native null正文。
        // 既有Native叶已直接走449DD0 raw分派；view不加表边界或空函数保护。
        return RenderDrawTransitionAt4C39E0(
            *static_cast<const DrawManagerNativeBody2Ch*>(body), operation, *graphics);
    }};
}

DrawTransitionView::DrawTransitionView(const DrawManagerNativeBody2Ch* body,
    IGraphicsTransitApi* graphics) noexcept
    : body_(body), methods_(&methods), graphics_(graphics)
{
}

DrawTransitionView::DrawTransitionView(const DrawManagerNativeBody2Ch& body,
    IGraphicsTransitApi& graphics) noexcept : DrawTransitionView(&body, &graphics)
{
}

const DrawTransitionViewMethods& DrawTransitionView::Methods() const
{
    if (!methods_)
        throw std::logic_error("DrawTransitionView is not bound");
    return *methods_;
}
}
