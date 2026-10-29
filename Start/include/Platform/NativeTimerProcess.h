#pragma once

#include <Platform/FrameTimer.h>

namespace KaGuYa::Start::Platform
{
// 稳定进程外观：首次取用只冷构造元数据，不Capture、不读tick、不建立计时源。
// API/source/分配器以显式存储常驻，不登记C++析构；source只用三参共享creator构造。
[[nodiscard]] Win32TimerPlatformApi& ProcessNativeTimerPlatformApi() noexcept;
[[nodiscard]] ProcessTimerSource& ProcessNativeTimerSource() noexcept;

// 5AAD80与43FBF0(flags=0)同一释放叶：重设表、释放当时source(flags=1)、返回后清槽。
// 不写count，不销毁creator/API/source外观；显式dependencies也供隔离测试使用。
// 原5AAD80无本地EH/terminate包装，此叶不另加catch或noexcept。
void ReleaseNativeTimerCreatorAt5AAD80(NativeTimerDependencies& dependencies);
}
