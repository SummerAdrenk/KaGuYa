#include "LikabilityDialogNative.Internal.h"
#include "LikabilityDialogNative.Profile.h"

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace LikabilityNativeDetail;
void InvalidateBackground(Body& b)
{
    const auto base = Read(b.backgroundAt78.baseAt00);
    auto* surface = ResolveWithBase<Surface>(b.backgroundAt78, base);
    const auto w = Read(surface->widthAt08), h = Read(surface->heightAt0C);
    surface = ResolveWithBase<Surface>(b.backgroundAt78, base);
    const auto x = Read(surface->originXAt10), y = Read(surface->originYAt14);
    auto* dirty = b.common.vptrAt00->dirtyAt00(&b.common);
    dirty->vptrAt00->addXYWHAt10(dirty, x, y, w, h);
}
}
void BeginLikabilityDialogNativeAt50C950(LikabilityDialogNativeBody94h& b)
{
    using namespace LikabilityNativeDetail;
    auto& s = RequireLikabilityDialogNativeServices();
    auto* draw = CaptureDraw(0x50C97F, 0x50C984);
    Slot fresh;
    auto* result = s.source.CaptureBackgroundAt4ACDD0(draw, &fresh, 0x50C9AF);
    Local temporary{fresh}; // 原state0只在4ACDD0完整返回后生效。
    Assign(b.backgroundAt78, *result, true);
    temporary.Current(true); // state1：normal release再抛必须terminate。
    auto& c = b.common;
    c.vptrAt00->writeEnabledAt24(&c, 1);
    c.vptrAt00->resetChildrenAt68(&c);
    c.vptrAt00->resetInputAt3C(&c);
    ClearGuiClickedEventFlagsAt532200(*Resolve<GuiClickedEventReferences>(b.clickedManagerAt6C));
    RefreshLikabilityDialogNativeAt528F20(b);
    c.activeAt4C = nullptr;
    b.phaseAt90 = 0;
}
std::int32_t PollLikabilityDialogNativeAt52D4A0(LikabilityDialogNativeBody94h& b)
{
    auto& c = b.common;
    switch (LikabilityNativeDetail::Read(b.phaseAt90))
    {
    case 0:
        InvalidateBackground(b);
        b.phaseAt90 = 3;
        c.vptrAt00->flushDirtyAt58(&c);
        return 1;
    case 3:
        if (b.profileState) TickLikabilityProfileGui(*b.profileState);
        c.vptrAt00->processInputAt54(&c);
        c.vptrAt00->flushDirtyAt58(&c);
        return 1;
    case 4:
        if (b.profileState) CommitLikabilityProfileClose(*b.profileState);
        InvalidateBackground(b);
        c.vptrAt00->writeEnabledAt24(&c, 0);
        b.phaseAt90 = -1;
        c.vptrAt00->flushDirtyAt58(&c);
        return 1;
    default: return 0;
    }
}
void ProcessLikabilityDialogInputNativeAt518F30(LikabilityDialogNativeBody94h& b)
{
    using namespace LikabilityNativeDetail;
    if (b.profileState && !LikabilityProfileHasCancelInput(*b.profileState))
    {
        // 01–07原普通GUI输入没有后期派生页的action0/右键关闭扩展。
        ProcessGuiDialogInputAt518C30(b.common);
        DispatchLikabilityProfileEvent(*b.profileState,
            PollEventsAt50FDB0(Resolve<GuiClickedEventReferences>(b.clickedManagerAt6C)));
        return;
    }
    auto& source = RequireLikabilityDialogNativeServices().source;
    auto* root = source.CaptureInputAt418210(0x518F72);
    auto* keyboard = Resolve<KeyboardManagerNativeBody438h>(root->keyboardAt18);
    auto* at = Read(keyboard->actionsAt14[0].begin);
    auto* const end = Read(keyboard->actionsAt14[0].end);
    if (at != end)
    {
        const auto buffer = Read(keyboard->bufferAt10);
        for (; at != end; ++at)
        {
            const auto address = reinterpret_cast<std::uintptr_t>(at);
            const auto current = *reinterpret_cast<const volatile std::uint8_t*>(address + buffer + 1u);
            const auto previous = *reinterpret_cast<const volatile std::uint8_t*>(address - buffer + 2u);
            if (current && !previous) { b.phaseAt90 = 4; return; }
        }
    }
    root = source.CaptureInputAt418210(0x519072);
    auto* mouse = Resolve<MouseManagerNativeBody3Ch>(root->mouseAt00);
    if (!Read(mouse->previousAt30[1]) && Read(mouse->pressedAt10[1]) && !Read(mouse->suppressionAt28))
    { b.phaseAt90 = 4; return; }
    ProcessGuiDialogInputAt518C30(b.common);
    const auto event = PollEventsAt50FDB0(Resolve<GuiClickedEventReferences>(b.clickedManagerAt6C));
    if (b.profileState) DispatchLikabilityProfileEvent(*b.profileState, event);
    else if (event == 1) b.phaseAt90 = 4;
}
void FlushLikabilityDialogDirtyNativeAt5261A0(LikabilityDialogNativeBody94h& b)
{
    using namespace LikabilityNativeDetail;
    auto& c = b.common;
    auto* dirty = c.vptrAt00->dirtyAt00(&c);
    dirty->vptrAt00->coalesceAt04(dirty);
    dirty = c.vptrAt00->dirtyAt00(&c);
    const auto count = std::bit_cast<std::int32_t>(dirty->vptrAt00->countAt18(dirty));
    if (!count) return;
    dirty = c.vptrAt00->dirtyAt00(&c);
    auto* rectangles = dirty->vptrAt00->rectsAt1C(dirty);
    for (std::int32_t i = 0; i < count; ++i)
    {
        auto* rectangle = rectangles + i;
        // 旧款可能只是小幅/透明画布；现代modal先恢复同一背景，再叠共享控件。
        // 18维持原来只在disable后恢复背景的路径。
        if (b.profileState || !c.vptrAt00->readEnabledAt28(&c))
        {
            const auto handle = Read(Resolve<Surface>(b.backgroundAt78)->handleAt04);
            if (handle) RequireLikabilityDialogNativeServices().source.DrawBackgroundAt52624D(handle, rectangle);
        }
        c.vptrAt00->drawAt60(&c, rectangle);
        RequireGuiDialogNativeScreenSource().InvalidateRectAt52603A(rectangle);
    }
    dirty = c.vptrAt00->dirtyAt00(&c);
    dirty->vptrAt00->clearAt00(dirty);
}
}
