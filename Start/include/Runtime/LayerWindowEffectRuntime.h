#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <Windows.h>
#include <Runtime/ProgramWindowFieldReferences.h>

namespace KaGuYa::Start::Runtime
{
class IGraphicsScreenUpdateApi;

class ILayerWindowEffectServices
{
public:
    virtual ~ILayerWindowEffectServices() = default;
    virtual void ClearScreenIfLoaded() = 0;
    [[nodiscard]] virtual std::int32_t ReadProgramHeightAt1C() const = 0;
    [[nodiscard]] virtual std::int32_t ReadProgramWidthAt18() const = 0;
};

// 尺寸借用 Program 全局字段，不从 Graphics 或 Pattern 画布反推，也不缓存快照。
class LayerWindowEffectServicesRuntime final : public ILayerWindowEffectServices
{
public:
    LayerWindowEffectServicesRuntime(IGraphicsScreenUpdateApi& screen,
        ProgramDwordReference programWidthAt18, ProgramDwordReference programHeightAt1C) noexcept;
    void ClearScreenIfLoaded() override;
    std::int32_t ReadProgramHeightAt1C() const override;
    std::int32_t ReadProgramWidthAt18() const override;
private:
    IGraphicsScreenUpdateApi& screen_;
    ProgramDwordReference width_;
    ProgramDwordReference height_;
};

struct LayerWindowEffectDirection final
{
    std::int32_t x = 0;
    std::int32_t y = 0;

    [[nodiscard]] bool operator==(const LayerWindowEffectDirection&) const noexcept = default;
};

enum class LayerWindowEffectKind : std::uint8_t
{
    Toss,
    Roll,
};

struct LayerWindowEffectState final
{
    LayerWindowEffectKind kind = LayerWindowEffectKind::Toss;
    std::int32_t valueAt04 = 0;
    std::int32_t valueAt08 = 2;
    std::int32_t valueAt0C = 10;
    std::int32_t valueAt10 = 0;
    std::int32_t valueAt14 = 0;
    std::int32_t valueAt18 = 0;
    std::array<LayerWindowEffectDirection, 2> directions{};
    bool byteAt28 = false;

    void AdvanceMotionAt4D42D0(RECT& dirty, std::uint8_t stop,
        ILayerWindowEffectServices& services);
    void ResetAt4D4C90() noexcept;

    [[nodiscard]] bool operator==(const LayerWindowEffectState&) const noexcept = default;
};

class ILayerWindowEffectBoundary
{
public:
    virtual ~ILayerWindowEffectBoundary() = default;

    // 4D6B98..4D6BDD：非负 selector 返回 -1 时，标脏 Window 内部 LayerList。
    virtual void InvalidateWindowFullScreen() = 0;
};

// 对应 CLayerWin+0x3C/+0x40/+0x48 与 0x4D6C10/0x4D6860 的 selector 状态机。
class LayerWindowEffectController final
{
public:
    explicit LayerWindowEffectController(ILayerWindowEffectBoundary& boundary) noexcept;

    // 返回值只表示 selector 是否发生变化；相同 selector 在原函数中完全不执行后续操作。
    [[nodiscard]] bool Select(std::int32_t selector);
    void AdvanceMotionAt4D42D0(RECT& dirty, std::uint8_t stop,
        ILayerWindowEffectServices& services);
    // 468C45：只释放拥有对象；不能通过 Select(-1) 触发额外的失效操作。
    void ReleaseEffect() noexcept;
    // 47ED4C：先写selector=-1再释放，不走Select的屏幕失效分支。
    void ResetForRootAt47ED4C() noexcept;

    [[nodiscard]] std::int32_t Selector() const noexcept;
    [[nodiscard]] const std::optional<LayerWindowEffectState>& Effect() const noexcept;

private:
    ILayerWindowEffectBoundary& boundary_;
    std::int32_t selector_ = -1;
    std::optional<LayerWindowEffectState> effect_;
};
}
