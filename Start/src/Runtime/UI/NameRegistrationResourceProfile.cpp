#include <Runtime/NameRegistrationResourceProfile.h>
#include <Runtime/GameBuildProfile.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
const NameRegistrationResourceProfile& CurrentNameRegistrationResourceProfile()
{
    const auto* profile=CompiledGameProfile().nativeName;
    // 01–10走资源112姓名框；不能将未绑定Native页解释为18号默认图。
    if(!profile)throw std::logic_error("Native name-registration profile is not bound");
    return *profile;
}
}
