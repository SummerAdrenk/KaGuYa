#include <Runtime/AppreciationSceneResourceProfile.h>
#include <Runtime/GameBuildProfile.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
const AppreciationSceneResourceProfile& CurrentAppreciationSceneProfile()
{
    // 只接受当前编译游戏的显式绑定，禁止以18号参数补缺。
    const auto* profile = CompiledGameProfile().appreciationScene;
    if (!profile) throw std::logic_error("Appreciation scene resource profile is not bound");
    return *profile;
}
}
