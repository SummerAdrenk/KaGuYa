#pragma once
#include <Runtime/ScrVmYieldDispatcher.h>
#include <string_view>

namespace KaGuYa::Start::Runtime
{
// 正常VM与读档恢复共用的编译项路由；不按运行时名称猜测游戏版本。
std::string_view GameFixedActionSceneName(ScrVmFixedAction);
}
