#include <Runtime/DrawSelectionView.h>
#include <Runtime/SelectWindowLayerRuntime.h>

#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
SelectWindowLayerRuntime& Selection(void* body)
{
    // 保留现有slot5消费者的受控类型拒绝；构造借用view不提前查询或强转Native。
    auto* selection = dynamic_cast<SelectWindowLayerRuntime*>(static_cast<FrameLayerItem*>(body));
    if (!selection) throw std::logic_error("controlled Draw slot5 is not CSelectWin");
    return *selection;
}
ISelectWindowStateServices& Choices(const DrawSelectionControlledServices& services)
{
    if (!services.choices) throw std::logic_error("controlled Draw Selection choices services were not bound");
    return *services.choices;
}
constexpr DrawSelectionViewMethods methods{
    [](void* body, SelectWindowNativeWorkflowServices*, const DrawSelectionControlledServices& services) {
        auto& selection = Selection(body);
        auto& choices = Choices(services);
        if (!services.text || !services.glyphs)
            throw std::logic_error("controlled Draw Selection text/glyph services were not bound");
        // IDA 4369E0：不另建Dialog、状态或算法，只调用已有受控入口。
        selection.RebuildAt4369E0(choices, *services.text, *services.glyphs);
    },
    [](void* body, std::uint8_t blocked, SelectWindowNativeWorkflowServices*,
        const DrawSelectionControlledServices& services) {
        auto& selection = Selection(body);
        // Tick没有文字服务依赖，choices单独绑定即可；完整BYTE原样传递。
        return selection.TickAt4BBFD0(blocked, Choices(services));
    },
    [](void* body) { return Selection(body).EnabledByteAt0C(); }};
}

DrawSelectionView::DrawSelectionView(FrameLayerItem* body, DrawSelectionControlledServices services) noexcept
    : body_(body), methods_(&methods), controlled_(services) {}
DrawSelectionView::DrawSelectionView(SelectWindowLayerRuntime& body,
    DrawSelectionControlledServices services) noexcept
    : DrawSelectionView(static_cast<FrameLayerItem*>(&body), services) {}
DrawSelectionView::DrawSelectionView(SelectWindowLayerRuntime& body, ISelectWindowStateServices& choices,
    IGraphicsTextApi& text, IGraphicsTextGlyphApi& glyphs) noexcept
    : DrawSelectionView(body, {&choices, &text, &glyphs}) {}
}
