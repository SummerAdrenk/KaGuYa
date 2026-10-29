#include <Runtime/LikabilityResourceProfile.h>
#include <Runtime/GameBuildProfile.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
const LikabilityResourceProfile& GameLikabilityResourceProfile()
{
    const auto* profile = CompiledGameProfile().likability;
    if (!profile) throw std::logic_error("Likability resource profile is not bound");
    return *profile;
}
void RequireUsableLikabilityProfile(const LikabilityResourceProfile& profile)
{
    // 06原命名工厂并非不存在；调用到这里时明确拒绝缺配套资源/变量的残留页。
    if (const auto* residual = std::get_if<LikabilityResidualData>(&profile.paint))
        throw std::runtime_error(residual->reason);
    if (profile.flow.pages <= 0) throw std::logic_error("Likability profile has no pages");
}
}
