#pragma once
#include <Runtime/LoadAfterSelectionNative.h>

namespace KaGuYa::Start::Runtime
{
class RootRuntimeOwner;
class IGraphicsRuntimeCalls;
class IGuiControlResourceLoadServices;
class IReadScriptWaitStateSource;
class ITextWindowSoundSource;
class RootScriptRecoveryRuntime;
class IRestoredScriptNativeServices;
class IRestoredScriptNativeExecution;
struct PatternLayerNativeRenderServices;
struct DrawTextNativeRebuildServices;
struct RootLoadAfterSelectionNativeDependencies final
{
    RootRuntimeOwner& root;
    EnvironmentRootOwner& environment;
    IGraphicsRuntimeCalls& graphics;
    IGuiControlResourceLoadServices& pictures;
    IReadScriptWaitStateSource& wait;
    ITextWindowSoundSource& sounds;
    PatternLayerNativeRenderServices& patterns;
    DrawTextNativeRebuildServices& text;
    RootScriptRecoveryRuntime& recovery;
    IRestoredScriptNativeServices& restored;
    // 与restored的业务执行叶同源；caller固定tuple后才建立惰性图的调用域。
    IRestoredScriptNativeExecution& execution;
    IRuntimeStringManagerSource& strings;
};
// 只借长期服务；各次取根在原调用点执行，不持有现代LoadAfter/Surface镜像。
class RootLoadAfterSelectionNativeSource final:public ILoadAfterSelectionNativeSource
{
public:
    explicit RootLoadAfterSelectionNativeSource(RootLoadAfterSelectionNativeDependencies) noexcept;
    bool LoadBlackAt480000(YtlReferenceSlotRuntime&,const char*) override;
    std::uint32_t ReadModeAt04() override;
    void WriteModeAt04(std::uint32_t) override;
    void WriteRootByteAt218(std::uint8_t) override;
    void WriteRootByteAt219(std::uint8_t) override;
    std::int32_t ReadStepAt497900() override;
    bool ReadGateAt6F() override;
    bool AccelerateAt4AA970() override;
    void RenderAt4C39E0(TransitOperation&) override;
    void CommitAt4B02E0() override;
    void WriteRootSubobjectDwordAt10(std::uint32_t) override;
    void StopMusicAt4383B0() override;
    void ClearVoicesAt4384D0() override;
    void StopMappedEffectsAt413270() override;
    std::int32_t RestoreScriptAt4D4CC0() override;
    void WriteSceneMarkerAt38(std::int32_t) override;
    void RestorePatternsAt4C14E0() override;
    void RebuildTextAt4C1650(std::uint32_t) override;
    void RestorePrimaryGauge() override;
    void PropagatePatternsAt4C4250() override;
    void DrawRegisteredAt4AC490() override;
    void CaptureScreenAt4ACDD0(YtlReferenceSlotRuntime&) override;
    void ClearScreenAt4AA890() override;
    void ClearCommandAt437680() override;
    void ClearScenesAt4C1460() override;
    void ClearScriptFlagsAt4B6766() override;
    void PushActiveAt4A9030(const char*) override;
    void ResetAndQueueTitleAt47E180() override;
    void QueueAt47F920(const char*) override;
private:
    void* CaptureArchive(std::uint32_t);
    RootLoadAfterSelectionNativeDependencies dependencies_;
};
}
