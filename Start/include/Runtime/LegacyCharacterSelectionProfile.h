#pragma once
#include <Runtime/GuiControlFactoryNative.h>
#include <Runtime/SaveLoadMessageBoxNativeChildFactoriesRuntime.h>
#include <Runtime/GuiNavigationNative.h>
#include <span>

namespace KaGuYa::Start::Runtime
{
enum class LegacyCharacterSelectionKind { None, Direct, TwoPage, ConfirmedPair, ResourcePair, TabbedSix };
struct LegacyCharacterSelectionWidget final
{
    int kind;
    GuiStaticDescriptorNative24h common;
    std::array<const char*,7> checked;
    int hitSlot, childId;
};
struct LegacyCharacterSelectionView final
{
    int id;
    std::span<const LegacyCharacterSelectionWidget> widgets;
};
struct LegacyCharacterSelectionChoice final { int view, event, result; };
struct LegacyCharacterSelectionCommand final { int event; const char* command; };
struct LegacyCharacterSelectionRule final { int event; const char* variable; int threshold; };
struct LegacyCharacterSelectionAnimation final
{
    const char* variable;
    LegacyCharacterSelectionWidget button;
    const char* commands;
    const char* pictures;
};
struct LegacyCharacterSelectionHover final { const char* variable; int event, overlay; };
struct LegacyCharacterSelectionEffect final { int value, icon, background; };
struct LegacyCharacterSelectionNavigation final
{ int view, input; std::span<const GuiDialogNavigationMapping> pairs; };
struct LegacyCharacterSelectionHotkey final { int view, input, event; };
struct LegacyCharacterSelectionTab final
{
    const char* route;
    const char* location;
    const char* eventType;
    int tab, background, select, canvas;
};
struct LegacyCharacterSelectionProfile final
{
    LegacyCharacterSelectionKind kind;
    const char* modeVariable;
    int modeCount;
    const char* optionVariable;
    int optionCount;
    // view: 0..为内容页，-1系统栏，-2选择确认，100..为08附加资源组。
    std::span<const LegacyCharacterSelectionView> views;
    std::span<const LegacyCharacterSelectionChoice> choices;
    std::span<const LegacyCharacterSelectionCommand> commands;
    std::span<const LegacyCharacterSelectionRule> disableRules;
    std::span<const int> disabledEvents;
    int autoEvent, skipEvent;
    const char* resultVariable;
    std::span<const char* const> bgm;
    const char* selectionCue;
    int confirmationMode;
    const char* confirmationFallback;
    const char* costVariable;
    std::span<const int> costs;
    std::span<const LegacyCharacterSelectionAnimation> animations;
    std::span<const POINT> animationPositions;
    std::span<const LegacyCharacterSelectionHover> hoverRows;
    std::span<const LegacyCharacterSelectionEffect> effects;
    std::span<const LegacyCharacterSelectionTab> tabs;
    std::span<const char* const> portraits;
    const char* otherLocation;
    const char* dateBase;
    std::span<const char* const> dateDigits;
    std::span<const POINT> datePositions;
    std::span<const char* const> timeImages;
    std::span<const LegacyCharacterSelectionNavigation> navigation;
    std::span<const LegacyCharacterSelectionHotkey> hotkeys;
};
const LegacyCharacterSelectionProfile& NoLegacyCharacterSelectionProfile() noexcept;
void ValidateLegacyCharacterSelectionProfile(const LegacyCharacterSelectionProfile&);
const char* LegacyCharacterSelectionSceneName() noexcept;
}
