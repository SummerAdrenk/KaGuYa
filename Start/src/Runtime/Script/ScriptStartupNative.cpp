#include <Runtime/ScriptStartupNative.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/ScriptParamsNativeBody.h>
#include <Runtime/ScrVmScriptCommandState.h>
#include <cstdint>

namespace KaGuYa::Start::Runtime
{
namespace
{
IEnvironmentArchiveRuntime* Archive(IScriptStartupNativeServices& services,std::uint32_t index)
{return ResolveEnvironmentArchiveAt417990(*services.CaptureEnvironmentAt4180C0(),index);}
template<class T>T Read(IEnvironmentArchiveRuntime* body,std::uint32_t offset)
{return *reinterpret_cast<const volatile T*>(reinterpret_cast<std::uintptr_t>(body)+offset);}
template<class T>void Write(IEnvironmentArchiveRuntime* body,std::uint32_t offset,T value)
{*reinterpret_cast<volatile T*>(reinterpret_cast<std::uintptr_t>(body)+offset)=value;}
void NamedWrite(RestoredScriptNativeRootReceiver root,IScriptStartupNativeServices& services,
    const char* name,std::int32_t value)
{
    auto* archive=Archive(services,0);
    const auto index=services.ResolveNamedValueAt498020(archive,name);
    auto* params=root.paramsAt47BDB0(root.object);
    services.WriteValueAt4C8220(*params,index,value);
}
// 5B5CD0原PE的CP932字节；不是用“相似文件名”选择replay调度脚本。
constexpr char replayDispatcher[]="\x83\x58\x83\x5E\x81\x5B\x83\x67\x81\x69\x83\x8D\x81\x5B\x83\x68\x81\x6A.scr";
}
std::uint8_t LoadStartupScriptNativeAt4D53D0(ScriptNativeBody& script,const char* name,
    std::int32_t entry,IScriptStartupNativeServices& services)
{
    services.ReplaceCapturedParamsAt481720(services.CaptureAfterEnsureAt413BE0());
    const bool replay=entry!=-1;
    script.commandAt04.flags=replay?2u:0u;
    services.OnPublishCommandFields(script,ScrVmScriptCommandState::FlagsAt04Mask);
    Write<std::uint32_t>(Archive(services,2),4,replay?2u:1u);
    Write<std::uint8_t>(Archive(services,2),0x0C,1);
    Write<std::uint32_t>(Archive(services,2),0x18,1);
    Write<std::uint32_t>(Archive(services,2),0x14,1);
    Write<std::uint32_t>(Archive(services,2),0x1C,1);
    Write<std::uint32_t>(Archive(services,2),0x20,1);
    if(!replay)
    {
        Write<std::uint8_t>(Archive(services,2),8,1);
        Write<std::uint8_t>(Archive(services,2),9,1);
        Write<std::uint8_t>(Archive(services,2),0x28,1);
    }
    Write<std::uint8_t>(Archive(services,2),0x29,0);
    const auto selected=services.FindScriptIdAt47A020(name);
    if(replay)
    {
        if(selected<0)return 0;
        NamedWrite(services.CaptureAfterEnsureAt413BE0(),services,"replay_file",selected);
        NamedWrite(services.CaptureAfterEnsureAt413BE0(),services,"replay_position",entry);
        // 4D3FA0不检查反查结果，-1也交回4D3C90原错误链。
        if(!services.LoadScriptAt4D3C90(script,services.FindScriptIdAt47A020(replayDispatcher)))return 0;
    }
    else
    {
        if(!services.LoadScriptAt4D3C90(script,selected))return 0;
        const auto root=services.CaptureAfterEnsureAt413BE0();
        const auto value=Read<std::uint16_t>(Archive(services,7),0x0C);
        NamedWrite(root,services,"prologue_flag",value);
    }
    const auto drawRoot=services.CaptureAfterEnsureAt413BE0();
    services.ResetDrawOwnersAt47E940(drawRoot.drawLayersAt18(drawRoot.object));
    const auto* first=Read<const char*>(Archive(services,1),0x58);
    services.SetGraphicsTextFirstNameAt4D6CD0(first);
    const auto* second=Read<const char*>(Archive(services,1),0x5C);
    services.SetGraphicsTextSecondNameAt4D6DB0(second);
    const auto root=services.CaptureAfterEnsureAt413BE0();
    auto* params=root.paramsAt47BDB0(root.object);
    // 4D572D/30只是history.end=history.begin，不清元素、不释放vector存储。
    params->historyAt3C.storage.end=params->historyAt3C.storage.begin;
    static_cast<void>(Archive(services,4)); // nullsub_3不读ECX，仍保留caller取源。
    script.commandAt24.bytes[2]=0;
    services.OnPublishCommandFields(script,ScrVmScriptCommandState::StateByteAt26Mask);
    return 1;
}
}
