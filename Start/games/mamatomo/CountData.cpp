#include "CountData.h"

namespace KaGuYa::Start::Runtime::MamatomoCountData
{
const CountResourceProfile& Profile() noexcept
{
    static constexpr CountResourceProfile profile{CountResourceKind::LikableWeight, nullptr};
    return profile;
}
}
