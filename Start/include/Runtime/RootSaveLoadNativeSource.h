#pragma once
#include <Runtime/SaveLoadDialogNativeConfirmation.h>
#include <Runtime/SaveLoadDialogNativeCommands.h>
#include <Runtime/SaveFileCatalogItemNative.h>
#include <Runtime/SaveFileContainerRuntime.h>
#include <Runtime/SaveLoadConfirmDialogNative.h>
#include <Runtime/SavePreviewCaptureApi.h>

namespace KaGuYa::Start::Runtime
{
class RootRuntimeOwner;
class NativeGraphicsRuntimeLibrary;
class IMessageEnvironmentPathSource;
struct SaveFileFormatProfile;
struct RootSaveLoadNativeDependencies final
{
    RootRuntimeOwner& root;
    EnvironmentRootOwner& environment;
    ParamsRuntimeRootOwner& params;
    NativeGraphicsRuntimeLibrary& graphics;
    IStartFileApi& files;
    const IMessageEnvironmentPathSource& messagePaths;
    IScrVmProgramRandSource& random;
    const SaveFileFormatProfile& saveProfile;
    ISavePreviewCaptureServices& previewCapture;
    SaveLoadConfirmDialogNativeServices& confirmation;
    IModalDialogRunnerServices& modal;
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource();
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory();
};
// 只借同一组根/服务，所有业务数据仍在Root/Environment原槽；构造不Capture。
class RootSaveLoadNativeSource final : public ISaveLoadDialogNativeConfirmationSource,
    public ISaveFileCatalogItemNativeSource, public ISaveFileContainerEnvironmentSource,
    public ISaveLoadDialogNativeDeleteConfirmationSource
{
public:
    explicit RootSaveLoadNativeSource(RootSaveLoadNativeDependencies d) noexcept : dependencies_(d) {}
    EnvironmentRootStorage* CaptureEnvironmentAt4180C0() override;
    IRuntimeStringManagerSource& Strings() override;
    BOOL DeleteFileA(const char*) override;
    void EnsureRootAt413BE0() override;
    void* RootAt6198C4() override;
    void* ParamsAt47BDB0(void*) override;
    std::uint32_t WriteContainerAt4C7B10(void*, const char*) override;
    std::uint32_t ReadContainerAt4C7420(void*, const char*, std::uint32_t) override;
    std::uint32_t ReadContainerAt4C7420(ScriptParamsNativeBody&, const char*, std::uint32_t) override;
    void CapturePreviewAt48DBE0(void*) override;
    std::uint32_t SavePreviewAt489420(void*, const char*) override;
    ParamsRuntimeRoot* CaptureParamsAt418480() override;
    YtlReferenceSlotRuntime& RootFileManagerSlotAt00(void*) override;
    YtlReferenceSlotRuntime& FileManagerMessageSlotAt10(void*) override;
    std::int32_t ReadFormatAt417C40(void*, std::int32_t, MessageFormatSelectionRawTarget,
        std::uint32_t, bool) override;
    void ReloadCatalogItemAt48EE00(void*, std::int32_t, std::int32_t) override;
    int MessageBoxA(HWND, const char*, const char*, UINT) override;
    void ConstructInputAt40B530(RuntimeCString*, const char*) override;
    std::int64_t CurrentTimeAt56A321() override;
    int LocalTime64At5697BB(std::tm*, const std::int64_t*) override;
    void ConfigureAt535FF0(GuiDialogNativeBody5Ch*, RuntimeCString&, RuntimeCString&,
        RuntimeCString&, RuntimeCString&, std::uint8_t) override;
    void AssignPreviewAt538430(GuiDialogNativeBody5Ch*, YtlReferenceSlotRuntime&) override;
    YtlReferenceSlotRuntime* CapturePreviewAt47D740(void*, YtlReferenceSlotRuntime*) override;
    GraphicsSurfaceNativeBody* ConstructPreviewSurfaceAt40C2E0(void*) override;
    std::uint32_t LoadPreviewAt511D90(GraphicsSurfaceNativeBody*, const char*, const void*) override;
    void* CaptureInformationAfter413BE0() override;
    IModalDialogRunnerServices& ModalServices() override;
    RuntimeStringBorrow CaptureDirectoryAt4180C0() override;
    std::uint8_t CaptureHistoryFlagAt4C7F9B() override;
    SaveLoadConfirmDialogNativeServices& ConfirmationServices() noexcept { return dependencies_.confirmation; }
private:
    RootSaveLoadNativeDependencies dependencies_;
};
// 须覆盖Save/Scene调用及最终Ref释放，不能只在构造时声明一次。
class RootSaveLoadNativeScope final
{
public:
    explicit RootSaveLoadNativeScope(RootSaveLoadNativeSource& source) noexcept
        : confirmation_(source.ConfirmationServices()) {}
private:
    SaveLoadConfirmDialogNativeScope confirmation_;
};
}
