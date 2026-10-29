#include "CgOperationData.h"

namespace KaGuYa::Start::Runtime::ChuXPetCgOperationData
{
CgOperationResourceProfile Profile() noexcept
{
    auto profile = SharedNative50CgOperationProfile();
    profile.hideRequiresNotDragging = true;
    return profile;
}
}
