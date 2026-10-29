#pragma once
#include <Runtime/SceneFactoryNativeRuntime.h>
#include <Runtime/LegacyTitleScene.h>
#include <Runtime/LegacyMenuSelectionNative.h>
#include <Runtime/LegacyCharacterSelectionNative.h>
#include <Runtime/LegacyItemAcquisitionNative.h>
#include <Runtime/LegacyMapSelectionNative.h>

namespace KaGuYa::Start::Runtime
{
// 必须借用已构造的真实服务；所有task/GUI的末Ref先退役，不能用空域或测试链尾补位。
struct CompleteSceneFactoryNativeConstructorsDependencies final
{
    DemoSceneNativeServices& demo;
    TitleSceneNativeServices& title;
    MusicSceneNativeServices& music;
    CgSceneNativeServices& cg;
    ReplaySceneNativeServices& replay;
    LogSceneNativeServices& log;
    SaveLoadSceneNativeServices& saveLoad;
    LoadAfterSelectionNativeServices& loadAfter;
    ReadScriptSceneNativeServices& readScript;
    SettingSceneNativeServices& setting;
    LikabilitySceneNativeServices& likability;
    MovieSceneNativeServices& movie;
    TextDisplayNativeServices& text;
    ProgramSceneNativeServices& program;
    GraffitiPageNativeServices& graffiti;
    // 非空仅在完整旧服务图已装配时提供；默认现代分支不改变，不能传测试/抛空叶。
    LegacyTitleSceneServices* legacyTitle = nullptr;
    LegacyMenuSelectionServices* menuSelection = nullptr;
    LegacyCharacterSelectionServices* characterSelection = nullptr;
    LegacyItemAcquisitionServices* itemAcquisition = nullptr;
    LegacyMapSelectionServices* mapSelection = nullptr;
};

class CompleteSceneFactoryNativeConstructorsRuntime final:public ISceneFactoryNativeConstructors
{
public:
    explicit CompleteSceneFactoryNativeConstructorsRuntime(
        CompleteSceneFactoryNativeConstructorsDependencies dependencies) noexcept
        :dependencies_(dependencies){}
    CompleteSceneFactoryNativeConstructorsRuntime(const CompleteSceneFactoryNativeConstructorsRuntime&)=delete;
    CompleteSceneFactoryNativeConstructorsRuntime& operator=(const CompleteSceneFactoryNativeConstructorsRuntime&)=delete;
    TaskNativeBody4h* Construct(SceneFactoryNativeBranch,void* storage) override;
    std::size_t AllocationSize(SceneFactoryNativeBranch, std::size_t) const noexcept override;
    SceneFactoryNativeTaskServices TaskServices() const noexcept override;
private:
    CompleteSceneFactoryNativeConstructorsDependencies dependencies_;
};
}
