#include "CgOperationData.h"

namespace KaGuYa::Start::Runtime::BakuAne2OtoutoIppaiShibocchauZoCgOperationData
{
CgOperationResourceProfile Profile() noexcept
{
    auto profile = SharedNative50CgOperationProfile();
    profile.hideRequiresNotDragging = true;
    return profile;
}
}
