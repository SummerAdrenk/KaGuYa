#pragma once

#include <Windows.h>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace KaGuYa::Start::Runtime
{
struct GraphicsSurfaceNativeBody;
struct LayerListNativeBody18h;
struct LayerItemNativeBody14h;

// 5B1F0C只有三个槽；其后5B1F18是RTTI，不是基类的第四个方法。
struct LayerItemNativeVtable final
{
    void (__thiscall* drawAt00)(LayerItemNativeBody14h*, const RECT*);
    void (__thiscall* compositeAt04)(LayerItemNativeBody14h*, GraphicsSurfaceNativeBody*, const RECT*);
    LayerItemNativeBody14h* (__thiscall* deletingDestructorAt08)(LayerItemNativeBody14h*, std::uint32_t);
};

// 仅供已证明具有窗口扩展槽的派生层；不能把任意基类表当成此表。
// 479DB0的RECT返回使用显式隐藏输出指针，返回该指针，ret 4。
struct LayerItemNativeFrameVtable final
{
    LayerItemNativeVtable base;
    RECT* (__thiscall* boundsAt0C)(LayerItemNativeBody14h*, RECT*);
    std::uint8_t (__thiscall* requestAt10)(LayerItemNativeBody14h*, std::uint8_t, std::int32_t);
    void (__thiscall* advanceAt14)(LayerItemNativeBody14h*);
    void (__thiscall* resetAt18)(LayerItemNativeBody14h*);

    [[nodiscard]] const LayerItemNativeVtable* AsBase() const noexcept { return &base; }
};

struct LayerItemNativeBody14h final
{
    const LayerItemNativeVtable* vptrAt00;
    std::uint32_t valueAt04;
    std::uint32_t valueAt08;
    std::uint8_t enabledAt0C;
    std::uint8_t unwrittenAt0D[3];
    LayerListNativeBody18h* parentAt10;
};
static_assert(sizeof(LayerItemNativeVtable) == 0x0C);
static_assert(offsetof(LayerItemNativeVtable, deletingDestructorAt08) == 8);
static_assert(sizeof(LayerItemNativeFrameVtable) == 0x1C);
static_assert(offsetof(LayerItemNativeFrameVtable, base) == 0);
static_assert(offsetof(LayerItemNativeFrameVtable, boundsAt0C) == 0x0C);
static_assert(offsetof(LayerItemNativeFrameVtable, requestAt10) == 0x10);
static_assert(offsetof(LayerItemNativeFrameVtable, advanceAt14) == 0x14);
static_assert(offsetof(LayerItemNativeFrameVtable, resetAt18) == 0x18);
static_assert(sizeof(LayerItemNativeBody14h) == 0x14 && alignof(LayerItemNativeBody14h) == 4);
static_assert(offsetof(LayerItemNativeBody14h, valueAt04) == 4);
static_assert(offsetof(LayerItemNativeBody14h, valueAt08) == 8);
static_assert(offsetof(LayerItemNativeBody14h, enabledAt0C) == 0x0C);
static_assert(offsetof(LayerItemNativeBody14h, parentAt10) == 0x10);
static_assert(std::is_standard_layout_v<LayerItemNativeBody14h>);
static_assert(std::is_trivially_destructible_v<LayerItemNativeBody14h>);

[[nodiscard]] const LayerItemNativeVtable& LayerItemNativeMethods() noexcept;
LayerItemNativeBody14h* ConstructLayerItemNativeAt441270(
    LayerItemNativeBody14h& body, LayerListNativeBody18h* parent);
void DestroyLayerItemNativeAt441310(LayerItemNativeBody14h& body) noexcept;
LayerItemNativeBody14h* DeleteLayerItemNativeAt441570(
    LayerItemNativeBody14h& body, std::uint32_t flags) noexcept;
// 492E60所需动态入口；本模块不创建或拥有ILayerItem引用控制块。
LayerItemNativeBody14h* DeleteLayerItemNativeDynamic(
    LayerItemNativeBody14h* body, std::uint32_t flags);
}
