#pragma once
#include <Runtime/SelectDialogNative.h>
#include <Runtime/LayerWindowNativeBase.h>
#include <Runtime/LayerListNative.h>
#include <Runtime/TextWindowNativeColors.h>

namespace KaGuYa::Start::Runtime
{
struct SelectWindowNativeBody6Ch final
{
    LayerWindowNativeBody24h window;
    std::uint32_t unwrittenAt24;
    std::uint32_t unwrittenAt28;
    union { YtlReferenceSlotRuntime canvasAt2C; };
    union { YtlReferenceSlotRuntime dialogAt38; };
    std::uint8_t savedByteAt44;
    std::uint8_t savedByteAt45;
    std::uint8_t unwrittenAt46[2];
    union { YtlReferenceSlotRuntime normalColorAt48; };
    union { YtlReferenceSlotRuntime selectedColorAt54; };
    union { YtlReferenceSlotRuntime shadowColorAt60; };
    SelectWindowNativeBody6Ch() noexcept {}
};
static_assert(sizeof(SelectWindowNativeBody6Ch) == 0x6C);
static_assert(offsetof(SelectWindowNativeBody6Ch, canvasAt2C) == 0x2C);
static_assert(offsetof(SelectWindowNativeBody6Ch, dialogAt38) == 0x38);
static_assert(offsetof(SelectWindowNativeBody6Ch, savedByteAt44) == 0x44);
static_assert(offsetof(SelectWindowNativeBody6Ch, normalColorAt48) == 0x48);
static_assert(offsetof(SelectWindowNativeBody6Ch, selectedColorAt54) == 0x54);
static_assert(offsetof(SelectWindowNativeBody6Ch, shadowColorAt60) == 0x60);
static_assert(std::is_standard_layout_v<SelectWindowNativeBody6Ch>);
static_assert(std::is_trivially_destructible_v<SelectWindowNativeBody6Ch>);
struct SelectWindowNativeServices final
{
    const SelectDialogNativeProfile& profile;
    ISelectDialogNativeSource& menu;
    IGuiControlResourceLoadServices& resources;
    ITextWindowNativeColorSource& colors;
    EnvironmentRootOwner& environment;
    IGraphicsSurfaceApi& surfaces;
    IGraphicsWindowSurfaceApi& drawing;
    IRuntimeStringManagerSource& strings;
    IEnvironmentRootMemory& memory;
};
class SelectWindowNativeScope final
{
public:
    explicit SelectWindowNativeScope(SelectWindowNativeServices&) noexcept;
    ~SelectWindowNativeScope() noexcept;
    SelectWindowNativeScope(const SelectWindowNativeScope&) = delete;
    SelectWindowNativeScope& operator=(const SelectWindowNativeScope&) = delete;
private:
    SelectWindowNativeServices* previous_;
    GraphicsSurfaceNativeScope surfaces_;
    LayerListNativeScope layers_;
    SelectDialogNativeScope dialog_;
};
const LayerItemNativeFrameVtable& SelectWindowNativeMethods() noexcept;
SelectWindowNativeBody6Ch* ConstructSelectWindowNativeAt45CC30(SelectWindowNativeBody6Ch&,
    LayerListNativeBody18h*, SelectWindowNativeServices&);
void DestroySelectWindowNativeAt463CF0(SelectWindowNativeBody6Ch&) noexcept;
SelectWindowNativeBody6Ch* DeleteSelectWindowNativeAt4693F0(SelectWindowNativeBody6Ch&, std::uint32_t) noexcept;
}
