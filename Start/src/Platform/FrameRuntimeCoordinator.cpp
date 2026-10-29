#include <Platform/FrameRuntimeCoordinator.h>

namespace KaGuYa::Start::Platform
{
FrameRuntimeCoordinator::FrameRuntimeCoordinator(IFrameRuntimeServices& services) noexcept
    : services_(services)
{
}

bool FrameRuntimeCoordinator::TickFrame()
{
    try
    {
        // 483524..483540：两个帧字段置位后，输入快照必须先于引擎更新。
        services_.BeginFrameState();
        services_.UpdateInput();
        services_.UpdatePreSceneEngine();

        // 483545..483657：AdvanceModal 判定 5105C0，DrawModal 实际执行 5075B0
        // 完整模态循环，不是只绘制一次；正常返回后跳过普通场景、提交与帧等待。
        if (services_.ModalProcessingEnabled() && services_.AdvanceModal())
        {
            services_.DrawModal();
            return true;
        }

        // 483658..4836F5：任一待提交容器非空时进入同一个 46BC90 边界。
        if (services_.HasPendingSceneWork())
        {
            services_.RunSceneTasks();
        }

        // 4836F5..483793：this+0x48 非空时不调用 Graphics::RenderSceen。
        if (!services_.DirectMoviePresent())
        {
            services_.RenderScreen();
        }

        // 483795..4837A7：引擎更新可以清除本帧的等待标志。
        if (services_.FrameWaitEnabled())
        {
            services_.WaitForFrame();
        }
        return true;
    }
    catch (...)
    {
        // 4837BB..4837C3 是 catch (...)，仅在该边界把任意异常转换为 false。
        return false;
    }
}

bool FrameRuntimeCoordinator::ExitRequested() const
{
    return services_.ExitRequested();
}
}
