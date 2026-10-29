#pragma once
#include <Runtime/NativeLayerWindowScrVmUpdateRuntime.h>
#include <Runtime/ScriptMenuCommandNative.h>

namespace KaGuYa::Start::Runtime
{
// 与UPDATE共享同一CScript接收者和原生Root图层来源，不另建现代LayerWindow。
class NativeLayerWindowScrVmMenuRuntime final : public IScrVmMenuLayerBoundary
{
public:
    explicit NativeLayerWindowScrVmMenuRuntime(NativeLayerWindowScrVmUpdateRuntime& updates) noexcept
        : updates_(updates) {}
    void ApplyResultLayerList(std::int32_t listIndex) override;
    void SelectEffect(std::int32_t selector) override;
private:
    NativeLayerWindowScrVmUpdateRuntime& updates_;
};
}
