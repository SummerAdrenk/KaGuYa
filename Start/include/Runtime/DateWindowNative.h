#pragma once

#include <Runtime/LayerWindowNativeBase.h>
#include <Runtime/GuiControlResourceNative.h>
#include <Runtime/GraphicsSurfaceRuntime.h>
#include <Runtime/ScriptNamedValueNative.h>
#include <array>
#include <span>

namespace KaGuYa::Start::Runtime
{
class RootRuntimeOwner;
class IScrVmCommandErrorSink;

enum class DateWindowStrategy
{
    EroZemiDigits,
    ShigokareWholeDate,
    LoveHolicDays
};

// 只接受已确认的三种资源结构；保留Ero Zemi原六字段初始化，
// 整日图片策略使用末尾的独立资源表。
struct DateWindowResourceProfile final
{
    const char* background;
    std::span<const char* const> digits;
    std::span<const char* const> months;
    std::span<const char* const> weekdays;
    std::span<const char* const> times;
    std::span<const POINT> dayPositions; // 个位日期、两位日期的个位、十位。
    DateWindowStrategy strategy = DateWindowStrategy::EroZemiDigits;
    std::span<const char* const> wholeDays{};
};

class IDateWindowStateSource
{
public:
    virtual ~IDateWindowStateSource() = default;
    virtual std::int32_t ReadDateValue(const char* name) = 0;
};

class RootDateWindowStateSource final : public IDateWindowStateSource
{
public:
    RootDateWindowStateSource(RootRuntimeOwner&, ScriptNamedValueNativeTables,
        IScrVmCommandErrorSink&, IRuntimeStringManagerSource&) noexcept;
    std::int32_t ReadDateValue(const char* name) override;
private:
    RootRuntimeOwner& root_;
    ScriptNamedValueNativeTables tables_;
    IScrVmCommandErrorSink& errors_;
    IRuntimeStringManagerSource& strings_;
};

struct DateWindowNativeServices final
{
    IGuiControlResourceLoadServices& resources;
    IDateWindowStateSource& state;
    IGraphicsSurfaceApi& surfaces;
    IGraphicsWindowSurfaceApi& drawing;
    IRuntimeStringManagerSource& strings;
    const DateWindowResourceProfile& profile;
};

struct DateWindowValues final
{
    std::int32_t display = -1;
    std::int32_t month = -1;
    std::int32_t date = -1;
    std::int32_t weekday = -1;
    std::int32_t time = -1;
    std::int32_t days = -1; // LoveHolic script value, zero-based; not day-of-month.
};

struct DateWindowStorage;
// 只对接共享图层虚槽，不复刻原旧EXE的内部vector/allocator布局。
struct DateWindowNativeBody final
{
    LayerWindowNativeBody24h window;
    DateWindowNativeServices* services;
    DateWindowStorage* storage;
};
static_assert(std::is_standard_layout_v<DateWindowNativeBody>);
static_assert(offsetof(DateWindowNativeBody, window) == 0);

DateWindowNativeBody* ConstructDateWindowNative(DateWindowNativeBody&, LayerListNativeBody18h*,
    DateWindowNativeServices&);
DateWindowNativeBody* DeleteDateWindowNative(DateWindowNativeBody&, std::uint32_t flags) noexcept;
const DateWindowValues& ReadDateWindowValues(const DateWindowNativeBody&) noexcept;
void UpdateDateWindowNative(DateWindowNativeBody&);
std::uint8_t RequestDateWindowNative(DateWindowNativeBody&, std::uint8_t requested, std::int32_t mode);
void DrawDateWindowNative(DateWindowNativeBody&, const RECT*);
void CompositeDateWindowNative(DateWindowNativeBody&, GraphicsSurfaceNativeBody*, const RECT*);
void ResetDateWindowNative(DateWindowNativeBody&) noexcept;
}
