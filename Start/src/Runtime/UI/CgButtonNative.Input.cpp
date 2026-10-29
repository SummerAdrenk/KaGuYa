#include "CgButtonNative.Internal.h"
#include <Runtime/CgPageResourceProfile.h>

namespace KaGuYa::Start::Runtime
{
using namespace CgButtonNativeDetail;
namespace
{
KeyboardManagerNativeBody438h* Keyboard(unsigned site)
{ return Resolve<KeyboardManagerNativeBody438h>(RequireCgButtonNativeServices().source.CaptureInputAt418210(site)->keyboardAt18); }
MouseManagerNativeBody3Ch* Mouse(unsigned site)
{ return Resolve<MouseManagerNativeBody3Ch>(RequireCgButtonNativeServices().source.CaptureInputAt418210(site)->mouseAt00); }
void Changed(Fields84& b, Button& button) { Methods(b)->invalidateAt08(b.address, ButtonRect(button)); }
}
int ReadCgScButtonInputNativeAt4A9FC0(void* receiver)
{
    Fields84 b(receiver);
    const auto result = ReadModeButtonInputNativeAt4AA200(b.common);
    if(!CurrentCgPageResourceProfile().pageKeysEnabled)return result;
    if (std::uint32_t(Add(result, 2)) > 1u) return result;
    const auto direction = Read(b.common.directionAt08);
    auto* keyboard = Keyboard(0x4A9FEE);
    if (direction == 10 || direction == 11)
    {
        if (ReadKeyboardHeldAt4198D0(*keyboard, direction == 10 ? 0x12 : 0x13)) return result;
        b.pageAt6C = Add(Read(b.pageAt6C), direction == 10 ? -1 : 1);
        b.common.directionAt08 = -1;
        if (direction == 10)
        { if (Read(b.pageAt6C) < 0) b.pageAt6C = Sub(Count(b.pagesAt50), 1); }
        else if (Read(b.pageAt6C) >= Count(b.pagesAt50)) b.pageAt6C = 0;
        ResetCgScButtonNativeAt4C1260(receiver, 1);
        Cue(0x61A564, 0, true, 0x4AA198, 0x4AA1B8, 0x4AA1C1);
        return result;
    }
    int index;
    if (ReadKeyboardPressedAt47F4E0(*keyboard, 0x12))
    {
        if (!Read(b.pageButtonsAt5C.baseAt00) || !Read(b.pageAt6C)) return result;
        b.common.directionAt08 = 10;
        index = 0;
    }
    else
    {
        if (!ReadKeyboardPressedAt47F4E0(*Keyboard(0x4AA084), 0x13)) return result;
        if (!Read(b.pageButtonsAt5C.baseAt00) || Read(b.pageAt6C) == Sub(Count(b.pagesAt50), 1)) return result;
        b.common.directionAt08 = 11;
        index = 1;
    }
    Resolve<Button>(b.pageButtonsAt5C, index)->stateAt04 = 2;
    Changed(b, *Resolve<Button>(b.pageButtonsAt5C, index));
    return -2;
}
int ProcessCgScButtonNativeAt4BEAF0(void* receiver)
{
    Fields84 b(receiver);
    auto& s = RequireCgButtonNativeServices();
    auto result = Methods(b)->inputAt10(receiver);
    if (result == -1)
    {
        for (int i = 0; i < Count(b.common.publicGuiAt44); i = Add(i, 1))
        {
            if (i == Read(b.common.skippedGuiAt40)) continue;
            const auto old = Read(Resolve<Button>(b.common.publicGuiAt44, i)->stateAt04);
            if (ProcessCgUiButtonNativeAt4A98E0(*Resolve<Button>(b.common.publicGuiAt44, i), 0, s.mode.buttons) >= 2)
                result = Add(Read(b.common.itemCountAt3C), i);
            if (old != Read(Resolve<Button>(b.common.publicGuiAt44, i)->stateAt04))
                Changed(b, *Resolve<Button>(b.common.publicGuiAt44, i));
        }
        if (Read(b.selectorAt78.baseAt00))
        {
            if (!Selector(b).ProcessAtVslot48())
            {
                // 4BEC88先取公共count，再+50，最后才读取+3C。
                const auto count = Count(b.common.publicGuiAt44);
                const auto selection = Selector(b).SelectionAtVslot50();
                result = Add(Add(selection, Read(b.common.itemCountAt3C)), count);
            }
            Selector(b).AttachDirtyRectsAtVslot94(&b.common.dirtyAt24);
        }
        if (Read(b.pageButtonsAt5C.baseAt00))
        {
            auto* mouse = Mouse(0x4BECDD);
            const auto axis = Read(mouse->secondaryAt2C) ? 0 : Read(mouse->wheelAt34);
            if (axis)
            {
                const auto old = Read(b.pageAt6C);
                b.pageAt6C = Add(old, axis < 0 ? 1 : -1);
                if (axis < 0)
                { if (Read(b.pageAt6C) >= Count(b.pagesAt50)) b.pageAt6C = Sub(Count(b.pagesAt50), 1); }
                else if (Read(b.pageAt6C) < 0) b.pageAt6C = 0;
                if (old != Read(b.pageAt6C)) { ResetCgScButtonNativeAt4C1260(receiver, 1); return -1; }
                // clamp未变时仍进入主按钮扫描，不播放页音，不扫描页按钮。
            }
            else for (int i = 0; i < Count(b.pageButtonsAt5C); i = Add(i, 1))
            {
                if (!Read(b.pageAt6C) && !i) continue;
                if (Read(b.pageAt6C) == Sub(Count(b.pagesAt50), 1) && i) continue;
                const auto old = Read(Resolve<Button>(b.pageButtonsAt5C, i)->stateAt04);
                if (ProcessCgUiButtonNativeAt4A98E0(*Resolve<Button>(b.pageButtonsAt5C, i), 0, s.mode.buttons) >= 2)
                {
                    b.pageAt6C = Add(Read(b.pageAt6C), i ? 1 : -1);
                    if (!i)
                    { if (Read(b.pageAt6C) < 0) b.pageAt6C = Sub(Count(b.pagesAt50), 1); }
                    else if (Read(b.pageAt6C) >= Count(b.pagesAt50)) b.pageAt6C = 0;
                    ResetCgScButtonNativeAt4C1260(receiver, 1);
                    Cue(0x61A564, 0, false, 0x4BEF10, 0x4BEF48, 0x4BEF4D);
                    return -1;
                }
                if (old != Read(Resolve<Button>(b.pageButtonsAt5C, i)->stateAt04))
                    Changed(b, *Resolve<Button>(b.pageButtonsAt5C, i));
            }
        }
        for (int i = 0; i < Count(b.common.childrenAt14); i = Add(i, 1))
        {
            auto* child = Child(b, i);
            if (!child || !OnPage(b, i)) continue;
            // 4BEFC5..4BF033没有+3C上界门控，依原ASM遍历vector；不能补现代过滤。
            const auto old = Read(child->stateAt04);
            if (result == -1 && ProcessCgUiButtonNativeAt4A98E0(*child, 0, s.mode.buttons) >= 2) result = i;
            const auto state = Read(child->stateAt04);
            if (old == state) continue;
            Changed(b, *child);
            if (state == 1 && result != i) Cue(0x61A568, 1, false, 0x4BF0CD, 0x4BF107, 0x4BF10C);
        }
    }
    if (result >= Read(b.common.itemCountAt3C)) Cue(0x61A56C, 0, false, 0x4BF181, 0x4BF1FA, 0x4BF1FF);
    else if (result >= 0) Cue(0x61A564, 0, false, 0x4BF24F, 0x4BF1FA, 0x4BF1FF);
    else if (result == -4) Cue(0x61A568, 1, false, 0x4BF2E6, 0x4BF35F, 0x4BF364);
    return result;
}
int NavigateCgScButtonNativeAt4B4BC0(void* receiver)
{
    Fields84 b(receiver);
    auto current = Methods(b)->currentAt2C(receiver);
    Methods(b)->resetAt18(receiver, current);
    if (current < 0)
    {
        auto* first = Read(b.common.childrenAt14.begin);
        const auto minimum = Mul(Read(b.perPageAt70), Read(b.pageAt6C));
        for (auto* at = first; at != Read(b.common.childrenAt14.end); at = Advance(at, 1))
        {
            const auto index = Distance(first, at);
            if (index < minimum || index >= Mul(Add(Read(b.pageAt6C), 1), Read(b.perPageAt70))) continue;
            // 4B4C47直接访问解析正文；此分支没有null保护。
            if (Count(Resolve<Button>(*at)->surfacesAt5C)) { current = index; break; }
        }
    }
    else if (auto* table = Read(b.common.neighborsAt20))
    {
        auto neighbor = [&]() { return Read(*Advance(table, Add(Mul(current, 4), Read(b.common.directionAt08)))); };
        if (neighbor() >= 0)
        {
            // 原追踪不检查direction范围/环/步数；首个负邻居才直接保留current。
            for (;;)
            {
                current = neighbor();
                table = Read(b.common.neighborsAt20);
                if (current < 0 || current >= Count(b.common.childrenAt14)) continue;
                if (!Child(b, current)) continue;
                auto* child = ResolveModeButtonChildNativeAt4ACF20(b.common, current);
                if (Count(child->surfacesAt5C)) break;
                table = Read(b.common.neighborsAt20);
            }
        }
    }
    Methods(b)->selectAt28(receiver, current);
    return Methods(b)->focusAt1C(receiver, current);
}
void ResetCgScButtonNativeAt4C1260(void* receiver, int invalidate)
{
    Fields84 b(receiver);
    for (int i = 0; i < Read(b.common.itemCountAt3C); i = Add(i, 1))
    {
        if (i < 0 || i >= Count(b.common.childrenAt14)) continue;
        auto* child = Child(b, i);
        if (child) { child->stateAt04 = 0; child->latchAt20 = 0; }
    }
    if (Read(b.selectorAt78.baseAt00)) Selector(b).ResetAtVslot90();
    if (Read(b.pageButtonsAt5C.baseAt00))
        for (int i = 0; i < Count(b.pageButtonsAt5C); i = Add(i, 1))
        { auto* child = Resolve<Button>(b.pageButtonsAt5C, i); child->stateAt04 = 0; child->latchAt20 = 0; }
    b.common.releaseStateAt10 = 0;
    Methods(b)->selectAt28(receiver, -1);
    if (invalidate)
    {
        auto& source = RequireCgButtonNativeServices().source;
        const auto height = Read(source.CaptureEnvironmentAt4180C0(0x4C135D)->heightAt1C);
        const auto width = Read(source.CaptureEnvironmentAt4180C0(0x4C1365)->widthAt18);
        Methods(b)->invalidateAt08(receiver, {0, 0, std::bit_cast<int>(width), std::bit_cast<int>(height)});
    }
}
int ReadCgButtonInputNativeAt4A9FC0(Body& b){return ReadCgScButtonInputNativeAt4A9FC0(&b);}
int ProcessCgButtonNativeAt4BEAF0(Body& b){return ProcessCgScButtonNativeAt4BEAF0(&b);}
int NavigateCgButtonNativeAt4B4BC0(Body& b){return NavigateCgScButtonNativeAt4B4BC0(&b);}
void ResetCgButtonNativeAt4C1260(Body& b,int invalidate){ResetCgScButtonNativeAt4C1260(&b,invalidate);}
}
