#include <Runtime/LogPageResourceProfile.h>
#include <Runtime/GameBuildProfile.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
const LogPageResourceProfile& CurrentLogPageResourceProfile()
{
    const auto* profile=CompiledGameProfile().logPage;
    if(!profile)throw std::logic_error("Log page profile is not bound");
    return *profile;
}
}
