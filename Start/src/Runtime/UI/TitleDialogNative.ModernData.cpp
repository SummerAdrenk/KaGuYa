#include <Runtime/TitleDialogNative.h>
#include "../../../games/baku-ane-otouto-shibocchau-zo/TitleData.h"

#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
const TitleResourceProfile& RequireConfiguredTitleResources(const TitleResourceProfile* profile)
{
    if (!profile || !profile->blackName || !profile->overlayName || !profile->bgmName ||
        !profile->clickCue || !profile->hoverCue)
        throw std::logic_error("Configured title resource data is incomplete");
    if (*profile->overlayName)
        throw std::logic_error("Animated title overlay is not supported by the shared static-title path");
    return *profile;
}

const GuiStaticDescriptorNative24h& ModernTitleDialogStatic(const TitleDialogProfile& profile)
{
    return profile.buttonResources == TitleButtonResources::Configured
        ? RequireConfiguredTitleResources(profile.resourceProfile).background
        : ModernTitleDialogStatic(profile.buttonResources);
}
std::span<const GuiButtonDescriptorNative2Ch, 6> ModernTitleDialogButtons(const TitleDialogProfile& profile)
{
    return profile.buttonResources == TitleButtonResources::Configured
        ? RequireConfiguredTitleResources(profile.resourceProfile).buttons
        : ModernTitleDialogButtons(profile.buttonResources);
}
const char* ModernTitleDialogBlackName(const TitleDialogProfile& profile)
{
    return profile.buttonResources == TitleButtonResources::Configured
        ? RequireConfiguredTitleResources(profile.resourceProfile).blackName
        : ModernTitleDialogBlackName();
}
namespace TitleDialogData
{
std::span<const GuiButtonDescriptorNative2Ch, 6> PureHolicButtons() noexcept;
std::span<const GuiButtonDescriptorNative2Ch, 6> BunnysMamaButtons();
}

std::span<const GuiButtonDescriptorNative2Ch, 6> ModernTitleDialogButtons(TitleButtonResources resources)
{
    switch (resources)
    {
    case TitleButtonResources::Mamatomo: return ModernTitleDialogButtons();
    case TitleButtonResources::PureHolic: return TitleDialogData::PureHolicButtons();
    case TitleButtonResources::BunnysMama: return TitleDialogData::BunnysMamaButtons();
    case TitleButtonResources::BakuAne: return BakuTitleData::SharedButtons();
    default: throw std::logic_error("Title button resources are not implemented or audited");
    }
}
const GuiStaticDescriptorNative24h& ModernTitleDialogStatic(TitleButtonResources resources)
{
    switch (resources)
    {
    case TitleButtonResources::BakuAne: return BakuTitleData::SharedBackground();
    case TitleButtonResources::Mamatomo:
    case TitleButtonResources::PureHolic:
    case TitleButtonResources::BunnysMama: return ModernTitleDialogStatic();
    default: throw std::logic_error("Title background resources are not configured");
    }
}
}
