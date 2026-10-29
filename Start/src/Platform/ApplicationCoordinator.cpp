#include <Platform/ApplicationCoordinator.h>

namespace KaGuYa::Start::Platform
{
void Win32ApplicationWindowApi::PostClose(const HWND window)
{
    // 434782..43478E 调用 42CE40，后者严格封装 PostMessageA(WM_CLOSE, 0, 0)。
    PostMessageA(window, WM_CLOSE, 0, 0);
}

void Win32ApplicationWindowApi::Show(const HWND window, const int command)
{
    ShowWindow(window, command);
}

ApplicationCoordinator::ApplicationCoordinator(
    MessageLoop& messageLoop,
    const IExitState& exitState,
    IApplicationWindowApi& windowApi) noexcept
    : messageLoop_(messageLoop), exitState_(exitState), windowApi_(windowApi)
{
}

int ApplicationCoordinator::Run(IApplicationStages& stages, const int showCommand)
{
    int result = 0;
    try
    {
        // 434433..43443A -> 43473C：前置失败抛整数 1 仅用于转移控制，不作为返回码。
        if (!stages.InitializeBeforeWindow())
        {
            throw 1;
        }

        const HWND window = stages.CreateMainWindow();
        if (!stages.InitializeAfterWindow())
        {
            // 434782..4347B7：后置失败先投递关闭，再运行消息循环；catch helper
            // 从 var_30 恢复循环退出码，而不是返回用于选路的整数 0。
            windowApi_.PostClose(window);
            result = messageLoop_.Run(exitState_);
        }
        else
        {
            // 434643..434663：只有后置阶段成功才显示窗口，随后进入同一个循环。
            windowApi_.Show(window, showCommand);
            result = messageLoop_.Run(exitState_);
        }
    }
    catch (...)
    {
        // 5E31DC 是无类型 catch-all；434689 恢复 var_30，不读取异常对象。
        // 43441D 初始为 0，4347A4 才保存后置失败路径的循环结果；保留 result。
    }

    // 43468C..4346A3：正常、前置失败、创建失败和后置失败均汇合到 477620。
    // 434691已经将EH state置2，原catch只覆盖3..4；释放异常不可转为result或terminate。
    stages.ReleaseSubsystems();
    return result;
}
}
