#pragma once
#include <Runtime/DemoSceneNative.h>
#include <Runtime/TextDisplayNative.h>
#include <Runtime/GraffitiPageNative.h>
#include <Runtime/LoadAfterSelectionNative.h>
#include <Runtime/GraffitiCountDialogNative.h>
#include <Runtime/ReadScriptSceneNative.h>
#include <Runtime/TitleSceneNative.h>
#include <Runtime/SaveLoadSceneNative.h>
#include <Runtime/SettingSceneNative.h>
#include <Runtime/LogSceneNative.h>
#include <Runtime/MusicSceneNative.h>
#include <Runtime/CgSceneNativeScope.h>
#include <Runtime/ReplaySceneNativeScope.h>
#include <Runtime/LikabilitySceneNativeScope.h>
#include <Runtime/MovieSceneNativeScope.h>
#include <Runtime/ProgramSceneNative.h>
#include <optional>

namespace KaGuYa::Start::Runtime
{
// 外部服务绑定不进入原ITask正文；无业务状态、额外task或引用控制块。
struct SceneFactoryNativeTaskServices final
{
    DemoSceneNativeServices* demo=nullptr;
    TextDisplayNativeServices* text=nullptr;
    GraffitiPageNativeServices* graffiti=nullptr;
    LoadAfterSelectionNativeServices* loadAfter=nullptr;
    GraffitiCountDialogNativeServices* graffitiCount=nullptr;
    ReadScriptSceneNativeServices* readScript=nullptr;
    TitleSceneNativeServices* title=nullptr;
    SaveLoadSceneNativeServices* saveLoad=nullptr;
    SettingSceneNativeServices* setting=nullptr;
    LogSceneNativeServices* log=nullptr;
    MusicSceneNativeServices* music=nullptr;
    CgSceneNativeServices* cg=nullptr;
    ReplaySceneNativeServices* replay=nullptr;
    LikabilitySceneNativeServices* likability=nullptr;
    MovieSceneNativeServices* movie=nullptr;
    ProgramSceneNativeServices* program=nullptr;
};
class SceneFactoryNativeTaskScope final
{
public:
    explicit SceneFactoryNativeTaskScope(SceneFactoryNativeTaskServices services) noexcept
    {
        if(services.demo)demo_.emplace(*services.demo);
        if(services.text)text_.emplace(*services.text);
        if(services.graffiti)graffiti_.emplace(*services.graffiti);
        if(services.loadAfter)loadAfter_.emplace(*services.loadAfter);
        if(services.graffitiCount)graffitiCount_.emplace(*services.graffitiCount);
        if(services.readScript)readScript_.emplace(*services.readScript);
        if(services.title)title_.emplace(*services.title);
        if(services.saveLoad)saveLoad_.emplace(*services.saveLoad);
        if(services.setting)setting_.emplace(*services.setting);
        if(services.log)log_.emplace(*services.log);
        if(services.music)music_.emplace(*services.music);
        if(services.cg)cg_.emplace(*services.cg);
        if(services.replay)replay_.emplace(*services.replay);
        if(services.likability)likability_.emplace(*services.likability);
        if(services.movie)movie_.emplace(*services.movie);
        if(services.program)program_.emplace(*services.program);
    }
    SceneFactoryNativeTaskScope(const SceneFactoryNativeTaskScope&)=delete;
    SceneFactoryNativeTaskScope& operator=(const SceneFactoryNativeTaskScope&)=delete;
private:
    std::optional<DemoSceneNativeScope> demo_;
    std::optional<TextDisplayNativeScope> text_;
    std::optional<GraffitiPageNativeScope> graffiti_;
    std::optional<LoadAfterSelectionNativeScope> loadAfter_;
    std::optional<GraffitiCountDialogNativeScope> graffitiCount_;
    std::optional<ReadScriptSceneNativeScope> readScript_;
    std::optional<TitleSceneNativeScope> title_;
    std::optional<SaveLoadSceneNativeScope> saveLoad_;
    std::optional<SettingSceneNativeScope> setting_;
    std::optional<LogSceneNativeScope> log_;
    std::optional<MusicSceneNativeScope> music_;
    std::optional<CgSceneNativeScope> cg_;
    std::optional<ReplaySceneNativeScope> replay_;
    std::optional<LikabilitySceneNativeScope> likability_;
    std::optional<MovieSceneNativeScope> movie_;
    std::optional<ProgramSceneNativeScope> program_;
};
}
