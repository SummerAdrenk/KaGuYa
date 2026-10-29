#include <Runtime/CompleteSceneFactoryNativeConstructorsRuntime.h>
#include <Runtime/CgSceneNative.h>
#include <Runtime/ReplaySceneNative.h>
#include <Runtime/LikabilitySceneNative.h>
#include <Runtime/MovieSceneNative.h>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
std::size_t CompleteSceneFactoryNativeConstructorsRuntime::AllocationSize(
    SceneFactoryNativeBranch branch, std::size_t recordedSize) const noexcept
{
    if(branch == SceneFactoryNativeBranch::LegacyMenuSelection)
        return sizeof(LegacyMenuSelectionScene);
    if(branch == SceneFactoryNativeBranch::LegacyCharacterSelection)
        return sizeof(LegacyCharacterSelectionScene);
    if(branch == SceneFactoryNativeBranch::LegacyItemAcquisition)
        return sizeof(LegacyItemAcquisitionScene);
    if(branch == SceneFactoryNativeBranch::LegacyMapSelection)
        return sizeof(LegacyMapSelectionScene);
    return branch == SceneFactoryNativeBranch::Title && dependencies_.legacyTitle ?
        sizeof(LegacyTitleScene) : recordedSize;
}
TaskNativeBody4h* CompleteSceneFactoryNativeConstructorsRuntime::Construct(
    SceneFactoryNativeBranch branch,void* raw)
{
    using Branch=SceneFactoryNativeBranch;
    switch(branch)
    {
    case Branch::Logo:
        return reinterpret_cast<TaskNativeBody4h*>(ConstructLogoNativeAt4AAFEF(
            *::new(raw) DemoSceneNativeBody18h,dependencies_.demo));
    case Branch::Title:
        if (dependencies_.legacyTitle)
            return &ConstructLegacyTitleScene(*::new(raw) LegacyTitleScene, *dependencies_.legacyTitle)->task;
        return reinterpret_cast<TaskNativeBody4h*>(ConstructTitleSceneNativeAt4A2850(
            *::new(raw) TitleSceneNativeBody1Ch,dependencies_.title));
    case Branch::Music:
        return reinterpret_cast<TaskNativeBody4h*>(ConstructMusicSceneNativeAt4A0730(
            *::new(raw) MusicSceneNativeBody64h,dependencies_.music));
    case Branch::Cg:
        return reinterpret_cast<TaskNativeBody4h*>(ConstructCgSceneNativeAt49E3B0(
            *::new(raw) CgSceneNativeBodyB4h,dependencies_.cg));
    case Branch::Scene:
        return reinterpret_cast<TaskNativeBody4h*>(ConstructReplaySceneNativeAt49EC90(
            *::new(raw) ReplaySceneNativeBody80h,dependencies_.replay));
    case Branch::Log:
        return reinterpret_cast<TaskNativeBody4h*>(ConstructLogSceneNativeAt49E180(
            *::new(raw) LogSceneNativeBody20h,dependencies_.log));
    case Branch::Save:
        return reinterpret_cast<TaskNativeBody4h*>(ConstructSaveLoadSceneNativeAt4A17B0(
            *::new(raw) SaveLoadSceneNativeBody24h,1u,dependencies_.saveLoad));
    case Branch::Load:
        return reinterpret_cast<TaskNativeBody4h*>(ConstructSaveLoadSceneNativeAt4A17B0(
            *::new(raw) SaveLoadSceneNativeBody24h,0u,dependencies_.saveLoad));
    case Branch::LoadAfter:
        return reinterpret_cast<TaskNativeBody4h*>(ConstructLoadAfterSelectionNativeAt4AB47B(
            *::new(raw) LoadAfterSelectionNativeBody58h,dependencies_.loadAfter));
    case Branch::Script:
        return reinterpret_cast<TaskNativeBody4h*>(ConstructReadScriptSceneNativeAt4A14D0(
            *::new(raw) ReadScriptSceneNativeBodyBCh,dependencies_.readScript));
    case Branch::Setting:
        return reinterpret_cast<TaskNativeBody4h*>(ConstructSettingSceneNativeAt4A1A00(
            *::new(raw) SettingSceneNativeBody20h,dependencies_.setting));
    case Branch::Affection:
        return reinterpret_cast<TaskNativeBody4h*>(ConstructLikabilitySceneNativeAt49DFB0(
            *::new(raw) LikabilitySceneNativeBody20h,dependencies_.likability));
    case Branch::Movie:
        return reinterpret_cast<TaskNativeBody4h*>(ConstructMovieSceneNativeAt4A0B00(
            *::new(raw) MovieSceneNativeBody0Ch,dependencies_.movie));
    case Branch::Ending:
        return reinterpret_cast<TaskNativeBody4h*>(ConstructEndingNativeAt49DA10(
            *::new(raw) DemoSceneNativeBody18h,dependencies_.demo));
    case Branch::Text:
        return reinterpret_cast<TaskNativeBody4h*>(ConstructTextDisplayNativeAt4A1F30(
            *::new(raw) TextDisplayNativeBody90h,dependencies_.text));
    case Branch::Program:
        return reinterpret_cast<TaskNativeBody4h*>(ConstructProgramSceneNativeAt4A11E0(
            *::new(raw) ProgramSceneNativeBody20h,dependencies_.program));
    case Branch::Graffiti:
        return reinterpret_cast<TaskNativeBody4h*>(ConstructGraffitiPageNativeAt49DCA0(
            *::new(raw) GraffitiPageNativeBody20h,dependencies_.graffiti));
    case Branch::LegacyMenuSelection:
        if(!dependencies_.menuSelection)
            throw std::logic_error("Legacy menu selection services are not bound");
        return reinterpret_cast<TaskNativeBody4h*>(ConstructLegacyMenuSelectionScene(raw,*dependencies_.menuSelection));
    case Branch::LegacyCharacterSelection:
        if(!dependencies_.characterSelection)
            throw std::logic_error("Legacy character selection services are not bound");
        return reinterpret_cast<TaskNativeBody4h*>(ConstructLegacyCharacterSelectionScene(raw,*dependencies_.characterSelection));
    case Branch::LegacyItemAcquisition:
        if(!dependencies_.itemAcquisition)
            throw std::logic_error("Legacy item acquisition services are not bound");
        return reinterpret_cast<TaskNativeBody4h*>(ConstructLegacyItemAcquisitionScene(raw,*dependencies_.itemAcquisition));
    case Branch::LegacyMapSelection:
        if(!dependencies_.mapSelection)
            throw std::logic_error("Legacy map selection services are not bound");
        return reinterpret_cast<TaskNativeBody4h*>(ConstructLegacyMapSelectionScene(raw,*dependencies_.mapSelection));
    default:
        throw std::invalid_argument("invalid SceneFactoryNativeBranch");
    }
}

SceneFactoryNativeTaskServices CompleteSceneFactoryNativeConstructorsRuntime::TaskServices() const noexcept
{
    SceneFactoryNativeTaskServices result;
    result.demo=&dependencies_.demo;
    result.text=&dependencies_.text;
    result.graffiti=&dependencies_.graffiti;
    result.loadAfter=&dependencies_.loadAfter;
    result.readScript=&dependencies_.readScript;
    result.title=&dependencies_.title;
    result.saveLoad=&dependencies_.saveLoad;
    result.setting=&dependencies_.setting;
    result.log=&dependencies_.log;
    result.music=&dependencies_.music;
    result.cg=&dependencies_.cg;
    result.replay=&dependencies_.replay;
    result.likability=&dependencies_.likability;
    result.movie=&dependencies_.movie;
    result.program=&dependencies_.program;
    // 保留原装饰器的查询时点，不在冷构造时调用或缓存Count服务。
    result.graffitiCount=dependencies_.graffiti.dialogs.CountDialogServices();
    return result;
}
}
