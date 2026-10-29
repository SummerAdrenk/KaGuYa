#include <Runtime/ReadScriptMoveEffectRuntime.h>
#include <Runtime/PatternLayerRuntime.h>
#include <Runtime/WaitMarkServicesRuntime.h>

#include <bit>
#include <stdexcept>
#include <emmintrin.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
std::int32_t Sub(std::int32_t a, std::int32_t b) noexcept
    { return std::bit_cast<std::int32_t>(std::uint32_t(a) - std::uint32_t(b)); }
float SubExtent(float value, std::int32_t extent) noexcept
{
    return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(value),
        _mm_cvtepi32_ps(_mm_cvtsi32_si128(extent))));
}
RECT SpriteUnionAt4ADDE0(const PatternLayerRuntime& pattern)
{
    RECT result{};
    for (std::size_t index = 0; index < pattern.SpriteSlotCount(); ++index)
    {
        const auto& sprite = pattern.SpriteAt(index);
        if (sprite.LogicalName().empty() || static_cast<std::uint32_t>(sprite.LoadMode()) > 2u)
            continue;
        RECT rect{0, 0, sprite.Width(), sprite.Height()};
        ::OffsetRect(&rect, sprite.X(), sprite.Y());
        ::UnionRect(&result, &result, &rect);
    }
    return result;
}
}

void ReadScriptMoveEffectRuntime::CaptureItemAt49CDE4(
    ReadScriptMoveEffectItem& item, const std::uint32_t mirror)
{
    auto snapshot = std::make_unique<RuntimeGraphicsSurface>(dependencies_.surfaces);
    auto& pattern = *item.pattern;
    const RECT bounds = SpriteUnionAt4ADDE0(pattern);
    const auto width = Sub(bounds.right, bounds.left), height = Sub(bounds.bottom, bounds.top);
    // 415010 的 false 不取消 CSurface 对象；空 handle 与空 Ref 是不同状态。
    static_cast<void>(snapshot->Create(width, height, pattern.MainSurfaceAt18().HasAlpha(), true));
    WaitMarkRendererRuntime spriteRenderer(dependencies_.drawing);
    for (std::size_t index = 0; index < pattern.SpriteSlotCount(); ++index)
    {
        auto& sprite = pattern.SpriteAt(index);
        if (sprite.LogicalName().empty()) continue;
        const auto x = sprite.X(), y = sprite.Y();
        sprite.SetOrigin(Sub(x, bounds.left), Sub(y, bounds.top));
        // 复用已完整实现的 46B390 静态/AN 映射渲染；此处不调用 WaitMark owner。
        // 只在正常返回后恢复原点。绘制抛出时保留原程序可见的中间坐标。
        static_cast<void>(spriteRenderer.DrawToSurface(sprite, snapshot->Handle(), nullptr));
        sprite.SetOrigin(x, y);
    }
    pattern.FilterMoveSnapshotAt49D00C(snapshot);
    item.snapshot = std::move(snapshot);
    item.offsetX = bounds.left;
    item.offsetY = bounds.top;
    if ((mirror & 1u) != 0)
    {
        item.offsetX = Sub(0, item.offsetX);
        item.currentX = SubExtent(item.currentX, width);
        item.targetX = Sub(item.targetX, width);
    }
    if ((mirror & 2u) != 0)
    {
        item.offsetY = Sub(0, item.offsetY);
        item.currentY = SubExtent(item.currentY, height);
        item.targetY = Sub(item.targetY, height);
    }
}

// 此方法操作本 Pattern 已有的 loader/filter/+54 成员，不创建 Move 私有替身。
void PatternLayerRuntime::FilterMoveSnapshotAt49D00C(
    std::unique_ptr<RuntimeGraphicsSurface>& snapshot)
{
    if (filterMode_ <= 0 || filterMode_ == 10) return;
    if (filterMode_ == 3)
    {
        auto clone = std::make_unique<RuntimeGraphicsSurface>(*graphicsApi_);
        // 44454C在clone分配后才读源对象尺寸，不提前阻止其它mode的合法空Ref路径。
        if (!snapshot) throw std::logic_error("Move filter clone source ref is absent");
        // 444470(a3=0) 仍以 clear=1 创建，只是不复制像素；false 清空 clone Ref。
        if (!clone->Create(snapshot->Width(), snapshot->Height(), snapshot->HasAlpha(), true))
            clone.reset();
        if (!clone)
        {
            // 442B7D/442B9F 先校验参数，失败由 442900 catch 返回 false；
            // 这条路径尚未解引用目标，随后 465870 确实把截图赋为空 Ref。
            if (filterArguments_.size() != 2 || filterArguments_[0] > 1u)
            {
                snapshot.reset();
                return;
            }
            throw std::runtime_error("Move filter clone ref creation failed");
        }
        static_cast<void>(ApplyPatternPixelFilterAt442900(*filters_, filterMode_,
            filterArguments_, *clone, snapshot.get(), temporaryFilterSurface_.get(), nullptr));
        snapshot = std::move(clone); // 465870：释放原截图后才转交 clone。
    }
    else if (filterMode_ == 6)
    {
        // 49CF49保存参数地址；49CF4F取Params，49CF5C读ID，49CF5E才解Pattern。
        const std::span<const std::uint32_t> operands(filterArguments_);
        const auto root = filterResources_->CaptureParamsForNames();
        if (operands.empty())
            throw std::out_of_range("Move resource filter requires a resource ID");
        const auto id = std::bit_cast<std::int32_t>(operands.front());
        const auto names = root.CapturePatternNames();
        const char* const rawName = names.FindNameAt4AD1B0(id);
        const std::string name(rawName ? rawName : "");
        if (!temporaryFilterSurface_)
            temporaryFilterSurface_ = std::make_shared<RuntimeGraphicsSurface>(*graphicsApi_);
        if (filterResources_->LoadInto(*temporaryFilterSurface_, name))
            static_cast<void>(ApplyPatternPixelFilterAt442900(*filters_, filterMode_,
                filterArguments_, snapshot.get(), nullptr, temporaryFilterSurface_.get(), nullptr));
        // 正常 true/false 均解绑；加载/诊断抛出则保留 Pattern+54，不能用 scope guard 清掉。
        temporaryFilterSurface_.reset();
    }
    else
        static_cast<void>(ApplyPatternPixelFilterAt442900(*filters_, filterMode_,
            filterArguments_, snapshot.get(), nullptr, temporaryFilterSurface_.get(), nullptr));
}
}
