#include <Runtime/RootReadScriptMenuNativeServices.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/DrawManagerNative.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/ScriptArithmeticCommandNative.h>
#include <Runtime/ScriptNativeBody.h>
#include <Runtime/PostWindowNativeLayers.h>
#include <Runtime/CommandSelectionNative.h>
#include <Runtime/GameBuildProfile.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
RootReadScriptMenuNativeServices::RootReadScriptMenuNativeServices(
    RootReadScriptMenuNativeServicesDependencies dependencies, bool command) noexcept
    : dependencies_(dependencies), command_(command) {}

RootRuntimePayload& RootReadScriptMenuNativeServices::CaptureRootAfter413BE0()
{
    dependencies_.root.EnsureCreatedAt413BE0();
    return *dependencies_.root.ReadPublishedAt6198C4();
}

DrawLayersView RootReadScriptMenuNativeServices::CaptureLayersAfter413BE0()
{
    auto& root = CaptureRootAfter413BE0();
    auto* draw = ResolveDrawManagerReferenceAt4175A0(root.DrawManagerSlotAt18());
    return DrawLayersView(draw).WithNativeCallServices(root.PostWindow().BorrowNativeDrawCallServices());
}

void RootReadScriptMenuNativeServices::RebuildChoicesAt4369E0()
{
    // 4B126F..4B128A：只选本次Root+18的owning slot5；4375BA原catch留在workflow。
    auto& root=CaptureRootAfter413BE0();
    auto* draw=ResolveDrawManagerReferenceAt4175A0(root.DrawManagerSlotAt18());
    const auto services=root.PostWindow().BorrowNativeDrawCallServices();
    if(!services.layers)throw std::logic_error("native ReadScript Menu requires captured Root selection services");
    if (command_)
    {
        if (!services.layers->command) throw std::logic_error("Command selection services are absent");
        auto* window = reinterpret_cast<CommandSelectionNativeBody*>(DrawLayerAt418C10(*draw, StartupCommandSelectionLayerIndex()));
        BeginCommandSelectionNative(*window);
        return;
    }
    const SelectWindowNativeScope scope(services.layers->select);
    DrawLayersView(draw).WithNativeCallServices(services).SelectionAtSlotFive(&dependencies_.selection).RebuildAt4369E0();
}

void RootReadScriptMenuNativeServices::ClearToolByteAt38()
{
    // 4B1294..4B12B2是单BYTE存储，不调用Reset/434FB0，也不触碰邻接三个字节。
    CaptureLayersAfter413BE0().ToolAtSlotTwo().WriteLatchByteAt38(0);
}

std::uint8_t RootReadScriptMenuNativeServices::ReadScriptByteAt24()
{
    // 4B12DD..4B12ED：47CD00可懒创建CScript；读取原始BYTE，不转bool或借publication快照。
    auto* script = CaptureRootAfter413BE0().ScriptAt47CD00();
    return *static_cast<const volatile std::uint8_t*>(&script->commandAt24.bytes[0]);
}

std::int32_t RootReadScriptMenuNativeServices::TickChoicesAt4BBFD0(std::uint8_t blocked)
{
    // 4B16A0..4B16C4：该原点独立于Tool轮询；完整BYTE交4BBFD8，尾部声音刷新仍执行。
    auto& root=CaptureRootAfter413BE0();
    auto* draw=ResolveDrawManagerReferenceAt4175A0(root.DrawManagerSlotAt18());
    const auto services=root.PostWindow().BorrowNativeDrawCallServices();
    if(!services.layers)throw std::logic_error("native ReadScript Menu requires captured Root selection services");
    if (command_)
    {
        if (!services.layers->command) throw std::logic_error("Command selection services are absent");
        auto* window = reinterpret_cast<CommandSelectionNativeBody*>(DrawLayerAt418C10(*draw, StartupCommandSelectionLayerIndex()));
        return TickCommandSelectionNative(*window, blocked, dependencies_.selection);
    }
    const SelectWindowNativeScope scope(services.layers->select);
    return DrawLayersView(draw).WithNativeCallServices(services).SelectionAtSlotFive(&dependencies_.selection).TickAt4BBFD0(blocked);
}

void RootReadScriptMenuNativeServices::WriteMenuResult(std::int32_t ordinal)
{
    // 4B16D9/4B16E1先固定Root；498020不消费slot0正文，但4180C0/417990不能省略。
    auto& root = CaptureRootAfter413BE0();
    static_cast<void>(ResolveEnvironmentArchiveAt417990(*dependencies_.environment.CaptureAt4180C0(), 0));
    const auto index = ResolveScriptNamedValueNativeAt498020("menu_result", dependencies_.names, dependencies_.strings);
    // 4B1704在名字字符串正常退役之后才从已捕获Root取CParams；不可重新Ensure或预取values。
    auto& params = root.ParamsAt47BDB0();
    WriteScriptValueAt4C8220(params, index, ordinal, dependencies_.errors);
}
}
