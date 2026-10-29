#pragma once

#include <Runtime/DrawManagerLayerRuntime.h>
#include <Runtime/TransitRuntime.h>

namespace KaGuYa::Start::Runtime
{
// 458770/462BB0的manager owner：注册列表、拥有图层数组与独立CTransit引用。
// 具体图层由原上层装配链加入；本对象不虚构尚未装配的窗口槽。
class DrawManagerRuntime final
{
public:
    DrawManagerRuntime(ICrtRandomSource& random, IGraphicsTransitApi& graphics);
    ~DrawManagerRuntime();
    DrawManagerRuntime(const DrawManagerRuntime&) = delete;
    DrawManagerRuntime& operator=(const DrawManagerRuntime&) = delete;

    [[nodiscard]] DrawManagerLayerRuntime& Layers() noexcept;
    [[nodiscard]] const DrawManagerLayerRuntime& Layers() const noexcept;
    [[nodiscard]] const TransitRuntime& Transit() const;
    [[nodiscard]] std::uint8_t RenderTransitionAt4C39E0(const TransitOperation& operation) const;

private:
    DrawManagerLayerRuntime layers_;
    std::unique_ptr<TransitRuntime> transit_;
    IGraphicsTransitApi& graphics_;
};
}
