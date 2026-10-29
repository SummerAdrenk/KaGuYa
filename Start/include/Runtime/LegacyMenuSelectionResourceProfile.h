#pragma once
#include <Runtime/GuiControlFactoryNative.h>
#include <Runtime/GuiNavigationNative.h>
#include <array>
#include <span>

namespace KaGuYa::Start::Runtime
{
struct LegacyMenuSelectionCheckDescriptor final
{
    int event;
    std::array<const char*, 7> normal, checked;
    const POINT* position;
    int hitSlot, childId;
};
struct LegacyMenuSelectionNavigation final { int input; const GuiDialogNavigationMapping* pairs; };
struct LegacyMenuSelectionRoute final { int event; const char* command; };
struct LegacyMenuSelectionResourceProfile final
{
    std::span<const GuiStaticDescriptorNative24h> statics;
    std::span<const GuiButtonDescriptorNative2Ch> choices;
    std::span<const GuiButtonDescriptorNative2Ch> systemBeforeChecks, systemAfterChecks;
    std::span<const LegacyMenuSelectionCheckDescriptor> systemChecks;
    std::span<const LegacyMenuSelectionNavigation> navigation;
    std::span<const LegacyMenuSelectionRoute> routes;
    std::array<const char*, 3> labelVariables, colorTags;
    const char* resultVariable;
    POINT labelOffset;
    int textLayerEvent;
};
// 仅由主线显式绑定12号；无默认18号替身。
const LegacyMenuSelectionResourceProfile& RequireLegacyMenuSelectionProfile(
    const LegacyMenuSelectionResourceProfile*);
}
