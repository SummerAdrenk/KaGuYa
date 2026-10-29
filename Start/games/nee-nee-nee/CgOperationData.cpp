#include "CgOperationData.h"

namespace KaGuYa::Start::Runtime::NeeNeeNeeCgOperationData
{
CgOperationResourceProfile Profile() noexcept
{
    auto profile = SharedEarly20CgOperationProfile();
    profile.hideRequiresNotDragging = true;
    return profile;
}
}
