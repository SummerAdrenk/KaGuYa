#include <Runtime/ReadScriptMoveEffectRuntime.h>
#include <Runtime/DrawManagerLayerRuntime.h>
#include <Runtime/FreeScrollRuntime.h>
#include <Runtime/GuiInputStateRuntime.h>
#include <Runtime/ParamsRuntimeProfile.h>
#include <Runtime/PatternLayerRuntime.h>
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/ScrVmRuntimeComposition.h>
#include <Runtime/UnSaveDataRuntime.h>

#include <algorithm>
#include <array>
#include <bit>
#include <stdexcept>
#include <xmmintrin.h>
#include <emmintrin.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
std::int32_t WrapSub(std::uint32_t a, std::int32_t b) noexcept
    { return std::bit_cast<std::int32_t>(a - std::uint32_t(b)); }
std::int32_t WrapAdd(std::int32_t a, std::int32_t b) noexcept
    { return std::bit_cast<std::int32_t>(std::uint32_t(a) + std::uint32_t(b)); }
float SignedFloat(std::int32_t value) noexcept
    { return _mm_cvtss_f32(_mm_cvtepi32_ps(_mm_cvtsi32_si128(value))); }
float UnsignedFloat(std::uint32_t value) noexcept
    { return _mm_cvtss_f32(_mm_cvtpd_ps(_mm_set_sd(static_cast<double>(value)))); }
float Sub(float a, float b) noexcept
    { return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(a), _mm_set_ss(b))); }
float Add(float a, float b) noexcept
    { return _mm_cvtss_f32(_mm_add_ss(_mm_set_ss(a), _mm_set_ss(b))); }
float Div(float a, float b) noexcept
    { return _mm_cvtss_f32(_mm_div_ss(_mm_set_ss(a), _mm_set_ss(b))); }

// PE 原 DWORD 表；没有 497900 那样的索引裁剪。越界显式失败，绝不补速度默认值。
constexpr std::array<std::int32_t, 4> kDenominatorsAt602648{64, 32, 16, 8};
constexpr std::array<std::int32_t, 4> kThresholdsAt602658{8, 12, 16, 32};
}

ReadScriptMoveEffectRuntime::ReadScriptMoveEffectRuntime(
    ReadScriptMoveEffectDependencies dependencies) : dependencies_(dependencies)
{
    // 5EEFB0 TryBlockMap：state 2..16h；成员初始化在这层 catch 之外。
    try { ConstructAt49C0F0(); }
    catch (...) { ready_ = 0; }
}

ReadScriptMoveEffectRuntime::~ReadScriptMoveEffectRuntime() { ReleaseItemsForward(); }
void ReadScriptMoveEffectRuntime::ReleaseItemsForward() noexcept
{
    for (auto& item : items_) item.reset(); // 498AF0 前向释放，不依赖 STL 析构遍历方向。
    items_.clear();
}
void ReadScriptMoveEffectRuntime::RequireLive() const
{
    if (finishAttempted_) throw std::logic_error("Move effect already finished or finish failed");
}
const ReadScriptMoveEffectItem& ReadScriptMoveEffectRuntime::ItemAt(std::size_t index) const
    { return *items_.at(index); }
std::uint8_t ReadScriptMoveEffectRuntime::ReadyAtVslot08() const
    { RequireLive(); return ready_; }
LayerWindowRuntime& ReadScriptMoveEffectRuntime::LayerWindow() const
{
    auto* layer = dynamic_cast<LayerWindowRuntime*>(dependencies_.layers.LayerAt418C10(0));
    if (!layer) throw std::logic_error("Move requires the actual owning layer window at slot0");
    return *layer;
}
void ReadScriptMoveEffectRuntime::Append(std::unique_ptr<ReadScriptMoveEffectItem> item)
{
    // 498DC0 按 max(size+1, capacity+capacity/2) 增长，不预分配全部条目。
    if (items_.size() == items_.capacity())
    {
        const auto capacity = items_.capacity();
        if (capacity >= items_.max_size()) throw std::length_error("Move item capacity exhausted");
        const auto grown = capacity > items_.max_size() - capacity / 2
            ? items_.max_size() : capacity + capacity / 2;
        items_.reserve((std::max)(items_.size() + 1, grown));
    }
    items_.push_back(std::move(item));
}

void ReadScriptMoveEffectRuntime::ConstructAt49C0F0()
{
    const SIZE size = dependencies_.sizes.ReadScreenSizeAt47CC00();
    viewport_ = RECT{0, 0, size.cx, size.cy};
    for (std::uint32_t index = 0;; ++index)
    {
        const auto* const game = dependencies_.gameSystem.CaptureGameSystemAt418480();
        if (!game) throw std::logic_error("Move GameSystem body is null");
        const auto count = static_cast<std::uint32_t>(game->startupConfigBytes.size());
        // 49C291 是有符号 jge；循环每次重取真实 GameSystem 数量。
        if (std::bit_cast<std::int32_t>(index) >=
            std::bit_cast<std::int32_t>(count)) break;
        auto* pattern = dynamic_cast<PatternLayerRuntime*>(LayerWindow().PatternLayerAt(index));
        if (!pattern) throw std::logic_error("Move pattern ref is absent or has the wrong owner");
        if (pattern->EnabledByteAt0C() == 0) continue;
        auto item = std::make_unique<ReadScriptMoveEffectItem>();
        item->pattern = pattern;
        item->currentX = SignedFloat(pattern->X());
        item->currentY = SignedFloat(pattern->Y());
        const auto pair = dependencies_.bundle.scriptStorage.EffectAt4AD140().PairAt(index);
        item->targetX = item->finalX = pair.first;
        item->targetY = item->finalY = pair.second;
        std::uint32_t mirror = 0;
        if (pattern->FilterMode() == 11)
        {
            if (pattern->FilterArguments().empty())
                throw std::out_of_range("Move mirror requires its original flags DWORD");
            mirror = pattern->FilterArguments().front();
        }
        if ((mirror & 1u) != 0)
        {
            item->currentX = Sub(UnsignedFloat(dependencies_.programWidthAt18), item->currentX);
            item->targetX = WrapSub(dependencies_.programWidthAt18, item->targetX);
        }
        if ((mirror & 2u) != 0)
        {
            item->currentY = Sub(UnsignedFloat(dependencies_.programHeightAt1C), item->currentY);
            item->targetY = WrapSub(dependencies_.programHeightAt1C, item->targetY);
        }
        if (item->currentX != SignedFloat(item->targetX) ||
            item->currentY != SignedFloat(item->targetY))
        {
            ComputeStepsAt49C5D8(*item);
            if (item->directions == 0) continue;
            if (!dependencies_.input.ReadHeldAtVslot00(6) &&
                (dependencies_.unsave.FlagsAt14() & 2u) == 0 && dependencies_.save.valueAt6F != 0)
                CaptureItemAt49CDE4(*item, mirror);
        }
        Append(std::move(item)); // 静止项和被门控的无截图移动项均保留。
    }
    LayerWindow().SetEnabledByteAt0C(0);
    ready_ = 1;
}

void ReadScriptMoveEffectRuntime::ComputeStepsAt49C5D8(ReadScriptMoveEffectItem& item)
{
    float dx = 0, dy = 0;
    const float tx = SignedFloat(item.targetX), ty = SignedFloat(item.targetY);
    if (item.currentX != tx)
    {
        if (tx > item.currentX) { dx = Sub(tx, item.currentX); item.directions |= 1u; }
        else { dx = Sub(item.currentX, tx); item.directions |= 2u; }
    }
    if (item.currentY != ty)
    {
        if (ty > item.currentY) { dy = Sub(ty, item.currentY); item.directions |= 4u; }
        else { dy = Sub(item.currentY, ty); item.directions |= 8u; }
    }
    float denominator = SignedFloat(kDenominatorsAt602648.at(dependencies_.save.valueAt70));
    const float distance = dy > dx ? dy : dx;
    while (!(SignedFloat(kThresholdsAt602658.at(dependencies_.save.valueAt70)) >
        Div(distance, denominator)))
        denominator = Add(denominator,
            SignedFloat(kDenominatorsAt602648.at(dependencies_.save.valueAt70)));
    item.stepX = Div(dx, denominator);
    item.stepY = Div(dy, denominator);
}

bool ReadScriptMoveEffectRuntime::TickAtVslot00()
{
    RequireLive();
    bool moved = false;
    for (auto& owner : items_)
    {
        auto& item = *owner;
        const float tx = SignedFloat(item.targetX), ty = SignedFloat(item.targetY);
        if (item.currentX == tx && item.currentY == ty) continue;
        if ((item.directions & 2u) != 0)
        {
            item.currentX = Sub(item.currentX, item.stepX);
            if (tx > item.currentX) item.currentX = tx;
        }
        else if ((item.directions & 1u) != 0)
        {
            item.currentX = Add(item.currentX, item.stepX);
            if (item.currentX > tx) item.currentX = tx;
        }
        if ((item.directions & 8u) != 0)
        {
            item.currentY = Sub(item.currentY, item.stepY);
            if (ty > item.currentY) item.currentY = ty;
        }
        else if ((item.directions & 4u) != 0)
        {
            item.currentY = Add(item.currentY, item.stepY);
            if (item.currentY > ty) item.currentY = ty;
        }
        moved = true;
        if (item.directions == 0) continue;
        if (!item.snapshot) return false; // 先更新浮点状态，再立即退出，不处理后面的条目。
        item.pattern->SetPosition(_mm_cvtt_ss2si(_mm_set_ss(item.currentX)),
            _mm_cvtt_ss2si(_mm_set_ss(item.currentY)));
    }
    return moved; // 刚到终点的帧仍为 true，下一帧才为 false。
}

void ReadScriptMoveEffectRuntime::RenderAndInvalidate()
{
    auto& registered = dependencies_.layers.RegisteredLayers();
    registered.DirtyRects().Add(viewport_);
    registered.RenderDirtyRects([this](const RECT& rect) {
        dependencies_.screen.InvalidatedRectIfLoaded(&rect);
    });
    dependencies_.screen.InvalidatedRectIfLoaded(nullptr);
}
void ReadScriptMoveEffectRuntime::DrawAtVslot04()
{
    RequireLive();
    for (auto& owner : items_)
    {
        const auto& item = *owner;
        const auto x = item.pattern->X(), y = item.pattern->Y();
        if (item.directions != 0)
        {
            if (!item.snapshot) throw std::logic_error("Move draw requires its snapshot ref");
            if (item.snapshot->Handle() != 0)
                static_cast<void>(dependencies_.drawing.SurfaceDrawRaw(item.snapshot->Handle(),
                    WrapAdd(x, item.offsetX), WrapAdd(y, item.offsetY), nullptr, nullptr, nullptr, 0));
        }
        else
        {
            // 4B5605 的 vslot00(null)：482C60 选择缓存面，不能用空 RECT 替代 null clip。
            const auto* source = item.pattern->ProcessedSurfaceAt64();
            if (!source) source = &item.pattern->MainSurfaceAt18();
            if (source->Handle() != 0)
                static_cast<void>(dependencies_.drawing.SurfaceDrawRaw(source->Handle(),
                    0, 0, nullptr, nullptr, nullptr, 0));
        }
    }
    RenderAndInvalidate();
}

void ReadScriptMoveEffectRuntime::Finish() noexcept
{
    // 5EF094 的 EHFlags bit2 在 CRT 搜索阶段直接 terminate，不补 catch 或回滚。
    RequireLive();
    finishAttempted_ = true;
    if (ready_ != 0)
    {
        for (auto& owner : items_)
        {
            auto& item = *owner;
            if (item.directions == 0) continue;
            // 4A431D..4351：先提交未镜像的原目标，再失效与重绘；异常不可回滚坐标。
            item.pattern->SetPosition(item.finalX, item.finalY);
            const auto& main = item.pattern->MainSurfaceAt18();
            item.pattern->DirtyRects().Add(RECT{0, 0, main.Width(), main.Height()});
            item.pattern->RefreshMainAndPropagateAt443B00();
        }
        LayerWindow().SetEnabledByteAt0C(1);
        RenderAndInvalidate();
    }
    ReleaseItemsForward();
    std::vector<std::unique_ptr<ReadScriptMoveEffectItem>>().swap(items_);
}

ReadScriptMoveEffectFactoryRuntime::ReadScriptMoveEffectFactoryRuntime(
    ReadScriptMoveEffectDependencies dependencies) noexcept : dependencies_(dependencies) {}
std::unique_ptr<IReadScriptEffectRuntime> ReadScriptMoveEffectFactoryRuntime::CreateMoveAt49C0F0()
{
    // 外层 new 失败必须传播，不能伪造一个 ready=0 的空对象。
    return std::make_unique<ReadScriptMoveEffectRuntime>(dependencies_);
}
}
