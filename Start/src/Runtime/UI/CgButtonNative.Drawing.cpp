#include "CgButtonNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
using namespace CgButtonNativeDetail;
void DrawCgScButtonNativeAt4ABE80(void* receiver)
{
    Fields84 b(receiver);
    auto dirty = [&]() { return Resolve<GuiDirtyRectNativeBody80h>(b.common.dirtyAt24); };
    auto* d = dirty();
    Read(d->vptrAt00)->coalesceAt04(d);
    d = dirty();
    const auto count = std::bit_cast<int>(Read(d->vptrAt00)->countAt18(d));
    if (!count) return; // 原count==0不执行clear。
    d = dirty();
    auto* rects = Read(d->vptrAt00)->rectsAt1C(d);
    auto& s = RequireCgButtonNativeServices();
    for (int i = 0; i < count; i = Add(i, 1))
    {
        auto* rect = Advance(rects, i);
        auto* page = Page(b);
        if (Read(page->handleAt04)) s.source.DrawPageAfter4182F0(page, rect);
        for (int j = 0; j < Read(b.common.itemCountAt3C); j = Add(j, 1))
        {
            if (j < 0 || j >= Count(b.common.childrenAt14)) continue;
            auto* child = Child(b, j);
            if (child && OnPage(b, j)) DrawButton(*child, rect);
        }
        if (Read(b.selectorAt78.baseAt00)) Selector(b).DrawAtVslot60(rect);
        for (int j = 0; j < Count(b.common.publicGuiAt44); j = Add(j, 1))
            if (j != Read(b.common.skippedGuiAt40)) DrawButton(*Resolve<Button>(b.common.publicGuiAt44, j), rect);
        if (Read(b.pageButtonsAt5C.baseAt00))
            for (int j = 0; j < Count(b.pageButtonsAt5C); j = Add(j, 1))
                if (PageVisible(b, j)) DrawButton(*Resolve<Button>(b.pageButtonsAt5C, j), rect);
        s.source.InvalidatedRectAt4AC316(rect);
    }
    d = dirty();
    Read(d->vptrAt00)->clearAt00(d);
}
void CompositeCgScButtonNativeAt4A80B0(void* receiver, Surface* target, const RECT* clip)
{
    Fields84 b(receiver);
    Blit(target, Page(b), 0, 0, clip);
    auto& s = RequireCgButtonNativeServices();
    for (int i = 0; i < Read(b.common.itemCountAt3C); i = Add(i, 1))
    {
        if (i < 0 || i >= Count(b.common.childrenAt14)) continue;
        auto* child = Child(b, i);
        if (child && OnPage(b, i)) static_cast<void>(DrawCgUiButtonNativeAt4B5290(*child, target, clip, s.mode.buttons));
    }
    // 原451500(target,owning=0)由GUI+5C消费，包括异常退役；调用方不二次释放。
    if (Read(b.selectorAt78.baseAt00)) Selector(b).CompositeSurfaceAtVslot5C(target, clip);
    for (int i = 0; i < Count(b.common.publicGuiAt44); i = Add(i, 1))
        if (i != Read(b.common.skippedGuiAt40)) CompositeButton(*Resolve<Button>(b.common.publicGuiAt44, i), target, clip);
    if (Read(b.pageButtonsAt5C.baseAt00))
        for (int i = 0; i < Count(b.pageButtonsAt5C); i = Add(i, 1))
            if (PageVisible(b, i)) CompositeButton(*Resolve<Button>(b.pageButtonsAt5C, i), target, clip);
}
void DrawCgButtonNativeAt4ABE80(Body& b){DrawCgScButtonNativeAt4ABE80(&b);}
void CompositeCgButtonNativeAt4A80B0(Body& b,Surface* target,const RECT* clip){CompositeCgScButtonNativeAt4A80B0(&b,target,clip);}
}
