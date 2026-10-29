#pragma once

#include <Runtime/ParamsRuntimeProfile.h>
#include <Runtime/ScenePlaybackScriptServices.h>

namespace KaGuYa::Start::Runtime
{
// 持有只读 params 视图，根 owner 必须保证 profile 比目录活得更久。
// 标签查询每次返回新的二 DWORD 对象，与 4AD300 的拷贝而非 alias 行为一致。
class ScenePlaybackCatalogRuntime final
{
public:
    explicit ScenePlaybackCatalogRuntime(const ParamsRuntimeProfile& profile)
        : profile_(&profile) {}
    ScenePlaybackCatalogRuntime(ParamsRuntimeProfile&&) = delete;

    [[nodiscard]] ScenePlaybackSelectionRef FindLabel(const char* nameCp932) const;
    [[nodiscard]] ScenePlaybackSelectionRef FindSelection(
        std::int32_t group, std::int32_t item, std::int32_t segment) const;

private:
    const ParamsRuntimeProfile* profile_;
};
}
