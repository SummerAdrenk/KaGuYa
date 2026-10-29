#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace KaGuYa::Start::Runtime
{
class DrawManagerRuntime;
class IGraphicsTransitApi;
struct DrawManagerNativeBody2Ch;
struct TransitOperation;

struct DrawTransitionViewMethods final
{
    std::uint8_t (*render)(const void*, const TransitOperation&, IGraphicsTransitApi*);
};

// 固定完整CDrawManager receiver，而非构造时解析出的CTransit或root槽。
// 只借正文/分派表/外部Graphics；不增持manager、不复制Ref、函数表、随机表或packet。
// IDA 4C39E3..4C3A14：每次调用重新解析同一manager的+20/+24/+28。
class DrawTransitionView final
{
public:
    DrawTransitionView() noexcept = default;
    DrawTransitionView(std::nullptr_t) noexcept {}
    DrawTransitionView(const DrawManagerNativeBody2Ch*, IGraphicsTransitApi* = nullptr) noexcept;
    DrawTransitionView(const DrawManagerNativeBody2Ch&, IGraphicsTransitApi&) noexcept;
    explicit DrawTransitionView(const DrawManagerRuntime&) noexcept;

    explicit operator bool() const noexcept { return body_ != nullptr; }
    [[nodiscard]] const void* Receiver() const noexcept { return body_; }
    [[nodiscard]] std::uint8_t RenderTransitionAt4C39E0(const TransitOperation& operation) const
    { return Methods().render(body_, operation, graphics_); }

private:
    const DrawTransitionViewMethods& Methods() const;
    const void* body_ = nullptr;
    const DrawTransitionViewMethods* methods_ = nullptr;
    IGraphicsTransitApi* graphics_ = nullptr;
};
static_assert(std::is_trivially_copyable_v<DrawTransitionView>);
static_assert(std::is_standard_layout_v<DrawTransitionView>);
static_assert(sizeof(DrawTransitionView) == 3 * sizeof(void*));
}
