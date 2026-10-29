#pragma once
#include <Runtime/RuntimeCString.h>
#include <cstdint>
#include <string_view>

namespace KaGuYa::Start::Runtime
{
struct AppreciationSceneResourceProfile final
{
    std::int32_t categoryCount;
    std::string_view cgBgm;
    std::string_view replayBgm;
    std::string_view confirmCue;
    std::string_view focusCue;
    std::string_view cancelCue;
    bool listRightClickToTitle;
};

// 所有string_view均引用以NUL结尾的静态CP932字串。
const AppreciationSceneResourceProfile& CurrentAppreciationSceneProfile();

// 由CgSceneUiNative.Globals.cpp持有，CRT初始化，退出时仅释放一次。
// Root CG/Replay依赖可借用此槽作为取消cue，不接管其生命周期。
RuntimeCString& ProcessAppreciationSceneCancelCue() noexcept;
}
