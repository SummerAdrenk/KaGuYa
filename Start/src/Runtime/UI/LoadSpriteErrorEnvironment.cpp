#include <Runtime/LoadSpriteErrorEnvironment.h>
#include <Runtime/EnvironmentStartupRuntime.h>
#include <Runtime/EnvironmentRootRuntime.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
LoadSpriteErrorEnvironmentView RootLoadSpriteErrorEnvironment::CaptureAt4180C0()
{
    auto* const root = owner_.CaptureAt4180C0();
    // 原调用接着解引用+10/+2C；现代边界明确拒绝空根，不以空caption/HWND兜底。
    if (!root) throw std::logic_error("LoadSprite error environment root is null (4180C0)");
    return {root->titleAt10, root->windowAt2C};
}

LoadSpriteErrorEnvironmentView ExistingLoadSpriteErrorEnvironment::CaptureAt4180C0()
{
    const auto& root = environment_.CaptureAt4180C0();
    return {root.titleAt10, root.windowAt2C};
}
}
