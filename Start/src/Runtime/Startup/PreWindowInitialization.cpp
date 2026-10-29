#include <Runtime/PreWindowInitialization.h>

#include <Windows.h>

namespace KaGuYa::Start::Runtime
{
void PreWindowLifecycleState::Reset() noexcept
{
    Native().ResetAt4778B0();
    initializedAt4778B0 = true;
}

bool PreWindowInitializer::Run(
    PreWindowLifecycleState& lifecycleState,
    IPreWindowServices& services) const
{
    lifecycleState.Reset();

    try
    {
        // 47790A..477918：先注册 7 个环境 IArchive，再处理 env.ini。
        services.RegisterEnvironmentArchives(EnvironmentArchiveRegistrationOrder);
        if (!services.PrepareEnvironmentPersistence())
        {
            services.WriteEnvironmentPersistence();
        }

        // 47791D..477A1D：CWindowModeManager 在持久状态处理之后安装。
        services.InstallWindowModeManager();
        const auto& save = services.SaveDataAtSlot1();
        auto& click = services.MutableClickInfoAtSlot5();
        ApplyClickInfoMouseMapping(save, click);
        return true;
    }
    catch (...)
    {
        // 5E67D0 的 HandlerType.pType 为 null：这是 catch (...)，不是 int handler。
        std::string text = services.BuildInitializationErrorText();
        if (text.empty())
        {
            text = "Syntax error";
        }
        services.ShowInitializationError(text, "FirstInstance ", MB_ICONERROR);
        return false;
    }
}
}
