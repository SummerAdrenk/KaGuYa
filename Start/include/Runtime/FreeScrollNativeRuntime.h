#pragma once

#include <Runtime/FreeScrollRuntime.h>
#include <Runtime/FreeScrollNativeInput.h>
#include <Runtime/FreeScrollNativeDrawing.h>

namespace KaGuYa::Start::Runtime
{
// 仅把 Layer 服务的三个调用接到同一50h正文；不拥有正文、窗口、输入或Surface。
// CReadScript的拥有者须显式构造/析构正文，不能由此适配器多做一次End/释放。
class FreeScrollNativeRuntime final : public IFreeScrollOperations
{
public:
    FreeScrollNativeRuntime(FreeScrollNativeBody50h& body,
        IDrawManagerNativeRootSource& draw, const IStartupConfigGameSystemSource& params,
        FreeScrollNativeDrawServices& graphics, PatternLayerNativeRenderServices& pattern,
        FreeScrollNativeInputServices& input, const ISpriteAnimationInitializationSource& registry,
        IGraphicsScreenCalls& screen, IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory()) noexcept
        : body_(body), draw_(draw), params_(params), graphics_(graphics), pattern_(pattern),
          input_(input), registry_(registry), screen_(screen), memory_(memory) {}
    bool BeginAt4ADF70() override
        { return BeginFreeScrollNativeAt4ADF70(body_, draw_, params_, graphics_, memory_); }
    bool UpdateAt4BDD00() override
        { return UpdateFreeScrollNativeAt4BDD00(body_, input_, graphics_, registry_, screen_); }
    void EndAt4AC970() override
        { EndFreeScrollNativeAt4AC970(body_, draw_, params_, pattern_, input_); }
private:
    FreeScrollNativeBody50h& body_;
    IDrawManagerNativeRootSource& draw_;
    const IStartupConfigGameSystemSource& params_;
    FreeScrollNativeDrawServices& graphics_;
    PatternLayerNativeRenderServices& pattern_;
    FreeScrollNativeInputServices& input_;
    const ISpriteAnimationInitializationSource& registry_;
    IGraphicsScreenCalls& screen_;
    IEnvironmentRootMemory& memory_;
};
}
