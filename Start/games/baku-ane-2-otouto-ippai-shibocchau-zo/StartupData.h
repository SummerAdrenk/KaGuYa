#pragma once

#include <Runtime/ToolDialogNative.h>
#include <Runtime/WaitMarkNative.h>
#include <Runtime/SelectDialogNative.h>
#include <Runtime/OpacityDialogNative.h>

namespace KaGuYa::Start::Runtime::BakuAne2StartupData
{
const ToolDialogNativeProfile& Tool() noexcept;
const ToolDialogNativeRelation& ToolRelation() noexcept;
const WaitMarkNativeProfile& WaitMark() noexcept;
const SelectDialogNativeProfile& Selection() noexcept;
// Track only: original background (774,547) needs a shared profile extension.
const OpacityDialogNativeProfile& Opacity() noexcept;
bool HasCountdown() noexcept;
}
