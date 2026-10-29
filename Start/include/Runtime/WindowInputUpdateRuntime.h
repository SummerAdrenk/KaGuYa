#pragma once
#include <Platform/InputViews.h>

#include <Platform/Input.h>
#include <Platform/WindowCreation.h>
#include <Runtime/InputNativeObjects.h>

namespace KaGuYa::Start::Runtime
{
class WindowModeManagerRuntime;
class WindowModeView;
class EnvironmentStartupRuntime;
class RootRuntimeOwner;
class IRootLogicalRectangleSynchronization;

// 450AA0 的已构造输入对象路径；借用实际环境与mode manager，不缓存帧映射。
class WindowInputUpdateRuntime final : private IInputFrameNativeSource
{
public:
    WindowInputUpdateRuntime(Platform::IInputRuntime& input, const HWND& window,
        const Platform::ParamsGameProfile& profile, const WindowModeManagerRuntime& mode,
        const Platform::IMousePositionApi& mousePosition) noexcept;
    WindowInputUpdateRuntime(Platform::IInputRuntime& input, const EnvironmentStartupRuntime& environment,
        const std::unique_ptr<WindowModeManagerRuntime>& modeSlot,
        IRootLogicalRectangleSynchronization& root, const Platform::IMousePositionApi& mousePosition) noexcept;
    WindowInputUpdateRuntime(Platform::IInputRuntime& input, const EnvironmentStartupRuntime& environment,
        RootRuntimeOwner& root, const Platform::IMousePositionApi& mousePosition) noexcept;
    void UpdateAt450AA0();

private:
    HWND ReadWindowAt450ADB() override{return Window();}
    std::int32_t ReadWidthAt450AE6() override{return Width();}
    std::int32_t ReadHeightAt450AF1() override{return Height();}
    std::uint8_t ReadModeAt450B01() override;
    const std::int32_t* ReadViewportAt450B3F() override;
    const std::int32_t* ReadViewportAt450B7B() override;
    Platform::IInputRuntime& input_;
    [[nodiscard]] HWND Window() const;
    [[nodiscard]] int Width() const;
    [[nodiscard]] int Height() const;
    [[nodiscard]] WindowModeView CurrentMode() const;
    const HWND* window_ = nullptr;
    const Platform::ParamsGameProfile* profile_ = nullptr;
    const WindowModeManagerRuntime* mode_ = nullptr;
    const EnvironmentStartupRuntime* environment_ = nullptr;
    const std::unique_ptr<WindowModeManagerRuntime>* modeSlot_ = nullptr;
    IRootLogicalRectangleSynchronization* root_ = nullptr;
    RootRuntimeOwner* owner_ = nullptr;
    const Platform::IMousePositionApi& mousePosition_;
};
}
