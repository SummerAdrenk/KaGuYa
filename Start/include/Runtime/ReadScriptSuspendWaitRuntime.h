#pragma once
#include <Platform/InputViews.h>

#include <Runtime/GuiInputStateRuntime.h>
#include <Runtime/ReadScriptSceneRuntime.h>
#include <Runtime/DrawLayersView.h>
#include <Runtime/ReadScriptWaitStateSource.h>
#include <optional>

namespace KaGuYa::Start::Runtime
{
class DrawManagerLayerRuntime;
class IGraphicsScreenUpdateApi;
class SaveDataEnvironmentArchive;
class ScrVmExecutableProfile;
struct ScrVmRuntimeStateBundle;
class IReadScriptSoundWaitSource;
class UnSaveDataRuntime;

// 借用同一个 ReadScript 实例的 +1C/+28/+34/+40 owner，不另存 Effect 状态。
// Tick/Draw 各自重新解析 +1C；空引用不能返回假成功。Release 执行真实完成业务，
// 再按原顺序解绑四组引用；完成异常必须向外传播，不能只 reset 或吞掉异常。
class IReadScriptSuspendEffectOwner
{
public:
    virtual ~IReadScriptSuspendEffectOwner() = default;
    [[nodiscard]] virtual bool HasEffectAt1C() const = 0;
    [[nodiscard]] virtual bool TickEffectAtVslot00() = 0;
    virtual void DrawEffectAtVslot04() = 0;
    virtual void ReleaseResourcesAt4C0990() = 0;
};

// main2/main5 的生产消费者。标量只写传入 scene；所有依赖均借用同一根对象图。
// 本类不实现 main1，也不自动接入 Update 或替代尚未装配的每场景 Effect owner。
class ReadScriptSuspendWaitRuntime final
{
public:
    ReadScriptSuspendWaitRuntime(IReadScriptSuspendEffectOwner& effects,
        ScrVmRuntimeStateBundle& bundle, const ScrVmExecutableProfile& profile,
        const SaveDataEnvironmentArchive& save, UnSaveDataRuntime& unsave,
        Platform::MouseView mouse, Platform::KeyboardView keyboard,
        DrawLayersView manager, IReadScriptSoundWaitSource& sound,
        IGraphicsScreenUpdateApi& screen, std::uint8_t& rootByteAt1B0,
        PatternLayerNativeRenderServices* patterns=nullptr) noexcept;
    ReadScriptSuspendWaitRuntime(IReadScriptSuspendEffectOwner& effects,
        ScrVmRuntimeStateBundle& bundle,const ScrVmExecutableProfile& profile,
        const SaveDataEnvironmentArchive& save,UnSaveDataRuntime& unsave,
        Platform::MouseView mouse,Platform::KeyboardView keyboard,IDrawLayersViewSource& manager,
        IReadScriptSoundWaitSource& sound,IGraphicsScreenUpdateApi& screen,std::uint8_t& rootByteAt1B0,
        PatternLayerNativeRenderServices& patterns) noexcept;
    ReadScriptSuspendWaitRuntime(IReadScriptSuspendEffectOwner& effects,IReadScriptWaitStateSource& state,
        IDrawLayersViewSource& manager,IReadScriptSoundWaitSource& sound,IGraphicsScreenUpdateApi& screen,
        PatternLayerNativeRenderServices& patterns) noexcept;
    ReadScriptSuspendWaitRuntime(const ReadScriptSuspendWaitRuntime&)=delete;
    ReadScriptSuspendWaitRuntime& operator=(const ReadScriptSuspendWaitRuntime&)=delete;

    [[nodiscard]] bool RunSuspendAt4AC4C0(ReadScriptSceneState& state);
    [[nodiscard]] bool RunWaitAt4C4FC0(ReadScriptSceneState& state);

private:
    [[nodiscard]] bool SuppressEffectAt4C3430() const;
    [[nodiscard]] bool SkipEffectAt4AA970() const;
    [[nodiscard]] bool MousePressedEdge(std::size_t button) const;
    void WriteWaitSkipped(std::int32_t value);
    [[nodiscard]] bool CompleteWait(ReadScriptSceneState& state);
    [[nodiscard]] bool WaitForSound(ReadScriptSceneState& state, bool skip);
    [[nodiscard]] bool WaitForSprite(ReadScriptSceneState& state, bool skip);
    DrawLayersView CaptureLayers() const;
    std::uint8_t PrepareLayersAt46A980();

    IReadScriptSuspendEffectOwner& effects_;
    std::optional<FixedReadScriptWaitStateSource> fixedState_;
    IReadScriptWaitStateSource* state_;
    DrawLayersView manager_;
    IDrawLayersViewSource* source_=nullptr;
    PatternLayerNativeRenderServices* patterns_=nullptr;
    IReadScriptSoundWaitSource& sound_;
    IGraphicsScreenUpdateApi& screen_;
};
}
