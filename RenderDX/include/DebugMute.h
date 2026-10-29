#pragma once

#include <Windows.h>

namespace KaGuYa::RenderDX
{
// 原0502D：返回上次请求，保留完整DWORD；调用点显式传1/0，不恢复previous。
DWORD SetDebugMute(DWORD requested) noexcept;
}
