#include <Runtime/CountResourceProfile.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
const CountResourceProfile& NoCountResourceProfile() noexcept
{
    static constexpr CountResourceProfile profile{CountResourceKind::None, nullptr};
    return profile;
}
void ValidateCountResourceProfile(const CountResourceProfile& profile)
{
    if (profile.kind == CountResourceKind::None || profile.kind == CountResourceKind::LikableWeight)
    {
        if (profile.multi) throw std::invalid_argument("Count: unexpected multi-gauge data");
        return;
    }
    if (profile.kind != CountResourceKind::MultiGauge || !profile.multi)
        throw std::invalid_argument("Count: missing or unknown profile");
    const auto& d = *profile.multi;
    if (d.gauges.size() != 8 || d.icons.size() != 2 || d.digits.size() != 10 || d.variables.size() != 10)
        throw std::invalid_argument("Count: incomplete multi-gauge resources");
    for (auto names : {d.gauges, d.icons, d.digits})
        for (auto name : names)
            if (!name) throw std::invalid_argument("Count: null resource name");
    for (const auto& v : d.variables)
        if (!v.variable || v.frameIndex >= d.gauges.size() - 1)
            throw std::invalid_argument("Count: invalid character mapping");
    for (const auto& r : d.rows)
        if (!r.selector || !r.delta || !r.upperLimit)
            throw std::invalid_argument("Count: incomplete row parameters");
    for (std::size_t n = 0; n != d.layoutOffsets.size(); ++n)
        if (d.layoutOffsets[n] + n > d.layouts.size())
            throw std::invalid_argument("Count: invalid row layout");
    if (!d.countVariable || !d.centerXVariable || !d.centerYVariable || !d.initialDelayVariable ||
        !d.stepDelayVariable || !d.finalDelayVariable || !d.countCue)
        throw std::invalid_argument("Count: incomplete parameter or sound contract");
}
}
