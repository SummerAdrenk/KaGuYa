#include <Runtime/DrawManagerRuntime.h>

#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
DrawManagerRuntime::DrawManagerRuntime(ICrtRandomSource& random, IGraphicsTransitApi& graphics)
    : transit_(std::make_unique<TransitRuntime>(random)), graphics_(graphics)
{
    // 声明顺序使注册表先成立，再执行448A30的播种/256次rand；失败时按逆序展开。
}

DrawManagerRuntime::~DrawManagerRuntime()
{
    // 462BB0先释放+20 CTransit，随后layers_按正序释放图层，最后释放注册列表。
    transit_.reset();
}

DrawManagerLayerRuntime& DrawManagerRuntime::Layers() noexcept { return layers_; }
const DrawManagerLayerRuntime& DrawManagerRuntime::Layers() const noexcept { return layers_; }

const TransitRuntime& DrawManagerRuntime::Transit() const
{
    if (!transit_) throw std::logic_error("DrawManager CTransit reference has been released");
    return *transit_;
}

std::uint8_t DrawManagerRuntime::RenderTransitionAt4C39E0(const TransitOperation& operation) const
{
    // 4C39E0仅解引用并尾跳449DD0，不推进帧、不失效、不Present。
    return Transit().RenderAt449DD0(operation, graphics_);
}
}
