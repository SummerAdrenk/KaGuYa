#pragma once
#include <Platform/InputViews.h>

#include <Platform/Input.h>

#include <cstdint>
#include <Runtime/DrawLayersView.h>

namespace KaGuYa::Start::Runtime
{
class DrawManagerLayerRuntime;
class FreeScrollRuntime;
class IFreeScrollOperations;
class OpacityWindowRuntime;
class IOpacityDialogUpdateServices;

// 4B2730 的图层操作与模态宿主分离；不得以独立影子窗口替代 manager owning 槽。
class IReadScriptLayerServices
{
public:
    virtual ~IReadScriptLayerServices() = default;
    [[nodiscard]] virtual bool PrepareLayersAt46AD20(std::int32_t mode = 0) = 0;
    [[nodiscard]] virtual bool PrepareLayersAt46A980() = 0;
    virtual void SetLayerVisible(std::uint32_t slot, std::uint8_t requested, std::int32_t argument) = 0;
    virtual void ResetOpacityAt47EE40() = 0;
    virtual void BeginHiddenWindowAt4ADF70() = 0;
    virtual void EndHiddenWindowAt4AC970() = 0;
    [[nodiscard]] virtual bool UpdateHiddenWindowAt4BDD00() = 0;
    [[nodiscard]] virtual bool UpdateLayerSixAt4BE530() = 0;
};

class ReadScriptLayerServicesRuntime final : public IReadScriptLayerServices
{
public:
    ReadScriptLayerServicesRuntime(DrawLayersView manager, IFreeScrollOperations& scroll,
        Platform::MouseView mouse, Platform::KeyboardView keyboard,
        IOpacityDialogUpdateServices& opacity) noexcept;
    ReadScriptLayerServicesRuntime(IDrawLayersViewSource& manager,IFreeScrollOperations& scroll,
        IOpacityWindowInputSource& opacity) noexcept;
    bool PrepareLayersAt46AD20(std::int32_t mode = 0) override;
    bool PrepareLayersAt46A980() override;
    void SetLayerVisible(std::uint32_t slot, std::uint8_t requested, std::int32_t argument) override;
    void ResetOpacityAt47EE40() override;
    void BeginHiddenWindowAt4ADF70() override;
    void EndHiddenWindowAt4AC970() override;
    bool UpdateHiddenWindowAt4BDD00() override;
    bool UpdateLayerSixAt4BE530() override;

private:
    [[nodiscard]] DrawOpacityView OpacityAtSlotSix() const;
    DrawLayersView CaptureLayers() const;
    DrawLayersView manager_;
    IDrawLayersViewSource* source_=nullptr;
    IFreeScrollOperations& scroll_;
    DrawOpacityControlledServices controlled_;
    IOpacityWindowInputSource* native_=nullptr;
};
}
