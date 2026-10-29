#pragma once
#include <Runtime/GameNamePolicy.h>
#include <Runtime/GuiControlFactoryNative.h>
#include <span>

namespace KaGuYa::Start::Runtime
{
// 只借用编译游戏数据；表、字符串和位置必须具有静态寿命。
struct TitleResourceProfile final
{
    const GuiStaticDescriptorNative24h& background;
    std::span<const GuiButtonDescriptorNative2Ch, 6> buttons;
    const char* blackName;
    const char* overlayName;
    const char* bgmName;
    const char* clickCue;
    const char* hoverCue;
    GameNameByteLimits nameLimits;
};
}
