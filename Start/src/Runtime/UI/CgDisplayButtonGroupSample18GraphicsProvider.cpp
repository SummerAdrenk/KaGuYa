#include <Runtime/CgDisplayButtonGroupSample18GraphicsProvider.h>
#include <Runtime/CgDisplayButtonGroupSample18ButtonFactory.h>
#include <Runtime/CgDisplayButtonGroupSample18StaticData.h>

#include <array>
#include <stdexcept>
#include <string>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
constexpr std::uintptr_t kPageDigitResourceTableAt5B3F80 = 0x5B3F80u;
constexpr std::uintptr_t kPageDigitCoordinatesAt5B3FA8 = 0x5B3FA8u;
constexpr std::uintptr_t kItemDigitResourceTableAt5B4010 = 0x5B4010u;

constexpr char kPageDigitPrefixCp932[] =
    "vrm:\\\x8A\xD3\x8F\xDC\x89\xE6\x96\xCA"
    "\x89\xF1\x8E\xFB\x97\xA6\x90\x94\x92\x6C";
constexpr char kItemDigitPrefixCp932[] =
    "vrm:\\\x8A\xD3\x8F\xDC\x89\xE6\x96\xCA"
    "\x8D\xB7\x95\xAA\x95\x5C\x8E\xA6\x90\x94";

constexpr std::array<std::int32_t, 6> kPageDigitCoordinates{
    216, 690, 198, 690, 187, 690};

struct ItemCoordinateSequence final
{
    std::array<std::int32_t, 3> values{};
    std::size_t count = 0;
};

[[nodiscard]] ItemCoordinateSequence ResolveItemCoordinates(
    const std::uintptr_t address)
{
    for (std::size_t slot = 0; slot < 9u; ++slot)
    {
        const std::int32_t columnOffset =
            static_cast<std::int32_t>(slot % 3u) * 237;
        const auto slotAddress8 = static_cast<std::uintptr_t>(slot) * 8u;
        if (address == 0x5B40D0u + slotAddress8)
            return {{249 + columnOffset, 0, 0}, 1u};
        if (address == 0x5B40D4u + slotAddress8)
            return {{286 + columnOffset, 0, 0}, 1u};

        const auto slotAddress16 = static_cast<std::uintptr_t>(slot) * 16u;
        if (address == 0x5B4040u + slotAddress16)
            return {{255 + columnOffset, 242 + columnOffset, 0}, 2u};
        if (address == 0x5B4048u + slotAddress16)
            return {{293 + columnOffset, 280 + columnOffset, 0}, 2u};

        const auto slotAddress24 = static_cast<std::uintptr_t>(slot) * 24u;
        if (address == 0x5B4118u + slotAddress24)
            return {{255 + columnOffset, 242 + columnOffset,
                     229 + columnOffset}, 3u};
        if (address == 0x5B4124u + slotAddress24)
            return {{306 + columnOffset, 293 + columnOffset,
                     280 + columnOffset}, 3u};
    }
    throw std::out_of_range(
        "sample 18 CG display item digit coordinate address is unknown");
}

[[nodiscard]] const CgDisplayStringResourceIdentifierAt499DC0&
RequireStringIdentifier(
    const CgDisplayButtonGroupResourceIdentifierRefAt499DC0& resource)
{
    if (!resource)
        throw std::invalid_argument("CG display resource identifier is null");
    const auto* const stringIdentifier =
        dynamic_cast<const CgDisplayStringResourceIdentifierAt499DC0*>(
            resource.get());
    if (stringIdentifier == nullptr)
    {
        throw std::invalid_argument(
            "CG display resource identifier is not a CP932 string identifier");
    }
    return *stringIdentifier;
}
}

CgDisplayButtonGroupSample18GraphicsProviderAt499DC0::
    CgDisplayButtonGroupSample18GraphicsProviderAt499DC0(
        ICgOperationSurfaceFactory& surfaceFactory,
        IGraphicsSurfaceApi& surfaceApi,
        IGraphicsSurfaceCompositionApi& compositionApi,
        IGraphicsSurfaceCopyApi& copyApi) noexcept
    : surfaceFactory_(&surfaceFactory),
      surfaceApi_(&surfaceApi),
      compositionApi_(&compositionApi),
      copyApi_(&copyApi)
{
}

IGraphicsSurfaceCompositionApi&
CgDisplayButtonGroupSample18GraphicsProviderAt499DC0::CompositionApi()
{
    return *compositionApi_;
}

std::shared_ptr<RuntimeGraphicsSurface>
CgDisplayButtonGroupSample18GraphicsProviderAt499DC0::
    AllocateSurfaceAt40C2E0()
{
    return std::make_shared<RuntimeGraphicsSurface>(*surfaceApi_);
}

bool CgDisplayButtonGroupSample18GraphicsProviderAt499DC0::
    LoadSurfaceAt4800A0(
        RuntimeGraphicsSurface& destination,
        const CgDisplayButtonGroupResourceIdentifierRefAt499DC0& resource)
{
    const auto& identifier = RequireStringIdentifier(resource);
    auto loaded = surfaceFactory_->LoadStatic(
        std::string(identifier.Cp932Name()));
    if (!loaded)
        return false;
    destination = std::move(*loaded);
    return true;
}

bool CgDisplayButtonGroupSample18GraphicsProviderAt499DC0::
    CreateSurfaceAt415010(
        RuntimeGraphicsSurface& destination,
        const std::int32_t width,
        const std::int32_t height,
        const std::int32_t alphaArgument,
        const std::int32_t clearArgument)
{
    return destination.Create(
        width, height, alphaArgument != 0, clearArgument != 0);
}

std::shared_ptr<RuntimeGraphicsSurface>
CgDisplayButtonGroupSample18GraphicsProviderAt499DC0::CopySurfaceAt444470(
    const RuntimeGraphicsSurface& source,
    const std::int32_t copyPixelsArgument)
{
    auto destination = AllocateSurfaceAt40C2E0();
    if (!destination->Create(
            source.Width(), source.Height(), source.HasAlpha(), true))
    {
        return {};
    }

    if (copyPixelsArgument != 0)
    {
        // 0x444680..0x44469E 忽略 SurfaceCopyBlt 返回值，随后复制 AP 原点。
        static_cast<void>(copyApi_->CopyBlitSurface(
            destination->Handle(), source.Handle(), 0, 0,
            nullptr, nullptr, nullptr, 0));
        destination->SetOrigin(source.OriginX(), source.OriginY());
    }
    return destination;
}

void CgDisplayButtonGroupSample18GraphicsProviderAt499DC0::
    BlitSurfaceAt4425C0(
        RuntimeGraphicsSurface& destination,
        const RuntimeGraphicsSurface& source,
        const std::int32_t destinationX,
        const std::int32_t destinationY)
{
    // 0x4425C0 的返回值在 0x499DC0、0x4BC950、0x4B4D20 中均不参与分支。
    static_cast<void>(compositionApi_->BlitSurface(
        destination.Handle(), source.Handle(), destinationX, destinationY,
        nullptr, nullptr, nullptr, 0));
}

bool CgDisplayButtonGroupSample18GraphicsProviderAt499DC0::DrawDigit(
    RuntimeGraphicsSurface& destination,
    const char* const resourcePrefixCp932,
    const std::int32_t digit,
    const std::int32_t x,
    const std::int32_t y)
{
    if (digit < 0 || digit > 9)
        throw std::out_of_range("CG display digit is outside 0..9");
    std::string resourceName(resourcePrefixCp932);
    resourceName.push_back(static_cast<char>('0' + digit));
    resourceName += ".alp";
    auto digitSurface = surfaceFactory_->LoadStatic(resourceName);
    if (!digitSurface)
        return false;
    BlitSurfaceAt4425C0(destination, *digitSurface, x, y);
    return true;
}

bool CgDisplayButtonGroupSample18GraphicsProviderAt499DC0::
    DrawValueAt4BC950(
        const std::int32_t value,
        RuntimeGraphicsSurface& destination,
        const std::uintptr_t resourcePointerTableAddress,
        const std::uintptr_t coordinateTableAddress)
{
    // Replay 的 5B4238/5B4260 与 CG 的资源表/坐标表分别逐字节一致。
    if (!((resourcePointerTableAddress == kPageDigitResourceTableAt5B3F80 &&
           coordinateTableAddress == kPageDigitCoordinatesAt5B3FA8) ||
          (resourcePointerTableAddress == 0x5B4238u && coordinateTableAddress == 0x5B4260u)))
    {
        throw std::out_of_range(
            "sample 18 CG display page digit table address is unknown");
    }
    if (value < 0)
        throw std::out_of_range("CG display page value is negative");

    try
    {
        const std::int32_t tensAndHundreds = value / 10;
        if (!DrawDigit(destination, kPageDigitPrefixCp932, value % 10,
                kPageDigitCoordinates[0], kPageDigitCoordinates[1]))
            throw 1;
        if (tensAndHundreds != 0 &&
            !DrawDigit(destination, kPageDigitPrefixCp932,
                tensAndHundreds % 10,
                kPageDigitCoordinates[2], kPageDigitCoordinates[3]))
            throw 1;
        if (tensAndHundreds / 10 != 0 &&
            !DrawDigit(destination, kPageDigitPrefixCp932, 1,
                kPageDigitCoordinates[4], kPageDigitCoordinates[5]))
            throw 1;
        return true;
    }
    catch (const int&)
    {
        return false;
    }
}

bool CgDisplayButtonGroupSample18GraphicsProviderAt499DC0::
    DrawValueAt4B4D20(
        const std::int32_t value,
        RuntimeGraphicsSurface& destination,
        const std::uintptr_t resourcePointerTableAddress,
        const std::uintptr_t xCoordinateDwordArrayAddress,
        const std::int32_t y)
{
    if (resourcePointerTableAddress != kItemDigitResourceTableAt5B4010)
    {
        throw std::out_of_range(
            "sample 18 CG display item digit resource table address is unknown");
    }
    if (value < 0)
        throw std::out_of_range("CG display item value is negative");

    const ItemCoordinateSequence coordinates =
        ResolveItemCoordinates(xCoordinateDwordArrayAddress);
    const std::int32_t tensAndHundreds = value / 10;
    const std::size_t required = tensAndHundreds == 0
        ? 1u
        : (tensAndHundreds / 10 == 0 ? 2u : 3u);
    if (coordinates.count < required)
    {
        throw std::invalid_argument(
            "CG display item coordinate table is shorter than the requested value");
    }

    try
    {
        if (!DrawDigit(destination, kItemDigitPrefixCp932, value % 10,
                coordinates.values[0], y))
            throw 1;
        if (tensAndHundreds != 0 &&
            !DrawDigit(destination, kItemDigitPrefixCp932,
                tensAndHundreds % 10, coordinates.values[1], y))
            throw 1;
        if (tensAndHundreds / 10 != 0 &&
            !DrawDigit(destination, kItemDigitPrefixCp932,
                (tensAndHundreds / 10) % 10, coordinates.values[2], y))
            throw 1;
        return true;
    }
    catch (const int&)
    {
        return false;
    }
}

std::unique_ptr<CgDisplayButtonGroupRuntime>
CreateCgDisplayButtonGroupSample18ProductionAt499DC0(
    const std::int32_t categoryIndex,
    CgDisplayButtonGroupSample18ProductionDependenciesAt499DC0 dependencies)
{
    CgDisplayButtonGroupSample18StaticDataAt499DC0 staticData;
    CgDisplayButtonGroupSample18GraphicsProviderAt499DC0 graphicsProvider(
        dependencies.surfaceFactory,
        dependencies.surfaceApi,
        dependencies.compositionApi,
        dependencies.copyApi);
    CgDisplayButtonGroupSample18ButtonFactoryAt499DC0 buttonFactory(
        dependencies.buttonServices,
        dependencies.compositionApi,
        dependencies.surfaceFactory);
    return CreateCgDisplayButtonGroupProductionAt499DC0(
        categoryIndex,
        CgDisplayButtonGroupProductionDependenciesAt499DC0{
            dependencies.secretCg,
            dependencies.thumbnailParams,
            dependencies.slot1,
            dependencies.runtimeServices,
            staticData,
            graphicsProvider,
            buttonFactory,
            dependencies.sharedControls});
}
}
