#pragma once
#include <Runtime/GuiControlFactoryNative.h>
#include <Runtime/GuiDialogNative.h>
#include <Runtime/ModeSelectButtonNative.h>
#include <array>
#include <span>

namespace KaGuYa::Start::Runtime
{
enum class CgOperationVisualEncoding { NativeSevenState, GuiSevenSlot };
enum class CgOperationPagePolicy { ClampedButtonsAndWheel, WrappingButtons };
enum class CgOperationDigitUse { Primary, PrimaryInactive, Popup, PopupUnavailable };
enum class CgOperationAction { Previous, OpenPopup, Next, Hide, Return };
using CgOperationDigitPoints = std::array<POINT,3>;
struct CgOperationNavigation final
{
    int input;
    std::span<const GuiDialogNavigationMapping> mappings; // 包含原(-1,0)终止项。
};
struct CgOperationResourceProfile final
{
    CgOperationVisualEncoding encoding;
    CgOperationPagePolicy pagePolicy;
    std::span<const CgUiButtonRecordNative28h> nativePrimary, nativeCells, nativePages;
    // Native组的两个frame是由原背景名生成的共享GUI装配，不冒称原PE存在GUI表。
    GuiStaticDescriptorNative24h primaryFrame, popupFrame;
    std::span<const GuiButtonDescriptorNative2Ch> primaryButtons, pageButtons;
    GuiButtonDescriptorNative2Ch popupCell;
    std::span<const char* const> digitResources;
    std::array<std::array<int,10>,4> digitSlots;
    CgOperationDigitPoints primaryPoints;
    std::span<const CgOperationDigitPoints> popupPoints;
    std::array<CgOperationNavigation,4> navigation;
    POINT cellOffset;
    int pageCapacity;
    const char* popupCellBackground; // 旧50格无此stamp，不以18资源兜底。
    bool unsignedPrimaryGuard;
    bool hideRequiresNotDragging;
};
const CgOperationResourceProfile& CurrentCgOperationResourceProfile();
void ValidateCgOperationResourceProfile(const CgOperationResourceProfile&);
const CgOperationResourceProfile& SharedNative50CgOperationProfile() noexcept;
const CgOperationResourceProfile& SharedEarly20CgOperationProfile() noexcept;
const CgOperationResourceProfile& SharedLate20CgOperationProfile() noexcept;
}
