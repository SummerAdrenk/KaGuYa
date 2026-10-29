#pragma once

#include <Runtime/GuiControlFactoryNative.h>
#include <Runtime/GuiNavigationNative.h>
#include <span>

namespace KaGuYa::Start::Runtime
{
enum class LegacyMapSelectionKind { None, AnimatedActors, AreaActors, ProgressBook, CharacterDecision };
enum class LegacyMapSelectionIndex { Fixed, Time, BooleanTime, FormTwo, CharacterDisplay };
enum class LegacyMapSelectionAreaRule { PriorityMarkers, TimeMarkers, RouteVisibility, HideEmpty, StackedFive, StackedThree, SpecialEvents };

struct LegacyMapSelectionWidget final
{
    int kind;
    GuiStaticDescriptorNative24h common;
    std::array<const char*, 7> checked;
    int hitSlot, childId;
};
struct LegacyMapSelectionWidgetSet final { std::span<const LegacyMapSelectionWidget> widgets; };
struct LegacyMapSelectionView final
{
    int id;
    LegacyMapSelectionIndex index;
    const char* variable;
    std::span<const LegacyMapSelectionWidgetSet> variants;
};
struct LegacyMapSelectionNavigation final
{
    int view, input;
    std::span<const GuiDialogNavigationMapping> pairs;
};
struct LegacyMapSelectionHotkey final { int view, input, event; };
struct LegacyMapSelectionResult final { int event, value; };
struct LegacyMapSelectionCommand final { int event; const char* name; };
struct LegacyMapSelectionCue final { const char* name; int group; };
struct LegacyMapSelectionSystem final
{
    std::span<const LegacyMapSelectionCommand> commands;
    std::span<const int> disabled;
    int autoEvent=-1, skipEvent=-1;
};
struct LegacyMapSelectionBackground final
{
    LegacyMapSelectionIndex index=LegacyMapSelectionIndex::Fixed;
    std::span<const char* const> names;
    int event=-1;
    bool composeDateLayer=false;
    const char* dateBase=nullptr;
    std::span<const char* const> dateDigits;
    std::span<const POINT> datePositions;
};
struct LegacyMapSelectionActor final
{
    const char* route;
    const char* location;
    const char* eventType;
    int event, label;
    // 08/10/11/12：原索引范围内的NULL项是可选域，不以别款图片补齐。
    std::span<const LegacyMapSelectionWidget* const> variants;
};
struct LegacyMapSelectionBadge final { int event, actor; POINT position; };
struct LegacyMapSelectionArea final
{
    LegacyMapSelectionAreaRule rule{};
    int count=0, firstPlace=-1, firstMarker=-1, firstBackground=-1;
    int freeEvent=-1, cancelEvent=-1, freeResultBase=0;
    int hintArea=-1, hintCancel=-1, parentOverlay=-1;
    const char* explorationVariable=nullptr;
    std::span<const char* const> extraAreaVariables;
    std::span<const POINT> markerPositions;
    std::span<const std::array<const char*, 7>> markerStates;
    std::span<const char* const> badgeNames;
    std::span<const LegacyMapSelectionBadge> badges;
    LegacyMapSelectionCue placeCue{}, actorCue{};
    const char* animation=nullptr;
    POINT animationPosition{};
    int animationEvent=-1;
    int dateEvent=-1, gaugeEvent=-1;
};
struct LegacyMapSelectionAnimatedVariant final
{
    const LegacyMapSelectionWidget* button;
    const char* animation;
};
struct LegacyMapSelectionAnimatedActor final
{
    const char* location;
    const char* form;
    std::span<const LegacyMapSelectionAnimatedVariant> variants;
};
struct LegacyMapSelectionSpecial final
{
    std::span<const char* const> types;
    std::span<const LegacyMapSelectionWidget> typeWidgets;
    std::span<const char* const> flags;
    std::span<const LegacyMapSelectionWidget> flagWidgets;
    std::span<const char* const> otherLocations;
    std::span<const int> visibilitySets;
};
struct LegacyMapSelectionBookNode final { int event, threshold; POINT position; };
struct LegacyMapSelectionBookTrack final
{
    const char* progress;
    const char* state;
    const char* form;
    std::span<const LegacyMapSelectionBookNode> nodes;
    std::span<const char* const> prefixImages;
    const char* stateImage;
};
struct LegacyMapSelectionBookOverlay final
{
    const char* variable;
    std::span<const char* const> images;
};
struct LegacyMapSelectionBookPage final
{
    int view, canvasEvent;
    std::span<const LegacyMapSelectionBookTrack> tracks;
    std::span<const LegacyMapSelectionBookOverlay> overlays;
};
struct LegacyMapSelectionBookSymbol final { const char* name; POINT offset; };
struct LegacyMapSelectionBook final
{
    std::span<const LegacyMapSelectionBookPage> pages;
    std::span<const LegacyMapSelectionBookSymbol> symbols;
    std::span<const int> overviewResults;
    std::span<const int> pageOrder;
    const char* globalImage=nullptr;
    int globalWord=-1;
};
struct LegacyMapSelectionDecisionStage final { const char* name; int firstEvent, secondEvent; };
struct LegacyMapSelectionDecisionActor final
{
    const char* route;
    int background, popup, decision;
    std::array<const char*, 7> popupStates;
    const char* firstCounter;
    const char* secondCounter;
    const char* extraImage;
    std::span<const LegacyMapSelectionDecisionStage> stages;
};
struct LegacyMapSelectionDecision final
{
    std::span<const LegacyMapSelectionDecisionActor> actors;
    const char* counterSymbol=nullptr;
    POINT origin{};
    std::span<const POINT> firstPoints, secondPoints;
    LegacyMapSelectionCue characterCue{}, decisionCue{};
};
struct LegacyMapSelectionProfile final
{
    LegacyMapSelectionKind kind=LegacyMapSelectionKind::None;
    std::span<const LegacyMapSelectionView> views;
    std::span<const LegacyMapSelectionNavigation> navigation;
    std::span<const LegacyMapSelectionHotkey> hotkeys;
    LegacyMapSelectionSystem system{};
    LegacyMapSelectionBackground background{};
    std::span<const char* const> bgm;
    LegacyMapSelectionIndex bgmIndex=LegacyMapSelectionIndex::Fixed;
    bool readTimeForFixedBgm=false;
    const char* resultVariable=nullptr;
    const char* secondResultVariable=nullptr;
    std::span<const LegacyMapSelectionResult> results;
    std::span<const LegacyMapSelectionActor> actors;
    LegacyMapSelectionArea area{};
    std::span<const LegacyMapSelectionAnimatedActor> animatedActors;
    std::span<const POINT> animationPositions;
    LegacyMapSelectionSpecial special{};
    LegacyMapSelectionBook book{};
    LegacyMapSelectionDecision decision{};
};

const LegacyMapSelectionProfile& NoLegacyMapSelectionProfile() noexcept;
const char* LegacyMapSelectionSceneName() noexcept;
void ValidateLegacyMapSelectionProfile(const LegacyMapSelectionProfile&);
}
