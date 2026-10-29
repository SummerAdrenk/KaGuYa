#pragma once
#include <Runtime/RootMusicSceneNativeSource.h>
#include <optional>

namespace KaGuYa::Start::Runtime
{
struct ModeSelectButtonNativeServices;
struct OptionCharSelectDialogNativeDependencies;
// 长期借用服务装配；不创建Scene/按钮/GUI正文，不在构造中取根或装载资源。
class RootMusicSceneServicesRuntime final
{
public:
    // 完整冷装配入口：只拥有服务聚合，公共button/input/screen仍借同源owner。
    RootMusicSceneServicesRuntime(RootMusicSceneNativeDependencies,ModeSelectButtonNativeServices&,
        OptionCharSelectDialogNativeDependencies&)noexcept;
    RootMusicSceneServicesRuntime(RootMusicSceneNativeDependencies d,ModeButtonNativeServices& mode,
        MusicDialogNativeServices& dialog) noexcept
        :source_(d),dialog_(dialog),button_{mode,dialog_,source_,d.strings},buttons_(button_),
         scene_{source_,buttons_,mode.memory,dialog.surfaces}{}
    MusicSceneNativeServices& Scene() noexcept{return scene_;}
    MusicButtonNativeServices& Button() noexcept{return button_;}
    RootMusicSceneNativeSource& Source() noexcept{return source_;}
private:
    std::optional<ModeButtonNativeServices> ownedMode_;
    std::optional<MusicDialogNativeServices> ownedDialog_;
    RootMusicSceneNativeSource source_;
    MusicDialogNativeConstruction dialog_;
    MusicButtonNativeServices button_;
    MusicButtonNativeConstruction buttons_;
    MusicSceneNativeServices scene_;
};
}
