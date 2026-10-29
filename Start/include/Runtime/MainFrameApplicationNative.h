#pragma once
#include <Runtime/MainFrameWindowNative.h>
#include <Runtime/MainFrameWindowClassNative.h>
#include <Runtime/MainFrameApplicationNativeApi.h>
#include <Runtime/WinMainNative.h>

namespace KaGuYa::Start::Runtime
{
// 每次413BE0之后单独读取发布槽；三个生命周期入口必须作用于本次选定Root。
// 完整进程owner必须提供实际实现，不允许默认成功或throw-only替代。
class IMainFrameApplicationNativeSource
{
public:
    virtual ~IMainFrameApplicationNativeSource() = default;
    virtual void EnsureRootAt413BE0() = 0;
    virtual void* RootAt6198C4() = 0;
    virtual bool InitializeBeforeWindowAt4778B0(void*) = 0;
    virtual bool InitializeAfterWindowAt489C90(void*) = 0;
    virtual void ReleaseSubsystemsAt477620(void*) = 0;
    virtual EnvironmentRootStorage* EnvironmentAt4180C0() = 0;
};
struct MainFrameApplicationNativeServices final
{
    MainFrameWindowNativeServices& window;
    MainFrameWindowClassNativeInfo54h& windowClass;
    MainFrameWindowClassNativeServices& classes;
    IIdleLoopNativeRootSource& idleRoot;
    IModalDialogMessageApi& messages;
    IWindowClassNativeApi& arrays;
    IMainFrameApplicationNativeApi& api;
};
// 4342A0的raw90h/38h栈对象与六态EH；不使用现代WindowHost/ApplicationCoordinator。
int RunMainFrameApplicationNativeAt4342A0(LPSTR, int, IMainFrameApplicationNativeSource&,
    MainFrameApplicationNativeServices&);
class MainFrameApplicationNative final : public IWinMainNativeApplication
{
public:
    MainFrameApplicationNative(IMainFrameApplicationNativeSource& source,
        MainFrameApplicationNativeServices& services) noexcept : source_(source), services_(services) {}
    int RunApplicationAt4342A0(LPSTR line, int show) override
    { return RunMainFrameApplicationNativeAt4342A0(line, show, source_, services_); }
private:
    IMainFrameApplicationNativeSource& source_;
    MainFrameApplicationNativeServices& services_;
};
}
