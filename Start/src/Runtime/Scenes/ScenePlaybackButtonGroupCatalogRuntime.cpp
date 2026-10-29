#include <Runtime/ScenePlaybackButtonGroupCatalogRuntime.h>
#include <Runtime/ParamsThumbnailCatalogRuntime.h>

#include <bit>
#include <limits>
#include <mbstring.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
std::int32_t CheckedItemCount(const std::size_t size)
{
    if (size > static_cast<std::size_t>((std::numeric_limits<std::int32_t>::max)()))
        throw std::overflow_error("Replay item count exceeds signed 32-bit range");
    return static_cast<std::int32_t>(size);
}
}

ScenePlaybackButtonGroupCatalogRuntime::ScenePlaybackButtonGroupCatalogRuntime(
    const ParamsGameSystem& gameSystem, const SecretScArchive& secretSc) noexcept
    : gameSystem_(&gameSystem), secretSc_(&secretSc)
{
}

const ParamsRegistSceneGroupRuntime* ScenePlaybackButtonGroupCatalogRuntime::FindGroup(
    const std::int32_t group) const
{
    // 原版只有 signed 上界判断；负索引会越界读，重写明确拒绝而非回退分组。
    if (group < 0) throw std::out_of_range("Replay group index is negative");
    const auto& groups = gameSystem_->registScene;
    return static_cast<std::size_t>(group) < groups.size()
        ? &groups[static_cast<std::size_t>(group)] : nullptr;
}

std::int32_t ScenePlaybackButtonGroupCatalogRuntime::ItemCountFromRegistSceneAt110(
    const std::int32_t group) const
{
    const auto* const entry = FindGroup(group);
    return entry ? CheckedItemCount(entry->itemsAt04.size()) : -1;
}

bool ScenePlaybackButtonGroupCatalogRuntime::AllSegmentsUnlocked(
    const ParamsRegistSceneItemRuntime& item) const
{
    // 478230/47C050 都按 +F8 的 sceneNames 查首项，区分大小写。
    // 与 SceneLabel 的不区分大小写脚本入口查询、CG 的任一解锁均不同。
    for (auto* segment = item.scenesAt00.begin; segment != item.scenesAt00.end; ++segment)
    {
        std::size_t index = 0;
        for (; index < gameSystem_->sceneNames.size(); ++index)
        {
            if (_mbscmp(reinterpret_cast<const unsigned char*>(
                            gameSystem_->sceneNames[index].GetString()),
                    reinterpret_cast<const unsigned char*>(segment->GetString())) == 0)
                break;
        }
        if (index == gameSystem_->sceneNames.size()) return false;
        if (index >= secretSc_->CountAtVslot08())
            throw std::out_of_range("Replay scene name has no secret-SC flag");
        if (!secretSc_->GetFlagAtVslot0C(static_cast<std::uint32_t>(index))) return false;
    }
    // 空段数组没有失败迭代，原版保留解锁状态。
    return true;
}

std::int32_t ScenePlaybackButtonGroupCatalogRuntime::CompletionPercentAt47C050(
    const std::int32_t group) const
{
    const auto* const entry = FindGroup(group);
    if (!entry) return 0;
    const auto count = CheckedItemCount(entry->itemsAt04.size());
    std::uint32_t unlocked = 0;
    for (const auto& item : entry->itemsAt04)
        if (AllSegmentsUnlocked(item)) ++unlocked;
    // 原 x86 imul 保留低 32 位，再做 signed idiv；不把分母换成总段数。
    const auto numerator = std::bit_cast<std::int32_t>(100u * unlocked);
    const auto percent = numerator / (count == 0 ? 1 : count);
    return percent == 0 && unlocked != 0 ? 1 : percent;
}

std::optional<CgDisplayButtonGroupResourceIdentifierRefAt499DC0>
ScenePlaybackButtonGroupCatalogRuntime::ThumbnailAt478230(
    const std::int32_t group, const std::int32_t item) const
{
    const auto* const entry = FindGroup(group);
    if (!entry) return std::nullopt;
    if (item < 0) throw std::out_of_range("Replay item index is negative");
    if (static_cast<std::size_t>(item) >= entry->itemsAt04.size()) return std::nullopt;
    const auto& selected = entry->itemsAt04[static_cast<std::size_t>(item)];
    if (!AllSegmentsUnlocked(selected)) return std::nullopt;
    const auto* const alias = BorrowParamsThumbnailAliasAt477800(
        *gameSystem_, selected.nameAt0C.GetString());
    if (!alias) return std::nullopt;
    return std::make_shared<CgDisplayStringResourceIdentifierAt499DC0>(
        alias);
}
}
