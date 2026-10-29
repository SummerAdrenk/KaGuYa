#include <Runtime/CgDisplayButtonGroupSample18ButtonFactory.h>

#include <Runtime/CgDisplayButtonGroupCatalogRuntime.h>

#include <array>
#include <stdexcept>
#include <string>

namespace KaGuYa::Start::Runtime
{
namespace
{
struct PageButtonRecordAt480930 final
{
    std::array<const char*, 7> resources{};
    POINT position{};
    std::int32_t timelineIndex = -1;
};

constexpr PageButtonRecordAt480930 kPreviousPageRecordAt5B3FC0{{
    "vrm:\\\x8A\xD3\x8F\xDC\x89\xE6\x96\xCA\x83\x79\x81\x5B\x83\x57\x96\xDF\x82\xE9\x83\x49\x83\x93.alp",
    "vrm:\\\x8A\xD3\x8F\xDC\x89\xE6\x96\xCA\x83\x79\x81\x5B\x83\x57\x96\xDF\x82\xE9\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67.alp",
    "vrm:\\\x8A\xD3\x8F\xDC\x89\xE6\x96\xCA\x83\x79\x81\x5B\x83\x57\x96\xDF\x82\xE9\x83\x4E\x83\x8A\x83\x62\x83\x4E.alp",
    nullptr, nullptr, nullptr, nullptr},
    POINT{0, 0}, -1};

constexpr PageButtonRecordAt480930 kNextPageRecordAt5B3FE8{{
    "vrm:\\\x8A\xD3\x8F\xDC\x89\xE6\x96\xCA\x83\x79\x81\x5B\x83\x57\x90\x69\x82\xDE\x83\x49\x83\x93.alp",
    "vrm:\\\x8A\xD3\x8F\xDC\x89\xE6\x96\xCA\x83\x79\x81\x5B\x83\x57\x90\x69\x82\xDE\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67.alp",
    "vrm:\\\x8A\xD3\x8F\xDC\x89\xE6\x96\xCA\x83\x79\x81\x5B\x83\x57\x90\x69\x82\xDE\x83\x4E\x83\x8A\x83\x62\x83\x4E.alp",
    nullptr, nullptr, nullptr, nullptr},
    POINT{0, 0}, -1};

[[nodiscard]] const PageButtonRecordAt480930& RequireRecord(
    const std::uintptr_t recordAddress)
{
    // Replay 的 6023C0/6023E8 与 CG 的两条 40-byte 记录逐字节一致。
    if (recordAddress == 0x5B3FC0u || recordAddress == 0x6023C0u)
        return kPreviousPageRecordAt5B3FC0;
    if (recordAddress == 0x5B3FE8u || recordAddress == 0x6023E8u)
        return kNextPageRecordAt5B3FE8;
    throw std::out_of_range(
        "sample 18 CG display page-button record address is unknown");
}

[[nodiscard]] const CgDisplayStringResourceIdentifierAt499DC0&
RequireStringIdentifier(
    const CgDisplayButtonGroupResourceIdentifierRefAt499DC0& identifier)
{
    if (!identifier)
        throw std::invalid_argument("CG display button resource identifier is null");
    const auto* const concrete =
        dynamic_cast<const CgDisplayStringResourceIdentifierAt499DC0*>(
            identifier.get());
    if (concrete == nullptr)
    {
        throw std::invalid_argument(
            "CG display button resource identifier is not CP932 text");
    }
    return *concrete;
}
}

CgDisplayButtonGroupSample18ButtonFactoryAt499DC0::
    CgDisplayButtonGroupSample18ButtonFactoryAt499DC0(
        ICgUiButtonRuntimeServices& buttonServices,
        IGraphicsSurfaceCompositionApi& compositionApi,
        ICgOperationSurfaceFactory& surfaceFactory) noexcept
    : buttonServices_(&buttonServices),
      compositionApi_(&compositionApi),
      surfaceFactory_(&surfaceFactory)
{
}

std::shared_ptr<CgUiButtonRuntime>
CgDisplayButtonGroupSample18ButtonFactoryAt499DC0::AllocateButtonAt459530()
{
    return std::make_shared<CgUiButtonRuntime>(
        *buttonServices_, *compositionApi_);
}

bool CgDisplayButtonGroupSample18ButtonFactoryAt499DC0::
    AttachSurfaceAt4C1F20(
        CgUiButtonRuntime& button,
        const std::uint32_t stateSurfaceSlot,
        const CgDisplayButtonGroupResourceIdentifierRefAt499DC0& identifier,
        const std::shared_ptr<RuntimeGraphicsSurface>& surface)
{
    const auto& concrete = RequireStringIdentifier(identifier);
    return button.AttachNamedSurfaceAt4C1F20(
        stateSurfaceSlot, std::string(concrete.Cp932Name()), surface);
}

bool CgDisplayButtonGroupSample18ButtonFactoryAt499DC0::LoadButtonAt480930(
    CgUiButtonRuntime& button,
    const std::uintptr_t recordAddress,
    const std::int32_t finalArgument)
{
    const auto& record = RequireRecord(recordAddress);
    auto batch = surfaceFactory_->CreateBatch();
    if (!batch)
        throw std::logic_error("CG surface factory returned no batch");
    for (std::size_t slot = 0; slot < record.resources.size(); ++slot)
    {
        const char* const resourceName = record.resources[slot];
        if (resourceName == nullptr)
            continue;
        auto surface = batch->LoadStatic(resourceName);
        if (!surface || !button.AttachNamedSurfaceAt4C1F20(
                slot, resourceName, std::move(surface)))
        {
            return false;
        }
    }

    if (finalArgument != 0)
    {
        button.SetPositionAt24(record.position);
        button.SetUsePositionAt30At2C(false);
    }
    button.SetTimelineIndexAt68(record.timelineIndex);
    button.SetStateAt04(0);
    button.SetInputLatchAt20(false);
    return true;
}
}
