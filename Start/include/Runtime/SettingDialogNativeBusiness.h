#pragma once
#include <Runtime/SettingDialogNativeApply.h>
#include <Runtime/SettingDialogNativeConfirm.h>

namespace KaGuYa::Start::Runtime
{
// 父菜单只分派原业务；同源服务必须活到所有父/子及逃逸Ref退役之后。
class SettingDialogNativeBusiness final : public ISettingDialogNativeRemainingBusiness
{
public:
    SettingDialogNativeBusiness(SettingDialogNativeApplyServices& apply,
        SettingDialogNativeConfirmServices& confirm) noexcept : apply_(apply), confirm_(confirm) {}
    void ApplySettingsAt51CFC0(SettingDialogNativeBodyACh&) override;
    std::int32_t PollModeConfirmationAt51E4B0() override;
    std::int32_t PollDefaultsConfirmationAt51B730(SettingDialogNativeBodyACh&) override;
    std::int32_t PollNameConfirmationAt521AD0(SettingDialogNativeBodyACh&) override;
private:
    SettingDialogNativeApplyServices& apply_;
    SettingDialogNativeConfirmServices& confirm_;
};
}
