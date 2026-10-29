#pragma once
#include <Platform/InputViews.h>
#include <Runtime/RootProgramNamedValueRuntime.h>
#include <Runtime/PreSceneEngineUpdateRuntime.h>
#include <Runtime/ToolWindowInteractionRuntime.h>
#include <Runtime/ToolWindowLayerRuntime.h>
#include <Runtime/GuiInputStateRuntime.h>
#include <Runtime/DrawLayersView.h>

namespace KaGuYa::Start::Runtime
{
// 工具层固定为本次434FB0的receiver；内部查询仍按原地址取当前全局根。
class RootToolWindowServicesRuntime final : public IToolWindowInteractionServices,
    public IToolWindowReplaySource, public IMesWinButtonsUpdateBoundary
{
public:
    RootToolWindowServicesRuntime(EnvironmentStartupRuntime& environment, RootRuntimeOwner& root,
        const Platform::IInputRuntime& input, ScriptNamedValueNativeTables tables,
        decltype(&::MessageBoxA) messageBox,ToolWindowNativeInteractionServices* native=nullptr,
        IRuntimeStringManagerSource& strings=DefaultRuntimeStringManagerSource()) noexcept;
    // 只为调用者已选中的manager绑定业务服务；Popup不在这里额外Ensure或换receiver。
    DrawToolView BorrowToolAtSlotTwo(DrawLayersView layers) { return layers.ToolAtSlotTwo(native_,this); }
    void Invoke434FB0(std::int32_t argument) override;
    std::int32_t ReadReplayAt417B50() override;
    std::uint32_t ReadUnSaveModeAt04() override;
    std::uint8_t ReadUnSaveByteAt08() override;
    std::uint8_t ReadUnSaveByteAt09() override;
    std::uint8_t ReadUnSaveByteAt0C() override;
    std::uint32_t ReadUnSaveFlagsAt14() override;
    std::uint32_t ReadUnSaveFlagsAt18() override;
    std::uint32_t ReadUnSaveFlagsAt1C() override;
    std::uint32_t ReadUnSaveFlagsAt20() override;
    bool HasVoiceArrayStorageAt3C() override;
    std::int32_t ReadTextGlyphCursorAt50() override;
    std::uint8_t ReadMessageWindowEnabledAt0C() override;
    bool IsSkipKeyHeldAt4198D0() override;
    std::int32_t ReadReplayValueAt4C73A0() override;
    std::int32_t ReadAffectionDisplayFlagAt4C73A0() override;
private:
    DrawLayersView CaptureLayers();
    EnvironmentStartupRuntime& environment_;
    RootRuntimeOwner& root_;
    RootProgramNamedValueRuntime named_;
    GameGuiInputStateRuntime input_;
    ToolWindowNativeInteractionServices* native_;
};
}
