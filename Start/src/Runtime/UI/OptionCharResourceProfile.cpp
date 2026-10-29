#include <Runtime/OptionCharResourceProfile.h>
#include <Runtime/GameBuildProfile.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
const OptionCharResourceProfile& CurrentOptionCharResourceProfile()
{
    const auto* profile = CompiledGameProfile().optionCharacters;
    if (!profile) throw std::logic_error("Option character profile is not bound");
    return *profile;
}
void ValidateOptionCharResourceProfile(const OptionCharResourceProfile& p)
{
    const bool legacy = p.presentation == OptionCharPresentation::NaturalLegacy;
    const auto records = legacy ? p.nativeRecords.size() : p.records.size();
    if (p.instances.empty() || p.visibleCount <= 0 || p.reportedCount <= 0 ||
        records != static_cast<std::size_t>(p.reportedCount) ||
        (legacy ? !p.records.empty() || !p.positions.empty() || p.scanCount != 0
                : !p.nativeRecords.empty() || p.positions.size() != static_cast<std::size_t>(p.visibleCount) || p.scanCount != p.reportedCount))
        throw std::logic_error("Option character profile has inconsistent spans/counts");
    if (p.presentation != OptionCharPresentation::NaturalLegacy &&
        p.presentation != OptionCharPresentation::PositionWindow &&
        p.presentation != OptionCharPresentation::SlidingWindow)
        throw std::logic_error("Unknown option character presentation");
    for (std::size_t i = 0; i < p.instances.size(); ++i)
    {
        const auto& entry = p.instances[i];
        if (entry.event != static_cast<int>(i) || entry.normalRecord < 0 ||
            static_cast<std::size_t>(entry.normalRecord) >= records || entry.unavailableRecord < -1 ||
            (entry.unavailableRecord >= 0 && static_cast<std::size_t>(entry.unavailableRecord) >= records) ||
            entry.characterDisplayIndex < -1 || (legacy && (entry.unavailableRecord != -1 || entry.characterDisplayIndex != -1)))
            throw std::logic_error("Option character instance has invalid record/event binding");
        if (!legacy && (p.records[entry.normalRecord].common.eventAt00 != entry.event ||
            (entry.unavailableRecord >= 0 && p.records[entry.unavailableRecord].common.eventAt00 != entry.event)))
            throw std::logic_error("Option character variant changes the category event");
    }
    for (const auto& record : p.nativeRecords)
        if (record.timelineAt24 != -1)
            throw std::logic_error("Option character native timeline is not supported by the audited contract");
    if (p.presentation == OptionCharPresentation::SlidingWindow)
    {
        if (p.navigationButtons.size() != 2 || p.visibleCount >= static_cast<int>(p.instances.size()) ||
            p.navigationButtons[0].common.eventAt00 != p.previousBoundaryEvent ||
            p.navigationButtons[1].common.eventAt00 != p.nextBoundaryEvent ||
            p.previousBoundaryEvent < static_cast<int>(p.instances.size()) ||
            p.nextBoundaryEvent < static_cast<int>(p.instances.size()) || p.nextBoundaryEvent == p.previousBoundaryEvent)
            throw std::logic_error("Option character sliding navigation is invalid");
    }
    else if (!p.navigationButtons.empty() || p.visibleCount != static_cast<int>(p.instances.size()) ||
        p.previousBoundaryEvent < 0 || p.previousBoundaryEvent >= static_cast<int>(p.instances.size()) ||
        p.nextBoundaryEvent < 0 || p.nextBoundaryEvent >= static_cast<int>(p.instances.size()))
        throw std::logic_error("Option character wrap event is outside created choices");
}
}
