#pragma once

#include <Runtime/GuiDialogNative.h>
#include <Runtime/GuiControlFactoryNative.h>

namespace KaGuYa::Start::Runtime
{
// 4E27F0/4FCA80：未写的eventAt5C保留调用者字节，不以optional替代原DWORD。
struct ToolDialogNativeBody7Ch final
{
    union { GuiDialogNativeBody5Ch common; };
    std::int32_t eventAt5C;
    RECT boundsAt60;
    union { GuiClickedEventReferences eventsAt70; };
    ToolDialogNativeBody7Ch() noexcept {}
};
static_assert(sizeof(ToolDialogNativeBody7Ch) == 0x7C);
static_assert(offsetof(ToolDialogNativeBody7Ch, eventAt5C) == 0x5C);
static_assert(offsetof(ToolDialogNativeBody7Ch, boundsAt60) == 0x60);
static_assert(offsetof(ToolDialogNativeBody7Ch, eventsAt70) == 0x70);
static_assert(std::is_standard_layout_v<ToolDialogNativeBody7Ch>);
static_assert(std::is_trivially_destructible_v<ToolDialogNativeBody7Ch>);

struct ToolCheckDescriptorNative48h final
{
    std::int32_t eventAt00;
    std::array<const char*, 7> normalAt04;
    std::array<const char*, 7> checkedAt20;
    const POINT* positionAt3C;
    std::int32_t hitSlotAt40;
    std::int32_t childIdAt44;
};
struct ToolControlGroupNative0Ch final
{
    std::int32_t kindAt00;
    std::int32_t countAt04;
    const void* recordsAt08;
};
static_assert(sizeof(ToolCheckDescriptorNative48h) == 0x48);
static_assert(offsetof(ToolCheckDescriptorNative48h, positionAt3C) == 0x3C);
static_assert(sizeof(ToolControlGroupNative0Ch) == 0x0C);
struct ToolDialogNativeProfile final
{
    const ToolControlGroupNative0Ch* normalGroupsAt606DD4;
    const ToolControlGroupNative0Ch* replayGroupsAt606FA0;
};
const ToolDialogNativeProfile& Sample18ToolDialogNativeProfile() noexcept;

// 两个可写原全局值分别在52774C/5277A2读取，不在Dialog正文中缓存。
struct ToolDialogNativeRelation final
{
    std::int32_t controlIdAt606FE0 = 100;
    std::int32_t relatedIdAt606FDC = 10;
};
class ToolDialogNativeScope final
{
public:
    explicit ToolDialogNativeScope(const ToolDialogNativeRelation& relation) noexcept;
    ~ToolDialogNativeScope() noexcept;
    ToolDialogNativeScope(const ToolDialogNativeScope&) = delete;
    ToolDialogNativeScope& operator=(const ToolDialogNativeScope&) = delete;
private:
    const ToolDialogNativeRelation* previous_;
};

const GuiDialogNativeVtable35& ToolDialogNativeVtable() noexcept;
ToolDialogNativeBody7Ch* ConstructToolDialogNativeAt4E27F0(ToolDialogNativeBody7Ch& body,
    std::int32_t mode, const ToolDialogNativeProfile& profile,
    IGuiControlResourceLoadServices& loader, IGraphicsSurfaceApi& surfaces,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
ToolDialogNativeBody7Ch* DeleteToolDialogNativeAt4FCA80(ToolDialogNativeBody7Ch* body,
    std::uint32_t flags) noexcept;
void ResetToolDialogControlsAt532480(GuiDialogNativeBody5Ch& body);
void ReconcileToolDialogControlsAt527720(GuiDialogNativeBody5Ch& body);
void SetToolControlEnabledNativeAt508470(GuiDialogNativeBody5Ch&, std::int32_t id, std::int32_t enabled);
void SetToolControlCheckedNativeAt501FA0(GuiDialogNativeBody5Ch&, std::int32_t id, std::int32_t checked);
void ResetToolControlNativeAt5312B0(GuiDialogNativeBody5Ch&, std::int32_t id);
void InvalidateToolControlsNativeAt530DC0(GuiDialogNativeBody5Ch&);
std::int32_t ScanToolControlStateNativeAt50FD20(GuiDialogNativeBody5Ch&);
}
