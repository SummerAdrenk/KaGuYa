#include "CgOperationData.h"

namespace KaGuYa::Start::Runtime::MamagotoMamaToNaishoNoEcchiShimashoCgOperationData
{
CgOperationResourceProfile Profile() noexcept
{
    auto profile = SharedEarly20CgOperationProfile();
    profile.hideRequiresNotDragging = true;
    return profile;
}
}
