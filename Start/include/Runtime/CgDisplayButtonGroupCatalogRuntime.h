#pragma once

#include <Runtime/CgDisplayButtonGroupFactoryRuntime.h>
#include <Runtime/EnvironmentStateArchives.h>
#include <Runtime/ParamsRuntimeProfile.h>
#include <Runtime/ParamsPatternSource.h>
#include <Runtime/ProgramRuntimeServices.h>

#include <cstdint>
#include <string>
#include <string_view>

namespace KaGuYa::Start::Runtime
{
// 0x499DC0 对 root slot 1 的首个 CSoundParams 元素调用 vslot +0x84。
// 该虚槽尚未完成业务翻译，因此必须由其真正 owner 提供，禁止以 category 代替。
class ICgDisplayPageBaseVariantSourceAt499DC0
{
public:
    virtual ~ICgDisplayPageBaseVariantSourceAt499DC0() = default;

    [[nodiscard]] virtual std::uint8_t
        ReadFromCsaveDataSoundParamsVslot84(std::int32_t categoryIndex) const = 0;
};

class CgDisplayProgramRegistryPageBaseVariantSourceAt499DC0 final
    : public ICgDisplayPageBaseVariantSourceAt499DC0
{
public:
    explicit CgDisplayProgramRegistryPageBaseVariantSourceAt499DC0(
        const IProgramRegistrySlot1& slot1) noexcept;

    [[nodiscard]] std::uint8_t ReadFromCsaveDataSoundParamsVslot84(
        std::int32_t categoryIndex) const override;

private:
    const IProgramRegistrySlot1* slot1_ = nullptr;
};

// 477FD0 最终返回 Thumbnail 记录 +4 的 CP932 资源名。factory 保持标识不透明，
// 真正 Graphics provider 可在装载边界取得这一窄字符串。
class CgDisplayStringResourceIdentifierAt499DC0 final
    : public ICgDisplayButtonGroupResourceIdentifierAt499DC0
{
public:
    explicit CgDisplayStringResourceIdentifierAt499DC0(std::string cp932Name);
    explicit CgDisplayStringResourceIdentifierAt499DC0(
        const char* const* borrowedTextSlot) noexcept;

    [[nodiscard]] std::string_view Cp932Name() const noexcept;
    [[nodiscard]] const char* const* BorrowedTextSlot() const noexcept
        { return borrowedTextSlot_; }

private:
    // 477FD0/478230借用正文+4槽，不延长params寿命；拥有字符串的构造仅供独立适配输入。
    const char* const* borrowedTextSlot_ = nullptr;
    std::string cp932Name_;
};

class CgDisplayButtonGroupCatalogAt499DC0 final
    : public ICgDisplayButtonGroupCategoryContainerAt499DC0,
      public ICgDisplayButtonGroupCategoryVslot24At499DC0,
      public ICgDisplayButtonGroupGlobalVslot1CAt499DC0
{
public:
    CgDisplayButtonGroupCatalogAt499DC0(
        SecretCgArchive& secretCg,
        ISecretCgParamsRootSource& thumbnailParams,
        const ICgDisplayPageBaseVariantSourceAt499DC0& pageBaseVariantSource) noexcept;

    [[nodiscard]] std::int32_t ItemCountFromOffsets18And1C(
        std::int32_t categoryIndex) const override;
    [[nodiscard]] std::optional<CgDisplayButtonGroupCategoryItemAt499DC0>
        TryReadItemFromOffsets18And1C(
            std::int32_t categoryIndex,
            std::int32_t itemIndex) const override;
    [[nodiscard]] std::uint8_t ReadByteFromRuntimeSlot1Element0Vslot84(
        std::int32_t categoryIndex) const override;
    [[nodiscard]] std::int32_t ReadAtVslot24(
        std::int32_t categoryIndex) override;
    [[nodiscard]] std::optional<
        CgDisplayButtonGroupResourceIdentifierRefAt499DC0>
        ReadAtGlobalVslot1C(
            std::int32_t categoryIndex,
            std::int32_t itemIndex) override;

private:
    [[nodiscard]] const SecretCgMappingRuntime* TryCategory(
        std::int32_t categoryIndex) const noexcept;
    [[nodiscard]] bool IsFlagEnabled(std::int32_t flagIndex) const;

    SecretCgArchive* secretCg_ = nullptr;
    ISecretCgParamsRootSource* thumbnailParams_ = nullptr;
    const ICgDisplayPageBaseVariantSourceAt499DC0* pageBaseVariantSource_ = nullptr;
};

class CgDisplayButtonGroupProductionCatalogOwnerAt499DC0 final
{
public:
    CgDisplayButtonGroupProductionCatalogOwnerAt499DC0(
        SecretCgArchive& secretCg,
        ISecretCgParamsRootSource& thumbnailParams,
        const IProgramRegistrySlot1& slot1) noexcept;

    [[nodiscard]] CgDisplayButtonGroupCatalogAt499DC0& Catalog() noexcept;
    [[nodiscard]] const CgDisplayButtonGroupCatalogAt499DC0& Catalog() const noexcept;

private:
    // catalog_ 保存 pageBaseVariantSource_ 的地址，声明顺序不得交换。
    CgDisplayProgramRegistryPageBaseVariantSourceAt499DC0 pageBaseVariantSource_;
    CgDisplayButtonGroupCatalogAt499DC0 catalog_;
};

struct CgDisplayButtonGroupProductionDependenciesAt499DC0 final
{
    SecretCgArchive& secretCg;
    ISecretCgParamsRootSource& thumbnailParams;
    const IProgramRegistrySlot1& slot1;
    ICgDisplayButtonGroupRuntimeServices& runtimeServices;
    ICgDisplayButtonGroupStaticDataAt499DC0& staticData;
    ICgDisplayButtonGroupGraphicsProviderAt499DC0& graphicsProvider;
    ICgDisplayButtonGroupButtonFactoryAt499DC0& buttonFactory;
    ICgDisplayPublicControlsSourceAt499DC0& sharedControls;
};

// 生产调用点只能从同一 CSecretCG/params/registry owner 建立三条动态数据接口，
// 禁止像单元测试那样分别注入互相矛盾的 category/global 实现。
[[nodiscard]] std::unique_ptr<CgDisplayButtonGroupRuntime>
CreateCgDisplayButtonGroupProductionAt499DC0(
    std::int32_t categoryIndex,
    CgDisplayButtonGroupProductionDependenciesAt499DC0 dependencies);
}
