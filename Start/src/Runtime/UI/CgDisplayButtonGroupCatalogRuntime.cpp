#include <Runtime/CgDisplayButtonGroupCatalogRuntime.h>
#include <Runtime/ParamsThumbnailCatalogRuntime.h>

#include <limits>
#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
[[nodiscard]] std::int32_t CheckedSizeToInt32(const std::size_t size)
{
    if (size > static_cast<std::size_t>((std::numeric_limits<std::int32_t>::max)()))
        throw std::overflow_error("CG display collection exceeds signed 32-bit range");
    return static_cast<std::int32_t>(size);
}
}

CgDisplayStringResourceIdentifierAt499DC0::
    CgDisplayStringResourceIdentifierAt499DC0(std::string cp932Name)
    : cp932Name_(std::move(cp932Name))
{
}

CgDisplayStringResourceIdentifierAt499DC0::
    CgDisplayStringResourceIdentifierAt499DC0(const char* const* borrowedTextSlot) noexcept
    : borrowedTextSlot_(borrowedTextSlot)
{
}

CgDisplayProgramRegistryPageBaseVariantSourceAt499DC0::
    CgDisplayProgramRegistryPageBaseVariantSourceAt499DC0(
        const IProgramRegistrySlot1& slot1) noexcept
    : slot1_(&slot1)
{
}

std::uint8_t CgDisplayProgramRegistryPageBaseVariantSourceAt499DC0::
    ReadFromCsaveDataSoundParamsVslot84(
        const std::int32_t categoryIndex) const
{
    return slot1_->ReadChildVslot84Byte(categoryIndex) ? 1u : 0u;
}

std::string_view CgDisplayStringResourceIdentifierAt499DC0::Cp932Name() const noexcept
{
    if (borrowedTextSlot_)
    {
        // 现代view将合法的空alias表示为空；原始字段地址仍由BorrowedTextSlot保留。
        const char* const characters = *borrowedTextSlot_;
        return characters ? std::string_view(characters) : std::string_view{};
    }
    return cp932Name_;
}

CgDisplayButtonGroupCatalogAt499DC0::CgDisplayButtonGroupCatalogAt499DC0(
    SecretCgArchive& secretCg,
    ISecretCgParamsRootSource& thumbnailParams,
    const ICgDisplayPageBaseVariantSourceAt499DC0& pageBaseVariantSource) noexcept
    : secretCg_(&secretCg),
      thumbnailParams_(&thumbnailParams),
      pageBaseVariantSource_(&pageBaseVariantSource)
{
}

const SecretCgMappingRuntime* CgDisplayButtonGroupCatalogAt499DC0::TryCategory(
    const std::int32_t categoryIndex) const noexcept
{
    if (categoryIndex < 0 ||
        static_cast<std::size_t>(categoryIndex) >= secretCg_->CategoryCount())
    {
        return nullptr;
    }
    return &secretCg_->mappingsAt18.begin[static_cast<std::size_t>(categoryIndex)];
}

bool CgDisplayButtonGroupCatalogAt499DC0::IsFlagEnabled(
    const std::int32_t flagIndex) const
{
    // 原函数会以该值直接索引 CSecretCG+0x0C。构造期保留的 -1 或越界值
    // 到达这里说明对象图不一致，重写版明确报错而不静默忽略。
    if (flagIndex < 0 ||
        flagIndex >= secretCg_->CountAtVslot08())
    {
        throw std::out_of_range("CG display mapping references an invalid secret-CG flag");
    }
    return secretCg_->GetFlagAtVslot0C(static_cast<std::uint32_t>(flagIndex)) != 0;
}

std::int32_t CgDisplayButtonGroupCatalogAt499DC0::
    ItemCountFromOffsets18And1C(const std::int32_t categoryIndex) const
{
    const auto* const category = TryCategory(categoryIndex);
    return category == nullptr ? -1 : CheckedSizeToInt32(secretCg_->ItemCount(categoryIndex));
}

std::optional<CgDisplayButtonGroupCategoryItemAt499DC0>
CgDisplayButtonGroupCatalogAt499DC0::TryReadItemFromOffsets18And1C(
    const std::int32_t categoryIndex,
    const std::int32_t itemIndex) const
{
    const auto* const category = TryCategory(categoryIndex);
    if (category == nullptr || itemIndex < 0 ||
        static_cast<std::size_t>(itemIndex) >= secretCg_->ItemCount(categoryIndex))
    {
        return std::nullopt;
    }

    const auto indices = secretCg_->Indices(categoryIndex, itemIndex);
    std::int32_t enabledCount = 0;
    for (std::size_t index = 0; index < indices.size(); ++index)
    {
        if (IsFlagEnabled(indices[index]))
            ++enabledCount;
    }
    return CgDisplayButtonGroupCategoryItemAt499DC0{
        CheckedSizeToInt32(indices.size()), enabledCount};
}

std::uint8_t CgDisplayButtonGroupCatalogAt499DC0::
    ReadByteFromRuntimeSlot1Element0Vslot84(
        const std::int32_t categoryIndex) const
{
    return pageBaseVariantSource_->ReadFromCsaveDataSoundParamsVslot84(
        categoryIndex);
}

std::int32_t CgDisplayButtonGroupCatalogAt499DC0::ReadAtVslot24(
    const std::int32_t categoryIndex)
{
    if (!TryCategory(categoryIndex)) return 0;
    // 现代UI入口明确拒绝损坏的映射；合法输入直接执行原+24的DWORD计数语义。
    for (std::size_t item = 0; item < secretCg_->ItemCount(categoryIndex); ++item)
    {
        const auto indices = secretCg_->Indices(categoryIndex, item);
        for (std::size_t i = 0; i < indices.size(); ++i) static_cast<void>(IsFlagEnabled(indices[i]));
    }
    return secretCg_->PercentCategoryAtVslot24(categoryIndex);
}

std::optional<CgDisplayButtonGroupResourceIdentifierRefAt499DC0>
CgDisplayButtonGroupCatalogAt499DC0::ReadAtGlobalVslot1C(
    const std::int32_t categoryIndex,
    const std::int32_t itemIndex)
{
    const auto* const category = TryCategory(categoryIndex);
    if (category == nullptr || itemIndex < 0 ||
        static_cast<std::size_t>(itemIndex) >= secretCg_->ItemCount(categoryIndex))
    {
        return std::nullopt;
    }

    const auto indices = secretCg_->Indices(categoryIndex, itemIndex);
    for (std::size_t i = 0; i < indices.size(); ++i)
    {
        if (IsFlagEnabled(indices[i])) break;
    }
    // 真正477FD0负责增持mapping临时Ref、两次418480和借用alias释放顺序。
    if (const auto* alias = secretCg_->ThumbnailAt477FD0(
            *thumbnailParams_, categoryIndex, itemIndex))
        return std::make_shared<CgDisplayStringResourceIdentifierAt499DC0>(alias);
    return std::nullopt;
}

CgDisplayButtonGroupProductionCatalogOwnerAt499DC0::
    CgDisplayButtonGroupProductionCatalogOwnerAt499DC0(
        SecretCgArchive& secretCg,
        ISecretCgParamsRootSource& thumbnailParams,
        const IProgramRegistrySlot1& slot1) noexcept
    : pageBaseVariantSource_(slot1),
      catalog_(secretCg, thumbnailParams, pageBaseVariantSource_)
{
}

CgDisplayButtonGroupCatalogAt499DC0&
CgDisplayButtonGroupProductionCatalogOwnerAt499DC0::Catalog() noexcept
{
    return catalog_;
}

const CgDisplayButtonGroupCatalogAt499DC0&
CgDisplayButtonGroupProductionCatalogOwnerAt499DC0::Catalog() const noexcept
{
    return catalog_;
}

std::unique_ptr<CgDisplayButtonGroupRuntime>
CreateCgDisplayButtonGroupProductionAt499DC0(
    const std::int32_t categoryIndex,
    CgDisplayButtonGroupProductionDependenciesAt499DC0 dependencies)
{
    CgDisplayButtonGroupProductionCatalogOwnerAt499DC0 catalogOwner(
        dependencies.secretCg,
        dependencies.thumbnailParams,
        dependencies.slot1);
    auto& catalog = catalogOwner.Catalog();
    return CreateCgDisplayButtonGroupAt499DC0(
        categoryIndex,
        CgDisplayButtonGroupFactoryDependenciesAt499DC0{
            dependencies.runtimeServices,
            catalog,
            catalog,
            catalog,
            dependencies.staticData,
            dependencies.graphicsProvider,
            dependencies.buttonFactory,
            dependencies.sharedControls});
}
}
