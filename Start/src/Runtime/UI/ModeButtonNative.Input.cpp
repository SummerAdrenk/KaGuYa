#include "ModeButtonNative.Internal.h"
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
using namespace ModeButtonNativeDetail;
namespace
{
// 原PE 602628[8]/602638[4]，动作索引而非VK键码；保持4..7与0..3两个范围。
constexpr std::int32_t keysAt602628[8]{1, 2, 3, 4, 14, 15, 16, 17};
bool Pressed(std::int32_t action)
{ return ReadKeyboardPressedAt47F4E0(*CaptureKeyboard(), action); }
bool Held(std::int32_t action)
{ return ReadKeyboardHeldAt4198D0(*CaptureKeyboard(), action); }
Child& RequiredPublicChild(Body& body,std::int32_t index)
{
    // 公共栏装载失败会清空Ref；01实机F4已触发地址4写入，危险域改为明确失败。
    auto* child=Resolve<Child>(body.publicGuiAt44,index);
    if(!child)throw std::runtime_error("Appreciation controls are unavailable after resource loading failed");
    return *child;
}
}
std::uint8_t ReadModeButtonReleaseNativeAt4A9C60(Body& body)
{
    switch (Read(body.releaseStateAt10))
    {
    case 0:
    {
        if (Pressed(0)) { body.releaseStateAt10 = 2; return 0; }
        auto* mouse = CaptureMouse();
        if (!Read(mouse->previousAt30[1]) && Read(mouse->pressedAt10[1]) && !Read(mouse->suppressionAt28))
            body.releaseStateAt10 = 1;
        return 0;
    }
    case 1:
    {
        auto* mouse = CaptureMouse();
        if (Read(mouse->pressedAt10[1]) && !Read(mouse->suppressionAt28)) return 0;
        body.releaseStateAt10 = 0;
        return 1;
    }
    case 2:
        if (Held(0)) return 0;
        body.releaseStateAt10 = 0;
        return 1;
    default:
        return 0;
    }
}
std::int32_t ReadButtonBaseInputNativeAt4A9D90(Body& body)
{
    // 4A9D93与4A9DC2是两个独立捕获；短路时不能提前抓mouse快照。
    bool reset = Read(CaptureMouse()->movedAt33) != 0;
    if (!reset)
    {
        auto* mouse = CaptureMouse();
        reset = !Read(mouse->previousAt30[0]) && Read(mouse->pressedAt10[0]) && !Read(mouse->suppressionAt28);
    }
    if (reset)
    {
        body.vptrAt00->selectAt28(&body, -1);
        return -1;
    }
    std::int32_t result = -2;
    if (body.vptrAt00->currentAt2C(&body) >= 0)
    {
        if (Pressed(5))
        {
            const auto* methods = Read(body.vptrAt00);
            body.confirmLatchAt0C = 1;
            const auto current = methods->currentAt2C(&body);
            methods->pressAt20(&body, current);
        }
        else if (Read(body.confirmLatchAt0C))
        {
            if (!Held(5))
            {
                result = body.vptrAt00->currentAt2C(&body);
                body.vptrAt00->selectAt28(&body, -1);
                body.vptrAt00->commitAt24(&body, result);
            }
        }
        if (Read(body.confirmLatchAt0C)) return -3;
    }
    const auto direction = Read(body.directionAt08);
    if (direction == -1)
    {
        for (std::int32_t action = 1; action <= 4; ++action)
        {
            if (Pressed(action))
            {
                body.directionAt08 = action - 1;
                return result;
            }
        }
        if (Read(body.selectedAt04) == -1 && result == -2) return -1;
    }
    else if (std::uint32_t(direction) <= 3u)
    {
        if (!Held(keysAt602628[direction]))
        {
            const auto previous = body.vptrAt00->currentAt2C(&body);
            // 原slot30无栈参数，从正文+08读取方向；回调之后再次取current比较。
            body.vptrAt00->navigateAt30(&body);
            if (previous != body.vptrAt00->currentAt2C(&body)) return -4;
        }
    }
    return result;
}
std::int32_t ReadModeButtonInputNativeAt4AA200(Body& body)
{
    const auto result = ReadButtonBaseInputNativeAt4A9D90(body);
    if (std::uint32_t(Add(result, 2)) > 1u) return result;
    const auto direction = Read(body.directionAt08);
    if (std::uint32_t(Sub(direction, 4)) <= 3u)
    {
        if (Held(keysAt602628[direction])) return result;
        const auto index = Sub(Read(body.directionAt08), 4);
        const auto selected = Add(Read(body.itemCountAt3C), index);
        RequiredPublicChild(body,index).stateAt04 = 0;
        // 4AA27B在取RECT之前缓存本次虚表；publicGui对象在写state后重解引用。
        const auto* methods = Read(body.vptrAt00);
        const auto rect = ChildRect(RequiredPublicChild(body,index));
        methods->invalidateAt08(&body, rect);
        body.directionAt08 = -1;
        return selected;
    }
    for (std::int32_t index = 0; index != 4; ++index)
    {
        if (index == Read(body.skippedGuiAt40)) continue;
        if (!Pressed(keysAt602628[index + 4])) continue;
        body.directionAt08 = index + 4;
        RequiredPublicChild(body,index).stateAt04 = 2;
        const auto rect = ChildRect(RequiredPublicChild(body,index));
        body.vptrAt00->invalidateAt08(&body, rect);
        return -2;
    }
    return result;
}
std::int32_t ProcessModeButtonNativeAt4BE930(Body& body)
{
    if (body.vptrAt00->releaseAt0C(&body))
        return ChildCount(body);
    auto result = body.vptrAt00->inputAt10(&body);
    if (result != -1) return result;
    for (auto* at = Read(body.childrenAt14.begin); at != Read(body.childrenAt14.end); at = Advance(at, 1))
    {
        if (!ResourceCount(*Resolve<Child>(*at))) continue;
        const auto oldState = Read(Resolve<Child>(*at)->stateAt04);
        if (ProcessCgUiButtonNativeAt4A98E0(*Resolve<Child>(*at), 0, Services().buttons) >= 2)
            result = Distance(Read(body.childrenAt14.begin), at);
        if (oldState != Read(Resolve<Child>(*at)->stateAt04))
        {
            const auto rect = ChildRect(*Resolve<Child>(*at));
            body.vptrAt00->invalidateAt08(&body, rect);
        }
        // 原扫描不中断；多个项激活时最后一项覆盖结果，不借GUI click事件管理器。
    }
    return result;
}
}
