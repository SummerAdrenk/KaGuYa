#pragma once

#include <Runtime/GraphicsSurfaceRuntime.h>
#include <Runtime/GuiDirtyRectNative.h>

#include <Windows.h>

#include <cstddef>
#include <cstdint>
#include <functional>
#include <list>
#include <span>

namespace KaGuYa::Start::Runtime
{
class DirtyRectCollection final
{
public:
    static constexpr std::size_t MaximumResidentRectCount = 5;

    DirtyRectCollection() noexcept {}

    void Clear();
    void Coalesce();

    void Add(const RECT& rect);
    void Add(long left, long top, long right, long bottom);
    void AddPositionSize(long x, long y, long width, long height);

    [[nodiscard]] std::size_t Count() const;
    [[nodiscard]] std::span<const RECT> Rects() const;
    [[nodiscard]] GuiDirtyRectNativeBody80h& NativeBody() noexcept { return body_; }
    [[nodiscard]] const GuiDirtyRectNativeBody80h& NativeBody() const noexcept { return body_; }

private:
    // 441680/442060 与 GUI 共用同一正文和虚槽，不能另存数量或另写合并算法。
    GuiDirtyRectNativeBody80h body_;
};
static_assert(sizeof(DirtyRectCollection) == 0x80);

class LayerItem;

class LayerList final
{
public:
    using InvalidatedRectCallback = std::function<void(const RECT&)>;

    LayerList() = default;
    ~LayerList() = default;

    LayerList(const LayerList&) = delete;
    LayerList& operator=(const LayerList&) = delete;

    void Register(LayerItem* item);
    void Unregister(LayerItem* item) noexcept;
    void ClearRegisteredItems() noexcept;

    [[nodiscard]] bool Empty() const noexcept;
    [[nodiscard]] std::size_t RegisteredItemCount() const noexcept;

    [[nodiscard]] DirtyRectCollection& DirtyRects() noexcept;
    [[nodiscard]] const DirtyRectCollection& DirtyRects() const noexcept;

    // 441B20：按注册顺序向 enabled 图层分发 +04 合成，不推进帧。
    void CompositeToSurface(
        GraphicsSurfaceDestination destination, const RECT* destinationClip);
    void RenderDirtyRects(const InvalidatedRectCallback& invalidatedRect);
    void ClearDirtyRects();

private:
    std::list<LayerItem*> items_;
    DirtyRectCollection dirtyRects_;
};

class LayerItem
{
public:
    explicit LayerItem(LayerList* layerList);
    virtual ~LayerItem();

    LayerItem(const LayerItem&) = delete;
    LayerItem& operator=(const LayerItem&) = delete;

    [[nodiscard]] bool Enabled() const noexcept;
    void SetEnabled(bool enabled) noexcept;
    [[nodiscard]] std::uint8_t EnabledByteAt0C() const noexcept;
    void SetEnabledByteAt0C(std::uint8_t value) noexcept;
    [[nodiscard]] LayerList* RegisteredLayerList() const noexcept;

    virtual void Draw(const RECT& dirtyRect);
    // 原 +04 接收 CSurface 对象指针；目标引用跨图层/子控件保持正文身份。
    // 这是现代分发契约，不是原二进制 vtable ABI；nullptr clip 不附加裁剪。
    virtual void CompositeToSurface(
        GraphicsSurfaceDestination destination, const RECT* destinationClip);

private:
    std::uint8_t enabled_ = 1;
    LayerList* layerList_ = nullptr;
};
}
