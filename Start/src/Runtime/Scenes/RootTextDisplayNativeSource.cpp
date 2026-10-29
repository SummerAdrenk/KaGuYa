#include <Runtime/RootTextDisplayNativeSource.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/FileManagerNative.h>
#include <Runtime/MessageOwnerNative.h>
#include <Runtime/MessageTextSelectionRuntime.h>
#include <Runtime/ArchiveResourceNameRuntime.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/EnvironmentStateArchives.h>
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/UnSaveDataRuntime.h>
#include <Runtime/SceneManagerRuntime.h>
#include <Runtime/NativeGraphicsRuntimeLibrary.h>
#include <Runtime/ReadScriptEffectMaskNative.h>
#include <Runtime/ParamsRuntimeRoot.h>
#include <Runtime/ParamsPatternNative.h>
#include <Runtime/ParamsResourceNameRuntime.h>
#include <Runtime/TextWindowNativePlaybackServices.h>
#include <Runtime/ScriptArithmeticCommandNative.h>
#include <Runtime/ReadScriptFrameAdvanceNative.h>
#include <Runtime/LayerListNative.h>

#include <bit>
#include <cstring>

namespace KaGuYa::Start::Runtime
{
namespace
{
template<class T>T Read(const T& value) noexcept{return *static_cast<const volatile T*>(&value);}
RuntimeCString* TextCommandResult(void* command,RuntimeCString* result)
{
    using Function=RuntimeCString* (__thiscall*)(void*,RuntimeCString*);
    const void* table;std::memcpy(&table,command,4);
    Function function;std::memcpy(&function,static_cast<const std::byte*>(table)+4,4);
    return function(command,result);
}
std::uint8_t ByteAt(const void* body,std::size_t offset)
{return *reinterpret_cast<const volatile std::uint8_t*>(reinterpret_cast<std::uintptr_t>(body)+offset);}
}
const TextDisplayFontContextNative14h* TextDisplayFontContextsSample18At602198() noexcept
{
    // 原PE602050的60字节LOGFONTA与602198两条20字节记录已逐字节核对。
    static const LOGFONTA font=[] {auto f=TextDisplayFontSample18(0).font;f.lfHeight=0;return f;}();
    static const COLORREF black=0,white=0x00FFFFFF;
    static const TextDisplayFontContextNative14h table[]{{&font,42,1,2,&black},{&font,42,1,2,&white}};
    return table;
}
void BuildTextDisplayFontNativeAt4A2113(GraphicsTextNativeFontParameters& output,
    std::int32_t context,const TextDisplayFontContextNative14h* table)
{
    InitializeGraphicsTextFontNativeAt40C6E0(output);
    const auto address=std::uint32_t(reinterpret_cast<std::uintptr_t>(table))+std::uint32_t(context)*0x14u;
    const auto* row=reinterpret_cast<const volatile TextDisplayFontContextNative14h*>(address);
    const auto* font=row->fontAt00;
    std::memcpy(&output.fontAt00,font,sizeof(LOGFONTA));
    const auto height=row->heightAt04;
    output.renderModeAt3E=2;
    output.fontAt00.lfHeight=std::bit_cast<std::int32_t>(0u-std::uint32_t(height));
    const auto* color=row->colorAt10;output.textColorAt40=*color;
    output.lineSpacingAt3D=static_cast<std::uint8_t>(row->lineSpacingAt0C);
    output.characterSpacingAt3C=static_cast<std::uint8_t>(row->characterSpacingAt08);
    output.shadowColorAt44=0xFFFFFFFFu;output.shadowXAt4C=0;output.shadowYAt50=0;output.verticalAt55=0;
}
RootTextDisplayNativeSource::RootTextDisplayNativeSource(RootTextDisplayNativeDependencies d) noexcept
    :dependencies_(d),wait_(d.environment,d.input,d.root,d.errors,d.names,d.strings){}
DrawLayersView RootTextDisplayNativeSource::CaptureDrawLayersAt413BE0()
{dependencies_.root.EnsureCreatedAt413BE0();return dependencies_.root.Published()->PostWindow().DrawLayersAt18();}
IEnvironmentArchiveRuntime* RootTextDisplayNativeSource::CaptureArchive(std::uint32_t index)
{return ResolveEnvironmentArchiveAt417990(*dependencies_.environment.CaptureAt4180C0(),index);}
void* RootTextDisplayNativeSource::CaptureMessage()
{
    dependencies_.root.EnsureCreatedAt413BE0();
    auto* files=ResolveFileManagerAt479FF0(dependencies_.root.Published()->FileManagerSlotAt00());
    return ResolveMessageReference(files->messageAt10);
}
std::int32_t RootTextDisplayNativeSource::ReadNamedAt4C73A0(const char* name)
{
    dependencies_.root.EnsureCreatedAt413BE0();auto* root=dependencies_.root.Published();
    static_cast<void>(CaptureArchive(0));
    const auto index=ResolveScriptNamedValueNativeAt498020(name,dependencies_.names,dependencies_.strings);
    return ReadScriptValueAt4C73A0(root->ParamsAt47BDB0(),index,dependencies_.errors);
}
bool RootTextDisplayNativeSource::CreateFontAt4A2113(std::int32_t context)
{
    GraphicsTextNativeFontParameters parameters;
    BuildTextDisplayFontNativeAt4A2113(parameters,context,dependencies_.fonts);
    return dependencies_.graphics.TextCalls().CreateTextFontNativeAt4388B0(&parameters);
}
TextDisplayTextInitializationResult RootTextDisplayNativeSource::InitializeMessageAt4A2239(
    std::int32_t index,MessageFormatSelectionRawTarget output)
{
    auto* message=static_cast<MessageOwnerNative34h*>(CaptureMessage());
    auto version=message->SourceWithContext({dependencies_.files});
    const bool modern=version.ReadVersionAt47DDE0()>=4.0f;
    // 判代与真正读取之间有独立413BE0，不能复用第一代Message/reader。
    message=static_cast<MessageOwnerNative34h*>(CaptureMessage());
    auto reader=message->SourceWithContext({dependencies_.files});
    std::int32_t result;
    if(modern)
    {
        MessageTextSelectionRuntime texts(reader,message->flagsAt10[2]);
        result=texts.ReadTextRawAt4AD590(index,output);
    }
    else
    {
        auto flags=message->ReadFlagsWithSources(dependencies_.files,dependencies_.messagePaths);
        MessageFormatSelectionRuntime formats(reader,flags);
        result=formats.ReadFormatRawAt417C40(index,output,0,false);
    }
    const auto& selected=modern?output.body:output.name;
    if(result<0||!selected.baseAt00)return TextDisplayTextInitializationResult::ParseFailed;
    RuntimeCString* returned;
    auto text=ArchiveRawResourceNameRuntime(ArchiveRawResourceNameRuntime::ProducerTag{},[&](RuntimeCString* out){
        returned=TextCommandResult(ResolveEnvironmentObjectReference<MessageTextCommand>(selected),out);});
    const auto initialized=dependencies_.graphics.InitializeText(returned->GetString(),0,0);
    text.ReleaseNormal();
    return initialized?TextDisplayTextInitializationResult::Success:TextDisplayTextInitializationResult::GraphicsInitializationFailed;
}
bool RootTextDisplayNativeSource::LoadMaskAt4A26E4(YtlReferenceSlotRuntime& output,std::int32_t id)
{
    ParamsResourceNameRuntime names(dependencies_.params);
    const char* name=names.CaptureParamsForNames().CapturePatternNames().FindNameAt4AD1B0(id);
    auto text=ArchiveResourceNameAt40B530(name,dependencies_.strings);
    const char* chars=text.Value().GetString();
    const auto selected=CaptureDrawLayersAt413BE0();
    auto* manager=static_cast<DrawManagerNativeBody2Ch*>(const_cast<void*>(selected.Receiver()));
    const auto result=dependencies_.masks.LoadMaskAt480B30(manager,output,chars);
    // 4A273D在CString正常退役之前抛4，失败须由展开路径释放名称。
    if(!result)throw 4;
    text.ReleaseNormal();return true;
}
std::uint32_t RootTextDisplayNativeSource::ReadModeAt04(){return Read(static_cast<UnSaveDataRuntime*>(CaptureArchive(2))->modeAt04_);}
void RootTextDisplayNativeSource::WriteModeAt04(std::uint32_t value){static_cast<UnSaveDataRuntime*>(CaptureArchive(2))->modeAt04_=value;}
bool RootTextDisplayNativeSource::SuppressAt4C3430(){return wait_.SuppressEffectAt4C3430();}
bool RootTextDisplayNativeSource::AccelerateAt4AA970(){return wait_.SkipEffectAt4AA970();}
bool RootTextDisplayNativeSource::ReadFlagAt14Bit1(){return (ByteAt(CaptureArchive(2),0x14)&2u)!=0;}
bool RootTextDisplayNativeSource::ReadFlagAt18Bit1(){return (ByteAt(CaptureArchive(2),0x18)&2u)!=0;}
std::uint32_t RootTextDisplayNativeSource::ReadPaceAt3C()
{return *reinterpret_cast<const volatile std::uint32_t*>(reinterpret_cast<std::uintptr_t>(CaptureArchive(1))+0x3C);}
std::int32_t RootTextDisplayNativeSource::ReadStepAt497900()
{
    const auto index=Read(static_cast<SaveDataEnvironmentArchive*>(CaptureArchive(1))->valueAt70);
    static_cast<void>(CaptureArchive(0));
    constexpr std::int32_t steps[]{4,8,16,32};return steps[index<4?index:0];
}
bool RootTextDisplayNativeSource::ReadAdvanceAt4B0550()
{return ReadAdvanceInputNativeAt4B0550(*static_cast<ClickInfoArchive*>(CaptureArchive(5)),dependencies_.input);}
bool RootTextDisplayNativeSource::ReadNegativeWheelAt4AD740()
{
    auto* mouse=ResolveEnvironmentObjectReference<MouseManagerNativeBody3Ch>(dependencies_.input.CaptureAt418210()->mouseAt00);
    return Read(mouse->secondaryAt2C)==0&&Read(mouse->wheelAt34)<0;
}
bool RootTextDisplayNativeSource::ReadHeldSixAt4198D0(){return wait_.ReadHeldAt4198D0(6);}
bool RootTextDisplayNativeSource::SuppressFrameAt4C3350()
{
    return SuppressReadScriptDrawAt4B5A3A(dependencies_.environment,dependencies_.input);
}
std::int32_t RootTextDisplayNativeSource::CaptureDirtyCountAt4B5CF9()
{
    auto selected=CaptureDrawLayersAt413BE0();
    auto* list=ResolveDrawLayerListAt47AE80(*static_cast<DrawManagerNativeBody2Ch*>(const_cast<void*>(selected.Receiver())));
    auto* dirty=list->vptrAt00->dirtyAt14(list);
    return std::bit_cast<std::int32_t>(dirty->vptrAt00->countAt18(dirty));
}
const RECT* RootTextDisplayNativeSource::CaptureDirtyDataAt4B5D5C()
{
    auto selected=CaptureDrawLayersAt413BE0();
    auto* list=ResolveDrawLayerListAt47AE80(*static_cast<DrawManagerNativeBody2Ch*>(const_cast<void*>(selected.Receiver())));
    auto* dirty=list->vptrAt00->dirtyAt14(list);return dirty->vptrAt00->rectsAt1C(dirty);
}
void RootTextDisplayNativeSource::AdvanceSceneAt4C1760()
{dependencies_.root.EnsureCreatedAt413BE0();dependencies_.root.Published()->PostWindow().BorrowSceneManagerAt30().PopThenActivatePendingAt4C1760();}
}
