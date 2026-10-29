#include <Runtime/GuiModalNativeAdapters.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
GuiInputMouseBorrow GuiModalInputSource::MouseAt00()
{
    const auto selected=input_.MouseOrNull();
    if(!selected)throw std::logic_error("GameInputState mouse slot is empty");
    return selected;
}
GuiInputKeyboardBorrow GuiModalInputSource::KeyboardAt18()
{
    const auto selected=input_.KeyboardOrNull();
    if(!selected)throw std::logic_error("GameInputState keyboard slot is empty");
    return selected;
}

GuiDirtyRectNativeBody80h& GuiModalDirtyRectView::Body() const
{
    auto* body = ResolveGuiDirtyRectReference(slot_);
    if (!body) throw std::logic_error("GuiDialog dereferenced a null dirty rect body");
    return *body;
}
void GuiModalDirtyRectView::PrepareAtVslot04() { auto& body = Body(); body.vptrAt00->coalesceAt04(&body); }
std::uint32_t GuiModalDirtyRectView::CountAtVslot18() const { auto& body = Body(); return body.vptrAt00->countAt18(&body); }
RECT* GuiModalDirtyRectView::RectsAtVslot1C() const { auto& body = Body(); return body.vptrAt00->rectsAt1C(&body); }
void GuiModalDirtyRectView::AddAtVslot08(const RECT& rect) { auto& body = Body(); body.vptrAt00->addRectAt08(&body, &rect); }
void GuiModalDirtyRectView::ClearAtVslot00() { auto& body = Body(); body.vptrAt00->clearAt00(&body); }
}
