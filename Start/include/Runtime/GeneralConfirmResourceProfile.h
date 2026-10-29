#pragma once
#include <Runtime/GuiControlFactoryNative.h>
#include <Runtime/GuiDialogNative.h>
#include <optional>
#include <span>

namespace KaGuYa::Start::Runtime
{
enum class GeneralConfirmButtons { YesNo, Acknowledge };
enum class GeneralConfirmUnknownEvent { ThrowIntOne, KeepFirstFlagPending };
struct GeneralConfirmModeRow final
{
    std::int32_t mode;
    std::int32_t background;
    std::int32_t message; // -1表示不构造消息控件，不能补默认图片。
    GeneralConfirmButtons buttons;
};
struct GeneralConfirmNavigation final
{
    std::int32_t action;
    // 包含source=-1的终止项；工厂仍按原终止规则读取。
    std::span<const GuiDialogNavigationMapping> mappings;
};
struct GeneralConfirmEventResult final
{
    std::int32_t event;
    std::int32_t result;
};
struct GeneralConfirmResourceProfile final
{
    std::span<const GuiStaticDescriptorNative24h> statics;
    // 稀疏原mode编号：主线0-4；仅04/08提供已证额外模式，不迁回5-8删除。
    std::span<const GeneralConfirmModeRow> modes;
    std::span<const GuiStaticDescriptorNative24h> additionalStatics;
    std::span<const GuiButtonDescriptorNative2Ch> yesNoButtons;
    std::span<const GuiButtonDescriptorNative2Ch> acknowledgeButtons;
    std::span<const GeneralConfirmNavigation> navigation;
    std::span<const GeneralConfirmEventResult> eventResults;
    GeneralConfirmUnknownEvent unknownEvent;
};

// 编译profile是必需依赖；缺失/未提供模式明确抛出，不回退18。
const GeneralConfirmResourceProfile& CurrentGeneralConfirmResourceProfile();
const GeneralConfirmModeRow& FindGeneralConfirmMode(const GeneralConfirmResourceProfile&, std::int32_t mode);
// nullopt仅表示新版首个未知flag保持待处理；旧策略原样throw int 1。
std::optional<std::int32_t> GeneralConfirmResultForEvent(const GeneralConfirmResourceProfile&, std::int32_t event);
}
