#include <Runtime/LegacyMapSelectionProfile.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
const LegacyMapSelectionProfile& NoLegacyMapSelectionProfile() noexcept
{static const LegacyMapSelectionProfile none{};return none;}
const char* LegacyMapSelectionSceneName() noexcept
{return "\x83\x7D\x83\x62\x83\x76\x8F\x88\x97\x9D";}
void ValidateLegacyMapSelectionProfile(const LegacyMapSelectionProfile& p)
{
    if(p.kind==LegacyMapSelectionKind::None)return;
    if(!p.resultVariable||p.views.empty()||p.bgm.empty())
        throw std::invalid_argument("Map selection: incomplete profile");
    for(const auto& v:p.views)
    {
        if(v.variants.empty())throw std::invalid_argument("Map selection: empty view variants");
        for(const auto& set:v.variants)for(const auto& w:set.widgets)
            if(w.kind<0||w.kind>2)throw std::invalid_argument("Map selection: unsupported widget kind");
    }
    for(const auto& n:p.navigation)
        if(n.pairs.empty()||n.pairs.back().sourceEventValue>=0)
            throw std::invalid_argument("Map selection: unterminated navigation");
    if(p.kind==LegacyMapSelectionKind::AnimatedActors&&
        p.animationPositions.size()!=13*p.animatedActors.size())
        throw std::invalid_argument("Map selection: animated location table");
    if(p.kind==LegacyMapSelectionKind::AreaActors&&(p.area.count<=0||p.area.cancelEvent<0))
        throw std::invalid_argument("Map selection: area contract");
    if(p.kind==LegacyMapSelectionKind::ProgressBook&&
        (p.book.pages.size()!=5||p.book.symbols.size()!=3||p.book.pageOrder.size()!=5))
        throw std::invalid_argument("Map selection: book contract");
    if(p.kind==LegacyMapSelectionKind::CharacterDecision&&
        (p.decision.actors.size()!=5||!p.secondResultVariable))
        throw std::invalid_argument("Map selection: decision contract");
}
}
