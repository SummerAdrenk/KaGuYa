#include <Runtime/SaveLoadResourceProfile.h>
#include <Runtime/GameBuildProfile.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
const SaveLoadResourceProfile& CurrentSaveLoadResourceProfile()
{
    const auto* profile = CompiledGameProfile().saveLoad;
    if (!profile) throw std::logic_error("Save/load resource profile is not bound");
    return *profile;
}

int SaveLoadConsumerEvent(SaveLoadEventScope scope, int originalEvent)
{
    // 只转换明确登记的页面事件；未登记值原样传递，不对父phase或确认event加偏移。
    for (const auto& entry : CurrentSaveLoadResourceProfile().events)
        if (entry.scope == scope && entry.originalEvent == originalEvent)
            return entry.consumerEvent;
    return originalEvent;
}
}
