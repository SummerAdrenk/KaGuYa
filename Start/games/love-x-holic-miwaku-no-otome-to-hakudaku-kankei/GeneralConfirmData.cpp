#include "GeneralConfirmData.h"

namespace KaGuYa::Start::Runtime::LoveXHolicMiwakuNoOtomeToHakudakuKankeiGeneralConfirmData
{
namespace
{
// 原PE逐项依据：common-ui-profile-audit/tables/08.json；仅主线0-4及04/08确证额外模式。
// 5-8删除由现代SaveLoad承接；额外模式的数据/结果不代表父业务已接通。
constexpr const char* names[]{
    "\x8A\x6D\x94\x46\x98\x67\x2E\x76\x72\x6D",
    "\x8A\x6D\x94\x46\x83\x81\x83\x62\x83\x5A\x5F\x8F\x49\x97\xB9\x2E\x76\x72\x6D",
    "\x8A\x6D\x94\x46\x83\x81\x83\x62\x83\x5A\x5F\x83\x5E\x83\x43\x83\x67\x83\x8B\x2E\x76\x72\x6D",
    "\x8A\x6D\x94\x46\x83\x81\x83\x62\x83\x5A\x5F\x8F\x89\x8A\xFA\x90\xDD\x92\xE8\x2E\x76\x72\x6D",
    "\x8A\x6D\x94\x46\x83\x81\x83\x62\x83\x5A\x5F\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x5A\x81\x5B\x83\x75\x2E\x76\x72\x6D",
    "\x8A\x6D\x94\x46\x83\x81\x83\x62\x83\x5A\x5F\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x8D\x81\x5B\x83\x68\x2E\x76\x72\x6D",
    "\x8A\x6D\x94\x46\x5F\x83\x4C\x83\x83\x83\x89\x91\x49\x91\xF0\x2E\x76\x72\x6D",
    "\x8A\x6D\x94\x46\x5F\x91\xDE\x8F\x6F\x82\xB5\x82\xDC\x82\xB7\x82\xA9\x81\x48\x2E\x76\x72\x6D",
    "\x8A\x6D\x94\x46\x5F\x82\xB1\x82\xCC\x83\x41\x83\x43\x83\x65\x83\x80\x82\xF0\x8C\xF0\x8A\xB7\x82\xB5\x82\xDC\x82\xB7\x82\xA9\x81\x48\x2E\x76\x72\x6D",
    "\x8A\x6D\x94\x46\x5F\x96\x82\x91\x66\x82\xAA\x91\xAB\x82\xE8\x82\xC4\x82\xA2\x82\xDC\x82\xB9\x82\xF1\x2E\x76\x72\x6D",
    "\x8A\x6D\x94\x46\x91\x49\x91\xF0\x5F\x82\xCD\x82\xA2\x2E\x76\x72\x6D",
    "\x8A\x6D\x94\x46\x91\x49\x91\xF0\x5F\x82\xCD\x82\xA2\x83\x4A\x81\x5B\x83\x5C\x83\x8B\x2E\x76\x72\x6D",
    "\x8A\x6D\x94\x46\x91\x49\x91\xF0\x5F\x82\xCD\x82\xA2\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D",
    "\x8A\x6D\x94\x46\x91\x49\x91\xF0\x5F\x82\xA2\x82\xA2\x82\xA6\x2E\x76\x72\x6D",
    "\x8A\x6D\x94\x46\x91\x49\x91\xF0\x5F\x82\xA2\x82\xA2\x82\xA6\x83\x4A\x81\x5B\x83\x5C\x83\x8B\x2E\x76\x72\x6D",
    "\x8A\x6D\x94\x46\x91\x49\x91\xF0\x5F\x82\xA2\x82\xA2\x82\xA6\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D",
    "\x8A\x6D\x94\x46\x5F\x96\x82\x91\x66\x82\xAA\x91\xAB\x82\xE8\x82\xC4\x82\xA2\x82\xDC\x82\xB9\x82\xF1\x74\x61\x72\x67\x65\x74\x2E\x76\x72\x6D",
    "\x8A\x6D\x94\x46\x5F\x96\x82\x91\x66\x82\xAA\x91\xAB\x82\xE8\x82\xC4\x82\xA2\x82\xDC\x82\xB9\x82\xF1\x63\x6C\x69\x63\x6B\x2E\x76\x72\x6D",
};
constexpr GuiStaticDescriptorNative24h statics[]{
    {0, {names[0], nullptr, nullptr, names[0], names[0], nullptr, nullptr}, nullptr}, // 原表 0x576288
    {1, {names[1], nullptr, nullptr, names[1], names[1], nullptr, nullptr}, nullptr}, // 原表 0x5762AC
    {2, {names[2], nullptr, nullptr, names[2], names[2], nullptr, nullptr}, nullptr}, // 原表 0x5762D0
    {3, {names[3], nullptr, nullptr, names[3], names[3], nullptr, nullptr}, nullptr}, // 原表 0x5762F4
    {4, {names[4], nullptr, nullptr, names[4], names[4], nullptr, nullptr}, nullptr}, // 原表 0x576318
    {5, {names[5], nullptr, nullptr, names[5], names[5], nullptr, nullptr}, nullptr}, // 原表 0x57633C
    {10, {names[6], nullptr, nullptr, names[6], names[6], nullptr, nullptr}, nullptr}, // 原表 0x576414
    {13, {names[7], nullptr, nullptr, names[7], names[7], nullptr, nullptr}, nullptr}, // 原表 0x576438
    {14, {names[8], nullptr, nullptr, names[8], names[8], nullptr, nullptr}, nullptr}, // 原表 0x57645C
    {15, {names[9], nullptr, nullptr, names[9], names[9], nullptr, nullptr}, nullptr}, // 原表 0x576480
};
constexpr GuiButtonDescriptorNative2Ch buttons[]{
    {{11, {names[10], nullptr, nullptr, names[11], names[11], names[12], nullptr}, nullptr}, 7, -1}, // 原表 0x576538
    {{12, {names[13], nullptr, nullptr, names[14], names[14], names[15], nullptr}, nullptr}, 7, -1}, // 原表 0x576564
};
constexpr GuiButtonDescriptorNative2Ch ack[]{
    {{16, {nullptr, nullptr, nullptr, names[16], names[16], names[17], nullptr}, nullptr}, 7, 5}, // 原表 0x5764A4
};
constexpr GeneralConfirmModeRow modes[]{
    {0, 0, 1, GeneralConfirmButtons::YesNo},
    {1, 0, 2, GeneralConfirmButtons::YesNo},
    {2, 0, 3, GeneralConfirmButtons::YesNo},
    {3, 0, 4, GeneralConfirmButtons::YesNo},
    {4, 0, 5, GeneralConfirmButtons::YesNo},
    {9, 0, 6, GeneralConfirmButtons::YesNo},
    {10, 0, 7, GeneralConfirmButtons::YesNo},
    {11, 9, -1, GeneralConfirmButtons::Acknowledge},
    {12, 0, 8, GeneralConfirmButtons::YesNo},
};
constexpr GuiDialogNavigationMapping navigation0[]{{11, 12}, {12, 11}, {-1, 0}};
constexpr GuiDialogNavigationMapping navigation1[]{{11, 12}, {12, 11}, {-1, 0}};
constexpr GeneralConfirmNavigation navigation[]{
    {3, navigation0},
    {4, navigation1},
};
constexpr GeneralConfirmEventResult results[]{{11, 6}, {12, 7}, {16, 1}};
constexpr GeneralConfirmResourceProfile profile{
    .statics=statics, .modes=modes,
    .additionalStatics={},
    .yesNoButtons=buttons, .acknowledgeButtons=ack,
    .navigation=navigation, .eventResults=results,
    .unknownEvent=GeneralConfirmUnknownEvent::ThrowIntOne
};
}
GeneralConfirmResourceProfile Profile() noexcept { return profile; }
}
