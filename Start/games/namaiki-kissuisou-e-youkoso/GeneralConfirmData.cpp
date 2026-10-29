#include "GeneralConfirmData.h"

namespace KaGuYa::Start::Runtime::NamaikiKissuisouEYoukosoGeneralConfirmData
{
namespace
{
// 原PE逐项依据：common-ui-profile-audit/tables/02.json；仅主线0-4及04/08确证额外模式。
// 5-8删除由现代SaveLoad承接；额外模式的数据/结果不代表父业务已接通。
constexpr const char* names[]{
    "\x8A\x6D\x94\x46\x98\x67\x2E\x76\x72\x6D",
    "\x8A\x6D\x94\x46\x83\x81\x83\x62\x83\x5A\x5F\x8F\x49\x97\xB9\x2E\x76\x72\x6D",
    "\x8A\x6D\x94\x46\x83\x81\x83\x62\x83\x5A\x5F\x83\x5E\x83\x43\x83\x67\x83\x8B\x2E\x76\x72\x6D",
    "\x8A\x6D\x94\x46\x83\x81\x83\x62\x83\x5A\x5F\x8F\x89\x8A\xFA\x90\xDD\x92\xE8\x2E\x76\x72\x6D",
    "\x8A\x6D\x94\x46\x83\x81\x83\x62\x83\x5A\x5F\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x5A\x81\x5B\x83\x75\x2E\x76\x72\x6D",
    "\x8A\x6D\x94\x46\x83\x81\x83\x62\x83\x5A\x5F\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x8D\x81\x5B\x83\x68\x2E\x76\x72\x6D",
    "\x8A\x6D\x94\x46\x91\x49\x91\xF0\x5F\x82\xCD\x82\xA2\x2E\x76\x72\x6D",
    "\x8A\x6D\x94\x46\x91\x49\x91\xF0\x5F\x82\xCD\x82\xA2\x83\x4A\x81\x5B\x83\x5C\x83\x8B\x2E\x76\x72\x6D",
    "\x8A\x6D\x94\x46\x91\x49\x91\xF0\x5F\x82\xCD\x82\xA2\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D",
    "\x8A\x6D\x94\x46\x91\x49\x91\xF0\x5F\x82\xA2\x82\xA2\x82\xA6\x2E\x76\x72\x6D",
    "\x8A\x6D\x94\x46\x91\x49\x91\xF0\x5F\x82\xA2\x82\xA2\x82\xA6\x83\x4A\x81\x5B\x83\x5C\x83\x8B\x2E\x76\x72\x6D",
    "\x8A\x6D\x94\x46\x91\x49\x91\xF0\x5F\x82\xA2\x82\xA2\x82\xA6\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D",
};
constexpr GuiStaticDescriptorNative24h statics[]{
    {0, {names[0], nullptr, nullptr, names[0], names[0], nullptr, nullptr}, nullptr}, // 原表 0x5803B8
    {1, {names[1], nullptr, nullptr, names[1], names[1], nullptr, nullptr}, nullptr}, // 原表 0x5803DC
    {2, {names[2], nullptr, nullptr, names[2], names[2], nullptr, nullptr}, nullptr}, // 原表 0x580400
    {3, {names[3], nullptr, nullptr, names[3], names[3], nullptr, nullptr}, nullptr}, // 原表 0x580424
    {4, {names[4], nullptr, nullptr, names[4], names[4], nullptr, nullptr}, nullptr}, // 原表 0x580448
    {5, {names[5], nullptr, nullptr, names[5], names[5], nullptr, nullptr}, nullptr}, // 原表 0x58046C
};
constexpr GuiButtonDescriptorNative2Ch buttons[]{
    {{10, {names[6], nullptr, nullptr, names[7], names[7], names[8], nullptr}, nullptr}, 7, -1}, // 原表 0x580590
    {{11, {names[9], nullptr, nullptr, names[10], names[10], names[11], nullptr}, nullptr}, 7, -1}, // 原表 0x5805BC
};
constexpr GeneralConfirmModeRow modes[]{
    {0, 0, 1, GeneralConfirmButtons::YesNo},
    {1, 0, 2, GeneralConfirmButtons::YesNo},
    {2, 0, 3, GeneralConfirmButtons::YesNo},
    {3, 0, 4, GeneralConfirmButtons::YesNo},
    {4, 0, 5, GeneralConfirmButtons::YesNo},
};
constexpr GuiDialogNavigationMapping navigation0[]{{10, 11}, {11, 10}, {-1, 0}};
constexpr GuiDialogNavigationMapping navigation1[]{{10, 11}, {11, 10}, {-1, 0}};
constexpr GeneralConfirmNavigation navigation[]{
    {3, navigation0},
    {4, navigation1},
};
constexpr GeneralConfirmEventResult results[]{{10, 6}, {11, 7}};
constexpr GeneralConfirmResourceProfile profile{
    .statics=statics, .modes=modes,
    .additionalStatics={},
    .yesNoButtons=buttons, .acknowledgeButtons={},
    .navigation=navigation, .eventResults=results,
    .unknownEvent=GeneralConfirmUnknownEvent::ThrowIntOne
};
}
GeneralConfirmResourceProfile Profile() noexcept { return profile; }
}
