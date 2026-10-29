#include <Runtime/FreeScrollNativeInput.h>
#include <Runtime/FreeScrollNativeDrawing.h>

#include <bit>
#include <cstring>

namespace KaGuYa::Start::Runtime
{
namespace
{
std::int32_t Add(std::int32_t a, std::int32_t b) noexcept
{
    return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(a) + static_cast<std::uint32_t>(b));
}
std::int32_t Sub(std::int32_t a, std::int32_t b) noexcept
{
    return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(a) - static_cast<std::uint32_t>(b));
}
MouseManagerNativeBody3Ch* Mouse(FreeScrollNativeInputServices& services)
{
    return ResolveEnvironmentObjectReference<MouseManagerNativeBody3Ch>(
        services.input.CaptureAt418210()->mouseAt00);
}
KeyboardManagerNativeBody438h* Keyboard(FreeScrollNativeInputServices& services)
{
    return ResolveEnvironmentObjectReference<KeyboardManagerNativeBody438h>(
        services.input.CaptureAt418210()->keyboardAt18);
}
void MovePointerAxis(std::int32_t& value, LONG point, LONG previous, LONG minimum, LONG maximum)
{
    const auto difference = Sub(point, previous);
    const auto distance = difference < 0 ? Sub(0, difference) : difference;
    if (point > previous)
    {
        value = Sub(value, distance);
        if (value < minimum) value = minimum;
    }
    else if (point < previous)
    {
        value = Add(value, distance);
        if (value > maximum) value = maximum;
    }
}
void UpdateMouse(FreeScrollNativeBody50h& body, FreeScrollNativeInputServices& services)
{
    // 4BDD78先保存dragging，再捕获mouse；捕获回调不能改变本次分支选择。
    const auto dragging = body.draggingAt44;
    auto* mouse = Mouse(services);
    if (!dragging)
    {
        if (!mouse->previousAt30[0] && mouse->pressedAt10[0] && !mouse->suppressionAt28)
        {
            body.lastPointerAt48 = Mouse(services)->logicalAt18;
            body.draggingAt44 = 1;
        }
        return;
    }
    if (!mouse->pressedAt10[0] || mouse->suppressionAt28)
        body.draggingAt44 = 0;
    else
    {
        const POINT point = Mouse(services)->logicalAt18;
        auto* program = services.environment.CaptureAt4180C0();
        RECT client{};
        services.windowApi.ReadClientRectangle(reinterpret_cast<HWND>(program->windowAt2C), client);
        if (!services.windowApi.Contains(client, point)) body.draggingAt44 = 0;
    }
    if (!body.draggingAt44) return;
    const POINT point = Mouse(services)->logicalAt18;
    MovePointerAxis(body.xAt14, point.x, body.lastPointerAt48.x,
        body.limitsAt2C.left, body.limitsAt2C.right);
    MovePointerAxis(body.yAt18, point.y, body.lastPointerAt48.y,
        body.limitsAt2C.top, body.limitsAt2C.bottom);
    body.lastPointerAt48 = point;
}
void UpdateKeyboard(FreeScrollNativeBody50h& body, FreeScrollNativeInputServices& services)
{
    if (ReadKeyboardPressedAt47F4E0(*Keyboard(services), 1) ||
        ReadKeyboardPressedAt47F4E0(*Keyboard(services), 2) ||
        ReadKeyboardPressedAt47F4E0(*Keyboard(services), 3) ||
        ReadKeyboardPressedAt47F4E0(*Keyboard(services), 4))
    {
        body.repeatCountdownAt40 = 0;
        body.repeatWaitAt3C = 10;
        body.draggingAt44 = 0;
    }
    std::int32_t direction = -1;
    if (ReadKeyboardHeldAt4198D0(*Keyboard(services), 1)) direction = 0;
    else if (ReadKeyboardHeldAt4198D0(*Keyboard(services), 2)) direction = 1;
    else if (ReadKeyboardHeldAt4198D0(*Keyboard(services), 3)) direction = 2;
    else if (ReadKeyboardHeldAt4198D0(*Keyboard(services), 4)) direction = 3;
    else body.repeatCountdownAt40 = 1;
    if (body.repeatCountdownAt40)
    {
        body.repeatCountdownAt40 = Sub(body.repeatCountdownAt40, 1);
        return;
    }
    switch (direction)
    {
    case 0:
        body.yAt18 = Sub(body.yAt18, body.stepAt10);
        if (body.yAt18 < body.limitsAt2C.top) body.yAt18 = body.limitsAt2C.top;
        break;
    case 1:
        body.yAt18 = Add(body.yAt18, body.stepAt10);
        if (body.yAt18 > body.limitsAt2C.bottom) body.yAt18 = body.limitsAt2C.bottom;
        break;
    case 2:
        body.xAt14 = Sub(body.xAt14, body.stepAt10);
        if (body.xAt14 < body.limitsAt2C.left) body.xAt14 = body.limitsAt2C.left;
        break;
    case 3:
        body.xAt14 = Add(body.xAt14, body.stepAt10);
        if (body.xAt14 > body.limitsAt2C.right) body.xAt14 = body.limitsAt2C.right;
        break;
    }
    body.repeatCountdownAt40 = body.repeatWaitAt3C;
    body.repeatWaitAt3C = 0;
}
void UpdateWheel(FreeScrollNativeBody50h& body, FreeScrollNativeInputServices& services)
{
    const bool vertical = (body.axesAt0C & 2) != 0;
    if (!vertical && !(body.axesAt0C & 1)) return;
    auto* mouse = Mouse(services);
    auto& value = vertical ? body.yAt18 : body.xAt14;
    if (!mouse->secondaryAt2C)
    {
        const auto wheel = mouse->wheelAt34;
        if (wheel < 0) value = Add(value, body.stepAt10);
        else if (wheel > 0) value = Sub(value, body.stepAt10);
        if (wheel) body.draggingAt44 = 0;
    }
    const auto minimum = vertical ? body.limitsAt2C.top : body.limitsAt2C.left;
    const auto maximum = vertical ? body.limitsAt2C.bottom : body.limitsAt2C.right;
    if (value < minimum) value = minimum;
    if (value > maximum) value = maximum;
}
}

bool UpdateFreeScrollNativeAt4BDD00(FreeScrollNativeBody50h& body,
    FreeScrollNativeInputServices& services, FreeScrollNativeDrawServices& graphics,
    const ISpriteAnimationInitializationSource& registry, IGraphicsScreenCalls& screen)
{
    if (!body.axesAt0C) return false;
    const auto x = body.xAt14;
    const auto y = body.yAt18;
    auto* cursor = ResolveEnvironmentObjectReference<CursorNativeBody10h>(
        services.input.CaptureAt418210()->cursorAt0C);
    using Set = std::uint8_t(__thiscall*)(CursorNativeBody10h*, LPCSTR, std::uint8_t, std::uint8_t);
    Set set;
    std::memcpy(&set, reinterpret_cast<const std::byte*>(cursor->vptrAt00) + 4, sizeof(set));
    {
        InputNativeScope scope(services.inputServices);
        static_cast<void>(set(cursor, MAKEINTRESOURCEA(body.draggingAt44 ? 0xD0 : 0xCF), 0, 0));
    }
    UpdateMouse(body, services);
    if (x == body.xAt14 && y == body.yAt18) UpdateKeyboard(body, services);
    if (x == body.xAt14 && y == body.yAt18) UpdateWheel(body, services);
    RenderFreeScrollAt4AF200(body, x != body.xAt14 || y != body.yAt18, graphics, registry, screen);
    return true;
}
}
