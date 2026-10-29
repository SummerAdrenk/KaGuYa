#pragma once

#include <Platform/Input.h>

#include <cstddef>
#include <span>

namespace KaGuYa::Start::Platform
{
// 样本18的44FCC2：88个action容器，不是底层256字节VK状态的容量。
inline constexpr std::size_t MamatomoKeyboardActionCapacity = 0x58;

// 4509CE传入601358；保持51对原始顺序，不含6013BE的FF终止项。
// 返回进程期只读存储；仅提供键盘数据，不规定未初始化的Mouse+38。
[[nodiscard]] std::span<const KeyboardBinding> MamatomoKeyboardBindingsAt601358() noexcept;
}
