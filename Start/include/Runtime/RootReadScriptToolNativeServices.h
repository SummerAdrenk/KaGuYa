#pragma once
#include <Runtime/ReadScriptToolServicesRuntime.h>
#include <Runtime/ToolWindowNativeInteraction.h>
#include <Runtime/ReadScriptInputNative.h>

namespace KaGuYa::Start::Runtime
{
struct RootReadScriptToolNativeDependencies final
{
    RootRuntimeOwner& root;
    EnvironmentRootOwner& environment;
    InputRootOwner& input;
    ToolWindowNativeInteractionServices& tool;
    IScrVmCommandErrorSink& errors;
    ScriptNamedValueNativeTables names=GameScriptNamedValueTables();
    IRuntimeStringManagerSource& strings=DefaultRuntimeStringManagerSource();
    IEnvironmentRootMemory& memory=DefaultEnvironmentRootMemory();
};
// 原caller逐点取根；仅借元数据，不保存Tool/ClickInfo/UnSave/CParams或输入view。
class RootReadScriptToolNativeServices final:public IReadScriptToolServices
{
public:
    explicit RootReadScriptToolNativeServices(RootReadScriptToolNativeDependencies d)noexcept:dependencies_(d){}
    void SetToolModeAt434FB0(std::int32_t)override;
    void InitializeToolAt4AB910(bool)override;
    bool PollConfiguredKeyAt4B0630(std::uint32_t)override;
    bool ReadReplayValue()override;
    std::uint32_t UpdateToolAt4BE200()override;
    std::int32_t ToolEventAt34()override;
    std::int32_t ToolSkipPredicateAt4B0760(bool)override;
    std::int32_t ToolAutoPredicateAt4B0470(bool)override;
    bool PollSaveAt47F830()override;
    bool PollLoadAt47F650()override;
    bool PollQuickSaveAt47F780()override;
    bool PollQuickLoadAt47F6D0()override;
    bool PollLikabilityAt47F530(std::int32_t)override;
    void CaptureToolAt4C01F0()override;
    void SetToolCaptureModeAt4C2300(bool)override;
    void RestoreToolAt4BF4E0()override;
private:
    DrawToolView Window();
    RootReadScriptToolNativeDependencies dependencies_;
};
}
