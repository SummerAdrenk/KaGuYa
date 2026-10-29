#include <Runtime/SettingOtherResourceProfile.h>
#include <Runtime/GameBuildProfile.h>
#include <Runtime/SettingPage04Native.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
const SettingOtherResourceProfile& CurrentSettingOtherProfile()
{
    const auto* profile = CompiledGameProfile().settingsOther;
    if (!profile) throw std::logic_error("Game Other settings profile is not bound");
    return *profile;
}

const SettingOtherBlockProfile& SettingOtherBlockFor(SettingOtherBlock kind)
{
    for (const auto& block : CurrentSettingOtherProfile().blocks)
        if (block.kind == kind) return block;
    throw std::logic_error("Other settings block is absent from this game");
}

GameResourceNamePromptResult ISettingPage04NativeSource::RunLocalNamePrompt()
{
    throw std::logic_error("Page04 local resource name prompt is not bound");
}
}
