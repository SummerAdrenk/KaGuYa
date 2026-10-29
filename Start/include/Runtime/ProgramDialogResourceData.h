#pragma once
#include <Runtime/ProgramMain01DialogNative.h>
#include <Runtime/ProgramMain00DialogNative.h>
#include <array>
#include <cstdint>
#include <span>

namespace KaGuYa::Start::Runtime
{
enum class ProgramDialogImplementation : std::uint8_t { Configured, MamatomoTraining, MamatomoTrip };
enum class ProgramRuleTest : std::uint8_t { Constant, Nonzero, NotMinusOne, Nonnegative, Equal, AnyNonnegative };
enum class ProgramRuleTarget : std::uint8_t { State, Enabled };
struct ProgramControlRule final
{
    const char* variable;
    const char* alternative;
    std::int32_t control;
    ProgramRuleTest test;
    std::int32_t operand;
    ProgramRuleTarget target;
};
struct ProgramPanelProfile final
{
    const char* background = nullptr;
    std::span<const ProgramMain01DialogNativeControlGroup> groups;
    std::span<const ProgramMain01DialogNativeNavigationGroup> navigation;
    std::span<const ProgramControlRule> rules;
    const char* staticSelector = nullptr;
    std::span<const GuiStaticDescriptorNative24h> selectedStatics;
};
struct ProgramCalendarProfile final
{
    std::span<const char* const> monthImages;
    std::span<const char* const> timeImages;
    std::span<const char* const> digits;
    bool monthDigits = false;
    std::array<POINT, 4> positions{}; // month ones/tens, date ones/tens
    bool suppressLeadingZero = false;
    bool monthModuloOnly = false;
};
struct ProgramSystemRoute final { std::int32_t event; const char* name; };
struct ProgramSystemProfile final
{
    ProgramPanelProfile panel;
    std::array<std::int32_t, 2> resetControls;
    std::int32_t readToggle;
    std::int32_t skipToggle;
    std::span<const ProgramSystemRoute> routes;
};
struct ProgramChoiceProfile final
{
    std::int32_t event;
    std::int32_t result;
    // Empty rules with confirmation=true is the fixed yes/no dialog.
    bool confirmation;
    std::span<const ProgramControlRule> confirmationRules;
    std::array<std::int32_t, 2> confirmedResults;
};
struct ProgramConfirmationEvent final { std::int32_t event, outcome; }; // 0 cancel; 1/2 accepted branch
struct ProgramConfirmationProfile final
{
    ProgramPanelProfile panel;
    std::span<const ProgramConfirmationEvent> events;
    bool cancelOnBackOrRight = false;
};
struct ProgramTrainingProfileView final
{
    std::span<const GuiStaticDescriptorNative24h> initial;
    std::span<const GuiButtonDescriptorNative2Ch> buttons;
    std::span<const GuiStaticDescriptorNative24h> statics;
    std::span<const char* const> parameters, other, resources, markers;
    std::span<const POINT> numberPoints;
    std::span<const std::int32_t> events, extraEvents;
    std::span<const ProgramTrainingRecordNative14h> records;
};
struct ProgramModeProfile final
{
    std::int32_t scriptValue;
    ProgramDialogImplementation implementation;
    ProgramPanelProfile main;
    ProgramCalendarProfile calendar;
    ProgramSystemProfile system;
    ProgramConfirmationProfile confirmation;
    std::span<const ProgramChoiceProfile> choices;
    std::span<const char* const> timeBgm;
    const ProgramTrainingProfileView* training = nullptr;
};
}
