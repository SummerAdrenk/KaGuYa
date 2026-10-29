#include "ModeButtonNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
using namespace ModeButtonNativeDetail;
namespace
{
std::int32_t Neighbor(const std::int32_t* table, std::int32_t item, std::int32_t direction) noexcept
{
    const auto row = std::bit_cast<std::int32_t>(std::uint32_t(item) * 4u);
    return Read(*Advance(table, Add(row, direction)));
}
bool Navigable(Child& child) noexcept
{
    if (!ResourceCount(child)) return false;
    const auto state = Read(child.stateAt04);
    return state != 3 && (state != 4 || Read(child.inputModeAt1C) != 2);
}
ModeButtonNavigationNativeBody34h* ResolveNavigation(Slot& slot, std::uintptr_t base) noexcept
{
    if (!base) return nullptr;
    auto* control = static_cast<EnvironmentObjectReferenceControl*>(Read(slot.controlAt08));
    const auto stride = control ? Read(control->strideAt0C) : 0u;
    return reinterpret_cast<ModeButtonNavigationNativeBody34h*>(base + Read(slot.biasAt04) * stride);
}
}
std::int32_t NavigateModeButtonNativeAt4B4920(Body& body)
{
    auto selected = body.vptrAt00->currentAt2C(&body);
    body.vptrAt00->resetAt18(&body, selected);
    if (selected < 0)
    {
        auto* at = Read(body.childrenAt14.begin);
        auto* const last = Read(body.childrenAt14.end);
        for (; at != last; at = Advance(at, 1))
        {
            if (!ResourceCount(*Resolve<Child>(*at))) continue;
            // 首次选择只排除state3，原分支没有state4/kind2过滤。
            if (Read(Resolve<Child>(*at)->stateAt04) == 3) continue;
            selected = Distance(Read(body.childrenAt14.begin), at);
            break;
        }
    }
    else
    {
        const auto original = selected;
        auto* table = Read(body.neighborsAt20);
        if (table)
        {
            // 4B49DD先判邻接表，非空才读+08；不预读未进入分支的方向字段。
            auto direction = Read(body.directionAt08);
            if (Neighbor(table, selected, direction) >= 0)
            {
                auto candidate = selected;
                for (;;)
                {
                    candidate = Neighbor(table, candidate, direction);
                    if (candidate == selected) break;
                    auto* child = ResolveModeButtonChildNativeAt4ACF20(body, candidate);
                    if (child && Navigable(*child))
                    {
                        selected = candidate;
                        if (candidate != original) goto finish;
                        break;
                    }
                    if (candidate < 0) break;
                    table = Read(body.neighborsAt20);
                    direction = Read(body.directionAt08);
                    // 原链只靠负数/回到起点停止；不增加现代attempt上限或静态pair搜索。
                }
            }
        }
        {
            auto* at = Read(body.navigationAt30.begin);
            auto* const last = Read(body.navigationAt30.end);
            std::uintptr_t navigationBase = 0;
            for (; at != last; at = Advance(at, 1))
            {
                navigationBase = Read(at->baseAt00);
                if (Read(ResolveNavigation(*at, navigationBase)->itemAt00) == original) break;
            }
            if (at != last)
            {
                for (std::int32_t attempt = 0;; attempt = Add(attempt, 1))
                {
                    const auto direction = Read(body.directionAt08);
                    auto* navigation = ResolveNavigation(*at, navigationBase);
                    if (std::uint32_t(direction) > 3u) break;
                    // 4B4AD4/ADC/AE4/AEC分别选+1C/+28/+04/+10，不按controlID导航。
                    constexpr std::int32_t vectorByDirection[4]{2, 3, 0, 1};
                    auto& candidates = navigation->candidatesAt04[vectorByDirection[direction]];
                    auto* first = Read(candidates.begin);
                    const auto bytes = std::bit_cast<std::int32_t>(std::uint32_t(
                        reinterpret_cast<std::uintptr_t>(Read(candidates.end)) -
                        reinterpret_cast<std::uintptr_t>(first)));
                    if (attempt >= (bytes >> 2)) break;
                    const auto candidate = Read(*Advance(first, attempt));
                    if (candidate < 0) break;
                    if (candidate >= ChildCount(body)) continue;
                    auto* childSlot = Advance(Read(body.childrenAt14.begin), candidate);
                    if (!Read(childSlot->baseAt00)) continue;
                    auto* child = Resolve<Child>(*childSlot);
                    // 原4B4B4D仅在已找到非空child base时刷新navigation base。
                    navigationBase = Read(at->baseAt00);
                    if (!child || !Navigable(*child)) continue;
                    selected = candidate;
                    break;
                }
            }
        }
    }
finish:
    body.vptrAt00->selectAt28(&body, selected);
    // 返回focus/invalidation的EAX，不虚构为selected或导航成功布尔。
    return body.vptrAt00->focusAt1C(&body, selected);
}
}
