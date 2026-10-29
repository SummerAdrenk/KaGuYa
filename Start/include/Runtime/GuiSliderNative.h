#pragma once

#include <Runtime/GuiControlNative.h>

#include <array>

namespace KaGuYa::Start::Runtime
{
// 5BE170：前23槽沿用Control ABI，后8槽属于Slider，不能解释为CheckBox扩展。
struct GuiSliderNativeVtable31 final
{
    GuiControlNativeVtable23 common;
    void (__thiscall* setRangeAt5C)(GuiControlNativeBody50h*, std::int32_t, std::int32_t);
    std::int32_t (__thiscall* valueAt60)(GuiControlNativeBody50h*);
    void (__thiscall* setValueAt64)(GuiControlNativeBody50h*, std::int32_t);
    void (__thiscall* setStepAt68)(GuiControlNativeBody50h*, std::int32_t);
    std::int32_t (__thiscall* changedAt6C)(GuiControlNativeBody50h*);
    std::int32_t (__thiscall* reverseAt70)(GuiControlNativeBody50h*);
    void (__thiscall* setStepInputAt74)(GuiControlNativeBody50h*, std::int32_t);
    void (__thiscall* setHitSlotAt78)(GuiControlNativeBody50h*, std::int32_t);
};

// 4F3C00：唯一D8h正文。服务只借用外部scope，B0/CC构造时不写。
struct GuiSliderNativeBodyD8h final
{
    union { GuiControlNativeBody50h common; };
    std::int32_t orientationAt50;
    std::int32_t reverseAt54;
    std::int32_t stepInputAt58;
    POINT interactionOffsetAt5C;
    RECT interactionRectAt64;
    POINT travelOffsetAt74;
    RECT travelRectAt7C;
    std::int32_t draggingAt8C;
    std::int32_t hoveredAt90;
    std::int32_t hitSlotAt94;
    POINT thumbPositionAt98;
    std::uint32_t zeroAtA0[4];
    std::int32_t changedAtB0;
    std::int32_t valueAtB4;
    std::int32_t minimumAtB8;
    std::int32_t maximumAtBC;
    std::int32_t stepAtC0;
    POINT trackOffsetAtC4;
    std::int32_t pressedAtCC;
    POINT dragOffsetAtD0;

    // common内部Ref有默认初始化；union阻止其在4F3C00原写点之前清字段。
    GuiSliderNativeBodyD8h() noexcept {}
};

// 5053B5的栈配置：两个拥有Ref由调用方管理，4F3C00借用后复制增持。
struct GuiSliderConstructionNative40h final
{
    YtlReferenceSlotRuntime resourcesAt00[2];
    std::int32_t orientationAt18;
    std::int32_t reverseAt1C;
    RECT interactionRectAt20;
    RECT travelDeltaAt30;
};

// 607008/5052C0：保留null与非null空串，七槽组不能改为现代资源owner。
struct GuiSliderDescriptorNative6Ch final
{
    std::int32_t eventAt00;
    std::array<const char*, 7> thumbNamesAt04;
    std::array<const char*, 7> trackNamesAt20;
    const POINT* positionAt3C;
    std::int32_t hitSlotAt40;
    std::int32_t orientationAt44;
    std::int32_t reverseAt48;
    RECT interactionRectAt4C;
    RECT travelDeltaAt5C;
};

static_assert(sizeof(GuiSliderNativeVtable31) == 0x7C);
static_assert(offsetof(GuiSliderNativeVtable31, setRangeAt5C) == 0x5C);
static_assert(offsetof(GuiSliderNativeVtable31, setStepAt68) == 0x68);
static_assert(offsetof(GuiSliderNativeVtable31, setHitSlotAt78) == 0x78);
static_assert(sizeof(GuiSliderNativeBodyD8h) == 0xD8);
static_assert(alignof(GuiSliderNativeBodyD8h) == 4);
static_assert(std::is_standard_layout_v<GuiSliderNativeBodyD8h>);
static_assert(std::is_trivially_destructible_v<GuiSliderNativeBodyD8h>);
static_assert(offsetof(GuiSliderNativeBodyD8h, common) == 0);
static_assert(offsetof(GuiSliderNativeBodyD8h, orientationAt50) == 0x50);
static_assert(offsetof(GuiSliderNativeBodyD8h, interactionRectAt64) == 0x64);
static_assert(offsetof(GuiSliderNativeBodyD8h, travelRectAt7C) == 0x7C);
static_assert(offsetof(GuiSliderNativeBodyD8h, thumbPositionAt98) == 0x98);
static_assert(offsetof(GuiSliderNativeBodyD8h, changedAtB0) == 0xB0);
static_assert(offsetof(GuiSliderNativeBodyD8h, valueAtB4) == 0xB4);
static_assert(offsetof(GuiSliderNativeBodyD8h, pressedAtCC) == 0xCC);
static_assert(offsetof(GuiSliderNativeBodyD8h, dragOffsetAtD0) == 0xD0);
static_assert(sizeof(GuiSliderConstructionNative40h) == 0x40);
static_assert(offsetof(GuiSliderConstructionNative40h, interactionRectAt20) == 0x20);
static_assert(sizeof(GuiSliderDescriptorNative6Ch) == 0x6C);
static_assert(offsetof(GuiSliderDescriptorNative6Ch, positionAt3C) == 0x3C);
static_assert(offsetof(GuiSliderDescriptorNative6Ch, interactionRectAt4C) == 0x4C);

const GuiSliderNativeVtable31& GuiSliderVtable() noexcept;
GuiSliderNativeBodyD8h* ConstructGuiSliderAt4F3C00(GuiSliderNativeBodyD8h* body,
    void* parent, std::int32_t event, const GuiSliderConstructionNative40h& configuration);
void DestroyGuiSliderAt4F79A0(GuiSliderNativeBodyD8h* body) noexcept;
GuiSliderNativeBodyD8h* DeleteGuiSliderAt4FE4B0(GuiSliderNativeBodyD8h* body,
    std::uint32_t flags) noexcept;

// 505440：完成raw正文后才清输出/发布base，control失败不补删D8h正文。
YtlReferenceSlotRuntime* ConstructGuiSliderReferenceAt505440(YtlReferenceSlotRuntime& output,
    GuiSliderNativeBodyD8h* body);
GuiSliderNativeBodyD8h* ResolveGuiSlider(const YtlReferenceSlotRuntime& reference) noexcept;

// fresh输出；父Dialog借用。所需Graphics/Input来源沿用已有原生scope，不进入正文。
// 返回后调用方使用ReleaseGuiControlReferenceAt4F6460消费这份拥有Ref。
YtlReferenceSlotRuntime* CreateGuiSliderAt5052C0(YtlReferenceSlotRuntime& output,
    void* parent, const GuiSliderDescriptorNative6Ch& descriptor,
    IGuiControlResourceLoadServices& loader, IGraphicsSurfaceApi& surfaces,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
}
