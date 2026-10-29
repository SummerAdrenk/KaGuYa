#pragma once

#include <Runtime/PopupMenuServices.h>
#include <Runtime/PopupMenuPointerRuntime.h>

namespace KaGuYa::Start::Runtime
{
class EnvironmentStartupRuntime;
class RootRuntimeOwner;
class RootToolWindowServicesRuntime;
class ToolWindowLayerRuntime;
class DrawToolView;

// 真实root/Environment的借用装配器；不复制shared槽、不保存某一代payload或图层。
// 除4512E0内部三处外，413BE0由Popup调用者在原位置执行，查询不额外Ensure。
class RootPopupMenuServicesRuntime final : public IPopupMenuDialogServices,
    private IPopupMenuPointerContext
{
public:
    RootPopupMenuServicesRuntime(EnvironmentStartupRuntime& environment, RootRuntimeOwner& root,
        RootToolWindowServicesRuntime& tools, IPopupMenuPointerPlatformApi& pointer) noexcept;
    UnSaveDataRuntime& UnSaveAt4180C0() override;
    std::int32_t TextGlyphCursorAt50() override;
    bool ToolWindowEnabledAtSlot2() override;
    void RefreshToolWindowAt434FB0(std::int32_t value) override;
    void WriteScenePendingCommandAt1C(std::string_view cp932) override;
    void ReplayEnvironmentVoiceBatchAt42C4F0() override;
    void PopSceneAt5328E0() override;
    std::int32_t ReadGuiWidthAt18() override;
    std::int32_t ReadGuiHeightAt1C() override;
    void MovePointerAt4512E0(POINT logical) override;
private:
    DrawToolView CurrentTool() const;
    void SynchronizePointerRootAt413BE0() override;
    WindowModeView PointerModeAt0C() const override;
    HWND PointerWindowAt2C() override;
    EnvironmentStartupRuntime& environment_;
    RootRuntimeOwner& root_;
    RootToolWindowServicesRuntime& tools_;
    IPopupMenuPointerPlatformApi& pointer_;
};
}
