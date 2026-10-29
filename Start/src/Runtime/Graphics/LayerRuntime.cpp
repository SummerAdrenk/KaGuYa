#include <Runtime/LayerRuntime.h>

#include <bit>

namespace KaGuYa::Start::Runtime
{
void DirtyRectCollection::Clear()
{
    body_.vptrAt00->clearAt00(&body_);
}

void DirtyRectCollection::Coalesce()
{
    body_.vptrAt00->coalesceAt04(&body_);
}

void DirtyRectCollection::Add(const RECT& rect)
{
    body_.vptrAt00->addRectAt08(&body_, &rect);
}

void DirtyRectCollection::Add(
    const long left,
    const long top,
    const long right,
    const long bottom)
{
    body_.vptrAt00->addLTRBAt0C(&body_, left, top, right, bottom);
}

void DirtyRectCollection::AddPositionSize(
    const long x,
    const long y,
    const long width,
    const long height)
{
    body_.vptrAt00->addXYWHAt10(&body_, x, y, width, height);
}

std::size_t DirtyRectCollection::Count() const
{
    auto* const body = const_cast<GuiDirtyRectNativeBody80h*>(&body_);
    return body->vptrAt00->countAt18(body);
}

std::span<const RECT> DirtyRectCollection::Rects() const
{
    auto* const body = const_cast<GuiDirtyRectNativeBody80h*>(&body_);
    const auto count = Count();
    return std::span<const RECT>(body->vptrAt00->rectsAt1C(body), count);
}

void LayerList::Register(LayerItem* const item)
{
    // 0x441610 总是在链尾追加，并不去重。
    items_.push_back(item);
}

void LayerList::Unregister(LayerItem* const item) noexcept
{
    // 0x441910 会删除所有指针相等的节点，而非只删除首个节点。
    items_.remove(item);
}

void LayerList::ClearRegisteredItems() noexcept
{
    items_.clear();
}

bool LayerList::Empty() const noexcept
{
    return items_.empty();
}

std::size_t LayerList::RegisteredItemCount() const noexcept
{
    return items_.size();
}

DirtyRectCollection& LayerList::DirtyRects() noexcept
{
    return dirtyRects_;
}

const DirtyRectCollection& LayerList::DirtyRects() const noexcept
{
    return dirtyRects_;
}

void LayerList::CompositeToSurface(
    const GraphicsSurfaceDestination destination, const RECT* const destinationClip)
{
    for (LayerItem* const item : items_)
    {
        if (item->Enabled())
            item->CompositeToSurface(destination, destinationClip);
    }
}

void LayerList::RenderDirtyRects(const InvalidatedRectCallback& invalidatedRect)
{
    dirtyRects_.Coalesce();
    const auto count = static_cast<std::uint32_t>(dirtyRects_.Count());
    // 441BC9为空时不取RECT指针、不Clear；非空时固定count/首指针，循环使用有符号比较。
    if (count == 0) return;
    auto& dirty = dirtyRects_.NativeBody();
    const auto* rects = dirty.vptrAt00->rectsAt1C(&dirty);
    for (std::int32_t index = 0; index < std::bit_cast<std::int32_t>(count); ++index)
    {
        const RECT& rect = rects[index];
        for (LayerItem* const item : items_)
        {
            if (item->Enabled())
                item->Draw(rect);
        }
        invalidatedRect(rect);
    }
    dirtyRects_.Clear();
}

void LayerList::ClearDirtyRects()
{
    dirtyRects_.Clear();
}

LayerItem::LayerItem(LayerList* const layerList) : layerList_(layerList)
{
    if (layerList_)
        layerList_->Register(this);
}

LayerItem::~LayerItem()
{
    if (layerList_)
        layerList_->Unregister(this);
}

bool LayerItem::Enabled() const noexcept
{
    return enabled_ != 0;
}

void LayerItem::SetEnabled(const bool enabled) noexcept
{
    enabled_ = enabled ? 1u : 0u;
}

std::uint8_t LayerItem::EnabledByteAt0C() const noexcept { return enabled_; }

void LayerItem::SetEnabledByteAt0C(const std::uint8_t value) noexcept
{
    // 4C22CC 将 AL 原样写到基类 +0C；非零字节不能先正规化成 1。
    enabled_ = value;
}

LayerList* LayerItem::RegisteredLayerList() const noexcept
{
    return layerList_;
}

void LayerItem::Draw(const RECT&)
{
    // 原基类虚表 5B1F0C 的 +00 = 441E40，机器码 ret 4，确为空实现。
}

void LayerItem::CompositeToSurface(GraphicsSurfaceDestination, const RECT*)
{
    // 原基类 +04 = 441B70，机器码 ret 8，确为空实现；基类没有 +14。
}
}
