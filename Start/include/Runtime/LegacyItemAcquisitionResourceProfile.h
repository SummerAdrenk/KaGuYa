#pragma once
#include <Runtime/GuiControlFactoryNative.h>
#include <Runtime/GuiNavigationNative.h>
#include <array>
#include <span>
namespace KaGuYa::Start::Runtime
{
struct LegacyItemAcquisitionCheck final
{
    int event;
    std::array<const char*,7> normal,checked;
    const POINT* position;
    int hit,child;
};
struct LegacyItemAcquisitionProduct final { const char* variable; int cost,event; };
struct LegacyItemAcquisitionRoute final { int event; const char* command; };
struct LegacyItemAcquisitionNavigation final { int action; const GuiDialogNavigationMapping* pairs; };
struct LegacyItemAcquisitionResourceProfile final
{
    std::array<GuiStaticDescriptorNative24h,2> backgrounds;
    GuiButtonDescriptorNative2Ch close;
    std::span<const GuiButtonDescriptorNative2Ch> items,systemBefore,systemAfter;
    std::span<const LegacyItemAcquisitionCheck> checks;
    std::span<const LegacyItemAcquisitionNavigation> navigation;
    std::span<const LegacyItemAcquisitionProduct> products;
    std::span<const LegacyItemAcquisitionRoute> routes;
    const char* balance;
    const char* spent;
    const char* bgm;
    const char* selectionCue;
    const char* black;
};
const LegacyItemAcquisitionResourceProfile& RequireLegacyItemAcquisitionProfile(const LegacyItemAcquisitionResourceProfile*);
}
