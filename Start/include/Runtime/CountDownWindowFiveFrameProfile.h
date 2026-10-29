#pragma once

#include <Windows.h>
#include <array>

namespace KaGuYa::Start::Runtime
{
// Mamagoto原551BC8/551BDC：五个独立Sprite，无后缀/颜色/字号/透明度变体。
// 名称仅在构造时借用；构造将每个Sprite相对union原点平移到origin。
struct CountDownWindowFiveFrameResourceProfile final
{
    std::array<const char*, 5> namesCp932;
    POINT origin;
};
}
