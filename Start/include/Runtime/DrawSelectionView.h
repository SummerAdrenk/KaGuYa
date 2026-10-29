#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace KaGuYa::Start::Runtime
{
class FrameLayerItem;
class SelectWindowLayerRuntime;
class ISelectWindowStateServices;
class IGraphicsTextApi;
class IGraphicsTextGlyphApi;
struct SelectWindowNativeBody6Ch;
struct SelectWindowNativeWorkflowServices;
struct DrawGuiNativeServices;

// 只借外部choices/text/glyphs，不含Select/Dialog/画布/候选项或任何业务快照。
struct DrawSelectionControlledServices final
{
    ISelectWindowStateServices* choices = nullptr;
    IGraphicsTextApi* text = nullptr;
    IGraphicsTextGlyphApi* glyphs = nullptr;
};

struct DrawSelectionViewMethods final
{
    void (*rebuild)(void*, SelectWindowNativeWorkflowServices*, const DrawSelectionControlledServices&);
    std::int32_t (*tick)(void*, std::uint8_t, SelectWindowNativeWorkflowServices*,
        const DrawSelectionControlledServices&);
    std::uint8_t (*enabled)(void*);
};

// 按值固定receiver/方法表/服务指针元数据；不取根、不增持、不拥有正文。
// receiver及所借服务须覆盖使用期；Controlled context本身不必保活。
// 默认未绑定协议明确拒绝；显式Native空正文仍交原叶/原字段，不加现代null保护。
class DrawSelectionView final
{
public:
    DrawSelectionView() noexcept = default;
    DrawSelectionView(std::nullptr_t) noexcept {}
    DrawSelectionView(SelectWindowNativeBody6Ch*, SelectWindowNativeWorkflowServices* = nullptr) noexcept;
    DrawSelectionView(SelectWindowNativeBody6Ch&, SelectWindowNativeWorkflowServices&) noexcept;
    DrawSelectionView(FrameLayerItem*, DrawSelectionControlledServices = {}) noexcept;
    DrawSelectionView(SelectWindowLayerRuntime&, DrawSelectionControlledServices) noexcept;
    DrawSelectionView(SelectWindowLayerRuntime&, ISelectWindowStateServices&,
        IGraphicsTextApi&, IGraphicsTextGlyphApi&) noexcept;

    // 仅按值复制借用指针；空gui沿用调用方外层scope，不替代必需的业务服务。
    [[nodiscard]] DrawSelectionView WithNativeGuiServices(DrawGuiNativeServices* gui) const noexcept
    { auto result = *this; result.gui_ = gui; return result; }

    explicit operator bool() const noexcept { return body_ != nullptr; }
    const void* Receiver() const noexcept { return body_; }
    // IDA 4369E0/4375BA：catch和部分提交归既有实现；view不吞异常或重做选择算法。
    void RebuildAt4369E0() const;
    // IDA 4BBFD8完整BYTE与0比较；4BC1D0即使blocked也晚取声音并Refresh。
    [[nodiscard]] std::int32_t TickAt4BBFD0(std::uint8_t blocked) const;
    // IDA CSelectWin继承CLayerItem +0C BYTE；原生直接读取，受控仍须拒绝非CSelectWin。
    [[nodiscard]] std::uint8_t EnabledByteAt0C() const { return Methods().enabled(body_); }

private:
    const DrawSelectionViewMethods& Methods() const;
    void* body_ = nullptr;
    const DrawSelectionViewMethods* methods_ = nullptr;
    SelectWindowNativeWorkflowServices* native_ = nullptr;
    DrawSelectionControlledServices controlled_{};
    DrawGuiNativeServices* gui_ = nullptr;
};
static_assert(std::is_trivially_copyable_v<DrawSelectionControlledServices>);
static_assert(std::is_trivially_copyable_v<DrawSelectionView>);
static_assert(std::is_standard_layout_v<DrawSelectionView>);
static_assert(sizeof(DrawSelectionView) == 7 * sizeof(void*));
}
