#include <Runtime/RootSaveLoadNativeSource.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/EnvironmentPathRuntime.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/RuntimeCStringInputNative.h>
#include <Runtime/SaveFileContainerNativeBinding.h>
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/FileManagerNative.h>
#include <Runtime/MessageOwnerNative.h>
#include <bit>

namespace KaGuYa::Start::Runtime
{
EnvironmentRootStorage* RootSaveLoadNativeSource::CaptureEnvironmentAt4180C0()
{ return dependencies_.environment.CaptureAt4180C0(); }
IRuntimeStringManagerSource& RootSaveLoadNativeSource::Strings() { return dependencies_.strings; }
BOOL RootSaveLoadNativeSource::DeleteFileA(const char* path) { return ::DeleteFileA(path); }
void RootSaveLoadNativeSource::EnsureRootAt413BE0() { dependencies_.root.EnsureCreatedAt413BE0(); }
void* RootSaveLoadNativeSource::RootAt6198C4() { return dependencies_.root.ReadPublishedAt6198C4(); }
void* RootSaveLoadNativeSource::ParamsAt47BDB0(void* root)
{ return &static_cast<RootRuntimePayload*>(root)->ParamsAt47BDB0(); }
ParamsRuntimeRoot* RootSaveLoadNativeSource::CaptureParamsAt418480()
{ return dependencies_.params.CaptureAt418480(); }
YtlReferenceSlotRuntime& RootSaveLoadNativeSource::RootFileManagerSlotAt00(void* root)
{ return static_cast<RootRuntimePayload*>(root)->FileManagerSlotAt00(); }
YtlReferenceSlotRuntime& RootSaveLoadNativeSource::FileManagerMessageSlotAt10(void* files)
{ return static_cast<FileManagerNative28h*>(files)->messageAt10; }
std::int32_t RootSaveLoadNativeSource::ReadFormatAt417C40(void* receiver, std::int32_t index,
    MessageFormatSelectionRawTarget output, std::uint32_t variant, bool markRead)
{
    auto& message = *static_cast<MessageOwnerNative34h*>(receiver);
    auto reader = message.SourceWithContext({dependencies_.files});
    auto flags = message.ReadFlagsWithSources(dependencies_.files, dependencies_.messagePaths);
    MessageFormatSelectionRuntime query(reader, flags);
    return query.ReadFormatRawAt417C40(index, output, variant, markRead);
}
RuntimeStringBorrow RootSaveLoadNativeSource::CaptureDirectoryAt4180C0()
{
    auto* env = CaptureEnvironmentAt4180C0();
    return ResolveEnvironmentObjectReference<EnvironmentPathRuntime>(env->pathsAt48)->saveAt10;
}
std::uint8_t RootSaveLoadNativeSource::CaptureHistoryFlagAt4C7F9B()
{
    // 4C7F50..4C7F9B内联slot1，直到第三块才读取当次Env/Save，不沿用入口快照。
    auto* env = CaptureEnvironmentAt4180C0();
    const auto bytes = std::uint32_t(reinterpret_cast<std::uintptr_t>(env->archivesAt30.end)-
        reinterpret_cast<std::uintptr_t>(env->archivesAt30.begin));
    const auto count = std::uint32_t(std::bit_cast<std::int32_t>(bytes)/12);
    auto* save = count > 1 ? ResolveEnvironmentObjectReference<SaveDataEnvironmentArchive>(env->archivesAt30.begin[1]) : nullptr;
    return save->bytesAt74[1];
}
std::uint32_t RootSaveLoadNativeSource::WriteContainerAt4C7B10(void* receiver, const char* path)
{
    return WriteSaveFileContainerNativeAt4C7B10(*static_cast<ScriptParamsNativeBody*>(receiver), path,
        SaveFileContainerNativeServices{dependencies_.files, *this, dependencies_.random, dependencies_.saveProfile,
            {dependencies_.memory, dependencies_.strings, dependencies_.files}});
}
std::uint32_t RootSaveLoadNativeSource::ReadContainerAt4C7420(void* receiver, const char* path, std::uint32_t mode)
{ return ReadContainerAt4C7420(*static_cast<ScriptParamsNativeBody*>(receiver), path, mode); }
std::uint32_t RootSaveLoadNativeSource::ReadContainerAt4C7420(ScriptParamsNativeBody& receiver,
    const char* path, std::uint32_t mode)
{
    return ReadSaveFileContainerNativeAt4C7420(receiver, path, mode,
        SaveFileContainerNativeServices{dependencies_.files, *this, dependencies_.random, dependencies_.saveProfile,
            {dependencies_.memory, dependencies_.strings, dependencies_.files}});
}
void RootSaveLoadNativeSource::ReloadCatalogItemAt48EE00(void*, std::int32_t page, std::int32_t slot)
{
    // 原48EE00 wrapper不消费ECX；不得借该参数重复Ensure/Capture根。
    RefreshSaveFileCatalogItemNativeAt48EE00(page, slot, *this, dependencies_.memory);
}
int RootSaveLoadNativeSource::MessageBoxA(HWND window, const char* text, const char* caption, UINT flags)
{ return ::MessageBoxA(window, text, caption, flags); }
void RootSaveLoadNativeSource::ConstructInputAt40B530(RuntimeCString* out, const char* input)
{ ConstructRuntimeCStringInputNativeAt40B530(out, input, dependencies_.strings); }
std::int64_t RootSaveLoadNativeSource::CurrentTimeAt56A321() { return ::_time64(nullptr); }
int RootSaveLoadNativeSource::LocalTime64At5697BB(std::tm* out, const std::int64_t* time)
{ return ::_localtime64_s(out, time); }
void* RootSaveLoadNativeSource::CaptureInformationAfter413BE0()
{
    EnsureRootAt413BE0();
    auto* root = static_cast<RootRuntimePayload*>(RootAt6198C4());
    // 保存/加载modal调用前实际解析同一3C Ref，即使5071D0自身不消费ECX。
    const volatile auto& slot = root->InformationDisplaySlotAt3C();
    const auto base = slot.baseAt00;
    if (!base) return nullptr;
    const auto* control = static_cast<const volatile EnvironmentObjectReferenceControl*>(slot.controlAt08);
    const auto stride = control ? control->strideAt0C : 0u;
    return reinterpret_cast<void*>(base + slot.biasAt04*stride);
}
IModalDialogRunnerServices& RootSaveLoadNativeSource::ModalServices() { return dependencies_.modal; }
void RootSaveLoadNativeSource::ConfigureAt535FF0(GuiDialogNativeBody5Ch* body,
    RuntimeCString& page, RuntimeCString& slot, RuntimeCString& time, RuntimeCString& scene, std::uint8_t saveMode)
{
    SaveLoadConfirmDialogNativeScope scope(dependencies_.confirmation);
    ConfigureSaveLoadConfirmDialogNativeAt535FF0(body, page, slot, time, scene, saveMode);
}
void RootSaveLoadNativeSource::AssignPreviewAt538430(GuiDialogNativeBody5Ch* body, YtlReferenceSlotRuntime& owned)
{
    SaveLoadConfirmDialogNativeScope scope(dependencies_.confirmation);
    AssignSaveLoadConfirmDialogNativePreviewAt538430(body, owned);
}
}
