#include <Runtime/CgDisplayButtonGroupFactoryRuntime.h>

#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
constexpr std::int32_t kItemsPerPageAt70 = 9;
constexpr std::int32_t kMaximumConstructedItemsAt499DC0 = 27;

constexpr std::uintptr_t kPageBaseResourcePointerTableAt5B42C0 = 0x5B42C0u;
constexpr std::uintptr_t kPageOverlayResourcePointerTableAt602094 = 0x602094u;
constexpr std::uintptr_t kFixedResourcePointerAt6021D0 = 0x6021D0u;
constexpr std::uintptr_t kFixedResourcePointerAt6021D4 = 0x6021D4u;
constexpr std::uintptr_t kFixedResourcePointerAt6021D8 = 0x6021D8u;
constexpr std::uintptr_t kOptionalLayerResourcePointerAt5B4038 = 0x5B4038u;
constexpr std::uintptr_t kPageValueResourceTableAt5B3F80 = 0x5B3F80u;
constexpr std::uintptr_t kPageValueCoordinateTableAt5B3FA8 = 0x5B3FA8u;
constexpr std::uintptr_t kItemValueResourceTableAt5B4010 = 0x5B4010u;
constexpr std::uintptr_t kItemPositionXTableAt5B42E8 = 0x5B42E8u;
constexpr std::uintptr_t kItemPositionYTableAt5B42EC = 0x5B42ECu;
constexpr std::uintptr_t kItemPositionBaseXAt6021C0 = 0x6021C0u;
constexpr std::uintptr_t kItemPositionBaseYAt6021C4 = 0x6021C4u;
constexpr std::uintptr_t kLoadedItemBlitXAt6021C8 = 0x6021C8u;
constexpr std::uintptr_t kLoadedItemBlitYAt6021CC = 0x6021CCu;
constexpr std::uintptr_t kState0FormatAt5B51CC = 0x5B51CCu;
constexpr std::uintptr_t kState2FormatAt5B51E8 = 0x5B51E8u;
constexpr std::uintptr_t kState1FormatAt5B5204 = 0x5B5204u;
constexpr std::uintptr_t kFirstValueOneDigitXAt5B40D0 = 0x5B40D0u;
constexpr std::uintptr_t kFirstValueTwoDigitXAt5B4040 = 0x5B4040u;
constexpr std::uintptr_t kFirstValueThreeDigitXAt5B4118 = 0x5B4118u;
constexpr std::uintptr_t kSecondValueOneDigitXAt5B40D4 = 0x5B40D4u;
constexpr std::uintptr_t kSecondValueTwoDigitXAt5B4048 = 0x5B4048u;
constexpr std::uintptr_t kSecondValueThreeDigitXAt5B4124 = 0x5B4124u;
constexpr std::uintptr_t kFirstValueYTableAt5B4330 = 0x5B4330u;
constexpr std::uintptr_t kSecondValueYTableAt5B4334 = 0x5B4334u;
constexpr std::uintptr_t kOptionalLayerXTableAt5B41F0 = 0x5B41F0u;
constexpr std::uintptr_t kOptionalLayerYTableAt5B41F4 = 0x5B41F4u;
constexpr std::uintptr_t kPageButtonRecordTableAt5B3FC0 = 0x5B3FC0u;

[[noreturn]] void ThrowConstructionFailure(const std::int32_t value)
{
    // 原构造器抛出的对象类型就是 int；调用边界必须能够按相同类型捕获。
    throw value;
}

[[nodiscard]] std::int32_t DivideRoundAwayFromZeroAsAt499DC0(
    const std::int32_t value,
    const std::int32_t divisor)
{
    const std::int32_t quotient = value / divisor;
    return quotient + (value != divisor * quotient ? 1 : 0);
}

[[nodiscard]] std::shared_ptr<RuntimeGraphicsSurface> AllocateSurface(
    ICgDisplayButtonGroupGraphicsProviderAt499DC0& graphics)
{
    // operator new 的分配失败仍由 C++ 分配异常表达；不制造空 Surface。
    return graphics.AllocateSurfaceAt40C2E0();
}

[[nodiscard]] std::shared_ptr<RuntimeGraphicsSurface> LoadRequiredSurface(
    ICgDisplayButtonGroupGraphicsProviderAt499DC0& graphics,
    const CgDisplayButtonGroupResourceIdentifierRefAt499DC0& resource)
{
    auto surface = AllocateSurface(graphics);
    if (!graphics.LoadSurfaceAt4800A0(*surface, resource))
        ThrowConstructionFailure(1);
    return surface;
}

void RequireAttachedSurface(
    ICgDisplayButtonGroupButtonFactoryAt499DC0& buttonFactory,
    CgUiButtonRuntime& button,
    const std::uint32_t stateSurfaceSlot,
    const CgDisplayButtonGroupResourceIdentifierRefAt499DC0& identifier,
    const std::shared_ptr<RuntimeGraphicsSurface>& surface)
{
    if (!buttonFactory.AttachSurfaceAt4C1F20(
            button, stateSurfaceSlot, identifier, surface))
    {
        ThrowConstructionFailure(2);
    }
}

void DrawFirstItemValue(
    ICgDisplayButtonGroupStaticDataAt499DC0& staticData,
    ICgDisplayButtonGroupGraphicsProviderAt499DC0& graphics,
    RuntimeGraphicsSurface& destination,
    const std::int32_t value,
    const std::int32_t slotWithinPage,
    const std::uintptr_t slotDwordOffset)
{
    std::uintptr_t xAddress = 0;
    std::int32_t failure = 0;
    if (value < 10)
    {
        xAddress = kFirstValueOneDigitXAt5B40D0 + slotDwordOffset;
        failure = 4;
    }
    else if (value < 100)
    {
        xAddress = kFirstValueTwoDigitXAt5B4040 +
            static_cast<std::uintptr_t>(slotWithinPage) * 16u;
        failure = 5;
    }
    else
    {
        xAddress = kFirstValueThreeDigitXAt5B4118 +
            static_cast<std::uintptr_t>(slotWithinPage) * 24u;
        failure = 9;
    }

    const auto y = staticData.ReadDwordAtAddress(
        kFirstValueYTableAt5B4330 + slotDwordOffset);
    if (!graphics.DrawValueAt4B4D20(
            value, destination, kItemValueResourceTableAt5B4010, xAddress, y))
    {
        ThrowConstructionFailure(failure);
    }
}

void DrawSecondItemValue(
    ICgDisplayButtonGroupStaticDataAt499DC0& staticData,
    ICgDisplayButtonGroupGraphicsProviderAt499DC0& graphics,
    RuntimeGraphicsSurface& destination,
    const std::int32_t value,
    const std::int32_t slotWithinPage,
    const std::uintptr_t slotDwordOffset)
{
    std::uintptr_t xAddress = 0;
    std::int32_t failure = 0;
    if (value < 10)
    {
        xAddress = kSecondValueOneDigitXAt5B40D4 + slotDwordOffset;
        failure = 6;
    }
    else if (value < 100)
    {
        xAddress = kSecondValueTwoDigitXAt5B4048 +
            static_cast<std::uintptr_t>(slotWithinPage) * 16u;
        failure = 7;
    }
    else
    {
        xAddress = kSecondValueThreeDigitXAt5B4124 +
            static_cast<std::uintptr_t>(slotWithinPage) * 24u;
        failure = 10;
    }

    const auto y = staticData.ReadDwordAtAddress(
        kSecondValueYTableAt5B4334 + slotDwordOffset);
    if (!graphics.DrawValueAt4B4D20(
            value, destination, kItemValueResourceTableAt5B4010, xAddress, y))
    {
        ThrowConstructionFailure(failure);
    }
}
}

std::unique_ptr<CgDisplayButtonGroupRuntime>
CreateCgDisplayButtonGroupAt499DC0(
    const std::int32_t categoryIndex,
    CgDisplayButtonGroupFactoryDependenciesAt499DC0 dependencies)
{
    auto& categoryContainer = dependencies.categoryContainer;
    auto& categoryVslot24 = dependencies.categoryVslot24;
    auto& globalVslot1C = dependencies.globalVslot1C;
    auto& staticData = dependencies.staticData;
    auto& graphics = dependencies.graphicsProvider;
    auto& buttonFactory = dependencies.buttonFactory;

    const std::int32_t sourceItemCount =
        categoryContainer.ItemCountFromOffsets18And1C(categoryIndex);
    const std::int32_t pageCount = DivideRoundAwayFromZeroAsAt499DC0(
        sourceItemCount, kItemsPerPageAt70);

    std::int32_t pageBaseVariant = 0;
    if (static_cast<std::uint32_t>(categoryIndex) < 4u)
    {
        pageBaseVariant =
            categoryContainer.ReadByteFromRuntimeSlot1Element0Vslot84(
                categoryIndex) == 0u
            ? 1
            : 0;
        const auto candidate = staticData.ReadResourcePointerAtAddress(
            kPageBaseResourcePointerTableAt5B42C0 +
            static_cast<std::uintptr_t>(2 * categoryIndex + pageBaseVariant) *
                sizeof(std::uint32_t));
        // 499F53..499F60 明确将空的 alternate 槽恢复为索引 0。
        if (!candidate)
            pageBaseVariant = 0;
    }

    std::vector<std::shared_ptr<RuntimeGraphicsSurface>> pageSurfaces;
    for (std::int32_t pageIndex = 0; pageIndex < pageCount; ++pageIndex)
    {
        const auto pageBaseResource = staticData.ReadResourcePointerAtAddress(
            kPageBaseResourcePointerTableAt5B42C0 +
            static_cast<std::uintptr_t>(2 * categoryIndex + pageBaseVariant) *
                sizeof(std::uint32_t));
        auto pageSurface = LoadRequiredSurface(graphics, pageBaseResource);

        if (pageCount > 1)
        {
            // 原表只有三项；第四页在加载 base 后立即抛出 int 1。
            if (static_cast<std::uint32_t>(pageIndex) >= 3u)
                ThrowConstructionFailure(1);
            const auto overlayResource = staticData.ReadResourcePointerAtAddress(
                kPageOverlayResourcePointerTableAt602094 +
                static_cast<std::uintptr_t>(pageIndex) * sizeof(std::uint32_t));
            const auto overlay = LoadRequiredSurface(graphics, overlayResource);
            graphics.BlitSurfaceAt4425C0(
                *pageSurface, *overlay, overlay->OriginX(), overlay->OriginY());
        }
        pageSurfaces.push_back(std::move(pageSurface));
    }

    const std::int32_t pageValue = categoryVslot24.ReadAtVslot24(categoryIndex);
    for (const auto& pageSurface : pageSurfaces)
    {
        if (!graphics.DrawValueAt4BC950(
                pageValue,
                *pageSurface,
                kPageValueResourceTableAt5B3F80,
                kPageValueCoordinateTableAt5B3FA8))
        {
            ThrowConstructionFailure(2);
        }
    }

    // 49A3C8..49A561：四个固定资源的加载顺序不可交换。
    const auto fixedAt6021D0 = LoadRequiredSurface(
        graphics, staticData.ReadResourcePointerAtAddress(
            kFixedResourcePointerAt6021D0));
    const auto fixedAt6021D4 = LoadRequiredSurface(
        graphics, staticData.ReadResourcePointerAtAddress(
            kFixedResourcePointerAt6021D4));
    const auto fixedAt6021D8 = LoadRequiredSurface(
        graphics, staticData.ReadResourcePointerAtAddress(
            kFixedResourcePointerAt6021D8));
    const auto optionalLayerAt5B4038 = LoadRequiredSurface(
        graphics, staticData.ReadResourcePointerAtAddress(
            kOptionalLayerResourcePointerAt5B4038));

    const std::int32_t stateSurfaceWidth = fixedAt6021D0->Width();
    const std::int32_t stateSurfaceHeight = fixedAt6021D0->Height();
    std::vector<std::shared_ptr<CgUiButtonRuntime>> mainButtons;

    for (std::int32_t itemIndex = 0;
         itemIndex < kMaximumConstructedItemsAt499DC0;
         ++itemIndex)
    {
        const auto item = categoryContainer.TryReadItemFromOffsets18And1C(
            categoryIndex, itemIndex);
        if (!item)
            break;

        auto button = buttonFactory.AllocateButtonAt459530();
        mainButtons.push_back(button);

        const auto runtimeResource = globalVslot1C.ReadAtGlobalVslot1C(
            categoryIndex, itemIndex);
        const std::int32_t pageIndex = itemIndex / kItemsPerPageAt70;
        const std::int32_t slotWithinPage = itemIndex % kItemsPerPageAt70;
        const std::uintptr_t slotDwordOffset =
            static_cast<std::uintptr_t>(slotWithinPage) * 2u *
            sizeof(std::uint32_t);
        const std::int32_t positionX = staticData.ReadDwordAtAddress(
            kItemPositionXTableAt5B42E8 + slotDwordOffset);
        const std::int32_t positionY = staticData.ReadDwordAtAddress(
            kItemPositionYTableAt5B42EC + slotDwordOffset);

        if (runtimeResource)
        {
            auto loadedItem = LoadRequiredSurface(graphics, *runtimeResource);
            auto stateSurface = AllocateSurface(graphics);
            if (!graphics.CreateSurfaceAt415010(
                    *stateSurface,
                    stateSurfaceWidth,
                    stateSurfaceHeight,
                    1,
                    1))
            {
                ThrowConstructionFailure(1);
            }
            graphics.BlitSurfaceAt4425C0(
                *stateSurface,
                *loadedItem,
                staticData.ReadDwordAtAddress(kLoadedItemBlitXAt6021C8),
                staticData.ReadDwordAtAddress(kLoadedItemBlitYAt6021CC));

            RequireAttachedSurface(
                buttonFactory,
                *button,
                0u,
                staticData.FormatResourceIdentifierAt417130(
                    kState0FormatAt5B51CC, categoryIndex, itemIndex),
                stateSurface);

            auto state2Surface = graphics.CopySurfaceAt444470(*stateSurface, 1);
            // 0x444470 在 SurfaceCreate 失败时返回空引用。原代码随后把空对象
            // 送入绘制/附着链；重写版不能解引用空 shared_ptr，因此保留绘制的
            // 成功路径，并让下方 0x4C1F20 边界按原构造错误 2 明确失败。
            if (state2Surface)
            {
                graphics.BlitSurfaceAt4425C0(
                    *state2Surface, *fixedAt6021D0, 0, 0);
            }
            RequireAttachedSurface(
                buttonFactory,
                *button,
                2u,
                staticData.FormatResourceIdentifierAt417130(
                    kState2FormatAt5B51E8, categoryIndex, itemIndex),
                state2Surface);

            auto state1Surface = graphics.CopySurfaceAt444470(*stateSurface, 1);
            if (state1Surface)
            {
                graphics.BlitSurfaceAt4425C0(
                    *state1Surface, *fixedAt6021D4, 0, 0);
            }
            RequireAttachedSurface(
                buttonFactory,
                *button,
                1u,
                staticData.FormatResourceIdentifierAt417130(
                    kState1FormatAt5B5204, categoryIndex, itemIndex),
                state1Surface);

            button->SetPositionAt24(POINT{
                positionX - staticData.ReadDwordAtAddress(
                    kItemPositionBaseXAt6021C0),
                positionY - staticData.ReadDwordAtAddress(
                    kItemPositionBaseYAt6021C4)});
            button->SetUsePositionAt30At2C(false);
            button->SetStateAt04(0);
            button->SetInputLatchAt20(false);
        }
        else
        {
            // 49AE6F..49AED4：无 vslot+0x1C 资源时，只向所属分页画布写固定层。
            graphics.BlitSurfaceAt4425C0(
                *pageSurfaces[static_cast<std::size_t>(pageIndex)],
                *fixedAt6021D8,
                positionX,
                positionY);
        }

        auto& pageSurface =
            *pageSurfaces[static_cast<std::size_t>(pageIndex)];
        DrawFirstItemValue(
            staticData,
            graphics,
            pageSurface,
            item->referencedGlobalEntryByteAtPlus04NonzeroCount,
            slotWithinPage,
            slotDwordOffset);
        DrawSecondItemValue(
            staticData,
            graphics,
            pageSurface,
            item->refObjectCountAtPlus10,
            slotWithinPage,
            slotDwordOffset);

        // 0x49B158 检查的是成功装入后的 ref_object 首指针，不是 Surface handle。
        // LoadRequiredSurface 已保证该引用存在，因此这里每项都必须提交这一层。
        graphics.BlitSurfaceAt4425C0(
            pageSurface,
            *optionalLayerAt5B4038,
            staticData.ReadDwordAtAddress(
                kOptionalLayerXTableAt5B41F0 + slotDwordOffset),
            staticData.ReadDwordAtAddress(
                kOptionalLayerYTableAt5B41F4 + slotDwordOffset));
    }

    std::vector<std::shared_ptr<CgUiButtonRuntime>> pageButtons;
    if (pageSurfaces.size() >= 2u)
    {
        for (std::size_t index = 0; index < 2u; ++index)
        {
            auto button = buttonFactory.AllocateButtonAt459530();
            if (!buttonFactory.LoadButtonAt480930(
                    *button,
                    kPageButtonRecordTableAt5B3FC0 + index * 40u,
                    0))
            {
                ThrowConstructionFailure(8);
            }
            button->SetPixelHitTestAt08(false);
            button->SetExplicitHitRectAt0C(RECT{});
            pageButtons.push_back(std::move(button));
        }
    }

    // 0x49B2DD..0x49B32C 位于全部资源和翻页按钮构造之后；先取得
    // selector，再取得 mode-button 集，不能由调用方提前求值。
    auto selector = dependencies.sharedControls.CaptureSelectorAt478A30();
    auto modeButtons =
        dependencies.sharedControls.CaptureModesAt47B7B0();
    auto navigation = staticData.ReadNavigationTableAt6021E8();
    auto& composition = graphics.CompositionApi();
    return std::make_unique<CgDisplayButtonGroupRuntime>(
        dependencies.runtimeServices,
        composition,
        std::move(pageSurfaces),
        std::move(mainButtons),
        std::move(navigation),
        std::move(modeButtons),
        0,
        categoryIndex,
        std::move(pageButtons),
        std::move(selector));
}
}
