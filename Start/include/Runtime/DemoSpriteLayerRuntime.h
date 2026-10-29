#pragma once

#include <Runtime/DemoSpriteRuntime.h>
#include <Runtime/EnvironmentRootRuntime.h>

namespace KaGuYa::Start::Runtime
{
using DemoSpriteReferenceVector = EnvironmentVectorStorage<YtlReferenceSlotRuntime>;
static_assert(sizeof(DemoSpriteReferenceVector) == 12);

// 原 453F20 的慢插入；source/旧范围在分配之后重取，旧块删除失败不回滚新块。
YtlReferenceSlotRuntime* InsertDemoSpriteReferenceAt453F20(
    DemoSpriteReferenceVector& storage, YtlReferenceSlotRuntime* position,
    const YtlReferenceSlotRuntime& source,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
void ReleaseDemoSpriteReferenceAt460260(YtlReferenceSlotRuntime& reference) noexcept;
void ReleaseDemoSpriteReferenceCachedAt4BF945(YtlReferenceSlotRuntime& reference,
    YtlReferenceControlRuntime* cachedControl) noexcept;
void DestroyDemoSpriteReferenceVectorAt4615F0(DemoSpriteReferenceVector& storage,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory()) noexcept;

// CDemoSeq 的 +0x38 sprite 数组、+0x44 dirty rect 和 +0x50 屏幕矩形。
class DemoSpriteLayerRuntime final
{
public:
    // memory 的 body/control 分配必须兼容全局 sized delete；不在 18h control 塞 allocator。
    DemoSpriteLayerRuntime(IGraphicsSurfaceApi& graphics, const RECT& bounds,
        IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory()) noexcept;
    ~DemoSpriteLayerRuntime() noexcept;
    DemoSpriteLayerRuntime(const DemoSpriteLayerRuntime&) = delete;
    DemoSpriteLayerRuntime& operator=(const DemoSpriteLayerRuntime&) = delete;

    // 只创建并追加一个 Ref；Load caller 每轮重新读取 command slot 和当前 Count。
    void AppendOneDemoSpriteAt4BF864();
    // 4AD170 无 count 检查，base==0 才返回 null；安全边界由实际访问 caller 负责。
    [[nodiscard]] DemoSpriteRuntime* ResolveAt4AD170(std::uint32_t index) noexcept;
    [[nodiscard]] const DemoSpriteRuntime* ResolveAt4AD170(std::uint32_t index) const noexcept;
    [[nodiscard]] DemoSpriteReferenceVector& NativeSprites() noexcept { return sprites_; }
    [[nodiscard]] const DemoSpriteReferenceVector& NativeSprites() const noexcept { return sprites_; }

    // 仅现代便捷入口；native Load 不可把一次捕获的 slot 交给它完成整个增长循环。
    DemoSpriteRuntime& EnsureSlot(std::size_t index);
    [[nodiscard]] DemoSpriteRuntime& At(std::size_t index);
    [[nodiscard]] const DemoSpriteRuntime& At(std::size_t index) const;
    [[nodiscard]] std::size_t Count() const noexcept;
    [[nodiscard]] bool AnyMoving() const noexcept;
    [[nodiscard]] DirtyRectCollection& DirtyRects() noexcept;

    void UpdateSpritesAndRedraw(IGraphicsSurfaceCompositionApi& composition,
        IGraphicsScreenUpdateApi& screen);
    void Redraw(IGraphicsSurfaceCompositionApi& composition,
        IGraphicsScreenUpdateApi& screen);
    // 4A7F00：生成转场新画布，不推进动画、不裁到 dirty、不清 dirty。
    void ComposeInto(RuntimeGraphicsSurface& target,
        IGraphicsSurfaceCompositionApi& composition, IGraphicsScreenUpdateApi& screen);

private:
    IGraphicsSurfaceApi& graphics_;
    RECT bounds_;
    IEnvironmentRootMemory& memory_;
    DemoSpriteReferenceVector sprites_{};
    DirtyRectCollection dirty_;
};
}
