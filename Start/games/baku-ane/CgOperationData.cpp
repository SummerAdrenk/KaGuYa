#include "CgOperationData.h"

namespace KaGuYa::Start::Runtime::BakuAneOtoutoShibocchauZoCgOperationData
{
CgOperationResourceProfile Profile() noexcept
{
    auto profile = SharedNative50CgOperationProfile();
    profile.hideRequiresNotDragging = false;
    return profile;
}
}
