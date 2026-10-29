#include <Runtime/PostWindowInitializationNative.h>

namespace KaGuYa::Start::Runtime
{
bool RunPostWindowInitializationNativeAt489C90(IPostWindowRuntimeServices& services,
    IPostWindowInitializationNativeFiles& files)
{
    try
    {
        services.ConfirmFirstInstance();
        files.CreateInitializationProgressAt489D70();
        // 原489E58才置state3；此前startup确认、progress正文/control故障无此action。
        struct ProgressUnwind final
        {
            IPostWindowInitializationNativeFiles& files;
            ~ProgressUnwind() noexcept { files.ReleaseInitializationProgressAt460730(); }
        } unwind{files};
        files.CreateFileManagerAt489E56();
        PostWindowLifecycle lifecycle;
        lifecycle.InitializeAfterFileManager(services);
        return true;
    }
    catch (...)
    {
        return false; // 原48AE69；progress正常/展开释放均早于返回，无子系统回滚。
    }
}
}
