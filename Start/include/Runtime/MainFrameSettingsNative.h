#pragma once
#include <Runtime/GamePropertyNative.h>
#include <Runtime/RootMainFrameMenuNativeRuntime.h>
#include <Runtime/SettingDialogNativeApply.h>
#include <Runtime/SelectWindowNativeWorkflow.h>
#include <Runtime/ToolWindowNative.h>

namespace KaGuYa::Start::Runtime
{
class IMainFrameSettingsNativeAccess
{
public:
    virtual ~IMainFrameSettingsNativeAccess() = default;
    // 只投影已经固定的Root；不得再次Ensure或换用当前发布根。
    virtual void WriteMinimizeAt1B2(void* fixedRoot, BYTE value) = 0;
    virtual ScriptParamsNativeBody* ParamsAt47BDB0(void* fixedRoot) = 0;
    virtual std::int32_t ResolveNamedAt498020(void* fixedArchive, const char* name) = 0;
    virtual std::int32_t ReadValueAt4C73A0(ScriptParamsNativeBody*, std::int32_t index) = 0;
};
class RootMainFrameSettingsNativeAccess final : public IMainFrameSettingsNativeAccess
{
public:
    RootMainFrameSettingsNativeAccess(IScrVmCommandErrorSink&,
        IRuntimeStringManagerSource& = DefaultRuntimeStringManagerSource(),
        ScriptNamedValueNativeTables = GameScriptNamedValueTables()) noexcept;
    void WriteMinimizeAt1B2(void*, BYTE) override;
    ScriptParamsNativeBody* ParamsAt47BDB0(void*) override;
    std::int32_t ResolveNamedAt498020(void*, const char*) override;
    std::int32_t ReadValueAt4C73A0(ScriptParamsNativeBody*, std::int32_t) override;
private:
    IScrVmCommandErrorSink& errors_;
    IRuntimeStringManagerSource& strings_;
    ScriptNamedValueNativeTables names_;
};
class IMainFrameSettingsNativeApi
{
public:
    virtual ~IMainFrameSettingsNativeApi() = default;
    virtual BOOL WinNlsEnableIME(HWND, BOOL) = 0;
};
IMainFrameSettingsNativeApi& DefaultMainFrameSettingsNativeApi() noexcept;

// 服务全部借用，不进33Ch正文/Ref控制块，不建立第二份配置或声音状态。
struct MainFrameSettingsNativeServices final
{
    SettingDialogNativeApplyServices& apply;
    GamePropertyNativeServices& property;
    SoundManagerNativeVolume& volume;
    IMainFrameSettingsNativeAccess& access;
    IMainFrameSettingsNativeApi& api;
    EnvironmentRootOwner& environment;
    SelectWindowNativeServices& selectWindows;
    SelectWindowNativeWorkflowServices& selectWorkflow;
    ToolWindowNativeServices& tools;
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory();
};
// menu为MainFrame+54，menu+8是原页号；不接受完整MainFrame起点替代。
void RunMainFrameSettingsNativeAt428E30(void* menu, MainFrameSettingsNativeServices&);
class MainFrameSettingsCommandNativeRuntime final : public IMainFrameSettingsCommandNative
{
public:
    explicit MainFrameSettingsCommandNativeRuntime(MainFrameSettingsNativeServices& services) noexcept
        : services_(services) {}
    void RunAt428E30(void* menu) override;
private:
    MainFrameSettingsNativeServices& services_;
};
}
