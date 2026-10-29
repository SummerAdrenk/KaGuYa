#pragma once

#include <Runtime/StartupResourceProfile.h>
#include <Runtime/WaitMarkNative.h>
#include <Runtime/SelectDialogNative.h>
#include <Runtime/OpacityDialogNative.h>

namespace KaGuYa::Start::Runtime::HakoniwaStartupData
{
const ToolDialogNativeProfile& Tool() noexcept;
const ToolDialogNativeRelation& ToolRelation() noexcept;
const WaitMarkNativeProfile& WaitMark() noexcept;
const SelectDialogNativeProfile& Selection() noexcept;
const OpacityDialogNativeProfile& Opacity() noexcept;
bool HasCountdown() noexcept;
// 只组合五组已核数据；不隐式创建或禁用任何业务层。
const StartupResourceProfile& Resources() noexcept;
// 存在 Countdown 不等于能使用现代72资源，接线须同时检查下列能力。
bool CanReuseModernCountdownResources() noexcept;
bool HasCommandSelection() noexcept;
// 原 slot7=CmdSelectWin(mode==2)，slot8=Countdown；不能套普通八层边界。
}
