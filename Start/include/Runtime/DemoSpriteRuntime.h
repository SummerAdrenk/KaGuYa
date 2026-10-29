#pragma once

#include <Runtime/LayerRuntime.h>
#include <Runtime/SpriteRuntime.h>

#include <cstdint>

namespace KaGuYa::Start::Runtime
{
struct DemoSpriteMotionState final
{
    std::uint32_t direction = 0; // CDemoSprite+0xA0
    std::uint32_t fixedX = 0;
    std::uint32_t fixedY = 0;
    std::int32_t targetX = 0;
    std::int32_t targetY = 0;
    std::uint32_t stepX = 0;
    std::uint32_t stepY = 0;

    bool operator==(const DemoSpriteMotionState&) const = default;
};

// 4BF5A0 的 CDemoSprite 与 4A7690 的逐实例更新部分。
// 资源偏移与命令定位分别属于 CSprite+0x1C/+0x20 和 +0x2C/+0x30。
// 正文仍含现代 SpriteRuntime，不是原 BCh 布局；Ref 容器不能改变这一 ABI 边界。
class DemoSpriteRuntime final
{
public:
    explicit DemoSpriteRuntime(IGraphicsSurfaceApi& graphicsApi) noexcept;

    [[nodiscard]] SpriteRuntime& Sprite() noexcept;
    [[nodiscard]] const SpriteRuntime& Sprite() const noexcept;
    [[nodiscard]] const DemoSpriteMotionState& Motion() const noexcept;
    [[nodiscard]] RECT Bounds() const noexcept;
    [[nodiscard]] std::int32_t PositionX() const noexcept;
    [[nodiscard]] std::int32_t PositionY() const noexcept;
    [[nodiscard]] std::uint8_t VisibleByte() const noexcept;
    void SetVisibleByte(std::uint8_t value) noexcept;

    // type 9 只写坐标，不取消已经启动的运动，不主动追加 dirty rect。
    void SetPosition(std::int32_t x, std::int32_t y) noexcept;
    // 4BFD18..4BFE1B：caller 已完成两次 resolve、旧 RECT 和 live end 比较。
    // 此叶只用已捕获参数写 motion，不重复相等检查，不读取 Bounds，不写 dirty。
    void CommitMotionAt4BFD18(
        std::int32_t duration,
        std::int32_t startX, std::int32_t startY,
        std::int32_t endX, std::int32_t endY);
    // 现代便捷入口；native Move 必须由 caller 分开检查、提交和追加旧 RECT。
    void BeginMotion(
        std::int32_t duration,
        std::int32_t startX, std::int32_t startY,
        std::int32_t endX, std::int32_t endY,
        DirtyRectCollection& dirtyRects);

    // 只更新可见 Sprite。运动失效区域先裁到 sequence bounds；动画区域不在此裁剪。
    void Update(const RECT& sequenceBounds, DirtyRectCollection& dirtyRects);

private:
    SpriteRuntime sprite_;
    DemoSpriteMotionState motion_{};
};
}
