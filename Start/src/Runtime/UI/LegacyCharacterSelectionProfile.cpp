#include <Runtime/LegacyCharacterSelectionProfile.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
const LegacyCharacterSelectionProfile& NoLegacyCharacterSelectionProfile() noexcept
{
    // None为零值；整表值初始化保留原先省略成员的零/空状态。
    static_assert(LegacyCharacterSelectionProfile{}.kind == LegacyCharacterSelectionKind::None);
    static const LegacyCharacterSelectionProfile p{};
    return p;
}
const char* LegacyCharacterSelectionSceneName() noexcept
{ return "\x83\x4C\x83\x83\x83\x89\x91\x49\x91\xF0\x8F\x88\x97\x9D"; }
void ValidateLegacyCharacterSelectionProfile(const LegacyCharacterSelectionProfile& p)
{
    if(p.kind==LegacyCharacterSelectionKind::None)return;
    if(p.views.empty()||p.choices.empty()||!p.resultVariable||p.bgm.empty()||p.modeCount<1||p.optionCount<1)
        throw std::invalid_argument("Character selection: incomplete profile");
    for(const auto& v:p.views)for(const auto& w:v.widgets)
        if(w.kind<0||w.kind>2)throw std::invalid_argument("Character selection: unsupported widget");
    if(p.kind==LegacyCharacterSelectionKind::TwoPage && (p.animations.size()!=5||p.animationPositions.size()!=65||p.hoverRows.size()!=4))
        throw std::invalid_argument("Character selection: incomplete two-page data");
    if(p.kind==LegacyCharacterSelectionKind::ResourcePair && (!p.costVariable||p.costs.size()!=18||!p.selectionCue))
        throw std::invalid_argument("Character selection: incomplete cost contract");
    if(p.kind==LegacyCharacterSelectionKind::TabbedSix && (p.tabs.size()!=6||p.portraits.size()!=28||p.dateDigits.size()!=10||p.datePositions.size()!=2||p.timeImages.size()!=3))
        throw std::invalid_argument("Character selection: incomplete tab data");
}
}
