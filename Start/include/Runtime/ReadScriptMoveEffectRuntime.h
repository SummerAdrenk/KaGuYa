#pragma once

#include <Runtime/GraphicsSurfaceRuntime.h>
#include <Runtime/ReadScriptEffectRuntime.h>

#include <cstddef>
#include <memory>
#include <vector>

namespace KaGuYa::Start::Runtime
{
class DrawManagerLayerRuntime;
class LayerWindowRuntime;
class PatternLayerRuntime;
class GameGuiInputStateRuntime;
class SaveDataEnvironmentArchive;
class UnSaveDataRuntime;
class IStartupConfigGameSystemSource;
struct ScrVmRuntimeStateBundle;

// 全部引用借自同一根对象图。Program 尺寸是 +18/+1C 的实时字段，不能用
// GameSystem 或 DLL 尺寸代填；DLL 尺寸另用于本次 Effect 的失效矩形快照。
struct ReadScriptMoveEffectDependencies final
{
    DrawManagerLayerRuntime& layers;
    const IStartupConfigGameSystemSource& gameSystem;
    const std::uint32_t& programWidthAt18;
    const std::uint32_t& programHeightAt1C;
    const SaveDataEnvironmentArchive& save;
    const UnSaveDataRuntime& unsave;
    const GameGuiInputStateRuntime& input;
    const ScrVmRuntimeStateBundle& bundle;
    IGraphicsSurfaceApi& surfaces;
    IGraphicsWindowSurfaceApi& drawing;
    IGraphicsScreenSizeApi& sizes;
    IGraphicsScreenUpdateApi& screen;
};

// CEffectMove::_list 的真实状态；只通过 const 观察，不允许外部改写移动进度。
struct ReadScriptMoveEffectItem final
{
    float currentX = 0;
    float currentY = 0;
    std::int32_t targetX = 0;
    std::int32_t targetY = 0;
    std::int32_t finalX = 0;
    std::int32_t finalY = 0;
    float stepX = 0;
    float stepY = 0;
    std::uint32_t directions = 0;
    PatternLayerRuntime* pattern = nullptr;
    std::unique_ptr<RuntimeGraphicsSurface> snapshot;
    std::int32_t offsetX = 0;
    std::int32_t offsetY = 0;
};

// 49C0F0 / 5B4D50：独立 Sprite 截图、移动、绘制和最终 Pattern 提交。
// 构造体内原 catch(...) 只令 ready=0；不回滚已提交条目和外部模型副作用。
// 原 4A4280 的 EHFlags=5：Finish 失败终止进程，不外传；尝试后只允许销毁。
// 现代析构仅按前向释放资源，不偷偷执行完成业务。
class ReadScriptMoveEffectRuntime final : public IReadScriptEffectRuntime
{
public:
    explicit ReadScriptMoveEffectRuntime(ReadScriptMoveEffectDependencies dependencies);
    ~ReadScriptMoveEffectRuntime() override;
    ReadScriptMoveEffectRuntime(const ReadScriptMoveEffectRuntime&) = delete;
    ReadScriptMoveEffectRuntime& operator=(const ReadScriptMoveEffectRuntime&) = delete;

    [[nodiscard]] bool TickAtVslot00() override;
    void DrawAtVslot04() override;
    [[nodiscard]] std::uint8_t ReadyAtVslot08() const override;
    void Finish() noexcept override;

    [[nodiscard]] std::size_t ItemCount() const noexcept { return items_.size(); }
    [[nodiscard]] const ReadScriptMoveEffectItem& ItemAt(std::size_t index) const;
    [[nodiscard]] const RECT& Viewport() const noexcept { return viewport_; }

private:
    void ConstructAt49C0F0();
    void CaptureItemAt49CDE4(ReadScriptMoveEffectItem& item, std::uint32_t mirror);
    void ComputeStepsAt49C5D8(ReadScriptMoveEffectItem& item);
    void Append(std::unique_ptr<ReadScriptMoveEffectItem> item);
    void ReleaseItemsForward() noexcept;
    void RequireLive() const;
    [[nodiscard]] LayerWindowRuntime& LayerWindow() const;
    void RenderAndInvalidate();

    ReadScriptMoveEffectDependencies dependencies_;
    std::vector<std::unique_ptr<ReadScriptMoveEffectItem>> items_;
    RECT viewport_{};
    std::uint8_t ready_ = 0;
    bool finishAttempted_ = false;
};

class IReadScriptMoveEffectFactory
{
public:
    virtual ~IReadScriptMoveEffectFactory() = default;
    [[nodiscard]] virtual std::unique_ptr<IReadScriptEffectRuntime> CreateMoveAt49C0F0() = 0;
};

class ReadScriptMoveEffectFactoryRuntime final : public IReadScriptMoveEffectFactory
{
public:
    explicit ReadScriptMoveEffectFactoryRuntime(ReadScriptMoveEffectDependencies dependencies) noexcept;
    [[nodiscard]] std::unique_ptr<IReadScriptEffectRuntime> CreateMoveAt49C0F0() override;
private:
    ReadScriptMoveEffectDependencies dependencies_;
};
}
