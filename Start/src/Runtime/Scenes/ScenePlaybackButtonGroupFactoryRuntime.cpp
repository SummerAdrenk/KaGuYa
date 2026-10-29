#include <Runtime/ScenePlaybackButtonGroupFactoryRuntime.h>
#include <Runtime/CgDisplayButtonGroupSample18GraphicsProvider.h>
#include <Runtime/CgDisplayButtonGroupSample18ButtonFactory.h>

#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
struct PageButtonArray final
{
    // 原 eh vector constructor iterator 先构造两项，数组析构逆序执行。
    std::array<std::shared_ptr<CgUiButtonRuntime>, 2> values;
    ~PageButtonArray() { values[1].reset(); values[0].reset(); }
};

struct ConstructionOwner final
{
    std::unique_ptr<ICgDisplayButtonGroupDirtyRects> dirty = CreateCgDisplayButtonGroupDirtyRectsAt4A2E60();
    std::vector<std::shared_ptr<RuntimeGraphicsSurface>> pages;
    std::vector<std::shared_ptr<CgUiButtonRuntime>> buttons;
    std::vector<std::shared_ptr<CgUiButtonRuntime>> pageButtons;
    CgDisplayPublicSelectorRuntime selector;
    CgDisplayPublicModeButtonsRuntime modes;

    ~ConstructionOwner()
    {
        selector.Release();
        pageButtons.clear();
        pages.clear();
        modes.Release();
        buttons.clear();
    }
};
}

std::unique_ptr<CgDisplayButtonGroupRuntime> CreateScenePlaybackButtonGroupAt49F270(
    const std::int32_t group, ScenePlaybackButtonGroupFactoryDependenciesAt49F270 d)
{
    ConstructionOwner owner;
    auto& graphics = d.graphicsProvider;
    auto& buttonFactory = d.buttonFactory;
    const auto& data = d.staticData;
    const auto load = [&](const CgDisplayButtonGroupResourceIdentifierRefAt499DC0& name) {
        auto surface = graphics.AllocateSurfaceAt40C2E0();
        if (!graphics.LoadSurfaceAt4800A0(*surface, name)) throw 1;
        return surface;
    };
    const auto loadAt = [&](const std::uintptr_t address) {
        return load(data.ReadResourcePointerAtAddress(address));
    };
    const auto attach = [&](CgUiButtonRuntime& button, const std::uint32_t slot,
                            const std::uintptr_t format, const std::int32_t index,
                            const std::shared_ptr<RuntimeGraphicsSurface>& surface) {
        const auto name = data.FormatResourceIdentifierAt417130(format, group, index);
        if (!buttonFactory.AttachSurfaceAt4C1F20(button, slot, name, surface)) throw 2;
    };

    const auto count = d.catalog.ItemCountFromRegistSceneAt110(group);
    const auto pages = count / 9 + (count != 9 * (count / 9) ? 1 : 0);
    std::int32_t variant = 0;
    if (static_cast<std::uint32_t>(group) < 4u)
    {
        variant = d.pageBaseVariantSource.ReadFromCsaveDataSoundParamsVslot84(group) == 0 ? 1 : 0;
        if (!data.ReadResourcePointerAtAddress(0x602398u + 4u * (2u * group + variant)))
            variant = 0;
    }
    for (std::int32_t page = 0; page < pages; ++page)
    {
        auto base = loadAt(0x602398u + 4u * (2u * group + variant));
        if (pages > 1)
        {
            // 原版在第四页底图加载成功之后才抛 1；不能提前截断到三页。
            if (page >= 3) throw 1;
            auto overlay = loadAt(0x6021DCu + 4u * page);
            graphics.BlitSurfaceAt4425C0(*base, *overlay, overlay->OriginX(), overlay->OriginY());
        }
        owner.pages.push_back(std::move(base));
    }
    const auto percent = d.catalog.CompletionPercentAt47C050(group);
    for (const auto& page : owner.pages)
        if (!graphics.DrawValueAt4BC950(percent, *page, 0x5B4238u, 0x5B4260u)) throw 2;

    auto clickedFrame = loadAt(0x602468u);
    auto targetFrame = loadAt(0x60246Cu);
    auto disabledFrame = loadAt(0x602470u);
    for (std::int32_t index = 0; index < 27; ++index)
    {
        if (index >= d.catalog.ItemCountFromRegistSceneAt110(group)) break;
        auto button = buttonFactory.AllocateButtonAt459530();
        owner.buttons.push_back(button);
        const auto thumbnail = d.catalog.ThumbnailAt478230(group, index);
        const auto offset = 8u * static_cast<std::uint32_t>(index % 9);
        const auto x = data.ReadDwordAtAddress(0x602410u + offset);
        const auto y = data.ReadDwordAtAddress(0x602414u + offset);
        if (thumbnail)
        {
            auto image = load(*thumbnail);
            auto normal = graphics.AllocateSurfaceAt40C2E0();
            if (!graphics.CreateSurfaceAt415010(*normal, clickedFrame->Width(), clickedFrame->Height(), 1, 1))
                throw 1;
            graphics.BlitSurfaceAt4425C0(*normal, *image,
                data.ReadDwordAtAddress(0x602460u), data.ReadDwordAtAddress(0x602464u));
            attach(*button, 0, 0x5B51CCu, index, normal);
            auto clicked = graphics.CopySurfaceAt444470(*normal, 1);
            // Copy 创建失败返回空引用；不解引用空对象，仍由 Attach 返回构造错误 2。
            if (clicked) graphics.BlitSurfaceAt4425C0(*clicked, *clickedFrame, 0, 0);
            attach(*button, 2, 0x5B51E8u, index, clicked);
            clicked.reset();
            auto target = graphics.CopySurfaceAt444470(*normal, 1);
            if (target) graphics.BlitSurfaceAt4425C0(*target, *targetFrame, 0, 0);
            attach(*button, 1, 0x5B5204u, index, target);
            target.reset();
            button->SetPositionAt24(POINT{
                x - data.ReadDwordAtAddress(0x602458u), y - data.ReadDwordAtAddress(0x60245Cu)});
            button->SetUsePositionAt30At2C(false);
            button->SetStateAt04(0);
            button->SetInputLatchAt20(false);
        }
        else
        {
            graphics.BlitSurfaceAt4425C0(*owner.pages.at(static_cast<std::size_t>(index / 9)),
                *disabledFrame, x, y);
        }
    }
    targetFrame.reset();
    clickedFrame.reset();
    disabledFrame.reset();

    if (owner.pages.size() >= 2u)
    {
        auto array = std::make_shared<PageButtonArray>();
        array->values[0] = buttonFactory.AllocateButtonAt459530();
        array->values[1] = buttonFactory.AllocateButtonAt459530();
        for (auto& button : array->values)
            owner.pageButtons.emplace_back(array, button.get());
        for (std::size_t index = 0; index < 2u; ++index)
        {
            auto& button = *owner.pageButtons[index];
            if (!buttonFactory.LoadButtonAt480930(button, 0x6023C0u + 40u * index, 0)) throw 8;
            button.SetPixelHitTestAt08(false);
            button.SetExplicitHitRectAt0C(RECT{});
        }
    }
    owner.selector = d.sharedControls.CaptureSelectorAt478A30();
    owner.modes = d.sharedControls.CaptureModesAt47B7B0();
    auto navigation = data.ReadNavigationTableAt602478();
    return std::make_unique<CgDisplayButtonGroupRuntime>(d.runtimeServices, graphics.CompositionApi(),
        std::move(owner.dirty), std::move(owner.pages), std::move(owner.buttons), std::move(navigation),
        std::move(owner.modes), 1, group, std::move(owner.pageButtons), std::move(owner.selector));
}

std::unique_ptr<CgDisplayButtonGroupRuntime> CreateScenePlaybackButtonGroupSample18Production(
    const std::int32_t group, ScenePlaybackButtonGroupSample18ProductionDependencies d)
{
    ScenePlaybackButtonGroupCatalogRuntime catalog(d.gameSystem, d.secretSc);
    CgDisplayProgramRegistryPageBaseVariantSourceAt499DC0 variant(d.slot1);
    CgDisplayButtonGroupSample18StaticDataAt499DC0 data;
    CgDisplayButtonGroupSample18GraphicsProviderAt499DC0 graphics(
        d.surfaceFactory, d.surfaceApi, d.compositionApi, d.copyApi);
    CgDisplayButtonGroupSample18ButtonFactoryAt499DC0 buttons(
        d.buttonServices, d.compositionApi, d.surfaceFactory);
    return CreateScenePlaybackButtonGroupAt49F270(group,
        {d.runtimeServices, catalog, variant, data, graphics, buttons, d.sharedControls});
}
}
