#include <Runtime/LegacyItemAcquisitionResourceProfile.h>
#include <stdexcept>
namespace KaGuYa::Start::Runtime
{
const LegacyItemAcquisitionResourceProfile& RequireLegacyItemAcquisitionProfile(const LegacyItemAcquisitionResourceProfile* p)
{
    if(!p)throw std::logic_error("Legacy item acquisition is not bound for this game");
    if(p->items.size()!=8||p->products.size()!=8||p->checks.size()!=2||p->systemBefore.size()!=4
        ||p->systemAfter.size()!=4||p->navigation.size()!=6||p->routes.size()!=6)
        throw std::invalid_argument("Item acquisition profile differs from audited Love Holic tables");
    return *p;
}
}
