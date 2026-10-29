#pragma once
#include <Runtime/GuiDialogNative.h>
#include <Runtime/GuiControlFactoryNative.h>
#include <Runtime/ScriptCallStacksNative.h>
#include <cstddef>

namespace KaGuYa::Start::Runtime
{
class RootRuntimeOwner;
struct SelectDialogNativeBody88h final
{
    union { GuiDialogNativeBody5Ch common; };
    std::uint32_t unwrittenAt5C;
    std::int32_t resultAt60;
    std::int32_t phaseAt64;
    std::int32_t confirmLatchAt68;
    RECT boundsAt6C;
    union { GuiClickedEventReferences eventsAt7C; };
    SelectDialogNativeBody88h() noexcept {}
};
static_assert(sizeof(SelectDialogNativeBody88h) == 0x88);
static_assert(offsetof(SelectDialogNativeBody88h, resultAt60) == 0x60);
static_assert(offsetof(SelectDialogNativeBody88h, phaseAt64) == 0x64);
static_assert(offsetof(SelectDialogNativeBody88h, confirmLatchAt68) == 0x68);
static_assert(offsetof(SelectDialogNativeBody88h, boundsAt6C) == 0x6C);
static_assert(offsetof(SelectDialogNativeBody88h, eventsAt7C) == 0x7C);
static_assert(std::is_standard_layout_v<SelectDialogNativeBody88h>);
static_assert(std::is_trivially_destructible_v<SelectDialogNativeBody88h>);
struct SelectButtonPlacementNative10h final
{
    const GuiButtonDescriptorNative2Ch* descriptorAt00;
    std::int32_t xAt04;
    std::int32_t yAt08;
    std::int32_t idAt0C;
};
struct SelectNavigationDescriptorNative8h final
{
    std::int32_t inputAt00;
    const GuiDialogNavigationMapping* pairsAt04;
};
static_assert(sizeof(SelectButtonPlacementNative10h) == 0x10);
static_assert(sizeof(SelectNavigationDescriptorNative8h) == 8);
struct SelectDialogNativeProfile final
{
    const SelectButtonPlacementNative10h* fiveButtonsAt607098;
    std::array<const std::int32_t*, 4> visibleIdsAt6070E8;
    std::array<const SelectNavigationDescriptorNative8h*, 4> navigationAt607180;
    // 指针沿用18号地址命名；Baku 4E32C5仅遍历3项，既有三字段初始化仍为5项。
    std::size_t placementCount = 5;
};
const SelectDialogNativeProfile& Sample18SelectDialogNativeProfile() noexcept;
class ISelectDialogNativeSource
{
public:
    virtual ~ISelectDialogNativeSource() = default;
    virtual const ScriptDwordVectorStorage& CandidatesAt50F119() = 0;
};
class RootSelectDialogNativeSource final : public ISelectDialogNativeSource
{
public:
    explicit RootSelectDialogNativeSource(RootRuntimeOwner& root) noexcept : root_(root) {}
    const ScriptDwordVectorStorage& CandidatesAt50F119() override;
private:
    RootRuntimeOwner& root_;
};
class SelectDialogNativeScope final
{
public:
    SelectDialogNativeScope(const SelectDialogNativeProfile&, ISelectDialogNativeSource&) noexcept;
    ~SelectDialogNativeScope() noexcept;
    SelectDialogNativeScope(const SelectDialogNativeScope&) = delete;
    SelectDialogNativeScope& operator=(const SelectDialogNativeScope&) = delete;
private:
    const SelectDialogNativeProfile* previousProfile_;
    ISelectDialogNativeSource* previousSource_;
};
const GuiDialogNativeVtable35& SelectDialogNativeVtable() noexcept;
SelectDialogNativeBody88h* ConstructSelectDialogNativeAt4F17E0(SelectDialogNativeBody88h&,
    const SelectDialogNativeProfile&, IGuiControlResourceLoadServices&, IGraphicsSurfaceApi&,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
SelectDialogNativeBody88h* DeleteSelectDialogNativeAt4FE1E0(SelectDialogNativeBody88h*, std::uint32_t) noexcept;
RECT* SelectVisibleBoundsNativeAt50AD40(SelectDialogNativeBody88h&, RECT* output, std::int32_t ordinal);
}
