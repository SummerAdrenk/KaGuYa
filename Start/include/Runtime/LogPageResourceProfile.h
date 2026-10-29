#pragma once
#include <Runtime/GuiControlFactoryNative.h>
#include <Runtime/GuiSliderNative.h>
#include <array>

namespace KaGuYa::Start::Runtime
{
enum class LogPageScrollPolicy { SingleStep, PageKeysAndHoldRepeat };
enum class LogPageNameLocalization { OriginalCp932, MamatomoGbkBaseline };
struct LogPageResourceProfile final
{
    GuiStaticDescriptorNative24h background;
    std::array<GuiButtonDescriptorNative2Ch,2> scrollButtons;
    GuiButtonDescriptorNative2Ch voiceButton;
    GuiSliderDescriptorNative6Ch slider;
    const char* rowFrame;
    std::array<POINT,4> rowPositions;
    SIZE rowSurface;
    POINT voiceShift,frameShift;
    std::array<int,4> nameFont,bodyFont;
    POINT nameOrigin,bodyWithoutName,bodyWithName;
    // 01–09使用普通click；10–18由Scroll接管事件和30/2轮询重复。
    LogPageScrollPolicy scrollPolicy;
    const char* namePrefix;
    const char* nameSuffix;
    // 18保留既有GBK括号；其余为原PE的CP932字节，不伪称本地化值来自原表。
    LogPageNameLocalization nameLocalization;
};
const LogPageResourceProfile& CurrentLogPageResourceProfile();
}
