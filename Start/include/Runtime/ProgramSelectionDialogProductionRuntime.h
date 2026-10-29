#pragma once

#include <Runtime/ProgramSelectionMainDialogObjectGraphRuntime.h>
#include <Runtime/ProgramSelectionSystemDialogObjectGraphRuntime.h>

#include <Windows.h>

#include <cstdint>
#include <memory>

namespace KaGuYa::Start::Runtime
{
// 0x536EA0 所在的输入状态所有权边界。安装发生在整个对话框构造的最后，
// 释放则属于 0x4F7AC0 的 GuiDialog 基类清理阶段。
class IProgramSelectionDialogInputStateLifetime
{
public:
    virtual ~IProgramSelectionDialogInputStateLifetime() = default;

    virtual void InstallGameInputStateAt536EA0() = 0;
    virtual void ReleaseGameInputStateAt4F7AC0() noexcept = 0;
};

struct ProgramSelectionDialogProductionDependencies final
{
    ICgOperationSurfaceFactory& surfaceFactory;
    IGraphicsSurfaceCompositionApi& graphics;
    ICgOperationButtonInputServices& buttonInputServices;
    IProgramSelectionMainValueSource& values;
    IProgramSelectionMainDialogServices& mainServices;
    IProgramSelectionSystemDialogServices& systemServices;
    IProgramSelectionDialogCoreServices& dialogServices;
    IProgramSelectionDialogInputStateLifetime& inputStateLifetime;
    ProgramSelectionDialogMode mode = ProgramSelectionDialogMode::Mode00;
    std::int32_t screenWidth = 0;
    std::int32_t screenHeight = 0;
    IProgramSelectionMainDirtyRectSink* mainDirtyRectSink = nullptr;
};

// CharaSelect00/01Dialog（0x4DE1E0/0x4DF0E0）的生产强所有权边界。
// Main 与 System 按注册顺序持有；析构不能依赖 C++ 成员的逆序规则。
class ProgramSelectionDialogProductionRuntime final
{
public:
    explicit ProgramSelectionDialogProductionRuntime(
        ProgramSelectionDialogProductionDependencies dependencies);
    ~ProgramSelectionDialogProductionRuntime();

    ProgramSelectionDialogProductionRuntime(
        const ProgramSelectionDialogProductionRuntime&) = delete;
    ProgramSelectionDialogProductionRuntime& operator=(
        const ProgramSelectionDialogProductionRuntime&) = delete;

    [[nodiscard]] const ProgramSelectionDialogRef& Dialog() const noexcept;
    [[nodiscard]] const std::shared_ptr<
        ProgramSelectionMainDialogObjectGraphRuntime>& MainAtChild0() const noexcept;
    [[nodiscard]] const std::shared_ptr<
        ProgramSelectionSystemDialogObjectGraphRuntime>& SystemAtChild1() const noexcept;
    [[nodiscard]] const std::shared_ptr<RuntimeGraphicsSurface>&
        BackgroundFrameAt98() const noexcept;
    [[nodiscard]] const std::shared_ptr<RuntimeGraphicsSurface>&
        CompositeFrameAtA4() const noexcept;
    [[nodiscard]] const RECT& DialogBoundsAt74() const noexcept;

private:
    void ReleaseAt4F8D50() noexcept;

    IProgramSelectionDialogInputStateLifetime* inputStateLifetime_ = nullptr;
    bool inputStateInstalled_ = false;
    RECT dialogBoundsAt74_{};
    ProgramSelectionDialogRef dialog_;
    std::shared_ptr<ProgramSelectionMainDialogObjectGraphRuntime> mainAtChild0_;
    std::shared_ptr<ProgramSelectionSystemDialogObjectGraphRuntime> systemAtChild1_;
    std::shared_ptr<RuntimeGraphicsSurface> backgroundFrameAt98_;
    std::shared_ptr<RuntimeGraphicsSurface> compositeFrameAtA4_;
};
}
