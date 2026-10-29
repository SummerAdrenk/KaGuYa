#pragma once

#include <Runtime/FreeScrollNative.h>
#include <Runtime/InputNativeObjects.h>

namespace KaGuYa::Start::Runtime
{
class ISpriteAnimationInitializationSource;
class IGraphicsScreenCalls;
// 每次原418210/4180C0都重新捕获根；不保存Mouse/Keyboard或HWND快照。
struct FreeScrollNativeInputServices final
{
    InputRootOwner& input;
    InputNativeServices& inputServices;
    EnvironmentRootOwner& environment;
    const Platform::ICursorApi& windowApi;
};

bool UpdateFreeScrollNativeAt4BDD00(FreeScrollNativeBody50h& body,
    FreeScrollNativeInputServices& input, FreeScrollNativeDrawServices& graphics,
    const ISpriteAnimationInitializationSource& registry, IGraphicsScreenCalls& screen);
}
