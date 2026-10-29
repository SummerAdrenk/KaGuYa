#include <Runtime/SceneManagerOptionCharSelectDialogRuntime.h>

#include <array>

namespace KaGuYa::Start::Runtime
{
namespace
{
constexpr std::array<POINT, 5> kSample18ButtonPositions{{
    POINT{297, 623},
    POINT{394, 623},
    POINT{491, 623},
    POINT{588, 623},
    POINT{685, 623},
}};

[[nodiscard]] constexpr bool IsSample18EventId(
    const std::int32_t eventId) noexcept
{
    // 0x604138 的六条资源记录首 DWORD 为 0,1,2,3,4,3；
    // 末条是角色 3 未解锁时的替换资源，不增加新的事件 ID。
    return eventId >= 0 && eventId <= 4;
}
}

SceneManagerOptionCharSelectDialogRuntime::
SceneManagerOptionCharSelectDialogRuntime(
    ISceneManagerOptionCharSelectDialogServices& services) noexcept
    : services_(&services)
{
}

void SceneManagerOptionCharSelectDialogRuntime::InitializeAt50D930()
{
    // 0x50D930：与 Reset(+0x90) 相比，打开操作还会置 enabled=1，
    // 并把可见窗口与焦点同时归零。
    services_->ResetDialogBaseAt510750();
    fieldAt4C_ = 0;
    services_->ClearClickedEventsAt532200();
    services_->WriteDialogEnabledAtVslot24(1);
    firstVisibleChildAt80_ = 0;
    focusedChildAt84_ = 0;
    ApplyFiveButtonWindowAt53AF00();
    stateAt68_ = 3;
    selectionAt88_ = -1;
}

std::int32_t SceneManagerOptionCharSelectDialogRuntime::ProcessAtVslot48()
{
    // 0x52E100：0..2 是已消费但无副作用的过渡态。
    switch (stateAt68_)
    {
    case 0:
    case 1:
    case 2:
        return 1;
    case 3:
    {
        services_->ProcessGenericDialogAtVslot54();
        const std::int32_t eventId =
            services_->PollClickedEventAt50FDB0(1);
        if (!IsSample18EventId(eventId))
            return 1;
        focusedChildAt84_ = eventId;
        stateAt68_ = 4;
        return 1;
    }
    case 4:
        selectionAt88_ = focusedChildAt84_;
        stateAt68_ = -1;
        return 1;
    default:
        return 0;
    }
}

std::int32_t SceneManagerOptionCharSelectDialogRuntime::
SelectionAtVslot50() const
{
    // 0x50AB90 仅返回 +0x88，不附带清除行为。
    return selectionAt88_;
}

void SceneManagerOptionCharSelectDialogRuntime::AttachDirtyRectsAtVslot94(
    ICgDisplayButtonGroupDirtyRects& dirtyRects)
{
    // 0x512180：先准备自身 CDirtyRect，再逐项复制到调用者并清空自身。
    auto& own = services_->OwnDirtyRectsAtVslot00();
    own.PrepareAtVslot04();
    const std::size_t count = own.CountAtVslot18();
    const auto rects = own.RectsAtVslot1C();
    if (rects.size() < count)
        throw 1;
    for (std::size_t index = 0; index < count; ++index)
        static_cast<void>(dirtyRects.AddAtVslot08(rects[index]));
    own.ClearAtVslot00();
}

void SceneManagerOptionCharSelectDialogRuntime::ResetAtVslot90()
{
    // 0x532240 不写 enabled，也不重置 +0x80/+0x84。
    services_->ResetDialogBaseAt510750();
    fieldAt4C_ = 0;
    services_->ClearClickedEventsAt532200();
    ApplyFiveButtonWindowAt53AF00();
    stateAt68_ = 3;
    selectionAt88_ = -1;
}

void SceneManagerOptionCharSelectDialogRuntime::DrawAtVslot60(
    const RECT& clip)
{
    services_->DrawChildrenAt5230B0(clip);
}

void SceneManagerOptionCharSelectDialogRuntime::CompositeAtVslot5C(
    const CgDisplayTransitionSourceRef& target,
    const RECT* const destinationClip)
{
    services_->CompositeChildrenAt5231C0(target, destinationClip);
}

std::int32_t SceneManagerOptionCharSelectDialogRuntime::StateAt68() const noexcept
{
    return stateAt68_;
}

std::int32_t SceneManagerOptionCharSelectDialogRuntime::
FirstVisibleChildAt80() const noexcept
{
    return firstVisibleChildAt80_;
}

std::int32_t SceneManagerOptionCharSelectDialogRuntime::
FocusedChildAt84() const noexcept
{
    return focusedChildAt84_;
}

void SceneManagerOptionCharSelectDialogRuntime::ApplyFiveButtonWindowAt53AF00()
{
    // 0x53AF00 固定扫描 child ID 0..5；不存在的 ID 直接跳过。
    for (std::int32_t childId = 0; childId < 6; ++childId)
    {
        if (!services_->HasChildAtVslot14(childId))
            continue;
        services_->ResetChildAtVslot08(childId);
        const std::int32_t relative = childId - firstVisibleChildAt80_;
        if (relative < 0 || relative >=
                static_cast<std::int32_t>(kSample18ButtonPositions.size()))
        {
            services_->WriteChildEnabledAtVslot4C(childId, 0);
            continue;
        }

        services_->WriteChildEnabledAtVslot4C(childId, 1);
        services_->WriteChildPositionAt34And38(
            childId,
            kSample18ButtonPositions[static_cast<std::size_t>(relative)]);
        services_->WriteChildStateAtVslot44(
            childId,
            childId != focusedChildAt84_ ? 1 : 0);
    }
}
}
