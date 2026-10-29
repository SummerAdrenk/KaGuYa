#include <Runtime/ReadScriptSceneRestoreRuntime.h>

#include <Runtime/DrawLayersView.h>
#include <Runtime/ReadScriptSceneRuntime.h>
#include <Runtime/SceneManagerRuntime.h>
#include <Runtime/ScrVmScriptCommandState.h>
#include <Runtime/ToolWindowInteractionRuntime.h>
#include <Runtime/UnSaveDataRuntime.h>

#include <bit>
#include <cstdint>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
void RestoreReadScriptSceneAt4A14D0(ReadScriptSceneState& state,
    ScrVmScriptCommandState& scriptCommand, const SceneManagerRuntime& sceneManager,
    const UnSaveDataRuntime& unsave, DrawLayersView manager,
    IToolWindowInteractionServices& toolInteractionServices,ToolWindowNativeInteractionServices* native)
{
    state.drawEnabled = true;
    state.mainState = 0;
    state.substate = 0;
    if ((scriptCommand.ReadFlagsAt04() & 2u) != 0)
    {
        switch (sceneManager.ReadSceneMarkerAt38())
        {
        case 0:
        {
            state.mainState = 3;
            state.substate = 1;
            manager.ToolAtSlotTwo(native,&toolInteractionServices).SetModeAt434FB0(0);
            // 4A1755 再次查询 owning 槽；不能沿用 SetMode 前缓存的 Tool 指针。
            manager.ToolAtSlotTwo(native,&toolInteractionServices).WriteLatchByteAt38(0);
            state.interactionValueAt10 = std::bit_cast<std::int32_t>(unsave.ModeAt04());
            break;
        }
        case 1:
            state.mainState = 4;
            break;
        case 2:
            state.mainState = 7;
            break;
        }
    }
    // 4A1787/4A178E：先清 +04 DWORD，再清当前 +08 的低 byte，保留回调后的高 24 位。
    scriptCommand.WriteFlagsAt04(0);
    scriptCommand.WriteReloadByteAt08(0u);
}
}
