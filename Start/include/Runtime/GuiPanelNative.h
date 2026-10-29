#pragma once

#include <Runtime/GuiControlFactoryNative.h>
#include <Runtime/GuiNavigationNative.h>
#include <span>

namespace KaGuYa::Start::Runtime
{
struct GuiDialogNativeBody5Ch;
using GuiPanelIdStorage = EnvironmentVectorStorage<std::int32_t>;

// 仅建立存储：Static先构造00..4F，成功之后才写Panel尾部50..6B。
struct GuiPanelNativeBody6Ch final
{
    union { GuiControlNativeBody50h common; };
    GuiPanelIdStorage idsAt50;
    GuiNavigationReferences navigationAt5C;
    std::int32_t selectedAt68;
    GuiPanelNativeBody6Ch() noexcept {}
};
struct GuiPanelNativeVtable26 final
{
    GuiControlNativeVtable23 common;
    void (__thiscall* addIdAt5C)(GuiControlNativeBody50h*, std::int32_t);
    void (__thiscall* addNavigationAt60)(GuiControlNativeBody50h*, YtlReferenceSlotRuntime);
    void (__thiscall* selectAt64)(GuiControlNativeBody50h*, std::int32_t);
};
static_assert(sizeof(GuiPanelNativeBody6Ch) == 0x6C);
static_assert(alignof(GuiPanelNativeBody6Ch) == 4);
static_assert(std::is_standard_layout_v<GuiPanelNativeBody6Ch>);
static_assert(std::is_trivially_destructible_v<GuiPanelNativeBody6Ch>);
static_assert(offsetof(GuiPanelNativeBody6Ch, common) == 0x00);
static_assert(offsetof(GuiPanelNativeBody6Ch, idsAt50) == 0x50);
static_assert(offsetof(GuiPanelNativeBody6Ch, navigationAt5C) == 0x5C);
static_assert(offsetof(GuiPanelNativeBody6Ch, selectedAt68) == 0x68);
static_assert(sizeof(GuiPanelNativeVtable26) == 0x68);
static_assert(offsetof(GuiPanelNativeVtable26, addIdAt5C) == 0x5C);
static_assert(offsetof(GuiPanelNativeVtable26, addNavigationAt60) == 0x60);
static_assert(offsetof(GuiPanelNativeVtable26, selectAt64) == 0x64);

const GuiPanelNativeVtable26& GuiPanelVtable() noexcept;
std::span<const std::uint32_t, 26> GuiPanelOriginalVtableSample18() noexcept;

// 504F20..504FC0是内联构造，并非独立原函数。
// ownedGroup对应额外var_28 Ref，Static接收第二份已增持复制。
GuiPanelNativeBody6Ch* ConstructGuiPanelInlineAt504F20(void* storage, void* parent,
    std::int32_t event, YtlReferenceSlotRuntime ownedGroup);
GuiPanelNativeBody6Ch* DeleteGuiPanelAt4FDA10(GuiPanelNativeBody6Ch*, std::uint32_t flags) noexcept;
// fresh输出不释放旧位型；new18失败保留已经发布的完整正文。
YtlReferenceSlotRuntime* ConstructGuiPanelReferenceAt504FC4(YtlReferenceSlotRuntime& output,
    GuiPanelNativeBody6Ch* body);
YtlReferenceSlotRuntime* CreateGuiPanelAt504E90(YtlReferenceSlotRuntime& output,
    void* parent, const GuiStaticDescriptorNative24h& descriptor,
    IGuiControlResourceLoadServices& loader, IGraphicsSurfaceApi& surfaces,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
// 直接供INameRegistrationDialogNativeSource::CreatePanelAt504E90转交。
YtlReferenceSlotRuntime* CreateGuiPanelAt504E90(YtlReferenceSlotRuntime* freshOutput,
    GuiDialogNativeBody5Ch* parent, const GuiStaticDescriptorNative24h* descriptor,
    IGuiControlResourceLoadServices& loader, IGraphicsSurfaceApi& surfaces,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());

void AddGuiPanelIdAt4FF0E0(GuiPanelNativeBody6Ch&, std::int32_t id);
// 原491040分配叶：合法上限3FFFFFFF，超限/大块加23溢出均请求FFFFFFFF。
std::int32_t* AllocateGuiPanelIdsAt491040(std::uint32_t capacity);
// caller转交一份已有计数，正常尾不清参数位型。
void AddGuiPanelNavigationAt4FF290(GuiPanelNativeBody6Ch&, YtlReferenceSlotRuntime ownedNavigation);
void SelectGuiPanelAt535B10(GuiPanelNativeBody6Ch&, std::int32_t id);
void WriteGuiPanelStateAt536CE0(GuiPanelNativeBody6Ch&, std::int32_t value);
void WriteGuiPanelEnabledAt538670(GuiPanelNativeBody6Ch&, std::int32_t value);
std::int32_t KeyboardGuiPanelAt51FA60(GuiPanelNativeBody6Ch&);
}
