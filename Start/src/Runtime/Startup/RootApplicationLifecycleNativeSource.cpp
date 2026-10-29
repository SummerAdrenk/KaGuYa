#include <Runtime/RootApplicationLifecycleNativeSource.h>
#include <Runtime/FileInfoProgressNativeReference.h>
#include <Runtime/FileManagerRootRuntime.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/RootDrawServicesRuntime.h>
#include <Runtime/RootSceneAuxiliaryNativeServicesRuntime.h>
#include <Runtime/RootSceneNativeServicesRuntime.h>
#include <Runtime/PostWindowRuntimeServices.h>
#include <Runtime/PostWindowInitializationNative.h>
#include <Runtime/ProgramSceneNative.h>
#include <Runtime/ProgramSelectDialogNative.h>
#include <Runtime/ProgramMain01DialogNative.h>
#include <Runtime/ProgramSystemDialogNative.h>
#include <Runtime/GuiControlNative.h>
#include <Runtime/ReadScriptSceneNative.h>
#include <Runtime/GraffitiPageNative.h>
#include <Runtime/GraffitiCountDialogNative.h>
#include <Runtime/SaveLoadSceneNative.h>
#include <Runtime/SaveLoadDialogNativeRuntime.h>
#include <Runtime/SaveLoadPageNative.h>
#include <Runtime/SaveLoadSystemPageNative.h>
#include <Runtime/SaveLoadConfirmDialogNative.h>
#include <Runtime/DemoSceneNative.h>
#include <Runtime/TitleSceneNative.h>
#include <Runtime/TitleDialogNative.h>
#include <Runtime/InputRootNative.h>
#include <Logging/Logger.h>
#include <optional>

namespace KaGuYa::Start::Runtime
{
namespace
{
PostWindowRuntimeDependencies PostDependencies(RootRuntimePayload& selected, RootApplicationLifecycleNativeDependencies& d)
{
    return {.root=selected, .graphics=d.graphics, .drawRandom=d.random,
        .information=d.auxiliary.Information(), .drawSource=d.draws.DrawSource(), .layers=d.draws.Layers(),
        .layerRuntime=d.draws.LayerRuntime(), .windowEffects=d.draws.WindowEffects(), .gui=d.draws.Gui(),
        .scenes=d.scenes.ManagerServices(), .windowCommands=d.windowCommands, .owner=d.root,
        .resourceInstance=d.resourceInstance};
}
class Files final : public IPostWindowInitializationNativeFiles
{
public:
    Files(RootRuntimePayload& selected, RootRuntimeOwner& owner, FileInfoProgressNativeServices& services) noexcept
        : selected_(selected), owner_(owner), services_(services), view_(storage_.reference, services) {}
    void CreateInitializationProgressAt489D70() override
    {
        auto* created = NewFileInfoProgressNativeAt489D70(services_);
        ConstructFileInfoProgressNativeReferenceAt489DF5(storage_.reference, created, services_.endpoint.memory);
        borrow_.emplace(owner_.FileManagerSource(), view_);
    }
    void CreateFileManagerAt489E56() override
    {
        auto* created = NewFileManagerAt489E56(storage_.reference);
        InstallFileManagerAt489E8D(selected_.FileManagerSlotAt00(), created);
    }
    void ReleaseInitializationProgressAt460730() noexcept override
    { ReleaseFileInfoProgressNativeReferenceAt460730(storage_.reference); }
private:
    RootRuntimePayload& selected_;
    RootRuntimeOwner& owner_;
    FileInfoProgressNativeServices& services_;
    // 原Ref在progress正文构造返回后才初始化；构造失败不由本载体补释放。
    union Storage final
    {
        YtlReferenceSlotRuntime reference;
        Storage() noexcept {}
        ~Storage() noexcept {}
    } storage_;
    FileInfoProgressNativeReferenceView view_;
    std::optional<FileManagerInitializationProgressBorrow> borrow_;
};
}
RootApplicationLifecycleNativeSource::RootApplicationLifecycleNativeSource(RootApplicationLifecycleNativeDependencies dependencies) noexcept
    : dependencies_(dependencies), progressScope_(dependencies.progress) {}
void RootApplicationLifecycleNativeSource::EnsureRootAt413BE0() { dependencies_.root.EnsureCreatedAt413BE0(); }
void* RootApplicationLifecycleNativeSource::RootAt6198C4() { return dependencies_.root.ReadPublishedAt6198C4(); }
EnvironmentRootStorage* RootApplicationLifecycleNativeSource::EnvironmentAt4180C0()
{ return dependencies_.pre.environment.CaptureAt4180C0(); }
bool RootApplicationLifecycleNativeSource::InitializeBeforeWindowAt4778B0(void* receiver)
{
    auto& selected = *static_cast<RootRuntimePayload*>(receiver);
    RootPreWindowInitializationNativeServices services(selected, dependencies_.pre);
    return RunPreWindowInitializationNativeAt4778B0(selected.Frame().Lifecycle(), services);
}
bool RootApplicationLifecycleNativeSource::InitializeAfterWindowAt489C90(void* receiver)
{
    auto& selected = *static_cast<RootRuntimePayload*>(receiver);
    PostWindowRuntimeServices post(PostDependencies(selected, dependencies_));
    Files files(selected, dependencies_.root, dependencies_.progress);
    const auto result=RunPostWindowInitializationNativeAt489C90(post, files);
    const auto error=GetLastError();
    // 仅记录成功初始化后的实际槽地址，不创建正文或按原21Ch偏移解释现代owner。
    if(result)
    {
        if(auto* current=dependencies_.root.Published())
        {
            KaGuYa::Logging::WriteFormat(KaGuYa::Logging::Level::Info,
                "Root native slots: scenes=%p script=%p params=%p programVtable=%p readScriptVtable=%p environmentSlot=%p inputSlot=%p graffitiVtable=%p likableVtable=%p weightVtable=%p",
                static_cast<void*>(&current->SceneManagerSlotAt30()),
                static_cast<void*>(&current->ScriptSlotAt1CC()),
                static_cast<void*>(&current->ParamsSlotAt1C0().ReferenceAt1C0()),
                static_cast<const void*>(&ProgramSceneNativeMethods()),
                static_cast<const void*>(&ReadScriptSceneNativeMethods()),
                static_cast<void*>(&dependencies_.pre.environment.PublicationSlot()),
                dependencies_.diagnosticInput?static_cast<void*>(&dependencies_.diagnosticInput->PublicationSlot()):nullptr,
                static_cast<const void*>(&GraffitiPageNativeMethods()),
                static_cast<const void*>(&GraffitiLikableCountDialogVtable()),
                static_cast<const void*>(&GraffitiWeightCountDialogVtable()));
            // 仅公布真实类型和已装配根字段地址，供诊断区分Load/确认框，不读取未初始化GUI状态。
            KaGuYa::Logging::WriteFormat(KaGuYa::Logging::Level::Info,
                "SaveLoad native anchors: scene=%p dialog=%p loadPage=%p systemPage=%p confirmation=%p modalSlot=%p",
                static_cast<const void*>(&SaveLoadSceneNativeMethods()),
                static_cast<const void*>(&SaveLoadDialogNativeVtable()),
                static_cast<const void*>(&SaveLoadPage01NativeVtable()),
                static_cast<const void*>(&SaveLoadSystemPageNativeVtable()),
                static_cast<const void*>(&SaveLoadConfirmDialogNativeVtable()),
                static_cast<void*>(&current->Frame().Lifecycle().valueAt1B8));
            KaGuYa::Logging::WriteFormat(KaGuYa::Logging::Level::Info,
                "Departure native anchors: dialog=%p main=%p system=%p button=%p static=%p",
                static_cast<const void*>(&ProgramSelectDialogNativeTop01Vtable()),
                static_cast<const void*>(&ProgramMain01DialogNativeVtable()),
                static_cast<const void*>(&ProgramSystem01DialogNativeVtable()),
                static_cast<const void*>(&GuiButtonVtable()),
                static_cast<const void*>(&GuiStaticVtable()));
            // 仅一次性公布实际三表，不构造/调用场景；沿用外层GetLastError保持协议。
            KaGuYa::Logging::WriteFormat(KaGuYa::Logging::Level::Info,
                "Ending native anchors: scene=%p title=%p titleDialog=%p",
                static_cast<const void*>(&EndingSceneNativeMethods()),
                static_cast<const void*>(&TitleSceneNativeMethods()),
                static_cast<const void*>(&TitleDialogNativeVtable().common));
        }
    }
    SetLastError(error);return result;
}
void RootApplicationLifecycleNativeSource::ReleaseSubsystemsAt477620(void* receiver)
{
    auto& selected = *static_cast<RootRuntimePayload*>(receiver);
    PostWindowRuntimeServices post(PostDependencies(selected, dependencies_));
    PostWindowLifecycle lifecycle;
    lifecycle.Release(post);
}
}
