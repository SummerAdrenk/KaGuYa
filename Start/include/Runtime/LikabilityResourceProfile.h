#pragma once
#include <Runtime/GuiControlFactoryNative.h>
#include <Runtime/GuiNavigationNative.h>
#include <span>
#include <variant>

namespace KaGuYa::Start::Runtime
{
using LikabilityNames = std::span<const char* const>;
using LikabilityPoints = std::span<const POINT>;
using LikabilityIntegers = std::span<const std::int32_t>;

struct LikabilityHighestData final
{
    LikabilityNames variables, portraits, digits;
    const char* appearance;
    const char* hiddenPortrait;
    LikabilityPoints digitsAt;
};
struct LikabilityRepeatedRow final { const char* variable; const char* image; LikabilityPoints positions; };
struct LikabilityRepeatedData final { std::span<const LikabilityRepeatedRow> rows; };
struct LikabilityAnimatedData final
{
    std::span<const LikabilityRepeatedRow> hearts;
    const char* obtained; const char* used;
    const char* off; const char* on;
    LikabilityPoints clocks;
    LikabilityNames animation; // 02: ANM主成员，后四项分别是PLT辅助成员。
};
struct LikabilityRankData final
{
    LikabilityNames variables, ranks, routes, marks;
    LikabilityPoints rankOffsets, routeOffset, loveAt, lustAt;
    LikabilityIntegers thresholds, rankIndices;
};
struct LikabilityIndexedData final
{
    LikabilityNames variables, ranks, remainders, affectionVariables, affection;
};
struct LikabilityGaugeRow final { const char* variable; POINT barAt; POINT numberAt; };
struct LikabilityControlCondition final { std::int32_t event; const char* variable; bool nonnegative = false; };
struct LikabilityLoveHolicData final
{
    const char* gauge;
    LikabilityNames digits, smallDigits;
    LikabilityIntegers advances, smallAdvances;
    std::span<const LikabilityGaugeRow> first, second;
    const char* releaseFlag; const char* extraValue; POINT extraAt;
    std::span<const LikabilityControlCondition> items;
    LikabilityIntegers hoverIds, descriptionIds;
};
struct LikabilityFamilyRow final
{
    const char* affection; const char* maternal; const char* third;
    LikabilityPoints affectionAt, maternalAt, thirdAt;
};
struct LikabilityFamilyData final { std::span<const LikabilityFamilyRow> rows; LikabilityNames icons; };
struct LikabilityHeartPairData final { LikabilityNames variables, icons; LikabilityPoints positions; };
struct LikabilitySisterData final { LikabilityNames variables, icons; LikabilityPoints affectionAt, lustAt, haremAt; };
struct LikabilityHalfData final { LikabilityNames variables, icons; LikabilityPoints affectionAt, lustAt, possessionAt; };
struct LikabilityBadgeData final { LikabilityNames variables, icons; LikabilityPoints affectionAt; };
struct LikabilityCausalityData final { LikabilityNames variables, icons, digits; LikabilityPoints digitsAt, invasionAt; };
struct LikabilityNurseData final
{
    LikabilityNames variables, icons, digits;
    LikabilityIntegers limits;
    LikabilityPoints digitsAt, specialDigitsAt, upAt, extraAt;
};
struct LikabilityNumericRecord final { std::int32_t variable, alphabet, position; };
struct LikabilityBitRecord final { std::int32_t page, variable, image, count; };
struct LikabilityBitData final
{
    LikabilityNames variables, icons, digits;
    LikabilityPoints digitsAt;
    std::span<const LikabilityNumericRecord> numbers;
    std::span<const LikabilityBitRecord> bits;
    const char* unlock;
};
struct LikabilityBaseline18Data final { LikabilityNames variables, digits; LikabilityPoints positions, extraPositions; };
struct LikabilityResidualData final { const char* reason; const char* displayFlag; };
using LikabilityPaintData = std::variant<LikabilityHighestData, LikabilityRepeatedData,
    LikabilityAnimatedData, LikabilityRankData, LikabilityIndexedData, LikabilityLoveHolicData,
    LikabilityFamilyData, LikabilityHeartPairData, LikabilitySisterData, LikabilityHalfData,
    LikabilityBadgeData, LikabilityCausalityData, LikabilityNurseData, LikabilityBitData,
    LikabilityBaseline18Data, LikabilityResidualData>;

struct LikabilityControlProfile final
{
    GuiButtonDescriptorNative2Ch descriptor;
    bool button;
    std::int32_t page = -1; // -1为公共控件；08原各子页ID互不重叠，保留原ID。
    std::int32_t constructorVariant = -1; // 17: 0未解锁，1已解锁。
    bool canvasBackground = false; // 08静态页底由同一画布承载，避免遮住后画的数字。
};
struct LikabilityPageVisual final { std::int32_t event, page, variant; };
struct LikabilityNavigationProfile final
{
    std::int32_t page, action;
    std::span<const GuiDialogNavigationMapping> mappings; // 包含原负source终止项。
};
enum class LikabilityCurrentTab : std::uint8_t { Disable, Hide, Unchanged };
struct LikabilityFlowProfile final
{
    std::int32_t pages, close;
    LikabilityIntegers tabs;
    bool cancelInput, readRememberedPage, storePageOnClose;
    const char* forcedPage;
    LikabilityCurrentTab currentTab;
    std::int32_t next = -1, previous = -1, toggle = -1;
    std::span<const LikabilityPageVisual> visuals;
    std::span<const LikabilityControlCondition> conditions;
    const char* constructorVariant = nullptr;
    bool keepForcedCurrentTab = false; // 10指定页仅隐藏其它页签，当前页签仍可见但禁用。
};
struct LikabilityResourceProfile final
{
    std::int32_t number;
    const char* slug;
    LikabilityNames backgrounds, pages;
    std::span<const LikabilityControlProfile> controls;
    LikabilityFlowProfile flow;
    LikabilityPaintData paint;
    std::uint32_t constructorVa, refreshVa;
    std::span<const LikabilityNavigationProfile> navigation{};
};
const LikabilityResourceProfile& GameLikabilityResourceProfile();
// 动态profile查询可抛；不以noexcept把缺配置变成终止进程。
void RequireUsableLikabilityProfile(const LikabilityResourceProfile&);

struct LikabilityImageMetrics final { POINT origin; SIZE size; };
// 纯业务绘制接口供真实GUI与定向记录器共用；坐标是原屏幕坐标，适配器负责画布原点。
class ILikabilityPaintTarget
{
public:
    virtual ~ILikabilityPaintTarget() = default;
    virtual std::int32_t Read(const char*) = 0;
    virtual LikabilityImageMetrics Metrics(const char*) = 0;
    virtual void Draw(const char*, POINT, const RECT* source = nullptr, const RECT* clip = nullptr) = 0;
    virtual void Visible(std::int32_t event, bool) = 0;
};
void PaintLikabilityPage(const LikabilityResourceProfile&, std::int32_t page, ILikabilityPaintTarget&);
}
