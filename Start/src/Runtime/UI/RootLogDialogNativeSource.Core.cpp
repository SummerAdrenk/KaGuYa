#include <Runtime/RootLogDialogNativeSource.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/FileManagerNative.h>
#include <Runtime/MessageOwnerNative.h>

namespace KaGuYa::Start::Runtime
{
EnvironmentRootStorage* RootLogDialogNativeSource::CaptureEnvironmentAt4180C0(std::uint32_t)
{return dependencies_.environment.CaptureAt4180C0();}
ParamsRuntimeRoot* RootLogDialogNativeSource::CaptureParamsAt418480(std::uint32_t)
{return dependencies_.params.CaptureAt418480();}
InputRootNativeBody24h* RootLogDialogNativeSource::CaptureInputAt418210(std::uint32_t)
{return dependencies_.input.CaptureAt418210();}
void RootLogDialogNativeSource::EnsureRootAt413BE0(std::uint32_t){dependencies_.root.EnsureCreatedAt413BE0();}
void* RootLogDialogNativeSource::RootAt6198C4(std::uint32_t){return dependencies_.root.ReadPublishedAt6198C4();}
YtlReferenceSlotRuntime& RootLogDialogNativeSource::RootDrawSlotAt18(void* p)
{return static_cast<RootRuntimePayload*>(p)->DrawManagerSlotAt18();}
YtlReferenceSlotRuntime& RootLogDialogNativeSource::RootSoundSlotAt24(void* p)
{return static_cast<RootRuntimePayload*>(p)->SoundManagerSlotAt24();}
YtlReferenceSlotRuntime& RootLogDialogNativeSource::RootFileManagerSlotAt00(void* p)
{return static_cast<RootRuntimePayload*>(p)->FileManagerSlotAt00();}
YtlReferenceSlotRuntime& RootLogDialogNativeSource::FileManagerMessageSlotAt10(void* p)
{return static_cast<FileManagerNative28h*>(p)->messageAt10;}
ScriptParamsNativeBody* RootLogDialogNativeSource::ParamsAt47BDB0(void* p)
{return &static_cast<RootRuntimePayload*>(p)->ParamsAt47BDB0();}
const char* RootLogDialogNativeSource::FontFaceAt61A684(){return dependencies_.font.GetString();}
std::int32_t RootLogDialogNativeSource::ReadFormatAt417C40(void* p,std::int32_t index,
    MessageFormatSelectionRawTarget output,std::uint32_t variant,bool markRead)
{
    auto& message=*static_cast<MessageOwnerNative34h*>(p);
    auto reader=message.SourceWithContext({dependencies_.files});
    auto flags=message.ReadFlagsWithSources(dependencies_.files,dependencies_.messagePaths);
    MessageFormatSelectionRuntime selection(reader,flags);
    return selection.ReadFormatRawAt417C40(index,output,variant,markRead);
}
}
