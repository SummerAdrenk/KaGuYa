#include <Runtime/ReadScriptFlashShakeEffects.h>

#include <array>
#include <bit>
#include <limits>
#include <span>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
std::int32_t Add(std::int32_t a, std::int32_t b) noexcept
{
    return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(a) + static_cast<std::uint32_t>(b));
}

std::int32_t Multiply(std::int32_t a, std::int32_t b) noexcept
{
    return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(a) * static_cast<std::uint32_t>(b));
}

void DrawAt441980(const std::shared_ptr<RuntimeGraphicsSurface>& surface,
    IGraphicsWindowSurfaceApi& drawing, IGraphicsScreenUpdateApi& screen,
    std::int32_t x, std::int32_t y)
{
    // 原函数先解引用 CSurface+4，再判 handle/module；空对象不能冒充空句柄。
    if (!surface)
        throw std::logic_error("CEffect: null CSurface at 441980");
    const auto handle = surface->Handle();
    if (handle && screen.ModuleLoaded())
        static_cast<void>(drawing.SurfaceDrawRaw(handle, x, y, nullptr, nullptr, nullptr, 0));
}

// Start.exe:602120 的十五项 {count,POINT*}，数据来自 601A10..60204F。
// 保留实际顺序，不使用随机抖动或已有 Toss/Roll 算法替代。
constexpr POINT vertical[]{{0,1},{0,-1},{0,1},{0,-1},{0,1},{0,-1},{0,1},{0,-1}};
constexpr POINT horizontal[]{{1,0},{-1,0},{1,0},{-1,0},{1,0},{-1,0},{1,0},{-1,0}};
constexpr POINT corners[]{{1,1},{1,-1},{-1,1},{-1,-1},{1,1},{1,-1},{-1,1},{-1,-1}};
constexpr POINT down[]{{0,128},{0,0},{0,96},{0,0},{0,64},{0,0},{0,48},{0,0},{0,32},{0,0},{0,16},{0,0},{0,8},{0,0},{0,4},{0,0}};
constexpr POINT up[]{{0,-128},{0,0},{0,-96},{0,0},{0,-64},{0,0},{0,-48},{0,0},{0,-32},{0,0},{0,-16},{0,0},{0,-8},{0,0},{0,-4},{0,0}};
constexpr POINT right[]{{128,0},{0,0},{96,0},{0,0},{64,0},{0,0},{48,0},{0,0},{32,0},{0,0},{16,0},{0,0},{8,0},{0,0},{4,0},{0,0}};
constexpr POINT left[]{{-128,0},{0,0},{-96,0},{0,0},{-64,0},{0,0},{-48,0},{0,0},{-32,0},{0,0},{-16,0},{0,0},{-8,0},{0,0},{-4,0},{0,0}};
constexpr POINT downRight[]{{128,128},{0,0},{96,96},{0,0},{64,64},{0,0},{48,48},{0,0},{32,32},{0,0},{16,16},{0,0},{8,8},{0,0},{4,4},{0,0}};
constexpr POINT downLeft[]{{-128,128},{0,0},{-96,96},{0,0},{-64,64},{0,0},{-48,48},{0,0},{-32,32},{0,0},{-16,16},{0,0},{-8,8},{0,0},{-4,4},{0,0}};
constexpr POINT upRight[]{{128,-128},{0,0},{96,-96},{0,0},{64,-64},{0,0},{48,-48},{0,0},{32,-32},{0,0},{16,-16},{0,0},{8,-8},{0,0},{4,-4},{0,0}};
constexpr POINT upLeft[]{{-128,-128},{0,0},{-96,-96},{0,0},{-64,-64},{0,0},{-48,-48},{0,0},{-32,-32},{0,0},{-16,-16},{0,0},{-8,-8},{0,0},{-4,-4},{0,0}};
constexpr POINT diagonal[]{{1,1},{-1,-1},{1,1},{-1,-1},{1,1},{-1,-1},{1,1},{-1,-1}};
constexpr POINT antiDiagonal[]{{1,-1},{-1,1},{1,-1},{-1,1},{1,-1},{-1,1},{1,-1},{-1,1}};
constexpr POINT counterClockwise[]{{0,-1},{0,1},{-1,-1},{1,1},{-1,0},{1,0},{-1,1},{1,-1},{0,1},{0,-1},{1,1},{-1,-1},{1,0},{-1,0},{1,-1},{-1,1}};
constexpr POINT clockwise[]{{0,-1},{0,1},{1,-1},{-1,1},{1,0},{-1,0},{1,1},{-1,-1},{0,1},{0,-1},{-1,1},{1,-1},{-1,0},{1,0},{-1,-1},{1,1}};
constexpr std::array<std::span<const POINT>, 15> paths{vertical, horizontal, corners,
    down, up, right, left, downRight, downLeft, upRight, upLeft, diagonal, antiDiagonal,
    counterClockwise, clockwise};
}

ReadScriptFlashEffect::ReadScriptFlashEffect(IReadScriptEffectSurfaceServices& resources,
    IGraphicsWindowSurfaceApi& drawing, IGraphicsScreenUpdateApi& screen,
    const std::string& nameCp932, std::int32_t count)
    : drawing_(drawing), screen_(screen)
{
    // 49BD22..49BE97 位于同一个 catch(...)；失败保留已提交成员，ready 仍为零。
    try
    {
        if (!resources.LoadStaticAt480000(flash_, nameCp932))
            throw 1;
        const auto length = Add(Multiply(count, 2), -1);
        if (length > 0) // 412700 的有符号分支；不是把 count 限制为 1..3。
        {
            intervals_.resize(static_cast<std::size_t>(length));
            for (std::size_t i = 0; i < intervals_.size(); ++i)
                intervals_[i] = (i & 1) ? 2 : 4; // 原 PE:602668 两字节。
        }
        index_ = 0;
        remaining_ = 0;
        previous_ = resources.CaptureScreenAt4ACDD0();
        ready_ = 1; // 原构造没有把空捕获引用判为失败。
    }
    catch (...)
    {
        // 49BE9D 的空 handler；不得回滚 flash_ / intervals_。
    }
}

bool ReadScriptFlashEffect::TickAtVslot00()
{
    if (remaining_.value())
    {
        remaining_ = Add(*remaining_, -1);
        return true;
    }
    if (index_.value() >= static_cast<std::int32_t>(intervals_.size()))
        return false;
    DrawAt441980((*index_ & 1) ? previous_ : flash_, drawing_, screen_, 0, 0);
    screen_.InvalidatedRectIfLoaded(nullptr);
    // 4BB879..4BB8A8：完成绘制后重读 index，随后提交间隔和下一索引。
    const auto interval = intervals_.at(static_cast<std::size_t>(index_.value()));
    index_ = Add(*index_, 1);
    remaining_ = static_cast<std::int32_t>(interval) - 1;
    return true;
}

void ReadScriptFlashEffect::DrawAtVslot04() {}
std::uint8_t ReadScriptFlashEffect::ReadyAtVslot08() const { return ready_; }
void ReadScriptFlashEffect::Finish() noexcept
{
    // 4A4080 的 FuncInfo 5EF24C：EHFlags=5；恢复 Draw/Invalidate 也在终止边界内。
    // 原 CRT 543CB9 -> 543D62 -> 56A3EF，不允许将异常交回 owner。
    if (ready_)
    {
        DrawAt441980(previous_, drawing_, screen_, 0, 0);
        screen_.InvalidatedRectIfLoaded(nullptr);
    }
}

ReadScriptShakeEffect::ReadScriptShakeEffect(IReadScriptEffectSurfaceServices& resources,
    IGraphicsWindowSurfaceApi& drawing, IGraphicsScreenUpdateApi& screen,
    std::int32_t mode, std::int32_t scale, std::int32_t interval)
    : drawing_(drawing), screen_(screen), interval_(interval ? interval : 1), mode_(mode), scale_(scale)
{
    // 49D4E0 只有展开清理、没有 Flash 的局部 catch。捕获异常必须向外传播。
    previous_ = resources.CaptureScreenAt4ACDD0();
    ready_ = 1;
}

bool ReadScriptShakeEffect::TickAtVslot00()
{
    if (mode_ < 0 || mode_ >= static_cast<std::int32_t>(paths.size()))
        throw std::out_of_range("CEffectShake: trajectory table index outside 0..14");
    const auto path = paths[static_cast<std::size_t>(mode_)];
    if (index_ >= static_cast<std::int32_t>(path.size()))
        return false;
    if (counter_ == std::numeric_limits<std::int32_t>::min() && interval_ == -1)
        throw std::overflow_error("CEffectShake: original IDIV quotient overflow");
    if (counter_ % interval_ == 0)
    {
        const POINT point = path[static_cast<std::size_t>(index_)];
        const auto x = Multiply(point.x, scale_);
        const auto y = Multiply(point.y, scale_);
        screen_.ClearScreenIfLoaded(nullptr);
        DrawAt441980(previous_, drawing_, screen_, x, y);
        screen_.InvalidatedRectIfLoaded(nullptr);
        index_ = Add(index_, 1);
    }
    counter_ = Add(counter_, 1);
    return true;
}

void ReadScriptShakeEffect::DrawAtVslot04() {}
std::uint8_t ReadScriptShakeEffect::ReadyAtVslot08() const { return ready_; }
void ReadScriptShakeEffect::Finish() noexcept
{
    // 4A65F0 的 FuncInfo 5EF188 同为 EHFlags=5；仅 Finish 收紧，Tick 仍可抛。
    if (ready_)
    {
        // 4A65F0 的最后一帧不 ClearScreen，仍走普通 Draw 而不是 CopyDraw。
        DrawAt441980(previous_, drawing_, screen_, 0, 0);
        screen_.InvalidatedRectIfLoaded(nullptr);
    }
}
}
