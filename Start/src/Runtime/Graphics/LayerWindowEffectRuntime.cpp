#include <Runtime/LayerWindowEffectRuntime.h>
#include <Runtime/GraphicsSurfaceRuntime.h>

#include <bit>
#include <limits>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
std::int32_t IncrementDword(const std::int32_t value) noexcept
{
    return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(value) + 1u);
}

std::int32_t MultiplyDword(const std::int32_t left, const std::int32_t right) noexcept
{
    return std::bit_cast<std::int32_t>(
        static_cast<std::uint32_t>(left) * static_cast<std::uint32_t>(right));
}

LayerWindowEffectState MakeEffect(const LayerWindowEffectKind kind) noexcept
{
    LayerWindowEffectState effect;
    effect.kind = kind;
    if (kind == LayerWindowEffectKind::Toss)
    {
        // xmmword_5B6260：纵向两方向，保持原始插入顺序。
        effect.directions = {
            LayerWindowEffectDirection{0, 1},
            LayerWindowEffectDirection{0, -1}};
    }
    else
    {
        // xmmword_5B6250：横向两方向，保持原始插入顺序。
        effect.directions = {
            LayerWindowEffectDirection{1, 0},
            LayerWindowEffectDirection{-1, 0}};
    }
    return effect;
}
}

LayerWindowEffectServicesRuntime::LayerWindowEffectServicesRuntime(
    IGraphicsScreenUpdateApi& screen, ProgramDwordReference programWidthAt18,
    ProgramDwordReference programHeightAt1C) noexcept
    : screen_(screen), width_(programWidthAt18), height_(programHeightAt1C) {}

void LayerWindowEffectServicesRuntime::ClearScreenIfLoaded() { screen_.ClearScreenIfLoaded(nullptr); }
std::int32_t LayerWindowEffectServicesRuntime::ReadProgramHeightAt1C() const { return height_.Read(); }
std::int32_t LayerWindowEffectServicesRuntime::ReadProgramWidthAt18() const { return width_.Read(); }

void LayerWindowEffectState::ResetAt4D4C90() noexcept
{
    valueAt04 = 0;
    valueAt14 = 0;
    valueAt10 = 0;
    valueAt18 = 0;
    byteAt28 = false;
}

void LayerWindowEffectState::AdvanceMotionAt4D42D0(
    RECT& dirty, const std::uint8_t stop, ILayerWindowEffectServices& services)
{
    dirty = {};
    if (stop != 0)
    {
        if (byteAt28)
        {
            ResetAt4D4C90();
            const auto height = services.ReadProgramHeightAt1C();
            const auto width = services.ReadProgramWidthAt18();
            dirty = {0, 0, width, height};
        }
        return;
    }

    // 4D4368 是有符号 IDIV；原除零/INT_MIN除-1会触发异常，不能让 C++ 产生 UB。
    if (valueAt08 == 0 || (valueAt04 == (std::numeric_limits<std::int32_t>::min)() && valueAt08 == -1))
        throw std::domain_error("LayerWindow effect signed division fault");
    if (valueAt04 % valueAt08 == 0)
    {
        // 无效方向索引在原版为越界读取；现代边界明确拒绝，不取模修补。
        const auto& direction = directions.at(static_cast<std::size_t>(valueAt18));
        valueAt10 = MultiplyDword(valueAt0C, direction.x);
        valueAt14 = MultiplyDword(valueAt0C, direction.y);
        services.ClearScreenIfLoaded();
        const auto height = services.ReadProgramHeightAt1C();
        const auto width = services.ReadProgramWidthAt18();
        valueAt18 = IncrementDword(valueAt18);
        dirty = {0, 0, width, height};
        if (valueAt18 >= static_cast<std::int32_t>(directions.size()))
            valueAt18 = 0;
        byteAt28 = true;
    }
    // x86 INC/IMUL 保留低32位，不用有符号溢出或饱和运算改变原结果。
    valueAt04 = IncrementDword(valueAt04);
}

LayerWindowEffectController::LayerWindowEffectController(
    ILayerWindowEffectBoundary& boundary) noexcept
    : boundary_(boundary)
{
}

bool LayerWindowEffectController::Select(const std::int32_t selector)
{
    if (selector_ == selector)
        return false;

    // 原函数先释放 +0x48 的引用对象并清空 +0x40，再按新 selector 建立效果。
    effect_.reset();
    if (selector == -1)
    {
        if (selector_ >= 0)
            boundary_.InvalidateWindowFullScreen();
    }
    else if (selector == 0)
    {
        effect_ = MakeEffect(LayerWindowEffectKind::Toss);
    }
    else if (selector == 1)
    {
        effect_ = MakeEffect(LayerWindowEffectKind::Roll);
    }

    // 其他 selector 在 0x4D6860 中没有效果对象，但仍原样写入 CLayerWin+0x3C。
    selector_ = selector;
    return true;
}

std::int32_t LayerWindowEffectController::Selector() const noexcept
{
    return selector_;
}

const std::optional<LayerWindowEffectState>& LayerWindowEffectController::Effect() const noexcept
{
    return effect_;
}

void LayerWindowEffectController::AdvanceMotionAt4D42D0(
    RECT& dirty, const std::uint8_t stop, ILayerWindowEffectServices& services)
{
    if (!effect_)
        throw std::logic_error("LayerWindow effect motion requires an owned effect");
    effect_->AdvanceMotionAt4D42D0(dirty, stop, services);
}

void LayerWindowEffectController::ReleaseEffect() noexcept
{
    effect_.reset();
}

void LayerWindowEffectController::ResetForRootAt47ED4C() noexcept
{
    selector_ = -1;
    effect_.reset();
}
}
