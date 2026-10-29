#pragma once

#include <Platform/InputViews.h>

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace KaGuYa::Start::Runtime
{
class FrameLayerItem;
class OpacityWindowRuntime;
class IOpacityWindowInputSource;
class IOpacityDialogUpdateServices;
struct OpacityWindowNativeBody30h;
struct DrawGuiNativeServices;

// 只借显式输入view和更新服务；不复制输入、Dialog、dirty或任何业务字段。
struct DrawOpacityControlledServices final
{
    Platform::MouseView mouse;
    Platform::KeyboardView keyboard;
    IOpacityDialogUpdateServices* update = nullptr;
};

struct DrawOpacityViewMethods final
{
    bool (*tick)(void*, IOpacityWindowInputSource*, const DrawOpacityControlledServices&);
    void (*reset)(void*);
    std::uint8_t (*enabled)(void*);
};

// 按值固定receiver/方法表/服务元数据，不取根、不增持、不拥有正文。
// receiver及所借服务须覆盖使用期，Controlled context本身无需保活。
// 默认协议未绑定明确拒绝；显式Native空正文交原叶/原字段，不补现代null保护。
class DrawOpacityView final
{
public:
    DrawOpacityView() noexcept = default;
    DrawOpacityView(std::nullptr_t) noexcept {}
    DrawOpacityView(OpacityWindowNativeBody30h*, IOpacityWindowInputSource* = nullptr) noexcept;
    DrawOpacityView(OpacityWindowNativeBody30h&, IOpacityWindowInputSource&) noexcept;
    DrawOpacityView(FrameLayerItem*, DrawOpacityControlledServices = {}) noexcept;
    DrawOpacityView(OpacityWindowRuntime&, DrawOpacityControlledServices) noexcept;
    DrawOpacityView(OpacityWindowRuntime&, Platform::MouseView, Platform::KeyboardView,
        IOpacityDialogUpdateServices&) noexcept;

    // 仅按值复制借用指针；空gui沿用调用方外层scope，不替代必需的输入服务。
    [[nodiscard]] DrawOpacityView WithNativeGuiServices(DrawGuiNativeServices* gui) const noexcept
    { auto result = *this; result.gui_ = gui; return result; }

    explicit operator bool() const noexcept { return body_ != nullptr; }
    const void* Receiver() const noexcept { return body_; }
    // IDA 4BE534/4BE586：一次/两次418210由既有Native叶决定；4BE5BD/4BE600返回AL 0/1。
    [[nodiscard]] bool TickAt4BE530() const;
    // IDA 4B1ACC/4B372C读取当前vslot18；47EE40只是COpacityWin原表叶，不固定调用叶。
    void ResetAtVslot18() const;
    // IDA 47EE40写CLayerItem+0C BYTE；查询完整实时字节，不归一化、不依赖服务。
    [[nodiscard]] std::uint8_t EnabledByteAt0C() const { return Methods().enabled(body_); }

private:
    const DrawOpacityViewMethods& Methods() const;
    void* body_ = nullptr;
    const DrawOpacityViewMethods* methods_ = nullptr;
    IOpacityWindowInputSource* native_ = nullptr;
    DrawOpacityControlledServices controlled_{};
    DrawGuiNativeServices* gui_ = nullptr;
};
static_assert(std::is_trivially_copyable_v<DrawOpacityControlledServices>);
static_assert(std::is_trivially_copyable_v<DrawOpacityView>);
static_assert(std::is_standard_layout_v<DrawOpacityView>);
static_assert(sizeof(DrawOpacityView) == 9 * sizeof(void*));
}
