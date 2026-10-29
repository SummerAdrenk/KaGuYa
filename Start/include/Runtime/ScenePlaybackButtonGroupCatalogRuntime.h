#pragma once

#include <Runtime/CgDisplayButtonGroupCatalogRuntime.h>

namespace KaGuYa::Start::Runtime
{
class IScenePlaybackButtonGroupCatalog
{
public:
    virtual ~IScenePlaybackButtonGroupCatalog() = default;
    [[nodiscard]] virtual std::int32_t ItemCountFromRegistSceneAt110(
        std::int32_t group) const = 0;
    [[nodiscard]] virtual std::int32_t CompletionPercentAt47C050(
        std::int32_t group) const = 0;
    [[nodiscard]] virtual std::optional<CgDisplayButtonGroupResourceIdentifierRefAt499DC0>
        ThumbnailAt478230(std::int32_t group, std::int32_t item) const = 0;
};

// CSecretSC 的 +1C/+24 业务槽。借用同一 GameSystem 与可变解锁状态，
// 不缓存查询结果，避免保存状态变化后仍使用旧解锁/完成率。
class ScenePlaybackButtonGroupCatalogRuntime final : public IScenePlaybackButtonGroupCatalog
{
public:
    ScenePlaybackButtonGroupCatalogRuntime(
        const ParamsGameSystem& gameSystem, const SecretScArchive& secretSc) noexcept;

    [[nodiscard]] std::int32_t ItemCountFromRegistSceneAt110(
        std::int32_t group) const override;
    [[nodiscard]] std::int32_t CompletionPercentAt47C050(
        std::int32_t group) const override;
    [[nodiscard]] std::optional<CgDisplayButtonGroupResourceIdentifierRefAt499DC0>
        ThumbnailAt478230(std::int32_t group, std::int32_t item) const override;

private:
    [[nodiscard]] const ParamsRegistSceneGroupRuntime* FindGroup(std::int32_t group) const;
    [[nodiscard]] bool AllSegmentsUnlocked(const ParamsRegistSceneItemRuntime& item) const;

    const ParamsGameSystem* gameSystem_;
    const SecretScArchive* secretSc_;
};
}
