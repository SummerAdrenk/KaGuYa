#pragma once

#include <Runtime/LayerItemNative.h>

namespace KaGuYa::Start::Runtime
{
// 5B2B88：ILayerWindow在14h基类之后只有RECT，无隐藏服务或owner。
struct LayerWindowNativeBody24h final
{
    LayerItemNativeBody14h layer;
    RECT boundsAt14;
};
static_assert(sizeof(LayerWindowNativeBody24h) == 0x24);
static_assert(offsetof(LayerWindowNativeBody24h, boundsAt14) == 0x14);
static_assert(std::is_standard_layout_v<LayerWindowNativeBody24h>);
static_assert(std::is_trivially_destructible_v<LayerWindowNativeBody24h>);

const LayerItemNativeFrameVtable& LayerWindowNativeBaseMethods() noexcept;
void DestroyLayerWindowNativeAt464910(LayerWindowNativeBody24h& body) noexcept;
LayerItemNativeBody14h* DeleteLayerWindowNativeAt469870(LayerWindowNativeBody24h& body,
    std::uint32_t flags) noexcept;
}
