#pragma once

#include <Runtime/LayerWindowNativeBase.h>
#include <Runtime/GuiControlResourceNative.h>
#include <Runtime/GraphicsSurfaceRuntime.h>
#include <span>

namespace KaGuYa::Start::Runtime
{
// 07原468720：一张底图和55BE28起的50张计量帧；名字均为原CP932字节。
struct TokimekiGaugeResourceProfile final
{
    const char* background;
    std::span<const char* const, 50> frames;
};

class ITokimekiGaugeStateSource
{
public:
    virtual ~ITokimekiGaugeStateSource() = default;
    virtual std::int32_t ReadGaugeValue(const char* rawCp932Name) = 0;
};

// 所有服务均借用；装配方负责生命周期，组件不缓存Root或singleton。
struct TokimekiGaugeNativeServices final
{
    IGuiControlResourceLoadServices& resources;
    ITokimekiGaugeStateSource& state;
    IGraphicsSurfaceApi& surfaces;
    IGraphicsWindowSurfaceApi& drawing;
    IRuntimeStringManagerSource& strings;
    const TokimekiGaugeResourceProfile& profile;
};

struct TokimekiGaugeStorage;
struct TokimekiGaugeNativeBody final
{
    LayerWindowNativeBody24h window;
    TokimekiGaugeNativeServices* services;
    TokimekiGaugeStorage* storage;
};
static_assert(std::is_standard_layout_v<TokimekiGaugeNativeBody>);
static_assert(offsetof(TokimekiGaugeNativeBody, window) == 0);

// 52B5E0的前七槽复用共享Frame协议；第八槽只清计量帧，不隐藏/不标脏。
struct TokimekiGaugeNativeVtable final
{
    LayerItemNativeFrameVtable frame;
    std::int32_t (__thiscall* clearAt1C)(LayerItemNativeBody14h*);
};
static_assert(offsetof(TokimekiGaugeNativeVtable, frame) == 0);
static_assert(offsetof(TokimekiGaugeNativeVtable, clearAt1C) == 0x1C);
static_assert(sizeof(TokimekiGaugeNativeVtable) == 0x20);

const TokimekiGaugeNativeVtable& TokimekiGaugeNativeMethods() noexcept;
TokimekiGaugeNativeBody* ConstructTokimekiGaugeNative(TokimekiGaugeNativeBody&,
    LayerListNativeBody18h*, TokimekiGaugeNativeServices&);
TokimekiGaugeNativeBody* DeleteTokimekiGaugeNative(TokimekiGaugeNativeBody&, std::uint32_t flags) noexcept;
RECT ReadTokimekiGaugeBounds(const TokimekiGaugeNativeBody&) noexcept;
std::int32_t ReadTokimekiGaugeFrame(const TokimekiGaugeNativeBody&) noexcept;
void UpdateTokimekiGaugeNative(TokimekiGaugeNativeBody&);
std::uint8_t SetRequestedTokimekiGaugeNative(TokimekiGaugeNativeBody&, std::uint8_t requested,
    std::int32_t mode);
void InvalidateTokimekiGaugeNative(TokimekiGaugeNativeBody&);
void DrawTokimekiGaugeNative(TokimekiGaugeNativeBody&, const RECT*);
void CompositeTokimekiGaugeNative(TokimekiGaugeNativeBody&, GraphicsSurfaceNativeBody*, const RECT*);
std::int32_t ClearTokimekiGaugeNative(TokimekiGaugeNativeBody&) noexcept;
void ResetTokimekiGaugeNative(TokimekiGaugeNativeBody&) noexcept;
}
