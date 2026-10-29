#include <Runtime/ReadScriptSceneRestoreNative.h>

#include <Runtime/DrawLayersView.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/ReadScriptSceneRuntime.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/SceneManagerRuntime.h>
#include <Runtime/ScrVmScriptCommandState.h>
#include <Runtime/ToolWindowNativeInteraction.h>
#include <Runtime/UnSaveDataRuntime.h>

#include <bit>

namespace KaGuYa::Start::Runtime
{
RootReadScriptSceneRestoreSource::RootReadScriptSceneRestoreSource(RootRuntimeOwner& root,
    EnvironmentRootOwner& environment, IDrawLayersViewSource& draws,
    ToolWindowNativeInteractionServices& toolServices) noexcept
    : root_(root), environment_(environment), draws_(draws), toolServices_(toolServices) {}

std::uint32_t RootReadScriptSceneRestoreSource::ReadScriptFlagsAfter413BE0()
{
    root_.EnsureCreatedAt413BE0();
    auto* const root = root_.Published();
    auto* const script = root->ScriptAt47CD00();
    return static_cast<const volatile std::uint32_t&>(script->commandAt04.flags);
}

std::int32_t RootReadScriptSceneRestoreSource::ReadSceneMarkerAfter413BE0()
{
    root_.EnsureCreatedAt413BE0();
    return root_.Published()->PostWindow().BorrowSceneManagerAt30().ReadSceneMarkerAt38();
}

DrawToolView RootReadScriptSceneRestoreSource::CaptureToolAfter413BE0()
{
    // 4A16E6/4A1727：draw来源恰好承担本次根捕获，不在外层重复Ensure。
    return draws_.CaptureDrawLayersAt413BE0().ToolAtSlotTwo(&toolServices_);
}

std::uint32_t RootReadScriptSceneRestoreSource::ReadUnSaveModeAt4180C0()
{
    return static_cast<UnSaveDataRuntime*>(ResolveEnvironmentArchiveAt417990(
        *environment_.CaptureAt4180C0(), 2))->ModeAt04();
}

void RootReadScriptSceneRestoreSource::ClearScriptFlagsAndReloadAfter413BE0()
{
    root_.EnsureCreatedAt413BE0();
    auto* const root = root_.Published();
    auto* const script = root->ScriptAt47CD00();
    ScrVmScriptCommandState command(*script, root->ScriptPublication());
    // 4A1787/4A178E共用同一EAX；两次写入均不要求先读字段。
    command.WriteFlagsAt04(0);
    command.WriteReloadByteAt08(0);
}

void RestoreReadScriptSceneNativeAt4A14D0(ReadScriptSceneState& state,
    IReadScriptSceneRestoreSource& source)
{
    // 4A1646/4A164A/4A1651：均在4A163C之后，先写三字段再执行首次取根。
    state.drawEnabled = true;
    state.mainState = 0;
    state.substate = 0;
    if ((source.ReadScriptFlagsAfter413BE0() & 2u) != 0)
    {
        switch (source.ReadSceneMarkerAfter413BE0())
        {
        case 0:
            state.mainState = 3;
            state.substate = 1;
            source.CaptureToolAfter413BE0().SetModeAt434FB0(0);
            source.CaptureToolAfter413BE0().WriteLatchByteAt38(0);
            state.interactionValueAt10 = std::bit_cast<std::int32_t>(source.ReadUnSaveModeAt4180C0());
            break;
        case 1:
            state.mainState = 4;
            break;
        case 2:
            state.mainState = 7;
            break;
        }
    }
    source.ClearScriptFlagsAndReloadAfter413BE0();
}
}
