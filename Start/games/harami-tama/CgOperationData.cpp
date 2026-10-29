#include "CgOperationData.h"

namespace KaGuYa::Start::Runtime::HaramiTamaCgOperationData
{
CgOperationResourceProfile Profile() noexcept
{
    auto profile = SharedNative50CgOperationProfile();
    profile.hideRequiresNotDragging = true;
    return profile;
}
}
