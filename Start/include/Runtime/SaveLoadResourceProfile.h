#pragma once
#include <Runtime/GuiControlFactoryNative.h>
#include <Runtime/GuiDialogNative.h>
#include <Runtime/GuiSliderNative.h>
#include <array>
#include <span>

namespace KaGuYa::Start::Runtime
{
enum class SaveLoadCatalogKind { FixedDense, ExistingPhysicalPages };
enum class SaveLoadPreviewKind { SourceCrop, ShrinkIfOversized };
enum class SaveLoadLabelKind { SharedHover, PerSlot };
enum class SaveLoadConfirmationKind { Text, SingleThemeDigits, PairedDigits, PairedDigitsWithSeparators };
enum class SaveLoadEventScope { System, Page, Popup };
enum class SaveLoadGlyphValue { Slot, Year, Month, Day, Hour, Minute, Literal };
// 共享现代动作号；旧按钮索引在构造边界转换，不修改原父phase或确认3至6。
inline constexpr int SaveLoadCloseApplicationEvent = 1000;
inline constexpr int SaveLoadAddPageEvent = 1001;
inline constexpr int SaveLoadSelectTabFirstEvent = 1100;
inline constexpr int SaveLoadDeleteTabFirstEvent = 1200;

struct SaveLoadCatalogPolicy final
{
    SaveLoadCatalogKind kind;
    int slotsPerPage;
    int physicalPageLimit;
    int visiblePageTabs;
    bool rollingLatestFromFileTime;
    bool rollingNewestFirst;
    const char* regularPageMatchPattern;
};
struct SaveLoadTextStyle final
{
    const char* face;
    int height, weight;
    unsigned char characterSpacing, lineSpacing, renderMode;
    std::array<COLORREF, 2> colors; // 保存、读取；字体名为空串与空指针不混用。
    POINT origin;
    int maxCharacters, maxLines;
};
struct SaveLoadNavigation final
{
    int input;
    std::span<const GuiDialogNavigationMapping> pairs; // 包含原终止项。
};
struct SaveLoadEventMapping final
{
    SaveLoadEventScope scope;
    int originalEvent;
    int consumerEvent;
};
struct SaveLoadControlSet final
{
    std::span<const GuiStaticDescriptorNative24h> statics;
    std::span<const GuiButtonDescriptorNative2Ch> buttons;
    std::span<const SaveLoadNavigation> navigation;
};
struct SaveLoadPreviewPolicy final
{
    SaveLoadPreviewKind kind;
    POINT origin;
    SIZE extent;
};
struct SaveLoadGlyphPlacement final
{
    SaveLoadGlyphValue value;
    int divisor, modulus, resourceOffset;
    POINT position;
};
enum class SaveLoadSlotLayerOrder
{
    NumberDateContentLabelNew,
    ContentMarkerNumberDate,
    ContentNumberDate
};
struct SaveLoadSlotResources final
{
    std::span<const char* const> numbers, dateDigits;
    std::array<const char*, 2> noData, noPreview, newMarker;
    std::span<const POINT> positions;
    std::span<const SaveLoadGlyphPlacement> numberGlyphs, dateGlyphs;
    std::array<GuiButtonDescriptorNative2Ch, 2> fileButtons, deleteButtons;
    // 旧版槽号表从数字1开始选取；stride不等于每页槽数。
    int numberThemeStride, dateThemeStride;
    POINT deleteOffset, newOrigin, noDataOrigin;
    SaveLoadPreviewPolicy preview;
    SaveLoadLabelKind labelKind;
    SaveLoadTextStyle labelStyle;
    RECT hoverBounds;
    SaveLoadSlotLayerOrder layerOrder{SaveLoadSlotLayerOrder::NumberDateContentLabelNew};
};
struct SaveLoadPageResources final
{
    // 顺序固定为保存、普通读取、Quick、Auto，不沿用旧原模式编号。
    std::array<GuiStaticDescriptorNative24h, 4> backgrounds;
    std::array<SaveLoadControlSet, 2> system, page;
    std::span<const char* const> digits;
    std::span<const POINT> digitPositions;
    std::span<const POINT> tabPositions;
    std::array<GuiButtonDescriptorNative2Ch, 2> tabButtons, tabDeleteButtons;
    int digitStates;
    SaveLoadSlotResources slots;
};
struct SaveLoadPopupCheckResource final
{
    int event;
    std::array<const char*, 7> normal, checked;
    const POINT* position;
    int hitSlot, childId;
};
struct SaveLoadPopupTheme final
{
    GuiStaticDescriptorNative24h background;
    const char* bar;
    const GuiSliderDescriptorNative6Ch* slider;
    std::array<GuiButtonDescriptorNative2Ch, 2> arrows;
    SaveLoadPopupCheckResource checkbox;
    std::span<const char* const> digits, markers;
};
struct SaveLoadPopupResources final
{
    std::array<SaveLoadPopupTheme, 2> themes;
    std::span<const SaveLoadNavigation> navigation;
    std::span<const POINT> digitPositions;
    POINT barOrigin, firstRowOrigin, latestOrigin, usedOrigin;
    int visibleRows, digitStates;
    // 旧版分段框复用现代Popup；themes.markers在此策略下依次为上/中/下背景。
    bool tiledFrame;
    SIZE blankRowSize;
    int rowPitch, rowInset, sliderTravelLength;
};
struct SaveLoadConfirmationResources final
{
    SaveLoadConfirmationKind kind;
    SaveLoadControlSet controls;
    const char* mask;
    std::array<std::span<const char* const>, 4> digits; // 槽、页、日期、时间。
    std::span<const char* const> specialPages;
    std::array<std::span<const POINT>, 4> digitPositions;
    std::array<POINT, 2> specialPageOrigins;
    POINT previewAnchor;
    bool centerPreview;
    int pageLabelWidth, slotLabelWidth;
    std::array<SaveLoadTextStyle, 4> text; // 页、槽、时间、场景。
    std::array<std::array<int, 4>, 3> hiddenEvents;
};
struct SaveLoadDeleteResources final
{
    // 普通槽、普通整页、Quick、Auto各自绑定原确认内容；无数字不是无确认。
    SaveLoadControlSet controls;
    std::span<const char* const> digits;
    std::span<const POINT> digitPositions;
    std::array<std::span<const int>, 4> hiddenEvents;
    bool numbered;
    const char* pageLimitMessage;
};
struct SaveLoadResourceProfile final
{
    SaveLoadCatalogPolicy catalog;
    SaveLoadPageResources page;
    SaveLoadPopupResources popup;
    SaveLoadConfirmationResources confirmation;
    SaveLoadDeleteResources deletion;
    std::span<const SaveLoadEventMapping> events;
    std::array<int, 4> originalModes;
    bool closeApplicationButton;
    // 原Root实际生成/保存尺寸，不是槽页crop/shrink或确认框大小。
    SIZE thumbnailSize{};
};

// provider及所有引用均为静态存储；缺少绑定明确抛出，不回退18号。
const SaveLoadResourceProfile& CurrentSaveLoadResourceProfile();
// 仅在控件构造时转换原event；父phase和槽确认3至6永不经过此函数。
int SaveLoadConsumerEvent(SaveLoadEventScope scope, int originalEvent);
}
