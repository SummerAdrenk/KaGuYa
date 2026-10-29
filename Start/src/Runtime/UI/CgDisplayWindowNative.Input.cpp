#include "CgDisplayWindowNative.Internal.h"
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/UnSaveDataRuntime.h>
#include <cstring>

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace CgDisplayWindowNativeDetail;
MouseManagerNativeBody3Ch* Mouse(CgDisplayWindowNativeServices& services)
{
    return ResolveEnvironmentObjectReference<MouseManagerNativeBody3Ch>(
        services.input.CaptureAt418210()->mouseAt00);
}
KeyboardManagerNativeBody438h* Keyboard(CgDisplayWindowNativeServices& services)
{
    return ResolveEnvironmentObjectReference<KeyboardManagerNativeBody438h>(
        services.input.CaptureAt418210()->keyboardAt18);
}
std::uint32_t& Slot2(CgDisplayWindowNativeServices& services)
{
    auto* const root = services.environment.CaptureAt4180C0();
    return static_cast<UnSaveDataRuntime*>(ResolveEnvironmentArchiveAt417990(*root, 2))->ModeAt04();
}
void SetPointer(Body& body, CgDisplayWindowNativeServices& services)
{
    auto* const cursor = ResolveEnvironmentObjectReference<CursorNativeBody10h>(
        services.input.CaptureAt418210()->cursorAt0C);
    using Set = std::uint8_t (__thiscall*)(CursorNativeBody10h*, LPCSTR, std::uint8_t, std::uint8_t);
    Set set;
    std::memcpy(&set, reinterpret_cast<const std::byte*>(cursor->vptrAt00) + 4, sizeof(set));
    InputNativeScope scope(services.inputServices);
    (void)set(cursor, MAKEINTRESOURCEA(body.draggingAt54 ? 208 : 207), 0, 0);
}
void ResetPointer(CgDisplayWindowNativeServices& services)
{
    auto* const cursor = ResolveEnvironmentObjectReference<CursorNativeBody10h>(
        services.input.CaptureAt418210()->cursorAt0C);
    using Reset = std::uint8_t (__thiscall*)(CursorNativeBody10h*);
    Reset reset;
    std::memcpy(&reset, reinterpret_cast<const std::byte*>(cursor->vptrAt00) + 8, sizeof(reset));
    InputNativeScope scope(services.inputServices);
    (void)reset(cursor);
}
void MovePointerAxis(std::int32_t& value, LONG point, LONG previous,
    std::int32_t minimum, std::int32_t maximum)
{
    const auto difference = Sub(point, previous);
    const auto distance = difference < 0 ? Sub(0, difference) : difference;
    if (point > previous)
    {
        value = Add(value, distance);
        if (value > maximum) value = maximum;
    }
    else if (point < previous)
    {
        value = Sub(value, distance);
        if (value < minimum) value = minimum;
    }
}
void UpdateKeyboard(Body& body, std::int32_t& x, std::int32_t& y,
    CgDisplayWindowNativeServices& services)
{
    if (ReadKeyboardPressedAt47F4E0(*Keyboard(services), 1) ||
        ReadKeyboardPressedAt47F4E0(*Keyboard(services), 2) ||
        ReadKeyboardPressedAt47F4E0(*Keyboard(services), 3) ||
        ReadKeyboardPressedAt47F4E0(*Keyboard(services), 4))
    {
        body.repeatCountdownAt4C = 0;
        body.repeatWaitAt48 = 10;
        EndCgDisplayWindowDragNativeAt4C13A0(body, 1, services);
    }
    std::int32_t direction = -1;
    if (ReadKeyboardHeldAt4198D0(*Keyboard(services), 1)) direction = 0;
    else if (ReadKeyboardHeldAt4198D0(*Keyboard(services), 2)) direction = 1;
    else if (ReadKeyboardHeldAt4198D0(*Keyboard(services), 3)) direction = 2;
    else if (ReadKeyboardHeldAt4198D0(*Keyboard(services), 4)) direction = 3;
    else body.repeatCountdownAt4C = 1;
    if (body.repeatCountdownAt4C)
    {
        body.repeatCountdownAt4C = Sub(body.repeatCountdownAt4C, 1);
        return;
    }
    switch (direction)
    {
    case 0:
        y = Add(y, body.stepAt34);
        if (y > body.maximumYAt3C) y = body.maximumYAt3C;
        break;
    case 1:
        y = Sub(y, body.stepAt34);
        if (y < body.minimumYAt44) y = body.minimumYAt44;
        break;
    case 2:
        x = Add(x, body.stepAt34);
        if (x > body.maximumXAt38) x = body.maximumXAt38;
        break;
    case 3:
        x = Sub(x, body.stepAt34);
        if (x < body.minimumXAt40) x = body.minimumXAt40;
        break;
    default: break;
    }
    body.repeatCountdownAt4C = body.repeatWaitAt48;
    body.repeatWaitAt48 = 0;
}
}
void BeginCgDisplayWindowNativeAt4AA860(Body& body, CgDisplayWindowNativeServices& services)
{
    body.draggingAt54 = 0;
    ResetPointer(services);
}
void EndCgDisplayWindowDragNativeAt4C13A0(Body& body, std::int32_t restoreRegistry,
    CgDisplayWindowNativeServices& services)
{
    if (restoreRegistry && body.draggingAt54)
    {
        // 4C13C3在Capture/417990之后读取saved；不能用赋值表达式先求右值。
        auto& slot = Slot2(services);
        slot = body.savedSlot2At50;
    }
    const auto axes = body.axesAt30;
    body.draggingAt54 = 0;
    if (axes & 3u) SetPointer(body, services);
    else ResetPointer(services);
}
void UpdateCgDisplayWindowNativeAt4BD050(Body& body, CgDisplayWindowNativeServices& services)
{
    if (!(body.axesAt30 & 3u)) return;
    SetPointer(body, services);
    const auto oldX = X(body);
    const auto oldY = Y(body);
    auto x = oldX;
    auto y = oldY;
    // 4BD0B5先固定drag字节，再捕获Mouse；锁/来源回调不能重选此分支。
    const auto dragging = body.draggingAt54;
    auto* const mouse = Mouse(services);
    if (!dragging)
    {
        if (!mouse->previousAt30[0] && mouse->pressedAt10[0] && !mouse->suppressionAt28)
        {
            body.lastPointerAt58 = Mouse(services)->logicalAt18;
            body.draggingAt54 = 1;
            body.savedSlot2At50 = Slot2(services);
            Slot2(services) = 0;
        }
    }
    else
    {
        if (!mouse->pressedAt10[0] || mouse->suppressionAt28)
            EndCgDisplayWindowDragNativeAt4C13A0(body, 1, services);
        else
        {
            const auto point = Mouse(services)->logicalAt18;
            auto* const root = services.environment.CaptureAt4180C0();
            RECT client{};
            services.windowApi.ReadClientRectangle(reinterpret_cast<HWND>(root->windowAt2C), client);
            if (!services.windowApi.Contains(client, point))
                EndCgDisplayWindowDragNativeAt4C13A0(body, 1, services);
        }
        if (body.draggingAt54)
        {
            const auto point = Mouse(services)->logicalAt18;
            MovePointerAxis(x, point.x, body.lastPointerAt58.x, body.minimumXAt40, body.maximumXAt38);
            MovePointerAxis(y, point.y, body.lastPointerAt58.y, body.minimumYAt44, body.maximumYAt3C);
            body.lastPointerAt58 = point;
        }
    }
    if (oldX == x && oldY == y) UpdateKeyboard(body, x, y, services);
    if (oldX == x && oldY == y)
    {
        const auto axes = body.axesAt30;
        // 纵轴优先；横轴的4AD740同样解析InputRoot.mouseAt00，不能换成另一个输入源。
        if (axes & 2u)
        {
            auto* const axis = Mouse(services);
            if (!axis->secondaryAt2C)
            {
                const auto wheel = axis->wheelAt34;
                if (wheel < 0) y = Sub(y, body.stepAt34);
                else if (wheel > 0) y = Add(y, body.stepAt34);
                if (wheel) EndCgDisplayWindowDragNativeAt4C13A0(body, 1, services);
            }
            if (y > body.maximumYAt3C) y = body.maximumYAt3C;
            if (y < body.minimumYAt44) y = body.minimumYAt44;
        }
        else if (axes & 1u)
        {
            auto* const axis = Mouse(services);
            if (!axis->secondaryAt2C)
            {
                const auto wheel = axis->wheelAt34;
                if (wheel < 0) x = Sub(x, body.stepAt34);
                else if (wheel > 0) x = Add(x, body.stepAt34);
                if (wheel) EndCgDisplayWindowDragNativeAt4C13A0(body, 1, services);
            }
            if (x > body.maximumXAt38) x = body.maximumXAt38;
            if (x < body.minimumXAt40) x = body.minimumXAt40;
        }
    }
    if (oldX != x || oldY != y)
    {
        body.window.layer.valueAt04 = std::uint32_t(x);
        body.window.layer.valueAt08 = std::uint32_t(y);
        AddParentDirty(body, &body.window.boundsAt14);
    }
}
}
