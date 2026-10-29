#include <Runtime/CgOperationSurfaceResourceRuntime.h>
#include <Runtime/ArchiveReaderFactoryRuntime.h>

#include <algorithm>
#include <optional>
#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
[[nodiscard]] std::string Cp932(const char* const value)
{
    return std::string(value);
}

[[nodiscard]] RECT SurfaceRect(const RuntimeGraphicsSurface& surface) noexcept
{
    return RECT{
        surface.OriginX(),
        surface.OriginY(),
        surface.OriginX() + surface.Width(),
        surface.OriginY() + surface.Height()};
}

class CgOperationArchiveSurfaceBatch final : public ICgOperationSurfaceBatch
{
public:
    CgOperationArchiveSurfaceBatch(const ResourceCatalog& catalog,
        const FileTransactionContext* transaction, IStartFileApi& files,
        const IEnvironmentKeyTableSource& key, IGraphicsSurfaceApi& graphics,
        ArchiveCodecServices codecs) noexcept
        : factory_(catalog, transaction, files, memory_, codecs), key_(key), graphics_(graphics)
    {
    }

    std::shared_ptr<RuntimeGraphicsSurface> LoadStatic(const std::string& name) override
    {
        if (!factory_.Load(name, key_))
            return {};

        auto surface = std::make_shared<RuntimeGraphicsSurface>(graphics_);
        try
        {
            if (!surface->LoadArchiveReaderAt419D70(factory_.Reader(), 0u))
                return {};
        }
        catch (const StaticSurfaceResourceException&)
        {
            // 0x4800A0 只折叠 reader/图片解码失败；分配等其他异常继续传播。
            return {};
        }
        return surface;
    }

private:
    // 4809A9..480A97：同一批次保留工厂槽；每次 Load 换 reader，最后先销毁工厂。
    ArchiveReaderMemoryApi memory_;
    ArchiveReaderFactoryRuntime factory_;
    const IEnvironmentKeyTableSource& key_;
    IGraphicsSurfaceApi& graphics_;
};
}

CgOperationControlResourceGroupRuntime::
CgOperationControlResourceGroupRuntime(
    ICgOperationSurfaceFactory& factory,
    const CgOperationControlResourceDescriptor& descriptor)
{
    slotIndices_.fill(-1);
    std::vector<std::string> uniqueNames;
    uniqueNames.reserve(7u);
    uniqueResources_.reserve(7u);

    for (std::size_t slot = 0; slot < descriptor.resourceNamesAt04.size(); ++slot)
    {
        const std::string& name = descriptor.resourceNamesAt04[slot];
        if (name.empty())
            continue;

        const auto existing = std::ranges::find(uniqueNames, name);
        if (existing != uniqueNames.end())
        {
            slotIndices_[slot] = static_cast<std::int32_t>(
                std::distance(uniqueNames.begin(), existing));
            continue;
        }

        auto surface = factory.LoadStatic(name);
        if (!surface)
            throw 1;
        const RECT rect = SurfaceRect(*surface);
        RECT combined{};
        ::UnionRect(&combined, &combinedRect_, &rect);
        combinedRect_ = combined;
        slotIndices_[slot] = static_cast<std::int32_t>(uniqueResources_.size());
        uniqueNames.push_back(name);
        uniqueResources_.push_back(std::move(surface));
    }

    if (descriptor.absoluteOriginAt20)
    {
        const std::int32_t deltaX =
            descriptor.absoluteOriginAt20->x - combinedRect_.left;
        const std::int32_t deltaY =
            descriptor.absoluteOriginAt20->y - combinedRect_.top;
        for (const auto& surface : uniqueResources_)
        {
            surface->SetOrigin(
                surface->OriginX() + deltaX,
                surface->OriginY() + deltaY);
        }
        combinedRect_.right += deltaX;
        combinedRect_.bottom += deltaY;
        combinedRect_.left = descriptor.absoluteOriginAt20->x;
        combinedRect_.top = descriptor.absoluteOriginAt20->y;
    }
}

CgOperationControlResourceGroupRuntime::
CgOperationControlResourceGroupRuntime(
    std::shared_ptr<RuntimeGraphicsSurface> surface,
    const std::size_t slot)
{
    slotIndices_.fill(-1);
    if (!surface)
        throw std::invalid_argument("CGOperation existing control surface is null");
    if (slot >= slotIndices_.size())
        throw std::out_of_range("CGOperation existing control surface slot is invalid");
    slotIndices_[slot] = 0;
    combinedRect_ = SurfaceRect(*surface);
    uniqueResources_.push_back(std::move(surface));
}

const std::shared_ptr<RuntimeGraphicsSurface>&
CgOperationControlResourceGroupRuntime::ResourceForSlot(
    const std::size_t slot) const
{
    const std::int32_t index = ResourceIndexForSlot(slot);
    if (index < 0)
        throw std::out_of_range("CGOperation control resource slot is empty");
    return uniqueResources_[static_cast<std::size_t>(index)];
}

std::int32_t CgOperationControlResourceGroupRuntime::ResourceIndexForSlot(
    const std::size_t slot) const
{
    if (slot >= slotIndices_.size())
        throw std::out_of_range("CGOperation control resource slot is invalid");
    return slotIndices_[slot];
}

const RECT&
CgOperationControlResourceGroupRuntime::CombinedRect() const noexcept
{
    return combinedRect_;
}

std::size_t
CgOperationControlResourceGroupRuntime::UniqueResourceCount() const noexcept
{
    return uniqueResources_.size();
}

std::shared_ptr<CgOperationSliderRuntime>
CreateCgOperationSliderAt5052C0(
    ICgOperationSurfaceFactory& factory,
    ICgOperationSliderInputServices& inputServices,
    const CgOperationSliderDescriptor& descriptor)
{
    auto thumbDescriptor = descriptor.resourcesAt04And20[0];
    auto trackDescriptor = descriptor.resourcesAt04And20[1];
    if (descriptor.sharedOriginAt3C)
    {
        thumbDescriptor.absoluteOriginAt20 = descriptor.sharedOriginAt3C;
        trackDescriptor.absoluteOriginAt20 = descriptor.sharedOriginAt3C;
    }

    auto thumbResources =
        std::make_shared<CgOperationControlResourceGroupRuntime>(
            factory, thumbDescriptor);
    auto trackResources =
        std::make_shared<CgOperationControlResourceGroupRuntime>(
            factory, trackDescriptor);
    return std::make_shared<CgOperationSliderRuntime>(
        inputServices,
        descriptor.eventValueAt00,
        -1,
        std::move(thumbResources),
        std::move(trackResources),
        descriptor.orientationAt44,
        descriptor.reverseAt48,
        descriptor.rectAt4C,
        descriptor.offsetRectAt5C,
        descriptor.hitTestResourceSlotAt40);
}

const CgOperationChildDescriptor&
OpacityDialogStaticDescriptorSample18At606FE4()
{
    static const CgOperationChildDescriptor descriptor = [] {
        const std::string background = Cp932(
            "\x83\x52\x83\x7D\x83\x93\x83\x68\x83\x76\x83\x8C\x83\x43"
            "\x93\xA7\x96\xBE\x93\x78\x2E\x76\x72\x6D");
        return CgOperationChildDescriptor{
            CgOperationControlKind::Static,
            0,
            {{{background, {}, {}, background, background, {}, {}}},
             std::nullopt},
            7,
            -1};
    }();
    return descriptor;
}

const CgOperationSliderDescriptor&
OpacityDialogSliderDescriptorSample18At607008()
{
    static const CgOperationSliderDescriptor descriptor = [] {
        const std::string cursorOn = Cp932(
            "\x8B\xA4\x92\xCA\x83\x4A\x81\x5B\x83\x5C\x83\x8B\x83\x49"
            "\x83\x93\x2E\x76\x72\x6D");
        const std::string cursorOff = Cp932(
            "\x8B\xA4\x92\xCA\x83\x4A\x81\x5B\x83\x5C\x83\x8B\x83\x49"
            "\x83\x74\x2E\x76\x72\x6D");
        CgOperationSliderDescriptor result;
        result.eventValueAt00 = 1;
        result.resourcesAt04And20[0].resourceNamesAt04 = {
            cursorOn, cursorOff, {}, cursorOn, cursorOn, cursorOn, {}};
        // 0x607028..0x607040 是完整的七个空指针。原 5047A0 仍创建空组，
        // Slider 用它表示没有独立轨道图层，不能把它判为资源错误。
        result.hitTestResourceSlotAt40 = 0;
        result.orientationAt44 = 0;
        result.reverseAt48 = 0;
        result.rectAt4C = RECT{845, 661, 1009, 680};
        result.offsetRectAt5C = RECT{0, 0, -12, 0};
        return result;
    }();
    return descriptor;
}

const std::array<CgOperationChildDescriptor, 6>&
CgOperationButtonsChildDescriptorsAt604240()
{
    // 字符串逐字节来自样本 18 的 0x5B8970..0x5B8BF8，不经本地代码页转换。
    static const std::array<CgOperationChildDescriptor, 6> descriptors{{
        {CgOperationControlKind::Static, 2,
         {{{Cp932("\x43\x47\x8A\xD3\x8F\xDC\x92\x86\x89\xE6\x96\xCA\x5F\x83\x49\x83\x74\x2E\x76\x72\x6D"),
            Cp932("\x43\x47\x8A\xD3\x8F\xDC\x92\x86\x89\xE6\x96\xCA\x5F\x83\x49\x83\x74\x2E\x76\x72\x6D"), {},
            Cp932("\x43\x47\x8A\xD3\x8F\xDC\x92\x86\x89\xE6\x96\xCA\x5F\x83\x49\x83\x74\x2E\x76\x72\x6D"),
            Cp932("\x43\x47\x8A\xD3\x8F\xDC\x92\x86\x89\xE6\x96\xCA\x5F\x83\x49\x83\x74\x2E\x76\x72\x6D"), {}, {}}}, std::nullopt}, 7, -1},
        {CgOperationControlKind::Button, 3,
         {{{Cp932("\x43\x47\x8A\xD3\x8F\xDC\x92\x86\x89\xE6\x96\xCA\x5F\x8D\xB7\x95\xAA\x96\xDF\x82\xE8\x83\x49\x83\x93\x2E\x76\x72\x6D"), {}, {},
            Cp932("\x43\x47\x8A\xD3\x8F\xDC\x92\x86\x89\xE6\x96\xCA\x5F\x8D\xB7\x95\xAA\x96\xDF\x82\xE8\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D"),
            Cp932("\x43\x47\x8A\xD3\x8F\xDC\x92\x86\x89\xE6\x96\xCA\x5F\x8D\xB7\x95\xAA\x96\xDF\x82\xE8\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D"),
            Cp932("\x43\x47\x8A\xD3\x8F\xDC\x92\x86\x89\xE6\x96\xCA\x5F\x8D\xB7\x95\xAA\x96\xDF\x82\xE8\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D"),
            Cp932("\x43\x47\x8A\xD3\x8F\xDC\x92\x86\x89\xE6\x96\xCA\x5F\x8D\xB7\x95\xAA\x96\xDF\x82\xE8\x94\xCD\x88\xCD\x2E\x76\x72\x6D")}}, std::nullopt}, 6, 10},
        {CgOperationControlKind::Button, 4,
         {{{Cp932("\x43\x47\x8A\xD3\x8F\xDC\x92\x86\x89\xE6\x96\xCA\x5F\x8D\xB7\x95\xAA\x98\x67\x95\x5C\x8E\xA6\x83\x49\x83\x93\x2E\x76\x72\x6D"), {}, {},
            Cp932("\x43\x47\x8A\xD3\x8F\xDC\x92\x86\x89\xE6\x96\xCA\x5F\x8D\xB7\x95\xAA\x98\x67\x95\x5C\x8E\xA6\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D"),
            Cp932("\x43\x47\x8A\xD3\x8F\xDC\x92\x86\x89\xE6\x96\xCA\x5F\x8D\xB7\x95\xAA\x98\x67\x95\x5C\x8E\xA6\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D"),
            Cp932("\x43\x47\x8A\xD3\x8F\xDC\x92\x86\x89\xE6\x96\xCA\x5F\x8D\xB7\x95\xAA\x98\x67\x95\x5C\x8E\xA6\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D"), {}}}, std::nullopt}, 7, 11},
        {CgOperationControlKind::Button, 5,
         {{{Cp932("\x43\x47\x8A\xD3\x8F\xDC\x92\x86\x89\xE6\x96\xCA\x5F\x8D\xB7\x95\xAA\x91\x97\x82\xE8\x83\x49\x83\x93\x2E\x76\x72\x6D"), {}, {},
            Cp932("\x43\x47\x8A\xD3\x8F\xDC\x92\x86\x89\xE6\x96\xCA\x5F\x8D\xB7\x95\xAA\x91\x97\x82\xE8\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D"),
            Cp932("\x43\x47\x8A\xD3\x8F\xDC\x92\x86\x89\xE6\x96\xCA\x5F\x8D\xB7\x95\xAA\x91\x97\x82\xE8\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D"),
            Cp932("\x43\x47\x8A\xD3\x8F\xDC\x92\x86\x89\xE6\x96\xCA\x5F\x8D\xB7\x95\xAA\x91\x97\x82\xE8\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D"),
            Cp932("\x43\x47\x8A\xD3\x8F\xDC\x92\x86\x89\xE6\x96\xCA\x5F\x8D\xB7\x95\xAA\x91\x97\x82\xE8\x94\xCD\x88\xCD\x2E\x76\x72\x6D")}}, std::nullopt}, 6, 9},
        {CgOperationControlKind::Button, 6,
         {{{Cp932("\x43\x47\x8A\xD3\x8F\xDC\x92\x86\x89\xE6\x96\xCA\x5F\x94\xF1\x95\x5C\x8E\xA6\x83\x49\x83\x93\x2E\x76\x72\x6D"), {}, {},
            Cp932("\x43\x47\x8A\xD3\x8F\xDC\x92\x86\x89\xE6\x96\xCA\x5F\x94\xF1\x95\x5C\x8E\xA6\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D"),
            Cp932("\x43\x47\x8A\xD3\x8F\xDC\x92\x86\x89\xE6\x96\xCA\x5F\x94\xF1\x95\x5C\x8E\xA6\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D"),
            Cp932("\x43\x47\x8A\xD3\x8F\xDC\x92\x86\x89\xE6\x96\xCA\x5F\x94\xF1\x95\x5C\x8E\xA6\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D"),
            Cp932("\x43\x47\x8A\xD3\x8F\xDC\x92\x86\x89\xE6\x96\xCA\x5F\x94\xF1\x95\x5C\x8E\xA6\x94\xCD\x88\xCD\x2E\x76\x72\x6D")}}, std::nullopt}, 6, 8},
        {CgOperationControlKind::Button, 7,
         {{{Cp932("\x43\x47\x8A\xD3\x8F\xDC\x92\x86\x89\xE6\x96\xCA\x5F\x96\xDF\x82\xE9\x83\x49\x83\x93\x2E\x76\x72\x6D"),
            Cp932("\x43\x47\x8A\xD3\x8F\xDC\x92\x86\x89\xE6\x96\xCA\x5F\x96\xDF\x82\xE9\x83\x49\x83\x74\x2E\x76\x72\x6D"), {},
            Cp932("\x43\x47\x8A\xD3\x8F\xDC\x92\x86\x89\xE6\x96\xCA\x5F\x96\xDF\x82\xE9\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D"),
            Cp932("\x43\x47\x8A\xD3\x8F\xDC\x92\x86\x89\xE6\x96\xCA\x5F\x96\xDF\x82\xE9\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D"),
            Cp932("\x43\x47\x8A\xD3\x8F\xDC\x92\x86\x89\xE6\x96\xCA\x5F\x96\xDF\x82\xE9\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D"), {}}}, std::nullopt}, 7, 7},
    }};
    return descriptors;
}

std::vector<CgOperationChildControlRef>
CreateCgOperationButtonsChildrenAt608440(
    ICgOperationSurfaceFactory& factory)
{
    std::vector<CgOperationChildControlRef> result;
    result.reserve(6u);
    for (const auto& descriptor : CgOperationButtonsChildDescriptorsAt604240())
    {
        auto resources =
            std::make_shared<CgOperationControlResourceGroupRuntime>(
                factory, descriptor.resources);
        result.push_back(std::make_shared<CgOperationChildControlRuntime>(
            descriptor.kind,
            descriptor.eventValueAt04,
            descriptor.childIdAt0C,
            descriptor.initialResourceSlotAt24,
            std::move(resources)));
    }
    return result;
}

namespace
{
[[nodiscard]] CgOperationChildDescriptor PopupStatic8Descriptor()
{
    const std::string frame = Cp932(
        "\x43\x47\x8A\xD3\x8F\xDC\x92\x86\x89\xE6\x96\xCA\x5F"
        "\x8D\xB7\x95\xAA\x83\x79\x81\x5B\x83\x57\x98\x67\x2E\x76\x72\x6D");
    return {
        CgOperationControlKind::Static,
        8,
        {{{frame, {}, {}, frame, frame, {}, {}}}, std::nullopt},
        7,
        -1};
}

[[nodiscard]] CgOperationChildDescriptor PopupNumberButtonDescriptor(
    const std::int32_t eventValue)
{
    const std::int32_t zeroBased = eventValue - 12;
    const std::int32_t row = zeroBased / 2;
    const std::int32_t x = (zeroBased & 1) == 0 ? 1027 : 1137;
    const std::int32_t y = 120 + 46 * row;
    const std::string target = Cp932(
        "\x43\x47\x8A\xD3\x8F\xDC\x92\x86\x89\xE6\x96\xCA\x5F"
        "\x8D\xB7\x95\xAA\x90\x94\x92\x6C\x83\x7B\x83\x5E\x83\x93"
        "\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D");
    const std::string click = Cp932(
        "\x43\x47\x8A\xD3\x8F\xDC\x92\x86\x89\xE6\x96\xCA\x5F"
        "\x8D\xB7\x95\xAA\x90\x94\x92\x6C\x83\x7B\x83\x5E\x83\x93"
        "\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D");
    return {
        CgOperationControlKind::Button,
        eventValue,
        {{{{}, {}, {}, target, target, click, {}}}, POINT{x, y}},
        7,
        -1};
}

[[nodiscard]] CgOperationChildDescriptor PopupPageButtonDescriptor(
    const std::int32_t eventValue)
{
    const bool previous = eventValue == 10;
    const std::string on = previous
        ? Cp932("\x43\x47\x8A\xD3\x8F\xDC\x92\x86\x89\xE6\x96\xCA\x5F\x83\x79\x81\x5B\x83\x57\x96\xDF\x82\xE8\x83\x49\x83\x93\x2E\x76\x72\x6D")
        : Cp932("\x43\x47\x8A\xD3\x8F\xDC\x92\x86\x89\xE6\x96\xCA\x5F\x83\x79\x81\x5B\x83\x57\x91\x97\x82\xE8\x83\x49\x83\x93\x2E\x76\x72\x6D");
    const std::string target = previous
        ? Cp932("\x43\x47\x8A\xD3\x8F\xDC\x92\x86\x89\xE6\x96\xCA\x5F\x83\x79\x81\x5B\x83\x57\x96\xDF\x82\xE8\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D")
        : Cp932("\x43\x47\x8A\xD3\x8F\xDC\x92\x86\x89\xE6\x96\xCA\x5F\x83\x79\x81\x5B\x83\x57\x91\x97\x82\xE8\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D");
    const std::string click = previous
        ? Cp932("\x43\x47\x8A\xD3\x8F\xDC\x92\x86\x89\xE6\x96\xCA\x5F\x83\x79\x81\x5B\x83\x57\x96\xDF\x82\xE8\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D")
        : Cp932("\x43\x47\x8A\xD3\x8F\xDC\x92\x86\x89\xE6\x96\xCA\x5F\x83\x79\x81\x5B\x83\x57\x91\x97\x82\xE8\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D");
    return {
        CgOperationControlKind::Button,
        eventValue,
        {{{on, {}, {}, target, target, click, {}}}, std::nullopt},
        7,
        previous ? 10 : 9};
}

void UnionChildRect(RECT& destination, const RECT& source)
{
    RECT combined{};
    ::UnionRect(&combined, &destination, &source);
    destination = combined;
}
}

CgOperationPopupChildTreeRuntime::CgOperationPopupChildTreeRuntime(
    ICgOperationSurfaceFactory& factory,
    IGraphicsSurfaceCompositionApi& graphics)
{
    children_.reserve(24u);

    const auto static8Descriptor = PopupStatic8Descriptor();
    auto static8Resources =
        std::make_shared<CgOperationControlResourceGroupRuntime>(
            factory, static8Descriptor.resources);
    auto static8 = std::make_shared<CgOperationChildControlRuntime>(
        static8Descriptor.kind,
        static8Descriptor.eventValueAt04,
        static8Descriptor.childIdAt0C,
        static8Descriptor.initialResourceSlotAt24,
        static8Resources);
    UnionChildRect(combinedChildRectAt68_, static8->RectAtVslot3C());
    children_.push_back(static8);

    const RECT static8Rect = static8->RectAtVslot3C();
    auto blank = factory.CreateBlank(
        static8Rect.right - static8Rect.left,
        static8Rect.bottom - static8Rect.top,
        true,
        true);
    if (!blank)
        throw 1;
    blank->SetOrigin(static8Rect.left, static8Rect.top);
    initialPage_ = std::make_shared<CgOperationPopupPageSurfaceRuntime>(
        graphics, blank, static8Rect);
    pageResourcesAt88_.push_back(initialPage_);

    auto static9Resources =
        std::make_shared<CgOperationControlResourceGroupRuntime>(blank, 0u);
    auto static9 = std::make_shared<CgOperationChildControlRuntime>(
        CgOperationControlKind::Static, 9, -1, 7,
        std::move(static9Resources));
    UnionChildRect(combinedChildRectAt68_, static9->RectAtVslot3C());
    children_.push_back(std::move(static9));

    independentSurfaceAt7C_ = factory.LoadStatic(
        CgOperationPopupIndependentResourceNameAt5B9100());
    if (!independentSurfaceAt7C_)
        throw 1;

    for (std::int32_t eventValue = 12; eventValue <= 31; ++eventValue)
    {
        const auto descriptor = PopupNumberButtonDescriptor(eventValue);
        auto resources =
            std::make_shared<CgOperationControlResourceGroupRuntime>(
                factory, descriptor.resources);
        auto child = std::make_shared<CgOperationChildControlRuntime>(
            descriptor.kind,
            descriptor.eventValueAt04,
            descriptor.childIdAt0C,
            descriptor.initialResourceSlotAt24,
            std::move(resources));
        UnionChildRect(combinedChildRectAt68_, child->RectAtVslot3C());
        children_.push_back(std::move(child));
    }

    for (const std::int32_t eventValue : {10, 11})
    {
        const auto descriptor = PopupPageButtonDescriptor(eventValue);
        auto resources =
            std::make_shared<CgOperationControlResourceGroupRuntime>(
                factory, descriptor.resources);
        auto child = std::make_shared<CgOperationChildControlRuntime>(
            descriptor.kind,
            descriptor.eventValueAt04,
            descriptor.childIdAt0C,
            descriptor.initialResourceSlotAt24,
            std::move(resources));
        UnionChildRect(combinedChildRectAt68_, child->RectAtVslot3C());
        children_.push_back(std::move(child));
    }
}

const std::vector<CgOperationChildControlRef>&
CgOperationPopupChildTreeRuntime::ChildrenInInsertionOrder() const noexcept
{
    return children_;
}

const std::shared_ptr<RuntimeGraphicsSurface>&
CgOperationPopupChildTreeRuntime::IndependentSurfaceAt7C() const noexcept
{
    return independentSurfaceAt7C_;
}

const std::shared_ptr<CgOperationPopupPageSurfaceRuntime>&
CgOperationPopupChildTreeRuntime::InitialPage() const noexcept
{
    return initialPage_;
}

const std::vector<CgOperationPopupPageResourceRef>&
CgOperationPopupChildTreeRuntime::PageResourcesAt88() const noexcept
{
    return pageResourcesAt88_;
}

const RECT&
CgOperationPopupChildTreeRuntime::CombinedChildRectAt68() const noexcept
{
    return combinedChildRectAt68_;
}

CgOperationArchiveSurfaceFactory::CgOperationArchiveSurfaceFactory(
    const ResourceCatalog& catalog,
    const FileTransactionContext* const transactionContext,
    IStartFileApi& fileApi,
    const IEnvironmentKeyTableSource& environmentKeyBytes,
    IGraphicsSurfaceApi& graphicsApi, ArchiveCodecServices codecs) noexcept
    : catalog_(&catalog),
      transactionContext_(transactionContext),
      fileApi_(&fileApi),
      environmentKeyBytes_(&environmentKeyBytes),
      graphicsApi_(&graphicsApi),
      codecs_(codecs)
{
}

std::shared_ptr<RuntimeGraphicsSurface>
CgOperationArchiveSurfaceFactory::LoadStatic(
    const std::string& resourceNameCp932)
{
    CgOperationArchiveSurfaceBatch batch(*catalog_, transactionContext_, *fileApi_,
        *environmentKeyBytes_, *graphicsApi_, codecs_);
    return batch.LoadStatic(resourceNameCp932);
}

std::unique_ptr<ICgOperationSurfaceBatch> CgOperationArchiveSurfaceFactory::CreateBatch()
{
    return std::make_unique<CgOperationArchiveSurfaceBatch>(
        *catalog_, transactionContext_, *fileApi_, *environmentKeyBytes_, *graphicsApi_, codecs_);
}

std::shared_ptr<RuntimeGraphicsSurface>
CgOperationArchiveSurfaceFactory::CreateBlank(
    const std::int32_t width,
    const std::int32_t height,
    const bool hasAlpha,
    const bool clearOnCreate)
{
    auto surface = std::make_shared<RuntimeGraphicsSurface>(*graphicsApi_);
    if (!surface->Create(width, height, hasAlpha, clearOnCreate))
        return {};
    return surface;
}

std::shared_ptr<RuntimeGraphicsSurface>
CgOperationArchiveSurfaceFactory::CreateCompatible(
    const RuntimeGraphicsSurface& source,
    const bool copyPixels)
{
    auto surface = CreateBlank(
        source.Width(), source.Height(), source.HasAlpha(), true);
    if (!surface || !copyPixels)
        return surface;

    // 0x444470 在 copyPixels=1 时创建同规格 Surface 后调用 SurfaceCopyBlt，
    // 随后复制 CSurface+0x10/+0x14 的资源原点。SaveLoadMessageBox 的数字
    // 底图缓存依赖这条分支，不能把它退化成空白兼容 Surface。
    auto* const copyApi = dynamic_cast<IGraphicsSurfaceCopyApi*>(graphicsApi_);
    if (copyApi == nullptr)
    {
        throw std::logic_error(
            "Graphics surface API does not provide SurfaceCopyBlt");
    }
    static_cast<void>(copyApi->CopyBlitSurface(
        surface->Handle(), source.Handle(),
        0, 0, nullptr, nullptr, nullptr, 0));
    surface->SetOrigin(source.OriginX(), source.OriginY());
    return surface;
}

const std::array<std::string, 20>&
CgOperationDialogVisualResourceNamesAt608480()
{
    // 608480 的 20 个指针严格按数字 0..9、普通/オフ成对排列。
    static const std::array<std::string, 20> names = [] {
        std::array<std::string, 20> result{};
        constexpr char prefix[] =
            "CG\x8A\xD3\x8F\xDC\x92\x86\x89\xE6\x96\xCA"
            "_\x8D\xB7\x95\xAA\x90\x94\x92\x6C";
        constexpr char offSuffix[] = "\x83\x49\x83\x74.vrm";
        for (std::size_t digit = 0; digit < 10; ++digit)
        {
            std::string base(prefix);
            base.push_back(static_cast<char>('0' + digit));
            result[digit * 2] = base + ".vrm";
            result[digit * 2 + 1] = base + offSuffix;
        }
        return result;
    }();
    return names;
}

CgOperationDialogVisualResourcesRuntime::
CgOperationDialogVisualResourcesRuntime(ICgOperationSurfaceFactory& factory)
{
    resources_.reserve(20u);
    for (const std::string& resourceName :
         CgOperationDialogVisualResourceNamesAt608480())
    {
        auto surface = factory.LoadStatic(resourceName);
        if (!surface)
        {
            // 4DB3D0 在 4800A0 返回假后抛出整数 1。
            throw 1;
        }

        const RECT localRect{0, 0, surface->Width(), surface->Height()};
        RECT combined{};
        ::UnionRect(&combined, &combinedRect_, &localRect);
        combinedRect_ = combined;
        resources_.push_back(std::move(surface));
    }
}

const std::vector<std::shared_ptr<RuntimeGraphicsSurface>>&
CgOperationDialogVisualResourcesRuntime::Resources() const noexcept
{
    return resources_;
}

const RECT&
CgOperationDialogVisualResourcesRuntime::CombinedRect() const noexcept
{
    return combinedRect_;
}

CgOperationButtonsSurfaceResourcesRuntime::
CgOperationButtonsSurfaceResourcesRuntime(
    ICgOperationSurfaceFactory& factory,
    const RECT& templateRect,
    const CgOperationButtonsVisualLayout& layout)
{
    for (auto it = layout.digitPositions.rbegin();
         it != layout.digitPositions.rend(); ++it)
    {
        const RECT positioned{
            it->x,
            it->y,
            it->x + (templateRect.right - templateRect.left),
            it->y + (templateRect.bottom - templateRect.top)};
        RECT combined{};
        ::UnionRect(&combined, &combinedTargetRect_, &positioned);
        combinedTargetRect_ = combined;
    }

    const std::int32_t width =
        combinedTargetRect_.right - combinedTargetRect_.left;
    const std::int32_t height =
        combinedTargetRect_.bottom - combinedTargetRect_.top;
    target0At68_ = factory.CreateBlank(width, height, true, true);
    if (!target0At68_)
        throw 1;
    target0At68_->SetOrigin(
        combinedTargetRect_.left, combinedTargetRect_.top);

    target1At74_ = factory.CreateCompatible(*target0At68_, false);
    if (!target1At74_)
    {
        throw std::runtime_error(
            "CGOperationButtons second compatible surface creation failed");
    }
    target1At74_->SetOrigin(
        combinedTargetRect_.left, combinedTargetRect_.top);
}

const std::shared_ptr<RuntimeGraphicsSurface>&
CgOperationButtonsSurfaceResourcesRuntime::Target0At68() const noexcept
{
    return target0At68_;
}

const std::shared_ptr<RuntimeGraphicsSurface>&
CgOperationButtonsSurfaceResourcesRuntime::Target1At74() const noexcept
{
    return target1At74_;
}

const RECT&
CgOperationButtonsSurfaceResourcesRuntime::CombinedTargetRect() const noexcept
{
    return combinedTargetRect_;
}

const std::string& CgOperationPopupIndependentResourceNameAt5B9100()
{
    // 样本 18 的 0x5B9100，长度 0x23；保持原 CP932 字节，不经过本地代码页转换。
    static const std::string name(
        "CG\x8A\xD3\x8F\xDC\x92\x86\x89\xE6\x96\xCA"
        "_\x8D\xB7\x95\xAA\x90\x94\x92\x6C"
        "\x83\x7B\x83\x5E\x83\x93\x83\x49\x83\x93.vrm",
        0x23u);
    return name;
}

CgOperationPopupPageSurfaceRuntime::CgOperationPopupPageSurfaceRuntime(
    IGraphicsSurfaceCompositionApi& graphics,
    std::shared_ptr<RuntimeGraphicsSurface> surface,
    const RECT& layoutRect)
    : graphics_(&graphics),
      surface_(std::move(surface)),
      layoutRect_(layoutRect)
{
    if (!surface_)
        throw std::invalid_argument("CGOperationPopup page surface is null");
}

void CgOperationPopupPageSurfaceRuntime::ClearAt4143A0(
    const RECT* const rect)
{
    graphics_->ClearSurface(
        surface_->Handle(), surface_->ClearColor(), rect);
}

const std::shared_ptr<RuntimeGraphicsSurface>&
CgOperationPopupPageSurfaceRuntime::Surface() const noexcept
{
    return surface_;
}

const RECT&
CgOperationPopupPageSurfaceRuntime::LayoutRect() const noexcept
{
    return layoutRect_;
}

namespace
{
struct CgOperationPopupItemLayout final
{
    POINT ones{};
    POINT tens{};
    POINT hundreds{};
};

[[nodiscard]] const std::array<CgOperationPopupItemLayout, 20>&
PopupItemLayoutsAt6084E8()
{
    static const std::array<CgOperationPopupItemLayout, 20> layouts = [] {
        std::array<CgOperationPopupItemLayout, 20> result{};
        for (std::int32_t row = 0; row < 10; ++row)
        {
            const std::int32_t y = 126 + 46 * row;
            result[static_cast<std::size_t>(row * 2)] = {
                POINT{1084, y}, POINT{1060, y}, POINT{1036, y}};
            result[static_cast<std::size_t>(row * 2 + 1)] = {
                POINT{1194, y}, POINT{1170, y}, POINT{1146, y}};
        }
        return result;
    }();
    return layouts;
}
}

CgOperationPopupPageProducerRuntime::
CgOperationPopupPageProducerRuntime(
    ICgOperationSurfaceFactory& factory,
    IGraphicsSurfaceCompositionApi& graphics,
    std::vector<std::shared_ptr<RuntimeGraphicsSurface>> digitResources)
    : factory_(&factory),
      graphics_(&graphics),
      digitResources_(std::move(digitResources))
{
    if (digitResources_.size() != 20u ||
        std::ranges::any_of(digitResources_, [](const auto& resource) {
            return !resource;
        }))
    {
        throw std::invalid_argument(
            "CGOperationPopup requires twenty Dialog digit resources");
    }
}

void CgOperationPopupPageProducerRuntime::DrawSelectionAt4FEAE0(
    std::vector<CgOperationPopupPageResourceRef>& pageResources,
    const std::shared_ptr<RuntimeGraphicsSurface>& independentSurfaceAt7C,
    const std::int32_t itemIndex,
    const std::int32_t available)
{
    if (itemIndex < 0)
        throw std::out_of_range("CGOperationPopup item index is negative");
    if (pageResources.empty())
        throw std::out_of_range("CGOperationPopup +0x88 vector is empty");
    if (!independentSurfaceAt7C)
        throw std::invalid_argument("CGOperationPopup +0x7C surface is null");

    const std::int32_t pageIndex = itemIndex / 20;
    if (static_cast<std::size_t>(pageIndex) >= pageResources.size())
    {
        const auto firstPage =
            std::dynamic_pointer_cast<CgOperationPopupPageSurfaceRuntime>(
                pageResources.front());
        if (!firstPage)
            throw std::logic_error("CGOperationPopup first page is not a Surface page");

        auto surface = factory_->CreateCompatible(*firstPage->Surface(), false);
        if (!surface)
        {
            // 原 444470 返回空引用后会在 4FEAE0 的原点复制处失效；重写版
            // 在同一位置显式终止，不能继续生成缺页或半页。
            throw std::runtime_error(
                "CGOperationPopup compatible page creation failed");
        }
        surface->SetOrigin(
            firstPage->Surface()->OriginX(),
            firstPage->Surface()->OriginY());
        pageResources.push_back(
            std::make_shared<CgOperationPopupPageSurfaceRuntime>(
                *graphics_, std::move(surface), firstPage->LayoutRect()));
    }

    // 4FEAE0 只补一页，不循环填补跨页空洞。
    if (static_cast<std::size_t>(pageIndex) >= pageResources.size())
        throw std::out_of_range("CGOperationPopup page index skips original growth order");
    const auto page =
        std::dynamic_pointer_cast<CgOperationPopupPageSurfaceRuntime>(
            pageResources[static_cast<std::size_t>(pageIndex)]);
    if (!page)
        throw std::logic_error("CGOperationPopup page is not a Surface page");

    const RuntimeGraphicsSurface& destination = *page->Surface();
    const auto& layout = PopupItemLayoutsAt6084E8()[
        static_cast<std::size_t>(itemIndex % 20)];
    const auto blit = [this, &destination](
                          const RuntimeGraphicsSurface& source,
                          const POINT position) {
        static_cast<void>(graphics_->BlitSurface(
            destination.Handle(), source.Handle(),
            position.x - destination.OriginX(),
            position.y - destination.OriginY(),
            nullptr, nullptr, nullptr, 0));
    };

    // 独立按钮底图固定比百位坐标左 9、上 6，且先于数字写入。
    blit(*independentSurfaceAt7C,
         POINT{layout.hundreds.x - 9, layout.hundreds.y - 6});

    if (itemIndex >= 1000)
        return;

    const std::int32_t number = itemIndex + 1;
    const std::int32_t hundreds = number / 100;
    const std::int32_t remainder = number - 100 * hundreds;
    const std::int32_t tens = remainder / 10;
    const std::int32_t ones = remainder - 10 * tens;
    blit(DigitResourceAt50A7D0(hundreds, available), layout.hundreds);
    blit(DigitResourceAt50A7D0(tens, available), layout.tens);
    blit(DigitResourceAt50A7D0(ones, available), layout.ones);
}

const RuntimeGraphicsSurface&
CgOperationPopupPageProducerRuntime::DigitResourceAt50A7D0(
    const std::int32_t digit,
    const std::int32_t available) const
{
    const std::int32_t index = 2 * digit + (available == 0 ? 1 : 0);
    if (index < 0 || static_cast<std::size_t>(index) >= digitResources_.size())
        throw std::out_of_range("CGOperationPopup digit resource index is invalid");
    return *digitResources_[static_cast<std::size_t>(index)];
}

CgOperationPopupSurfaceResourcesRuntime::
CgOperationPopupSurfaceResourcesRuntime(
    ICgOperationSurfaceFactory& factory,
    IGraphicsSurfaceCompositionApi& graphics,
    const RECT& static8Rect)
{
    const std::int32_t width = static8Rect.right - static8Rect.left;
    const std::int32_t height = static8Rect.bottom - static8Rect.top;
    auto blank = factory.CreateBlank(width, height, true, true);
    if (!blank)
        throw 1;

    blank->SetOrigin(static8Rect.left, static8Rect.top);
    initialPage_ = std::make_shared<CgOperationPopupPageSurfaceRuntime>(
        graphics, std::move(blank), static8Rect);
    pageResourcesAt88_.push_back(initialPage_);

    independentSurfaceAt7C_ = factory.LoadStatic(
        CgOperationPopupIndependentResourceNameAt5B9100());
    if (!independentSurfaceAt7C_)
        throw 1;
}

const std::shared_ptr<RuntimeGraphicsSurface>&
CgOperationPopupSurfaceResourcesRuntime::IndependentSurfaceAt7C() const noexcept
{
    return independentSurfaceAt7C_;
}

const std::shared_ptr<CgOperationPopupPageSurfaceRuntime>&
CgOperationPopupSurfaceResourcesRuntime::InitialPage() const noexcept
{
    return initialPage_;
}

const std::vector<CgOperationPopupPageResourceRef>&
CgOperationPopupSurfaceResourcesRuntime::PageResourcesAt88() const noexcept
{
    return pageResourcesAt88_;
}
}
