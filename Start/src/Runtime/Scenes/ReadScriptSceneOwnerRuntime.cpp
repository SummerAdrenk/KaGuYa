#include <Runtime/ReadScriptSceneOwnerRuntime.h>
#include <Runtime/ReadScriptSceneRestoreRuntime.h>
#include <Runtime/ScrVmRuntimeComposition.h>

namespace KaGuYa::Start::Runtime
{
ReadScriptSceneOwnerRuntime::ReadScriptSceneOwnerRuntime(
    ReadScriptSceneOwnerDependencies dependencies)
    : context_(dependencies.context),
      scene_(context_.Update(), dependencies.drawing, dependencies.errors)
{
    // 4A163C 在 CString 成立后清四 Ref，随后才处理恢复及 Script 字段清写。
    context_.ReleaseResourcesAt4C0990();
    RestoreReadScriptSceneAt4A14D0(scene_.State(), dependencies.context.bundle.scriptCommand,
        dependencies.sceneManager, dependencies.context.unsave, dependencies.context.manager,
        dependencies.toolInteraction);
}

ReadScriptSceneOwnerRuntime::ReadScriptSceneOwnerRuntime(ReadScriptSceneOwnerNativeDependencies dependencies)
    : context_(dependencies.context), scene_(context_.Update(), dependencies.drawing, dependencies.errors)
{
    // 与普通构造同一时点：成员成立后先4C0990，再进入逐次取根的恢复片段。
    context_.ReleaseResourcesAt4C0990();
    RestoreReadScriptSceneNativeAt4A14D0(scene_.State(), dependencies.restore);
}

ReadScriptSceneOwnerRuntime::~ReadScriptSceneOwnerRuntime() noexcept
{
    // 原析构 FuncInfo 5F099C 的 EHFlags=5：Finish 异常终止，不吞掉或向普通 catch 传播。
    // 成功后 C++ 先销毁 scene CString，再销毁 context FreeScroll；不补调 Scroll.End。
    context_.ReleaseResourcesAt4C0990();
}

ReadScriptSceneConstructionServicesRuntime::ReadScriptSceneConstructionServicesRuntime(
    ISceneConstructionServices& upstream, ReadScriptSceneOwnerDependencies dependencies) noexcept
    : SceneConstructionServicesForwarderRuntime(upstream), dependencies_(dependencies)
{
}

ReadScriptSceneConstructionServicesRuntime::ReadScriptSceneConstructionServicesRuntime(
    ISceneConstructionServices& upstream, ReadScriptSceneOwnerNativeDependencies dependencies) noexcept
    : SceneConstructionServicesForwarderRuntime(upstream), dependencies_(dependencies) {}

std::shared_ptr<ISceneTask> ReadScriptSceneConstructionServicesRuntime::CreateScriptProcessing()
{
    // 恢复在 owner 构造体内：失败只展开已构造成员，不误走完整对象的 Finish。
    return std::visit([](const auto& dependencies) -> std::shared_ptr<ISceneTask> {
        auto owner = std::make_shared<ReadScriptSceneOwnerRuntime>(dependencies);
        return std::shared_ptr<ISceneTask>(owner, &owner->Scene());
    }, dependencies_);
}
}
