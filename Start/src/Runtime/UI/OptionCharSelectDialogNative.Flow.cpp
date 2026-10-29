#include "OptionCharSelectDialogNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
using namespace OptionCharSelectNativeDetail;
void ApplyOptionCharSelectWindowAt53AF00(OptionCharSelectDialogNativeBody8Ch& body)
{
    auto& common = body.common;
    const auto& profile = CurrentOptionCharResourceProfile();
    if (Legacy(profile))
    {
        // 01原4530A0保留disabled3；当前分类的绘制/命中排除由旧编码按钮表处理。
        for (const auto& instance : profile.instances)
        {
            auto* child = FindChild(&common, instance.event);
            if (!child) continue;
            child->vptrAt00->resetAt08(child);
            child->vptrAt00->writeEnabledAt4C(child, 1);
            if (child->vptrAt00->readStateAt48(child) != 3) child->vptrAt00->writeStateAt44(child, 0);
        }
        return;
    }
    for (const auto& arrow : profile.navigationButtons)
        if (auto* child = FindChild(&common, arrow.common.eventAt00)) child->vptrAt00->resetAt08(child);
    for (std::int32_t index = 0; index != profile.scanCount; ++index)
    {
        Slot child;
        common.vptrAt00->findChildAt14(&common, &child, index);
        LocalReference lifetime{child};
        if (child.baseAt00)
        {
            auto* receiver = ResolveGuiControl(child);
            receiver->vptrAt00->resetAt08(receiver);
            const auto first = body.firstVisibleAt80;
            if (index < first || index >= Add(first, profile.visibleCount))
            {
                receiver = ResolveGuiControl(child);
                receiver->vptrAt00->writeEnabledAt4C(receiver, 0);
            }
            else
            {
                receiver = ResolveGuiControl(child);
                receiver->vptrAt00->writeEnabledAt4C(receiver, 1);
                receiver = ResolveGuiControl(child);
                const auto relative = Sub(index, body.firstVisibleAt80);
                if (relative < 0 || static_cast<std::size_t>(relative) >= profile.positions.size())
                    throw std::out_of_range("Option character window position is outside the profile");
                const auto& point = profile.positions[static_cast<std::size_t>(relative)];
                receiver->xAt34 = point.x;
                receiver->yAt38 = point.y;
                receiver = ResolveGuiControl(child);
                receiver->vptrAt00->writeStateAt44(receiver, index != body.focusedAt84);
            }
        }
        lifetime.Reload();
    }
}
namespace OptionCharSelectNativeDetail
{
void __fastcall Begin(Body* common, void*)
{
    auto& body = Dialog(common);
    common->vptrAt00->resetInputAt3C(common);
    common->activeAt4C = nullptr;
    ClearGuiClickedEventFlagsAt532200(*Manager(body.clickedManagerAt5C));
    common->vptrAt00->writeEnabledAt24(common, 1);
    body.focusedAt84 = 0;
    body.firstVisibleAt80 = 0;
    ApplyOptionCharSelectWindowAt53AF00(body);
    body.phaseAt68 = 3;
    body.selectionAt88 = -1;
}
std::int32_t __fastcall Poll(Body* common, void*)
{
    auto& body = Dialog(common);
    switch (body.phaseAt68)
    {
    case 0: case 1: case 2: return 1;
    case 3:
    {
        const auto& profile = CurrentOptionCharResourceProfile();
        common->vptrAt00->processInputAt54(common);
        auto* manager = Manager(body.clickedManagerAt5C);
        const auto event = RequireOptionCharSelectDialogNativeDependencies().source.
            PollClickedEventManagerAt50FDB0(manager, 1);
        if (event < 0) return 1;
        if (profile.presentation == OptionCharPresentation::SlidingWindow &&
            (event == profile.previousBoundaryEvent || event == profile.nextBoundaryEvent))
        {
            // 08原4DCAE0：7/8是移动一分类的命令，最终结果绝不是箭头event。
            const int count = static_cast<int>(profile.instances.size());
            if (body.focusedAt84 < 0 || body.focusedAt84 >= count)
                throw std::out_of_range("Option character sliding focus is invalid");
            if (event == profile.previousBoundaryEvent)
            {
                if (!body.focusedAt84)
                { body.focusedAt84 = count - 1; body.firstVisibleAt80 = count - profile.visibleCount; }
                else if (--body.focusedAt84 < body.firstVisibleAt80) --body.firstVisibleAt80;
            }
            else if (body.focusedAt84 == count - 1)
            { body.focusedAt84 = 0; body.firstVisibleAt80 = 0; }
            else if (++body.focusedAt84 >= Add(body.firstVisibleAt80, profile.visibleCount)) ++body.firstVisibleAt80;
            body.phaseAt68 = 4;
            return 1;
        }
        bool accepted = false;
        if (Legacy(profile))
        {
            for (const auto& instance : profile.instances) if (event == instance.event) { accepted = true; break; }
        }
        else
        {
            for (const auto& record : profile.records) if (event == record.common.eventAt00) { accepted = true; break; }
        }
        if (accepted) { body.focusedAt84 = event; body.phaseAt68 = 4; }
        return 1;
    }
    case 4:
        body.selectionAt88 = body.focusedAt84;
        body.phaseAt68 = -1;
        return 1;
    default: return 0;
    }
}
std::int32_t __fastcall End(Body* common, void*)
{ common->vptrAt00->writeEnabledAt24(common, 0); return 1; }
std::int32_t __fastcall Result(Body* common, void*) { return Dialog(common).selectionAt88; }
std::int32_t __fastcall Count(Body*, void*) { return CurrentOptionCharResourceProfile().reportedCount; }
void __fastcall Reset(Body* common, void*)
{
    auto& body = Dialog(common);
    common->vptrAt00->resetInputAt3C(common);
    common->activeAt4C = nullptr;
    ClearGuiClickedEventFlagsAt532200(*Manager(body.clickedManagerAt5C));
    ApplyOptionCharSelectWindowAt53AF00(body);
    body.phaseAt68 = 3;
    body.selectionAt88 = -1;
}
void __fastcall AttachDirty(Body* common, void*, const Slot* target)
{
    if (!target->baseAt00) return;
    auto* dirty = common->vptrAt00->dirtyAt00(common);
    dirty->vptrAt00->coalesceAt04(dirty);
    dirty = common->vptrAt00->dirtyAt00(common);
    const auto count = std::bit_cast<std::int32_t>(dirty->vptrAt00->countAt18(dirty));
    if (count)
    {
        dirty = common->vptrAt00->dirtyAt00(common);
        auto* rectangles = dirty->vptrAt00->rectsAt1C(dirty);
        for (std::int32_t index = 0; index < count; ++index)
        {
            // 每轮按调用者Ref重解析，传原数组项地址；借用，不清target也不增持。
            auto* destination = ResolveGuiDirtyRectReference(*target);
            destination->vptrAt00->addRectAt08(destination, rectangles + index);
        }
    }
    dirty = common->vptrAt00->dirtyAt00(common);
    dirty->vptrAt00->clearAt00(dirty);
}
}
}
