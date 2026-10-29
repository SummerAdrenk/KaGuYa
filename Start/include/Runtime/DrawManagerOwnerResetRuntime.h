#pragma once

#include <Runtime/DrawManagerLayerRuntime.h>
#include <Runtime/NormalStartupScriptServices.h>

namespace KaGuYa::Start::Runtime
{
// Normal/Replay/Restore 共用47E940；既不是CParams容器，也不是registered绘制列表。
class DrawManagerOwnerResetRuntime final : public INormalStartupRootOwnerSynchronizer
{
public:
    DrawManagerOwnerResetRuntime(DrawManagerLayerRuntime& layers,
        IGraphicsWindowSurfaceApi& graphics) noexcept;
    void SynchronizeAtVslot18() override;
private:
    DrawManagerLayerRuntime& layers_;
    IGraphicsWindowSurfaceApi& graphics_;
};
}
