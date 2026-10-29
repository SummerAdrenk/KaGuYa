#pragma once

#include <Runtime/WaitMarkNative.h>
#include <Runtime/SelectDialogNative.h>
#include <Runtime/OpacityDialogNative.h>

namespace KaGuYa::Start::Runtime::BakuWindowData
{
const WaitMarkNativeProfile& WaitMark() noexcept;
const SelectDialogNativeProfile& Selection() noexcept;
const OpacityDialogNativeProfile& Opacity() noexcept;
}
