#include <Runtime/RootModeSelectButtonNativeSource.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/EnvironmentStartupRuntime.h>
#include <Runtime/GameBuildProfile.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
std::span<const CgUiButtonRecordNative28h,12> GameModeSelectButtonRecords() noexcept
{return CompiledGameProfile().modeSelectRecords();}
RootModeSelectButtonNativeSource::RootModeSelectButtonNativeSource(RootRuntimeOwner& root,
    EnvironmentStartupRuntime& environment,IStartFileApi& files,const IEnvironmentKeyTableSource& key,
    ArchiveCodecServices codecs,IGraphicsSurfaceApi& surfaces,IGuiControlResourceMessageApi& messages,
    IRuntimeStringManagerSource& strings,const FileTransactionContext* transaction) noexcept
    :root_(root),environment_(environment),archives_(root),files_(files),key_(key),codecs_(codecs),
    surfaces_(surfaces),messages_(messages),strings_(strings),transaction_(transaction){}
void* RootModeSelectButtonNativeSource::CaptureDrawAt45AD92()
{root_.EnsureCreatedAt413BE0();return root_.RequirePublished().PostWindow().DrawManagerAt18();}
const CgUiButtonRecordNative28h* RootModeSelectButtonNativeSource::RecordAt601658(std::uint32_t offset)
{
    const auto records=GameModeSelectButtonRecords();
    if(offset%sizeof(CgUiButtonRecordNative28h)!=0||offset/sizeof(CgUiButtonRecordNative28h)>=records.size())
        throw std::out_of_range("Appreciation mode-select record offset is outside its game table");
    return records.data()+offset/sizeof(CgUiButtonRecordNative28h);
}
std::uint8_t RootModeSelectButtonNativeSource::LoadAt480930(void* receiver,CgUiButtonNativeBody6Ch* body,
    const CgUiButtonRecordNative28h* record,std::int32_t setPosition)
{
    // 原480930不读ECX；保留outer取根，但每个reader另在483D90原点取资源目录。
    static_cast<void>(receiver);
    return LoadCgUiButtonRecordNativeAt480930(body,record,setPosition,archives_,transaction_,files_,key_,
        surfaces_,*this,codecs_,strings_);
}
void RootModeSelectButtonNativeSource::ShowAt480A2D(const RuntimeCString& text)
{
    auto& environment=environment_.CaptureAt4180C0();
    const auto* caption=environment.titleAt10.GetString();const auto* message=text.GetString();
    const auto window=reinterpret_cast<HWND>(std::uintptr_t(environment.windowAt2C));
    static_cast<void>(messages_.MessageBoxAt4801C2(window,message,caption,0x10));
}
}
