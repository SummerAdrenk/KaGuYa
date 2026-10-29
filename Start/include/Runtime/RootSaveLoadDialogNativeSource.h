#pragma once
#include <Runtime/RootSaveLoadNativeSource.h>
#include <Runtime/SaveLoadDialogNativeWorkflow.h>
#include <Runtime/SaveLoadMessageBoxNative.h>

namespace KaGuYa::Start::Runtime
{
class NativeInputRuntime;
// NeededDependencies：只借既有owner/叶服务，构造不Capture、不造Root或场景。
// environment必须是saveLoad所借的同一owner；全部依赖活过Scene及最后的确认Ref。
struct RootSaveLoadDialogNativeDependencies final
{
    RootSaveLoadNativeSource& saveLoad;
    EnvironmentRootOwner& environment;
    NativeInputRuntime& input;
    NativeGraphicsRuntimeLibrary& graphics;
    IGuiControlResourceLoadServices& pictures;
    SaveLoadMessageBoxNativeServices& deleteConfirmation;
};

class RootSaveLoadDialogNativeSource final : public ISaveLoadDialogNativeConstructionSource,
    public ISaveLoadDialogNativeBeginSource, public ISaveLoadDialogNativeCatalogSource,
    public ISaveLoadDialogNativeTransitionSource
{
public:
    explicit RootSaveLoadDialogNativeSource(RootSaveLoadDialogNativeDependencies d) noexcept
        : dependencies_(d) {}
    std::int32_t ReadInitialPageFromEnvironmentAt4EBC35() override;
    EnvironmentRootStorage* CaptureEnvironmentAt4180C0() override;
    void* CaptureDrawAfter413BE0() override;
    bool LoadIntoAt4800A0(void*, GraphicsSurfaceNativeBody*, const char*) override;
    std::uint8_t LoadSurfaceAt480000(void*, YtlReferenceSlotRuntime*, const char*) override;
    GuiDialogNativeBody5Ch* ConstructSlotConfirmationAt4EACA0(void*) override;
    GuiDialogNativeBody5Ch* ConstructDeleteConfirmationAt4EC8A0(void*) override;
    SaveLoadMessageBoxNativeServices* NativeDeleteConfirmationServices() const noexcept override;
    SaveLoadConfirmDialogNativeServices* NativeSlotConfirmationServices() const noexcept override;
    YtlReferenceSlotRuntime* CaptureBackgroundAfter413BE0(YtlReferenceSlotRuntime* fresh) override;
    InputRootNativeBody24h* CaptureInputAt418210() override;
    SaveFileStateNativeArchive* CaptureCatalogAt4180C0() override;
    void RequestCloseApplication() override;
    void ShowPageLimitMessage() override;
    std::uint8_t AccelerateAt4C3430() override;
    std::uint8_t AccelerateAt4AA970() override;
    TransitRuntime* CaptureTransitAfter413BE0() override;
    std::int32_t ReadStepAt51CC28() override;
    void CommitScreenAt4B02E0() override;
private:
    RootSaveLoadDialogNativeDependencies dependencies_;
};
}
