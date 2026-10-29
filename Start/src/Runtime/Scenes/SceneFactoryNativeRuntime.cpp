#include <Runtime/SceneFactoryNativeRuntime.h>
#include <Runtime/CgSceneNative.h>
#include <Runtime/ReplaySceneNative.h>
#include <Runtime/LikabilitySceneNative.h>
#include <Runtime/MovieSceneNative.h>
#include <Runtime/GameBuildProfile.h>
#include <Runtime/LegacyMenuSelectionSource.h>
#include <Runtime/LegacyCharacterSelectionNative.h>
#include <Runtime/LegacyItemAcquisitionNative.h>
#include <Runtime/LegacyMapSelectionNative.h>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Branch=SceneFactoryNativeBranch;
constexpr std::array<SceneFactoryNativeRecord,17> records{{
    {Branch::Logo,"\x83\x5E\x83\x43\x83\x67\x83\x8B\x83\x8D\x83\x53",0x5B33E8,0x18,0x4AB00F},
    {Branch::Title,"\x83\x5E\x83\x43\x83\x67\x83\x8B",0x5AE938,0x1C,0x4A2850},
    {Branch::Music,"\x89\xB9\x8A\x79\x8A\xD3\x8F\xDC\x83\x82\x81\x5B\x83\x68",0x5AE928,0x64,0x4A0730},
    {Branch::Cg,"\x82\x62\x82\x66\x95\x5C\x8E\xA6\x83\x82\x81\x5B\x83\x68",0x5AE904,0xB4,0x49E3B0},
    {Branch::Scene,"\x83\x56\x81\x5B\x83\x93\x8D\xC4\x90\xB6\x83\x82\x81\x5B\x83\x68",0x5AE914,0x80,0x49EC90},
    {Branch::Log,"\x83\x8D\x83\x4F\x89\xE6\x96\xCA",0x5B56E0,0x20,0x49E180},
    {Branch::Save,"\x83\x5A\x81\x5B\x83\x75\x89\xE6\x96\xCA",0x5AE218,0x24,0x4A17B0},
    {Branch::Load,"\x83\x8D\x81\x5B\x83\x68\x89\xE6\x96\xCA",0x5AE224,0x24,0x4A17B0},
    {Branch::LoadAfter,"\x83\x8D\x81\x5B\x83\x68\x91\x49\x91\xF0\x8C\xE3\x8F\x88\x97\x9D",0x5B4DB0,0x58,0x4AB47B},
    {Branch::Script,"\x83\x58\x83\x4E\x83\x8A\x83\x76\x83\x67\x8F\x88\x97\x9D",0x5B4DE8,0xBC,0x4A14D0},
    {Branch::Setting,"\x90\xDD\x92\xE8\x89\xE6\x96\xCA",0x5B4E68,0x20,0x4A1A00},
    {Branch::Affection,"\x8D\x44\x8A\xB4\x93\x78\x89\xE6\x96\xCA",0x5B4E80,0x20,0x49DFB0},
    {Branch::Movie,"\x83\x80\x81\x5B\x83\x72\x81\x5B\x8D\xC4\x90\xB6",0x5B5384,0x0C,0x4A0B00},
    {Branch::Ending,"\x83\x47\x83\x93\x83\x66\x83\x43\x83\x93\x83\x4F",0x5B5368,0x18,0x49DA10},
    {Branch::Text,"\x83\x65\x83\x4C\x83\x58\x83\x67\x95\x5C\x8E\xA6\x8F\x88\x97\x9D",0x5B5394,0x90,0x4A1F30},
    {Branch::Program,"\x83\x76\x83\x8D\x83\x4F\x83\x89\x83\x80\x91\x49\x91\xF0\x8F\x88\x97\x9D",0x5B4E04,0x20,0x4A11E0},
    {Branch::Graffiti,"\x97\x8E\x8F\x91\x82\xAB\x83\x79\x81\x5B\x83\x57\x8F\x88\x97\x9D",0x5B53A8,0x20,0x49DCA0}
}};
bool Same(const char* left,const char* right) noexcept
{
    // 4AAFC2等比较每轮顺序读取两个byte；NULL名称直接读故障，不转成未知名称。
    for(;;)
    {
        const auto first=*reinterpret_cast<const volatile unsigned char*>(left);
        if(first!=static_cast<unsigned char>(*right))return false;
        if(!first)return true;
        const auto second=*reinterpret_cast<const volatile unsigned char*>(left+1);
        if(second!=static_cast<unsigned char>(right[1]))return false;
        left+=2;right+=2;
        if(!second)return true;
    }
}
struct RawUnwind final
{
    void* raw;std::size_t size;IEnvironmentRootMemory& memory;bool armed=true;
    ~RawUnwind() noexcept{if(armed)memory.DeleteBody(raw,size);}
};
}
const std::array<SceneFactoryNativeRecord,17>& SceneFactoryNativeRecords() noexcept{return records;}
const SceneFactoryNativeRecord& MatchSceneFactoryNativeAt4AAF90(const char* name)
{
    for(const auto& row:records)if(Same(name,row.nameCp932))return row;
    const auto* character=CompiledGameProfile().legacyCharacterSelection;
    if(character && character->kind!=LegacyCharacterSelectionKind::None && Same(name,LegacyCharacterSelectionSceneName()))
    {
        // 原VA/尺寸逐款保留在审计；扩展记录使用现代正文，不伪造一份共同原VA。
        static const SceneFactoryNativeRecord selection{Branch::LegacyCharacterSelection,
            LegacyCharacterSelectionSceneName(),0,sizeof(LegacyCharacterSelectionScene),0};
        return selection;
    }
    // NeeNeeNee 497020/497707：独立旧菜单名，只有该款的显式配置可启用。
    if(CompiledGameProfile().legacyMenuSelection && Same(name,LegacyMenuSelectionSceneName()))
    {
        static const SceneFactoryNativeRecord menu{Branch::LegacyMenuSelection,
            LegacyMenuSelectionSceneName(),0x52C3AC,0x20,0x47BD20};
        return menu;
    }
    if(CompiledGameProfile().legacyItemAcquisition && Same(name,LegacyItemAcquisitionSceneName()))
    {
        static const SceneFactoryNativeRecord item{Branch::LegacyItemAcquisition,
            LegacyItemAcquisitionSceneName(),0x53FD2C,0x1C,0x495F90};
        return item;
    }
    const auto* map=CompiledGameProfile().legacyMapSelection;
    if(map && map->kind!=LegacyMapSelectionKind::None && Same(name,LegacyMapSelectionSceneName()))
    {
        // 十款原case9的名字相同；VA逐款见map-selection-integration/entry-contract.json。
        static const SceneFactoryNativeRecord selection{Branch::LegacyMapSelection,
            LegacyMapSelectionSceneName(),0,sizeof(LegacyMapSelectionScene),0};
        return selection;
    }
    throw 0;
}
TaskNativeBody4h* SceneFactoryNativeRuntime::CreateAt4AAF90(const char* name)
{
    const auto& row=MatchSceneFactoryNativeAt4AAF90(name);
    SceneFactoryNativeTaskScope scope(TaskServices());
    const auto size=constructors_.AllocationSize(row.branch,row.bodySize);
    auto* raw=memory_.AllocateBody(size);if(!raw)return nullptr;
    RawUnwind unwind{raw,size,memory_};
    auto* task=constructors_.Construct(row.branch,raw);
    // ctor正常返回NULL也不触发EH释放；不得加入RAII“修复”原孤儿资格。
    unwind.armed=false;return task;
}
TaskNativeBody4h* DemoSceneFactoryNativeConstructorsRuntime::Construct(Branch branch,void* raw)
{
    if(branch==Branch::Logo||branch==Branch::Ending)
    {
        auto& body=*::new(raw) DemoSceneNativeBody18h;
        auto* result=branch==Branch::Logo?ConstructLogoNativeAt4AAFEF(body,demo_):
            ConstructEndingNativeAt49DA10(body,demo_);
        return reinterpret_cast<TaskNativeBody4h*>(result);
    }
    return other_.Construct(branch,raw);
}
SceneFactoryNativeTaskServices DemoSceneFactoryNativeConstructorsRuntime::TaskServices() const noexcept
{auto result=other_.TaskServices();result.demo=&demo_;return result;}
TaskNativeBody4h* TextSceneFactoryNativeConstructorsRuntime::Construct(Branch branch,void* raw)
{
    if(branch==Branch::Text)return reinterpret_cast<TaskNativeBody4h*>(
        ConstructTextDisplayNativeAt4A1F30(*::new(raw) TextDisplayNativeBody90h,text_));
    return other_.Construct(branch,raw);
}
SceneFactoryNativeTaskServices TextSceneFactoryNativeConstructorsRuntime::TaskServices() const noexcept
{auto result=other_.TaskServices();result.text=&text_;return result;}
TaskNativeBody4h* GraffitiSceneFactoryNativeConstructorsRuntime::Construct(Branch branch,void* raw)
{
    if(branch==Branch::Graffiti)return reinterpret_cast<TaskNativeBody4h*>(
        ConstructGraffitiPageNativeAt49DCA0(*::new(raw) GraffitiPageNativeBody20h,graffiti_));
    return other_.Construct(branch,raw);
}
SceneFactoryNativeTaskServices GraffitiSceneFactoryNativeConstructorsRuntime::TaskServices() const noexcept
{
    auto result=other_.TaskServices();result.graffiti=&graffiti_;
    result.graffitiCount=graffiti_.dialogs.CountDialogServices();return result;
}
TaskNativeBody4h* LoadAfterSceneFactoryNativeConstructorsRuntime::Construct(Branch branch,void* raw)
{
    if(branch==Branch::LoadAfter)return reinterpret_cast<TaskNativeBody4h*>(
        ConstructLoadAfterSelectionNativeAt4AB47B(*::new(raw) LoadAfterSelectionNativeBody58h,loadAfter_));
    return other_.Construct(branch,raw);
}
SceneFactoryNativeTaskServices LoadAfterSceneFactoryNativeConstructorsRuntime::TaskServices() const noexcept
{auto result=other_.TaskServices();result.loadAfter=&loadAfter_;return result;}
TaskNativeBody4h* ReadScriptSceneFactoryNativeConstructorsRuntime::Construct(Branch branch,void* raw)
{
    if(branch==Branch::Script)return reinterpret_cast<TaskNativeBody4h*>(
        ConstructReadScriptSceneNativeAt4A14D0(*::new(raw) ReadScriptSceneNativeBodyBCh,readScript_));
    return other_.Construct(branch,raw);
}
SceneFactoryNativeTaskServices ReadScriptSceneFactoryNativeConstructorsRuntime::TaskServices() const noexcept
{auto result=other_.TaskServices();result.readScript=&readScript_;return result;}
TaskNativeBody4h* TitleSceneFactoryNativeConstructorsRuntime::Construct(Branch branch,void* raw)
{
    if(branch==Branch::Title)return reinterpret_cast<TaskNativeBody4h*>(
        ConstructTitleSceneNativeAt4A2850(*::new(raw) TitleSceneNativeBody1Ch,title_));
    return other_.Construct(branch,raw);
}
SceneFactoryNativeTaskServices TitleSceneFactoryNativeConstructorsRuntime::TaskServices() const noexcept
{auto result=other_.TaskServices();result.title=&title_;return result;}
TaskNativeBody4h* SaveLoadSceneFactoryNativeConstructorsRuntime::Construct(Branch branch,void* raw)
{
    if(branch==Branch::Save||branch==Branch::Load)return reinterpret_cast<TaskNativeBody4h*>(
        ConstructSaveLoadSceneNativeAt4A17B0(*::new(raw) SaveLoadSceneNativeBody24h,
            branch==Branch::Save?1u:0u,saveLoad_));
    return other_.Construct(branch,raw);
}
SceneFactoryNativeTaskServices SaveLoadSceneFactoryNativeConstructorsRuntime::TaskServices() const noexcept
{auto result=other_.TaskServices();result.saveLoad=&saveLoad_;return result;}
TaskNativeBody4h* SettingSceneFactoryNativeConstructorsRuntime::Construct(Branch branch,void* raw)
{
    if(branch==Branch::Setting)return reinterpret_cast<TaskNativeBody4h*>(
        ConstructSettingSceneNativeAt4A1A00(*::new(raw) SettingSceneNativeBody20h,setting_));
    return other_.Construct(branch,raw);
}
SceneFactoryNativeTaskServices SettingSceneFactoryNativeConstructorsRuntime::TaskServices() const noexcept
{auto result=other_.TaskServices();result.setting=&setting_;return result;}
TaskNativeBody4h* LogSceneFactoryNativeConstructorsRuntime::Construct(Branch branch,void* raw)
{
    if(branch==Branch::Log)return reinterpret_cast<TaskNativeBody4h*>(
        ConstructLogSceneNativeAt49E180(*::new(raw) LogSceneNativeBody20h,log_));
    return other_.Construct(branch,raw);
}
SceneFactoryNativeTaskServices LogSceneFactoryNativeConstructorsRuntime::TaskServices() const noexcept
{auto result=other_.TaskServices();result.log=&log_;return result;}
TaskNativeBody4h* MusicSceneFactoryNativeConstructorsRuntime::Construct(Branch branch,void* raw)
{
    if(branch==Branch::Music)return reinterpret_cast<TaskNativeBody4h*>(
        ConstructMusicSceneNativeAt4A0730(*::new(raw) MusicSceneNativeBody64h,music_));
    return other_.Construct(branch,raw);
}
SceneFactoryNativeTaskServices MusicSceneFactoryNativeConstructorsRuntime::TaskServices() const noexcept
{auto result=other_.TaskServices();result.music=&music_;return result;}
TaskNativeBody4h* CgSceneFactoryNativeConstructorsRuntime::Construct(Branch branch,void* raw)
{
    if(branch==Branch::Cg)return reinterpret_cast<TaskNativeBody4h*>(
        ConstructCgSceneNativeAt49E3B0(*::new(raw) CgSceneNativeBodyB4h,cg_));
    return other_.Construct(branch,raw);
}
SceneFactoryNativeTaskServices CgSceneFactoryNativeConstructorsRuntime::TaskServices() const noexcept
{auto result=other_.TaskServices();result.cg=&cg_;return result;}
TaskNativeBody4h* ReplaySceneFactoryNativeConstructorsRuntime::Construct(Branch branch,void* raw)
{
    if(branch==Branch::Scene)return reinterpret_cast<TaskNativeBody4h*>(
        ConstructReplaySceneNativeAt49EC90(*::new(raw)ReplaySceneNativeBody80h,replay_));
    return other_.Construct(branch,raw);
}
SceneFactoryNativeTaskServices ReplaySceneFactoryNativeConstructorsRuntime::TaskServices()const noexcept
{auto result=other_.TaskServices();result.replay=&replay_;return result;}
TaskNativeBody4h* LikabilitySceneFactoryNativeConstructorsRuntime::Construct(Branch branch,void* raw)
{
    if(branch==Branch::Affection)return reinterpret_cast<TaskNativeBody4h*>(
        ConstructLikabilitySceneNativeAt49DFB0(*::new(raw)LikabilitySceneNativeBody20h,likability_));
    return other_.Construct(branch,raw);
}
SceneFactoryNativeTaskServices LikabilitySceneFactoryNativeConstructorsRuntime::TaskServices()const noexcept
{auto result=other_.TaskServices();result.likability=&likability_;return result;}
TaskNativeBody4h* MovieSceneFactoryNativeConstructorsRuntime::Construct(Branch branch,void* raw)
{
    if(branch==Branch::Movie)return reinterpret_cast<TaskNativeBody4h*>(
        ConstructMovieSceneNativeAt4A0B00(*::new(raw)MovieSceneNativeBody0Ch,movie_));
    return other_.Construct(branch,raw);
}
SceneFactoryNativeTaskServices MovieSceneFactoryNativeConstructorsRuntime::TaskServices()const noexcept
{auto result=other_.TaskServices();result.movie=&movie_;return result;}
TaskNativeBody4h* ProgramSceneFactoryNativeConstructorsRuntime::Construct(Branch branch,void* raw)
{
    if(branch==Branch::Program)return reinterpret_cast<TaskNativeBody4h*>(
        ConstructProgramSceneNativeAt4A11E0(*::new(raw)ProgramSceneNativeBody20h,program_));
    return other_.Construct(branch,raw);
}
SceneFactoryNativeTaskServices ProgramSceneFactoryNativeConstructorsRuntime::TaskServices()const noexcept
{auto result=other_.TaskServices();result.program=&program_;return result;}
}
