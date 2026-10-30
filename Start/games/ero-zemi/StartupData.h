#pragma once

#include <Runtime/ToolDialogNative.h>
#include <Runtime/WaitMarkNative.h>
#include <Runtime/SelectDialogNative.h>
#include <Runtime/OpacityDialogNative.h>

namespace KaGuYa::Start::Runtime::EroZemiStartupData
{
const ToolDialogNativeProfile& Tool() noexcept;
const ToolDialogNativeRelation& ToolRelation() noexcept;
const WaitMarkNativeProfile& WaitMark() noexcept;
const SelectDialogNativeProfile& Selection() noexcept;
const OpacityDialogNativeProfile& Opacity() noexcept;
bool HasCountdown() noexcept;
}
