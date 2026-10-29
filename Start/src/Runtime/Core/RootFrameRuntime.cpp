#include <Runtime/RootFrameRuntime.h>
#include <Platform/FrameRuntimeCoordinator.h>
#include <Runtime/EnvironmentStartupRuntime.h>
#include <Runtime/GraphicsPresentNativeApi.h>
#include <Runtime/RootPreSceneEngineUpdateRuntime.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/SceneManagerRuntime.h>
#include <Runtime/InformationDisplayView.h>
#include <Runtime/WindowInputUpdateRuntime.h>

#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
class FrameOperation final : public Platform::IFrameRuntimeServices
{
public:
    FrameOperation(RootRuntimePayload& entry, RootRuntimeOwner& root,
        EnvironmentStartupRuntime& environment, WindowInputUpdateRuntime& input,
        RootPreSceneEngineUpdateRuntime& preScene, IGraphicsPresentNativeApi& graphics) noexcept
        : entry_(entry), root_(root), environment_(environment), input_(input),
          preScene_(preScene), graphics_(graphics) {}

    void BeginFrameState() override { entry_.Frame().BeginFrameAt483524(); }
    void UpdateInput() override { input_.UpdateAt450AA0(); }
    void UpdatePreSceneEngine() override
    {
        // 46D448未使用传入ECX，直接解Environment；内部根晚取由原生产链负责。
        preScene_.Update();
    }
    bool ModalProcessingEnabled() const override
    {
        return environment_.Archives().UnSaveData().ModeAt04() != 0;
    }
    bool AdvanceModal() override { return CaptureInformation().CanOpenAt5105C0(); }
    void DrawModal() override
    {
        // 4835DD再次解根；CanOpen回调更换父槽时必须见到新对象。
        static_cast<void>(CaptureInformation().RunAt5075B0());
    }
    bool HasPendingSceneWork() const override
    {
        root_.EnsureCreatedAt413BE0();
        scene_ = root_.RequirePublished().PostWindow().BorrowSceneManagerAt30();
        if (!scene_.IsNative()&&!scene_) throw std::logic_error("483689 accesses absent controlled SceneManager");
        return scene_.HasPendingSceneWork();
    }
    void RunSceneTasks() override
    {
        // 4836CD继续用ESI中的同一manager；局部对象不会被递归Tick覆盖。
        static_cast<void>(scene_.RunSceneTasks(0));
    }
    bool DirectMoviePresent() const override { return entry_.MovieBasePresentAt48(); }
    void RenderScreen() override { graphics_.RenderScreenIfLoaded(); }
    bool FrameWaitEnabled() const override { return entry_.Frame().FrameWaitEnabledAt1B0(); }
    void WaitForFrame() override { entry_.Frame().WaitFrameAt43FA60(); }
    bool ExitRequested() const override
    {
        root_.EnsureCreatedAt413BE0();
        return root_.RequirePublished().ExitRequestedAt1BC();
    }

private:
    InformationDisplayView CaptureInformation()
    {
        root_.EnsureCreatedAt413BE0();
        const auto value = root_.RequirePublished().PostWindow().BorrowInformationDisplayAt3C();
        if (!value.IsNative()&&!value) throw std::logic_error("4835CB accesses absent controlled InformationDisplay");
        return value;
    }
    RootRuntimePayload& entry_;
    RootRuntimeOwner& root_;
    EnvironmentStartupRuntime& environment_;
    WindowInputUpdateRuntime& input_;
    RootPreSceneEngineUpdateRuntime& preScene_;
    IGraphicsPresentNativeApi& graphics_;
    mutable SceneManagerView scene_{static_cast<SceneManagerRuntime*>(nullptr)};
};
}

RootFrameRuntime::RootFrameRuntime(RootRuntimeOwner& root, EnvironmentStartupRuntime& environment,
    WindowInputUpdateRuntime& input, RootPreSceneEngineUpdateRuntime& preScene,
    IGraphicsPresentNativeApi& graphics) noexcept
    : root_(root), environment_(environment), input_(input), preScene_(preScene), graphics_(graphics) {}

bool RootFrameRuntime::TickFrame()
{
    // 424E18发生在4834F0外；根构造异常不能被误收进帧catch返回false。
    root_.EnsureCreatedAt413BE0();
    return TickFrameAt4834F0(root_.RequirePublished());
}

bool RootFrameRuntime::TickFrameAt4834F0(RootRuntimePayload& entry)
{
    FrameOperation operation(entry, root_, environment_, input_, preScene_, graphics_);
    return Platform::FrameRuntimeCoordinator(operation).TickFrame();
}

bool RootFrameRuntime::ExitRequested() const
{
    // 424E31与43E9A5都重新解根，不复用上次帧receiver。
    root_.EnsureCreatedAt413BE0();
    return root_.RequirePublished().ExitRequestedAt1BC();
}
}
