#include <Runtime/DrawManagerLayerRuntime.h>
#include <Runtime/GameBuildProfile.h>

#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
DrawManagerLayerRuntime::~DrawManagerLayerRuntime()
{
    // 462BB0 -> 452440：正序释放引用，释放过程中保留数组长度和后续槽。
    // 每个 LayerItem 的析构会从尚存活的注册表中注销自身。
    for (auto& layer : ownedLayers_)
        layer.reset();
}

LayerList& DrawManagerLayerRuntime::RegisteredLayers() noexcept { return registeredLayers_; }
const LayerList& DrawManagerLayerRuntime::RegisteredLayers() const noexcept { return registeredLayers_; }
std::size_t DrawManagerLayerRuntime::OwnedLayerCount() const noexcept { return ownedLayers_.size(); }

FrameLayerItem* DrawManagerLayerRuntime::LayerAt418C10(const std::uint32_t index) const
{
    // 原条件是 count < index；等于 count 会越界，不将这条非法输入静默当成空槽。
    if (index > ownedLayers_.size())
        return nullptr;
    return ownedLayers_.at(index).get();
}

void DrawManagerLayerRuntime::AppendOwnedLayerAt46A880(std::unique_ptr<FrameLayerItem> layer)
{
    // 注册来自对象构造，而不是这里；原函数同样保留空引用槽，不去重或重新排序。
    ownedLayers_.push_back(std::move(layer));
}

void DrawManagerLayerRuntime::AdvanceFramesAt46B240()
{
    for (std::size_t index = 0; index < ownedLayers_.size(); ++index)
    {
        auto* const layer = ownedLayers_[index].get();
        if (!layer)
            throw std::logic_error("DrawManager frame update encountered a null owning layer");
        // 46B270 无 enabled 门控。禁用时是否更新，由具体对象的 +14 自己决定。
        layer->AdvanceOwnedFrame();
    }
}

void DrawManagerLayerRuntime::ResetOwnedLayersAt47E940(IGraphicsWindowSurfaceApi& graphics)
{
    for (std::size_t index = 0; index < ownedLayers_.size(); ++index)
    {
        auto* target = dynamic_cast<ILayerResetTarget*>(ownedLayers_[index].get());
        if (!target) throw std::logic_error("DrawManager reset requires owning layer vslot +18");
        // 47E970：无 enabled 门控，逐项调用；失败保留前项已重置的状态。
        target->ResetOwnedLayerAtVslot18(graphics);
    }
}

std::uint8_t DrawManagerLayerRuntime::RequestOwnedLayerAtVslot10(
    const std::uint32_t index, const std::uint8_t requested, const std::int32_t argument)
{
    auto* const layer = LayerAt418C10(index);
    if (!layer)
        throw std::logic_error("DrawManager prepare encountered a null owning layer");
    auto* const target = dynamic_cast<ILayerPrepareTarget*>(layer);
    if (!target)
        throw std::logic_error("DrawManager owning layer does not implement vslot +10");
    return target->SetRequestedEnabledAtVslot10(requested, argument);
}

std::uint8_t DrawManagerLayerRuntime::PrepareLayersAt46A980(std::int32_t mode)
{
    auto result = RequestOwnedLayerAtVslot10(1, 0);
    for (std::uint32_t index = 2; index <= LastStartupDrawLayerIndex(); ++index)
    {
        const auto request = StartupPrepareRequest(index, false, mode);
        result = static_cast<std::uint8_t>(result | RequestOwnedLayerAtVslot10(index, request.requested, request.argument));
    }
    return result;
}

std::uint8_t DrawManagerLayerRuntime::PrepareLayersAt46AD20(const std::int32_t mode)
{
    auto result = RequestOwnedLayerAtVslot10(1, 1);
    for (std::uint32_t index = 2; index <= LastStartupDrawLayerIndex(); ++index)
    {
        const auto request = StartupPrepareRequest(index, true, mode);
        result = static_cast<std::uint8_t>(result | RequestOwnedLayerAtVslot10(index, request.requested, request.argument));
    }
    return result;
}
}
