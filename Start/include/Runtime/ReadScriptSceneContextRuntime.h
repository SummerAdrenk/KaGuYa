#pragma once
#include <Platform/InputViews.h>

#include <Runtime/FreeScrollRuntime.h>
#include <Runtime/ReadScriptLayerServicesRuntime.h>
#include <Runtime/ReadScriptMenuInteractionRuntime.h>
#include <Runtime/ReadScriptTextInteractionRuntime.h>
#include <Runtime/ReadScriptTransitionWorkflowRuntime.h>
#include <Runtime/ReadScriptUpdateRuntime.h>
#include <Runtime/FreeScrollNativeRuntime.h>
#include <Runtime/ReadScriptUpdateEffectNative.h>
#include <Runtime/ReadScriptInteractionStateSource.h>

#include <cstdint>
#include <variant>

namespace KaGuYa::Start::Runtime
{
class IGraphicsRuntimeCalls;

// 每个字段借用同一生产根；factory/host/VM 的内部依赖也须由根装配保证一致。
// 不另收 FreeScrollDependencies 或 params 表，避免传入另一份 manager/save/bundle。
struct ReadScriptSceneContextDependencies final
{
    IReadScriptEffectFactory& effectFactory;
    ScrVmRuntimeStateBundle& bundle;
    const ScrVmExecutableProfile& profile;
    SaveDataEnvironmentArchive& save;
    UnSaveDataRuntime& unsave;
    Platform::MouseView mouse;
    Platform::KeyboardView keyboard;
    DrawManagerLayerRuntime& manager;
    IReadScriptSoundWaitSource& sound;
    // 多个 Graphics API 均由这一对象转换取得，不允许各分支使用不同模块。
    IGraphicsRuntimeCalls& graphics;
    std::uint8_t& rootByteAt1B0;

    ParamsRuntimeRootOwner& params;
    Platform::CursorView cursor;
    const HWND& window;
    const Platform::ICursorApi& windowApi;

    IOpacityDialogUpdateServices& opacity;
    // Menu host 已继承 Text host；两条交互分支共用这一个宿主。
    IReadScriptMenuInteractionHost& interactionHost;
    IReadScriptToolServices& tools;
    IReadScriptTextServices& text;
    IReadScriptMenuServices& menu;

    const ScrVmCore& vm;
    ScrScriptRuntimeState& script;
    IScrVmYieldBoundary& yield;
};

// 每项只借同一生产根的服务。此构造不需要现代Draw/Save/UnSave/EffectFactory占位。
struct ReadScriptSceneContextNativeDependencies final
{
    ReadScriptEffectFactoryNativeServices& effects;
    IReadScriptEffectMaskNativeServices& masks;
    IMoveEffectNativeConstructionSource& effectState;
    IReadScriptFlashShakeNativeSource& flashShake;
    IReadScriptWaitStateSource& waitState;
    IReadScriptInteractionStateSource& interactionState;
    IDrawLayersViewSource& layers;
    IOpacityWindowInputSource& opacity;
    const IStartupConfigGameSystemSource& startupConfig;
    FreeScrollNativeDrawServices& scrollDrawing;
    FreeScrollNativeInputServices& scrollInput;
    const ISpriteAnimationInitializationSource& animation;
    IReadScriptSoundWaitSource& sound;
    IGraphicsScreenUpdateApi& screen;
    IReadScriptMenuInteractionHost& interactionHost;
    IReadScriptToolServices& tools;
    IReadScriptTextServices& text;
    IReadScriptMenuServices& menu;
    const ScrVmCore& vm;
    ScrScriptRuntimeState& script;
    IScrVmYieldBoundary& yield;
};

// 4A14D0 的每场景四 Ref 与内嵌 FreeScroll，加上借用它们的真实 Update 对象图。
// scalar 仍只属于 ReadScriptSceneRuntime；恢复分派和 Script 清写由外层 ctor 接线。
class ReadScriptSceneContextRuntime final
{
public:
    explicit ReadScriptSceneContextRuntime(ReadScriptSceneContextDependencies dependencies);
    explicit ReadScriptSceneContextRuntime(ReadScriptSceneContextNativeDependencies dependencies);
    ~ReadScriptSceneContextRuntime() noexcept;
    ReadScriptSceneContextRuntime(const ReadScriptSceneContextRuntime&) = delete;
    ReadScriptSceneContextRuntime& operator=(const ReadScriptSceneContextRuntime&) = delete;
    ReadScriptSceneContextRuntime(ReadScriptSceneContextRuntime&&) = delete;
    ReadScriptSceneContextRuntime& operator=(ReadScriptSceneContextRuntime&&) = delete;

    [[nodiscard]] ReadScriptUpdateRuntime& Update() noexcept;
    [[nodiscard]] ReadScriptTransitionWorkflowRuntime& Transition() noexcept;
    [[nodiscard]] FreeScrollRuntime& Scroll();
    [[nodiscard]] IFreeScrollOperations& ScrollOperations() noexcept;
    [[nodiscard]] FreeScrollNativeBody50h& NativeScrollBody();
    [[nodiscard]] ReadScriptEffectSlotsNative30h& NativeEffectSlots();
    // 4A4920 的业务释放由完整 task owner 显式调用；半构造展开不能补调 Finish。
    void ReleaseResourcesAt4C0990();

private:
    struct ControlledGraph final
    {
        explicit ControlledGraph(ReadScriptSceneContextDependencies);
        ReadScriptTransitionWorkflowRuntime workflow;
        RootStartupConfigGameSystemSource startupConfig;
        FreeScrollRuntime scroll;
        ReadScriptLayerServicesRuntime layers;
        ReadScriptTextInteractionRuntime text;
        ReadScriptMenuInteractionRuntime menu;
        ReadScriptUpdateRuntime update;
    };
    struct NativeResources final
    {
        explicit NativeResources(ReadScriptEffectFactoryNativeServices&);
        ~NativeResources() noexcept;
        NativeResources(const NativeResources&) = delete;
        NativeResources& operator=(const NativeResources&) = delete;
        ReadScriptEffectFactoryNativeServices& services;
        union { ReadScriptEffectSlotsNative30h effects; };
        union { FreeScrollNativeBody50h scroll; };
    };
    struct NativeGraph final
    {
        explicit NativeGraph(ReadScriptSceneContextNativeDependencies);
        NativeResources resources;
        ReadScriptEffectOwnerNative owner;
        ReadScriptUpdateEffectNative actions;
        ReadScriptTransitionWorkflowRuntime workflow;
        FreeScrollNativeRuntime scroll;
        ReadScriptLayerServicesRuntime layers;
        ReadScriptTextInteractionRuntime text;
        ReadScriptMenuInteractionRuntime menu;
        ReadScriptUpdateRuntime update;
    };
    // 构造时明确选一种图；另一种不构造，没有并行owner或失败后换路。
    std::variant<ControlledGraph, NativeGraph> graph_;
};
}
