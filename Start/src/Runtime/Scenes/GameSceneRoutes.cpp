#include <Runtime/GameSceneRoutes.h>
#include <Runtime/GameBuildProfile.h>
#include <Runtime/LegacyMenuSelectionSource.h>
#include <Runtime/LegacyCharacterSelectionProfile.h>
#include <Runtime/ProgramFixedActionRuntime.h>

namespace KaGuYa::Start::Runtime
{
std::string_view GameFixedActionSceneName(ScrVmFixedAction action)
{
    if(action==ScrVmFixedAction::Code13)
    {
        const auto& profile=CompiledGameProfile();
        // 六款角色选择的正常/恢复返回13同源；非空None并不表示存在该场景。
        if(profile.legacyCharacterSelection && profile.legacyCharacterSelection->kind!=LegacyCharacterSelectionKind::None)
            return LegacyCharacterSelectionSceneName();
        // NeeNeeNee 4930B0 case13：其余款中只有12号转专属菜单。
        if(profile.legacyMenuSelection)return LegacyMenuSelectionSceneName();
    }
    return ProgramFixedActionRuntime::ActionName(action);
}
}
