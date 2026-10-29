#include "SaveLoadPopupNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace SaveLoadPopupNativeDetail;
GraphicsSurfaceNativeBody* NumberSurface(const Slot& number, unsigned state)
{ return Resolve<GraphicsSurfaceNativeBody>(Resolve<Number>(number)->surfacesAt10[state]); }
GraphicsSurfaceNativeBody* MarkerSurface(Body& b, unsigned index)
{ return Resolve<GraphicsSurfaceNativeBody>(b.markersAt94.begin[index]); }
void RenderDigit(Body& b, const Slot& number, int digit, unsigned state, unsigned glyphState, POINT position)
{
    auto* source = Resolve<GraphicsSurfaceNativeBody>(b.digitsAt88.begin[glyphState * 10 + digit]);
    Blit(NumberSurface(number, state), source, position);
}
void RenderMarker(Body& b, const Slot& number, unsigned marker, unsigned state, POINT position)
{ Blit(NumberSurface(number, state), MarkerSurface(b, marker), position); }
int FrameMiddleHeight(int rows)
{
    const auto& profile = CurrentSaveLoadResourceProfile().popup;
    return rows * profile.rowPitch + 2 * profile.rowInset;
}
RECT FrameBounds(Body& b, int rows)
{
    const auto origin = CurrentSaveLoadResourceProfile().popup.firstRowOrigin;
    auto* top = MarkerSurface(b, 0);
    auto* middle = MarkerSurface(b, 1);
    auto* bottom = MarkerSurface(b, 2);
    return {origin.x, origin.y,
        origin.x + std::max({top->widthAt08, middle->widthAt08, bottom->widthAt08}),
        origin.y + top->heightAt0C + FrameMiddleHeight(rows) + bottom->heightAt0C};
}
void DrawFrame(Body& b, const RECT* clip)
{
    const auto origin = CurrentSaveLoadResourceProfile().popup.firstRowOrigin;
    const int height = FrameMiddleHeight(VisibleRows(PageCount()));
    LONG y = origin.y;
    auto& api = RequireSaveLoadPopupNativeServices().graphics.CompositionAt4182F0();
    for (unsigned i = 0; i != 3; ++i)
    {
        auto* surface = MarkerSurface(b, i);
        const RECT crop{0, 0, surface->widthAt08, height};
        // 01:48AB10：中段只截取实际行数所需的原图区域，不拉伸、不叠NEW标记。
        if (surface->handleAt04 && (i != 1 || height > 0))
            static_cast<void>(api.DrawSurface(surface->handleAt04, origin.x, y,
                nullptr, i == 1 ? &crop : nullptr, clip, 0));
        y += i == 1 ? height : surface->heightAt0C;
    }
}
void UpdateControls(Common* p, int pages, int first)
{
    auto& b = Popup(p);
    const auto& profile = CurrentSaveLoadResourceProfile().popup;
    const int rows = VisibleRows(pages), maximum = MaximumFirstPage(pages);
    RECT bounds = profile.tiledFrame ? FrameBounds(b, rows) : b.boundsAt60;
    LONG bottomY = profile.firstRowOrigin.y;
    if (profile.tiledFrame)
        bottomY += MarkerSurface(b, 0)->heightAt0C + FrameMiddleHeight(rows);
    else
    {
        auto* firstNumber = Resolve<Number>(b.numbersAtA0.begin[0]);
        bottomY = firstNumber->originAt08.y;
        if (rows)
        {
            auto* last = Resolve<Number>(b.numbersAtA0.begin[rows - 1]);
            bottomY = last->originAt08.y + Resolve<GraphicsSurfaceNativeBody>(last->surfacesAt10[0])->heightAt0C;
        }
    }
    for (int event : {56, 57, 58, 59})
    {
        Fresh child;
        p->vptrAt00->findChildAt14(p, &child.r, event);
        Local life{child.r};
        if (!child.r.baseAt00) continue; // 现代bar已合成进背景，没有独立56控件。
        auto* control = Resolve<GuiControlNativeBody50h>(child.r);
        if (event == 57)
        {
            SetSliderRange(control, maximum);
            if (maximum)
                reinterpret_cast<const GuiSliderNativeVtable31*>(control->vptrAt00)->setValueAt64(control, first);
        }
        else if (event == 56) control->vptrAt00->writeEnabledAt4C(control, maximum > 0);
        else
        {
            const bool available = event == 58 ? first > 0 : first < maximum;
            control->vptrAt00->writeStateAt44(control, available);
            // 旧框本身含禁用箭头图；边界处隐藏活动箭头，避免重复alpha叠加。
            if (profile.tiledFrame) control->vptrAt00->writeEnabledAt4C(control, available);
            if (event == 59) control->yAt38 = bottomY;
        }
        if (control->vptrAt00->readEnabledAt50(control))
        {
            RECT rectangle;
            control->vptrAt00->rectAt3C(control, &rectangle);
            ::UnionRect(&bounds, &bounds, &rectangle);
        }
    }
    b.boundsAt60 = bounds;
}
}
void UpdateSaveLoadPopupNativeAt52B390(Common* p, std::int32_t requested, std::int32_t dirty)
{
    auto& b = Popup(p);
    const auto& profile = CurrentSaveLoadResourceProfile().popup;
    const int pages = PageCount(), rows = VisibleRows(pages);
    const int first = std::clamp(requested, 0, MaximumFirstPage(pages));
    const RECT previousBounds = b.boundsAt60;
    b.selectedPageAt7C = pages ? std::clamp(b.selectedPageAt7C, 0, pages - 1) : -1;
    b.pendingPageAt84 = b.selectedPageAt7C;
    int row = 0;
    for (auto* at = b.numbersAtA0.begin; at != b.numbersAtA0.end; ++at, ++row)
    {
        auto* number = Resolve<Number>(*at);
        const bool visible = row < rows;
        const int page = visible ? first + row : -1;
        number->pageAt04 = page;
        Fresh child;
        p->vptrAt00->findChildAt14(p, &child.r, number->eventAt00);
        Local childLife{child.r};
        if (child.r.baseAt00)
        {
            auto* control = Resolve<GuiControlNativeBody50h>(child.r);
            control->vptrAt00->writeEnabledAt4C(control, visible);
            reinterpret_cast<GuiCheckBoxNativeBody60h*>(control)->checkedAt5C = visible && b.selectedPageAt7C == page;
            if (!visible)
            {
                if (p->activeAt4C == control) p->vptrAt00->resetInputAt3C(p);
                control->vptrAt00->resetAt08(control);
            }
        }
        for (unsigned state = 0; state != 3; ++state) Clear(NumberSurface(*at, state));
        if (!visible) continue; // 空目录和屏外行绝不解引用catalog或绘制页码。

        auto& source = RequireSaveLoadPopupNativeServices().source;
        auto* save = Save(source.CaptureEnvironmentAt4180C0());
        int value = SaveFilePhysicalPageNumber(*save, page);
        const auto positions = DigitPositions();
        int divisor = profile.tiledFrame ? 1000 : 1;
        for (const auto position : positions)
        {
            const int digit = profile.tiledFrame ? value / divisor % 10 : value % 10;
            for (unsigned state = 0; state != 3; ++state)
            {
                unsigned glyphState = state;
                if (profile.tiledFrame)
                {
                    // 01:48FC50行状态表；07至11第四组是off，不是第四份交互surface。
                    if (profile.digitStates == 4 && b.selectedPageAt7C == page) glyphState = 3;
                    else if (profile.digitStates == 3)
                        glyphState = state ? 1 : (b.selectedPageAt7C == page ? 2 : 0);
                }
                RenderDigit(b, *at, digit, state, glyphState, position);
            }
            if (profile.tiledFrame) divisor /= 10;
            else value /= 10;
        }
        if (!profile.tiledFrame)
        {
            save = Save(source.CaptureEnvironmentAt4180C0());
            if (save->RegularPages().at(page).availabilityAt00)
            {
                RenderMarker(b, *at, b.selectedPageAt7C == page ? 2u : 1u, 0, UsedPosition());
                RenderMarker(b, *at, 2, 1, UsedPosition());
                RenderMarker(b, *at, 2, 2, UsedPosition());
            }
            save = Save(source.CaptureEnvironmentAt4180C0());
            if (save->regularLatestPageAt18 == page)
                for (unsigned state = 0; state != 3; ++state) RenderMarker(b, *at, 0, state, LatestPosition());
        }
    }
    b.firstPageAt80 = first;
    UpdateControls(p, pages, first);
    if (dirty)
    {
        RECT invalidated;
        // 缩短框后，旧下缘也必须失效，否则留下上一次窗口的残影。
        ::UnionRect(&invalidated, &previousBounds, &b.boundsAt60);
        p->vptrAt00->invalidateAt64(p, invalidated);
    }
}
namespace SaveLoadPopupNativeDetail
{
void __fastcall Draw(Common* p, void*, const RECT* clip)
{
    auto& b = Popup(p);
    const bool tiled = CurrentSaveLoadResourceProfile().popup.tiledFrame;
    if (tiled && p->vptrAt00->readEnabledAt28(p)) DrawFrame(b, clip);
    DrawSaveLoadPageBaseNativeAt524960(p, clip);
    if (!p->vptrAt00->readEnabledAt28(p)) return;
    for (auto* at = b.numbersAtA0.begin; at != b.numbersAtA0.end; ++at)
    {
        if (Resolve<Number>(*at)->pageAt04 < 0) continue;
        Fresh child;
        p->vptrAt00->findChildAt14(p, &child.r, Resolve<Number>(*at)->eventAt00);
        Local life{child.r};
        if (!child.r.baseAt00) continue;
        auto* control = Resolve<GuiControlNativeBody50h>(child.r);
        const auto checked = reinterpret_cast<GuiCheckBoxNativeBody60h*>(control)->checkedAt5C;
        const auto state = control->vptrAt00->resourceStateAt38(control);
        int image = -1;
        if (state == 0) image = !tiled && checked ? 2 : 0;
        else if (state == 3 || state == 4) image = !tiled && checked ? 2 : 1;
        else if (state == 5) image = 2;
        if (image < 0) continue;
        const auto origin = Resolve<Number>(*at)->originAt08;
        auto* surface = NumberSurface(*at, static_cast<unsigned>(image));
        if (surface->handleAt04)
        {
            auto& api = RequireSaveLoadPopupNativeServices().graphics.CompositionAt4182F0();
            static_cast<void>(api.DrawSurface(surface->handleAt04, origin.x, origin.y, nullptr, nullptr, clip, 0));
        }
    }
}
}
}
