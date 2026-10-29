#pragma once

namespace KaGuYa::Start::Runtime
{
struct ReadScriptSceneState;
class ScrVmScriptCommandState;
class SceneManagerRuntime;
class UnSaveDataRuntime;
class DrawLayersView;
struct ToolWindowNativeInteractionServices;
class IToolWindowInteractionServices;

// 4A1646..4A178E：在 owner 构造体内、四 Ref/FreeScroll/CString 及空资源释放之后调用。
// state 是该 scene 唯一 scalar；scriptCommand 必须来自同根 context.bundle。
// 可抛服务失败时保留已提交字段，不在 make_shared 返回后补做构造恢复。
void RestoreReadScriptSceneAt4A14D0(ReadScriptSceneState& state,
    ScrVmScriptCommandState& scriptCommand, const SceneManagerRuntime& sceneManager,
    const UnSaveDataRuntime& unsave, DrawLayersView manager,
    IToolWindowInteractionServices& toolInteractionServices,ToolWindowNativeInteractionServices* native=nullptr);
}
