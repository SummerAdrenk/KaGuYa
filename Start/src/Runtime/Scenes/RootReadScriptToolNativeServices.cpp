#include <Runtime/RootReadScriptToolNativeServices.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/ScriptArithmeticCommandNative.h>

namespace KaGuYa::Start::Runtime
{
DrawToolView RootReadScriptToolNativeServices::Window()
{
    dependencies_.root.EnsureCreatedAt413BE0();
    auto* const root=dependencies_.root.ReadPublishedAt6198C4();
    auto* const draw=ResolveEnvironmentObjectReference<DrawManagerNativeBody2Ch>(root->DrawManagerSlotAt18());
    return DrawLayersView(draw,dependencies_.memory)
        .WithNativeCallServices(root->PostWindow().BorrowNativeDrawCallServices()).ToolAtSlotTwo(&dependencies_.tool);
}
void RootReadScriptToolNativeServices::SetToolModeAt434FB0(std::int32_t mode){Window().SetModeAt434FB0(mode);}
void RootReadScriptToolNativeServices::InitializeToolAt4AB910(bool enabled){Window().ResetSkipLatchAt4AB910(std::uint8_t(enabled));}
bool RootReadScriptToolNativeServices::PollConfiguredKeyAt4B0630(std::uint32_t offset)
{
    // 4B2A50/57/5E先取Env5并读完整DWORD；只有4B0630支持的selector才取Input。
    auto* const click=ResolveEnvironmentArchiveAt417990(*dependencies_.environment.CaptureAt4180C0(),5);
    const auto selector=*reinterpret_cast<const volatile std::uint32_t*>(reinterpret_cast<std::uintptr_t>(click)+offset);
    return ReadMouseSelectorAt4B0630(dependencies_.input,selector);
}
bool RootReadScriptToolNativeServices::ReadReplayValue()
{
    // 4B2A0F/4B327F先锁定Root，498020回调换发布根仍读取已锁定Root的CParams。
    dependencies_.root.EnsureCreatedAt413BE0();
    auto* const root=dependencies_.root.ReadPublishedAt6198C4();
    static_cast<void>(ResolveEnvironmentArchiveAt417990(*dependencies_.environment.CaptureAt4180C0(),0));
    const auto index=ResolveScriptNamedValueNativeAt498020("replay",dependencies_.names,dependencies_.strings);
    return ReadScriptValueAt4C73A0(root->ParamsAt47BDB0(),index,dependencies_.errors)!=0;
}
std::uint32_t RootReadScriptToolNativeServices::UpdateToolAt4BE200(){return Window().TickAt4BE200();}
std::int32_t RootReadScriptToolNativeServices::ToolEventAt34(){return Window().ReadEventAt34();}
std::int32_t RootReadScriptToolNativeServices::ToolSkipPredicateAt4B0760(bool held)
{return ReadToolSkipStatusNativeAt4B0760(dependencies_.tool.state,held);}
std::int32_t RootReadScriptToolNativeServices::ToolAutoPredicateAt4B0470(bool held)
{return ReadToolAutoStatusNativeAt4B0470(dependencies_.tool.state,held);}
bool RootReadScriptToolNativeServices::PollSaveAt47F830(){return ReadScriptSaveKeyAt47F830(dependencies_.environment,dependencies_.input);}
bool RootReadScriptToolNativeServices::PollLoadAt47F650(){return ReadScriptLoadKeyAt47F650(dependencies_.environment,dependencies_.input);}
bool RootReadScriptToolNativeServices::PollQuickSaveAt47F780()
{return ReadScriptQuickSaveKeyAt47F780(dependencies_.environment,dependencies_.input,dependencies_.root,dependencies_.errors,dependencies_.names,dependencies_.strings);}
bool RootReadScriptToolNativeServices::PollQuickLoadAt47F6D0()
{return ReadScriptQuickLoadKeyAt47F6D0(dependencies_.environment,dependencies_.input,dependencies_.root,dependencies_.errors,dependencies_.names,dependencies_.strings);}
bool RootReadScriptToolNativeServices::PollLikabilityAt47F530(std::int32_t check)
{return ReadScriptLikabilityKeyAt47F530(dependencies_.environment,dependencies_.input,dependencies_.root,std::uint32_t(check),dependencies_.errors,dependencies_.names,dependencies_.strings);}
void RootReadScriptToolNativeServices::CaptureToolAt4C01F0(){Window().CaptureAt4C01F0();}
void RootReadScriptToolNativeServices::SetToolCaptureModeAt4C2300(bool enabled){Window().SetCaptureModeAt4C2300(std::uint8_t(enabled));}
void RootReadScriptToolNativeServices::RestoreToolAt4BF4E0(){Window().RestoreAt4BF4E0();}
}
