#include "CgOperationData.h"

namespace KaGuYa::Start::Runtime::AnetomoCgOperationData
{
CgOperationResourceProfile Profile() noexcept
{
    auto profile = SharedLate20CgOperationProfile();
    profile.hideRequiresNotDragging = true;
    return profile;
}
}
