#include <Runtime/SettingDialogNativeBusiness.h>

namespace KaGuYa::Start::Runtime
{
void SettingDialogNativeBusiness::ApplySettingsAt51CFC0(SettingDialogNativeBodyACh& body)
{
    // 509535：逐个child End之后才进入51CFC0，异常不得转成成功返回。
    SettingDialogNativeApplyScope scope(apply_);
    ApplySettingDialogNativeAt51CFC0(body);
}
std::int32_t SettingDialogNativeBusiness::PollModeConfirmationAt51E4B0()
{
    SettingDialogNativeConfirmScope scope(confirm_);
    return PollSettingDialogModeNativeAt51E4B0();
}
std::int32_t SettingDialogNativeBusiness::PollDefaultsConfirmationAt51B730(SettingDialogNativeBodyACh& body)
{
    SettingDialogNativeConfirmScope scope(confirm_);
    return PollSettingDialogDefaultsNativeAt51B730(body);
}
std::int32_t SettingDialogNativeBusiness::PollNameConfirmationAt521AD0(SettingDialogNativeBodyACh& body)
{
    SettingDialogNativeConfirmScope scope(confirm_);
    return PollSettingDialogNameNativeAt521AD0(body);
}
}
