#include "GaugeData.h"
#include <array>

namespace KaGuYa::Start::Runtime::ShigokareGaugeData
{
namespace
{
// 原55BE20/55BE28为文件内指针表，不是DateWin使用的CRT CString槽。
// CP932：ときめきゲージ_。显式保留全部50项，不按文件名猜测补帧。
#define GAUGE_PREFIX "\x82\xC6\x82\xAB\x82\xDF\x82\xAB\x83\x51\x81\x5B\x83\x57_"
constexpr char background[] = GAUGE_PREFIX "\x83\x78\x81\x5B\x83\x58\x83\x51\x81\x5B\x83\x57.vrm";
constexpr std::array<const char*, 50> frames{{
    GAUGE_PREFIX "00.vrm", GAUGE_PREFIX "02.vrm", GAUGE_PREFIX "04.vrm", GAUGE_PREFIX "06.vrm", GAUGE_PREFIX "08.vrm",
    GAUGE_PREFIX "10.vrm", GAUGE_PREFIX "12.vrm", GAUGE_PREFIX "14.vrm", GAUGE_PREFIX "16.vrm", GAUGE_PREFIX "18.vrm",
    GAUGE_PREFIX "20.vrm", GAUGE_PREFIX "22.vrm", GAUGE_PREFIX "24.vrm", GAUGE_PREFIX "26.vrm", GAUGE_PREFIX "28.vrm",
    GAUGE_PREFIX "30.vrm", GAUGE_PREFIX "32.vrm", GAUGE_PREFIX "34.vrm", GAUGE_PREFIX "36.vrm", GAUGE_PREFIX "38.vrm",
    GAUGE_PREFIX "40.vrm", GAUGE_PREFIX "42.vrm", GAUGE_PREFIX "44.vrm", GAUGE_PREFIX "46.vrm", GAUGE_PREFIX "48.vrm",
    GAUGE_PREFIX "50.vrm", GAUGE_PREFIX "52.vrm", GAUGE_PREFIX "54.vrm", GAUGE_PREFIX "56.vrm", GAUGE_PREFIX "58.vrm",
    GAUGE_PREFIX "60.vrm", GAUGE_PREFIX "62.vrm", GAUGE_PREFIX "64.vrm", GAUGE_PREFIX "66.vrm", GAUGE_PREFIX "68.vrm",
    GAUGE_PREFIX "70.vrm", GAUGE_PREFIX "72.vrm", GAUGE_PREFIX "74.vrm", GAUGE_PREFIX "76.vrm", GAUGE_PREFIX "78.vrm",
    GAUGE_PREFIX "80.vrm", GAUGE_PREFIX "82.vrm", GAUGE_PREFIX "84.vrm", GAUGE_PREFIX "86.vrm", GAUGE_PREFIX "88.vrm",
    GAUGE_PREFIX "90.vrm", GAUGE_PREFIX "92.vrm", GAUGE_PREFIX "94.vrm", GAUGE_PREFIX "96.vrm", GAUGE_PREFIX "98.vrm"
}};
#undef GAUGE_PREFIX
const TokimekiGaugeResourceProfile profile{background, frames};
}
const TokimekiGaugeResourceProfile& Profile() noexcept { return profile; }
}
