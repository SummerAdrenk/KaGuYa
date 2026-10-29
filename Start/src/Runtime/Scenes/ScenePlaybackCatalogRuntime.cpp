#include <Runtime/ScenePlaybackCatalogRuntime.h>

#include <atlbase.h>
#include <mbstring.h>
#include <bit>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
ScenePlaybackSelectionRef ScenePlaybackCatalogRuntime::FindLabel(
    const char* const nameCp932) const
{
    // 4AD300：非空表才检查 null，_mbsicmp 首次命中立即返回；重复项不覆盖。
    for (const auto& label : profile_->sceneLabels)
    {
        if (!nameCp932) throw ATL::CAtlException(E_FAIL);
        if (_mbsicmp(reinterpret_cast<const unsigned char*>(label.name.cp932.c_str()),
                reinterpret_cast<const unsigned char*>(nameCp932)) == 0)
        {
            return std::make_shared<ScenePlaybackSelection>(ScenePlaybackSelection{
                std::bit_cast<std::int32_t>(label.value1),
                std::bit_cast<std::int32_t>(label.value2)});
        }
    }
    return {};
}

ScenePlaybackSelectionRef ScenePlaybackCatalogRuntime::FindSelection(
    const std::int32_t group, const std::int32_t item, const std::int32_t segment) const
{
    // 4ADB79/4ADBF1 只有 signed 上界判断。原版负 group/item 会越界读，
    // 安全实现明确拒绝；合法上界未命中仍按原版返回空引用，不能抛加载失败。
    if (group < 0) throw std::out_of_range("Replay group index is negative");
    const auto& groups = profile_->gameSystem.registScene;
    if (static_cast<std::size_t>(group) >= groups.size()) return {};
    if (item < 0) throw std::out_of_range("Replay item index is negative");
    const auto& items = groups[static_cast<std::size_t>(group)].itemsAt04;
    if (static_cast<std::size_t>(item) >= items.size()) return {};
    const auto& scenes = items[static_cast<std::size_t>(item)].scenesAt00;
    // 原循环从 0 数到末尾；负 segment 只是无匹配，不访问负下标。
    std::uint32_t index = 0;
    for (auto* scene = scenes.begin; scene != scenes.end; ++scene, ++index)
        if (std::bit_cast<std::int32_t>(index) == segment) return FindLabel(scene->GetString());
    return {};
}
}
