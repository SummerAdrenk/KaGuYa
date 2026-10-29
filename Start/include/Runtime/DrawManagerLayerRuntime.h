#pragma once

#include <Runtime/LayerRuntime.h>
#include <Runtime/DrawLayersView.h>

#include <cstdint>
#include <memory>
#include <vector>

namespace KaGuYa::Start::Runtime
{
// 原 ILayerItem 基类没有 +14。只有进入 CDrawManager 帧数组的具体对象
// 实现这一现代接口，不给普通注册图层凭空增加原版不存在的虚槽。
class FrameLayerItem : public LayerItem
{
public:
    using LayerItem::LayerItem;
    virtual void AdvanceOwnedFrame() = 0;
};

// 七层准备只接受实际实现 vslot +10 的 owner，不把 SetEnabled 当成替代实现。
class ILayerPrepareTarget
{
public:
    virtual ~ILayerPrepareTarget() = default;
    [[nodiscard]] virtual std::uint8_t SetRequestedEnabledAtVslot10(
        std::uint8_t requested, std::int32_t secondArgument) = 0;
};

// 47E940 的 owning +18；现代参数只注入原全局 Graphics，不改变原虚槽业务。
class ILayerResetTarget
{
public:
    virtual ~ILayerResetTarget() = default;
    virtual void ResetOwnedLayerAtVslot18(IGraphicsWindowSurfaceApi& graphics) = 0;
};

// CDrawManager 的 +08 注册表和 +14 拥有数组子系统；不包含 +20 CTransit。
// 完整 manager 仍需按 462BB0 在释放本对象之前释放 CTransit。
class DrawManagerLayerRuntime final
{
public:
    DrawManagerLayerRuntime() = default;
    ~DrawManagerLayerRuntime();
    DrawManagerLayerRuntime(const DrawManagerLayerRuntime&) = delete;
    DrawManagerLayerRuntime& operator=(const DrawManagerLayerRuntime&) = delete;

    [[nodiscard]] LayerList& RegisteredLayers() noexcept;
    [[nodiscard]] const LayerList& RegisteredLayers() const noexcept;
    [[nodiscard]] std::size_t OwnedLayerCount() const noexcept;
    [[nodiscard]] FrameLayerItem* LayerAt418C10(std::uint32_t index) const;
    void AppendOwnedLayerAt46A880(std::unique_ptr<FrameLayerItem> layer);
    void AdvanceFramesAt46B240();
    void ResetOwnedLayersAt47E940(IGraphicsWindowSurfaceApi& graphics);
    // 绑定已存活的同一 manager；逐槽重新索引，不预检后续槽，不短路 AL 的按位 OR。
    [[nodiscard]] std::uint8_t PrepareLayersAt46A980(std::int32_t mode = 0);
    [[nodiscard]] std::uint8_t PrepareLayersAt46AD20(std::int32_t mode);

private:
    [[nodiscard]] std::uint8_t RequestOwnedLayerAtVslot10(
        std::uint32_t index, std::uint8_t requested, std::int32_t argument = 0);
    LayerList registeredLayers_;
    std::vector<std::unique_ptr<FrameLayerItem>> ownedLayers_;
};

// 已核的413BE0调用点各自捕获；接口不拥有或缓存某一代manager。
// 当前返回现代业务层视图，不把它冒充原2Ch/18h正文。
using IDrawManagerLayerSource=IDrawLayersViewSource;
}
