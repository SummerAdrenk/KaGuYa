#include <Runtime/LegacyMenuSelectionResourceProfile.h>
#include <stdexcept>
namespace KaGuYa::Start::Runtime
{
const LegacyMenuSelectionResourceProfile& RequireLegacyMenuSelectionProfile(
    const LegacyMenuSelectionResourceProfile* p)
{
    if (!p) throw std::logic_error("Legacy menu selection is not bound for this game");
    if (p->statics.size() != 4 || p->choices.size() != 3 || p->systemChecks.size() != 2
        || p->systemBeforeChecks.size() != 4 || p->systemAfterChecks.size() != 4
        || p->navigation.size() != 4 || p->routes.size() != 6)
        throw std::invalid_argument("Legacy menu profile does not match audited Nee Nee Nee tables");
    return *p;
}
}
