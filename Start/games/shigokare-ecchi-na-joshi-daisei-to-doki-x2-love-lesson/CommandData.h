#pragma once

#include <Runtime/CommandSelectionNative.h>

namespace KaGuYa::Start::Runtime::ShigokareCommandData
{
// 仅提供原PE已证实数据；共享控制器负责状态、命中、导航和结果生命周期。
const CommandSelectionResourceProfile& Profile() noexcept;
// CRT原串不同于公共ProgramControlSound，不可代用现代.ogg常量。
const char* ConfirmCue() noexcept;
const char* HoverCue() noexcept;
}
