#include "CgOperationData.h"

namespace KaGuYa::Start::Runtime::NeeChanNoSusumeOneeChanNoItazuraSeiseikatsuCgOperationData
{
CgOperationResourceProfile Profile() noexcept
{
    auto profile = SharedEarly20CgOperationProfile();
    profile.hideRequiresNotDragging = true;
    return profile;
}
}
