#include "CgOperationData.h"

namespace KaGuYa::Start::Runtime::BunnysMamaDaikouServiceCgOperationData
{
CgOperationResourceProfile Profile() noexcept
{
    auto profile = SharedLate20CgOperationProfile();
    profile.hideRequiresNotDragging = true;
    return profile;
}
}
