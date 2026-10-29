#pragma once

#include <Runtime/LayerWindowNativeBase.h>
#include <Runtime/GuiControlResourceNative.h>
#include <Runtime/GraphicsSurfaceRuntime.h>
#include <array>

namespace KaGuYa::Start::Runtime
{
struct LoveHolicPercentageGaugeProfile final
{
    const char* background;
    const char* bar;
    std::array<const char*, 10> digits;
    POINT unitsPosition;
};

struct LoveHolicMasoGaugeProfile final
{
    const char* background;
    const char* off;
    const char* on;
    std::array<POINT, 5> positions;
    const char* valueName;
    const char* displayName;
};

struct LoveHolicGaugesResourceProfile final
{
    LoveHolicPercentageGaugeProfile percentage;
    LoveHolicMasoGaugeProfile maso;
};

class ILoveHolicGaugeStateSource
{
public:
    virtual ~ILoveHolicGaugeStateSource() = default;
    // name保持原CP932字节；主线提供实时读取源，组件不缓存Root或命名值表。
    virtual std::int32_t ReadGaugeValue(const char* name) = 0;
};

struct LoveHolicGaugesNativeServices final
{
    IGuiControlResourceLoadServices& resources;
    ILoveHolicGaugeStateSource& state;
    IGraphicsSurfaceApi& surfaces;
    IGraphicsWindowSurfaceApi& drawing;
    IRuntimeStringManagerSource& strings;
    const LoveHolicGaugesResourceProfile& profile;
};

struct LoveHolicGaugeStorage;
// 只共享现代图层接口；不是原Percentage 50h/Maso 44h对象的内存镜像。
struct LoveHolicGaugeNativeBody final
{
    LayerWindowNativeBody24h window;
    LoveHolicGaugesNativeServices* services;
    LoveHolicGaugeStorage* storage;
};
static_assert(std::is_standard_layout_v<LoveHolicGaugeNativeBody>);
static_assert(offsetof(LoveHolicGaugeNativeBody, window) == 0);

// +00..+18使用共享Frame；原两表另有+1C局部清值，不等于+18隐藏重置。
struct LoveHolicGaugeNativeVtable final
{
    LayerItemNativeFrameVtable frame;
    void (__thiscall* clearValueAt1C)(LayerItemNativeBody14h*);
};
static_assert(offsetof(LoveHolicGaugeNativeVtable, clearValueAt1C) == 0x1C);

LoveHolicGaugeNativeBody* ConstructLoveHolicPercentageGaugeNative(
    LoveHolicGaugeNativeBody&, LayerListNativeBody18h*, LoveHolicGaugesNativeServices&);
LoveHolicGaugeNativeBody* ConstructLoveHolicMasoGaugeNative(
    LoveHolicGaugeNativeBody&, LayerListNativeBody18h*, LoveHolicGaugesNativeServices&);
LoveHolicGaugeNativeBody* DeleteLoveHolicGaugeNative(LoveHolicGaugeNativeBody&, std::uint32_t flags) noexcept;
RECT ReadLoveHolicGaugeBounds(const LoveHolicGaugeNativeBody&) noexcept;
std::int32_t ReadLoveHolicGaugePoint(const LoveHolicGaugeNativeBody&) noexcept;
void UpdateLoveHolicGaugeNative(LoveHolicGaugeNativeBody&);
std::uint8_t RequestLoveHolicGaugeNative(LoveHolicGaugeNativeBody&, std::uint8_t requested, std::int32_t mode);
void InvalidateLoveHolicGaugeNative(LoveHolicGaugeNativeBody&);
void DrawLoveHolicGaugeNative(LoveHolicGaugeNativeBody&, const RECT*);
void CompositeLoveHolicGaugeNative(LoveHolicGaugeNativeBody&, GraphicsSurfaceNativeBody*, const RECT*);
void ClearLoveHolicGaugeValueNative(LoveHolicGaugeNativeBody&) noexcept;
void ResetLoveHolicGaugeNative(LoveHolicGaugeNativeBody&) noexcept;
}
