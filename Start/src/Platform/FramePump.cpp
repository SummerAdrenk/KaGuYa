#include <Platform/FramePump.h>

namespace KaGuYa::Start::Platform
{
FramePump::FramePump(const HWND window, IFrameRuntime& runtime) noexcept
    : window_(window), runtime_(runtime)
{
}

void FramePump::OnIdle()
{
    // Start.exe:424E10 只在帧执行失败且引擎尚未请求退出时投递 WM_CLOSE。
    if (!runtime_.TickFrame() && !runtime_.ExitRequested())
    {
        PostMessageA(window_, WM_CLOSE, 0, 0);
    }
}
}
