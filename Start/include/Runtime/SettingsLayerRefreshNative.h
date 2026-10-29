#pragma once
#include <Runtime/LayerWindowNative.h>
#include <Runtime/SettingDialogNativeApply.h>

namespace KaGuYa::Start::Runtime
{
// 431000：每个已启用Pattern独立捕获Env，只更新具名AN的flags bit1。
void ApplySettingsAnimationFlagNativeAt431000(LayerWindowNativeBody4Ch&,
    EnvironmentRootOwner&);
// 431260：独立于433C50/4C1650；Clear在局部catch外，正文重绘失败提示后仍启用。
void RestoreSettingsTextVisibilityNativeAt431260(TextWindowLayerNativeBodyE8h&,
    SettingDialogNativeApplyServices&);
const char* SettingsTextVisibilityErrorSample18() noexcept;
}
