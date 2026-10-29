#pragma once

#include <Runtime/SelectDialogNative.h>
#include <Runtime/LayerWindowNativeBase.h>
#include <Runtime/GraphicsSurfaceRuntime.h>
#include <Runtime/ScriptNamedValueNative.h>
#include <Runtime/GuiMultiCheckButtonNative.h>
#include <span>

namespace KaGuYa::Start::Runtime
{
class RootRuntimeOwner;
class IScrVmCommandErrorSink;
struct SelectWindowNativeWorkflowServices;
enum class CommandSelectionAvailability { BitMask, All, LayoutTable };
struct CommandSelectionControlState final { std::int32_t event; std::int32_t state; };
struct CommandSelectionStateLayout final { std::span<const CommandSelectionControlState> controls; };
struct CommandSelectionResourceProfile final
{
    std::span<const GuiButtonDescriptorNative2Ch> buttons;
    std::span<const SelectNavigationDescriptorNative8h> navigation;
    std::span<const GuiStaticDescriptorNative24h> backgrounds{};
    std::span<const GuiMultiCheckButtonDescriptorNative44h> multiButtons{};
    CommandSelectionAvailability availability = CommandSelectionAvailability::BitMask;
    std::span<const CommandSelectionStateLayout> layouts{};
    // 空表表示原event直接返回；非空表显式映射特殊按钮结果。
    std::span<const std::int32_t> results{};
    // null明确选择已核实的公共Program音效；旧版两款显式提供.sed逻辑名。
    const char* confirmCue = nullptr;
    const char* hoverCue = nullptr;
};
class ICommandSelectionSource
{
public:
    virtual ~ICommandSelectionSource() = default;
    virtual SIZE CanvasSize() = 0;
    virtual std::int32_t ReadAvailabilityParameter() = 0;
    virtual void PlayCommandCue(bool confirm) = 0;
};
class RootCommandSelectionSource final : public ICommandSelectionSource
{
public:
    RootCommandSelectionSource(RootRuntimeOwner&, EnvironmentRootOwner&, ScriptNamedValueNativeTables,
        IScrVmCommandErrorSink&, IRuntimeStringManagerSource&) noexcept;
    SIZE CanvasSize() override;
    std::int32_t ReadAvailabilityParameter() override;
    void PlayCommandCue(bool confirm) override;
private:
    RootRuntimeOwner& root_;
    EnvironmentRootOwner& environment_;
    ScriptNamedValueNativeTables names_;
    IScrVmCommandErrorSink& errors_;
    IRuntimeStringManagerSource& strings_;
};
// Hakoniwa 4C3930/504810：共用GUI容器和输入，业务状态不冒充普通候选文字列表。
struct CommandSelectionDialogNativeBody final
{
    GuiDialogNativeBody5Ch common;
    GuiClickedEventReferences events;
    RECT bounds;
    std::int32_t phase;
    std::int32_t result;
    std::int32_t confirmLatch;
    ICommandSelectionSource* source;
    const CommandSelectionResourceProfile* profile;
};
static_assert(std::is_standard_layout_v<CommandSelectionDialogNativeBody>);
static_assert(offsetof(CommandSelectionDialogNativeBody, common) == 0);
struct CommandSelectionNativeServices final
{
    const CommandSelectionResourceProfile& profile;
    ICommandSelectionSource& source;
    IGuiControlResourceLoadServices& resources;
    IGraphicsSurfaceApi& surfaces;
    IRuntimeStringManagerSource& strings;
};
struct CommandSelectionNativeBody final
{
    LayerWindowNativeBody24h window;
    CommandSelectionDialogNativeBody* dialog;
    CommandSelectionNativeServices* services;
};
static_assert(std::is_standard_layout_v<CommandSelectionNativeBody>);
static_assert(offsetof(CommandSelectionNativeBody, window) == 0);

const GuiDialogNativeVtable35& CommandSelectionDialogMethods() noexcept;
CommandSelectionDialogNativeBody* ConstructCommandSelectionDialog(CommandSelectionDialogNativeBody&,
    CommandSelectionNativeServices&);
void DestroyCommandSelectionDialog(CommandSelectionDialogNativeBody&) noexcept;
CommandSelectionNativeBody* ConstructCommandSelectionNative(CommandSelectionNativeBody&,
    LayerListNativeBody18h*, CommandSelectionNativeServices&);
CommandSelectionNativeBody* DeleteCommandSelectionNative(CommandSelectionNativeBody&, std::uint32_t flags) noexcept;
void BeginCommandSelectionNative(CommandSelectionNativeBody&);
std::int32_t TickCommandSelectionNative(CommandSelectionNativeBody&, std::uint8_t blocked,
    SelectWindowNativeWorkflowServices&);
}
