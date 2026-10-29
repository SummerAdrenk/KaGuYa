#include <Runtime/CgPageResourceProfile.h>
#include <Runtime/GameBuildProfile.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
const CgPageResourceProfile& CurrentCgPageResourceProfile()
{
    const auto* value=CompiledGameProfile().cgPage;
    if(!value)throw std::logic_error("CG page profile is not bound");
    return *value;
}
}
