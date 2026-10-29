#include "SaveLoadPageSlotNative.Internal.h"
#include <Runtime/SaveFileNativePages.h>
#include <Runtime/SaveFileCatalog.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/SaveLoadResourceProfile.h>
#include <optional>
#include <stdexcept>

namespace KaGuYa::Start::Runtime::SaveLoadPageSlotNativeDetail
{
namespace
{
GraphicsSurfaceNativeBody* CachedSurface(const Slot& slot, YtlReferenceControlRuntime* control) noexcept
{
    const volatile auto& raw = slot;
    const auto base = raw.baseAt00;
    if (!base) return nullptr;
    const auto stride = control ? *reinterpret_cast<const volatile std::uint32_t*>(
        reinterpret_cast<const std::byte*>(control) + 0x0C) : 0u;
    return reinterpret_cast<GraphicsSurfaceNativeBody*>(base + raw.biasAt04 * stride);
}
SaveFileNativeItem* Item(const Slot& page, std::uint32_t index) noexcept
{
    auto* body = Required(Resolve<SaveFileNativePage>(page));
    const auto* begin = static_cast<const volatile SaveFileNativePage*>(body)->itemsAt04.begin;
    return Required(Resolve<SaveFileNativeItem>(At(begin, index)));
}
const std::int64_t* Time(const Slot& page, std::uint32_t index) noexcept
{ return reinterpret_cast<const std::int64_t*>(Resolve<SaveFileNativeTime>(Item(page, index)->timeAt00)); }
std::int32_t DatePart(const std::int64_t* time, unsigned part, ISaveLoadTimestampNativeSource& source)
{
    std::tm tm;
    const auto result = source.LocalTime64At5697BB(&tm, time);
    if (result != 0) return part < 2 ? 0 : -1;
    if (part == 0) return Signed(std::uint32_t(tm.tm_mon) + 1u);
    if (part == 1) return tm.tm_mday;
    if (part == 2) return tm.tm_hour;
    return tm.tm_min;
}
void Pair(SaveLoadPageSecondNativeBodyACh& page, const Slot& target, std::int32_t value,
    std::int32_t mode, unsigned position, SaveLoadPageSlotNativeServices& services)
{
    const auto tens = value / 10;
    DrawDateDigit(page, target, tens, mode, position, services);
    DrawDateDigit(page, target, Signed(std::uint32_t(value) - std::uint32_t(tens) * 10u),
        mode, position + 1, services);
}
void DrawSlotNumber(SaveLoadPageSecondNativeBodyACh& page, const Slot& target,
    std::uint32_t fileSlot, std::int32_t mode, const SaveLoadSlotResources& skin,
    SaveLoadPageSlotNativeServices& services)
{
    const auto& vector = Parent(page).slotDigitsAt120;
    const volatile auto& pointers = vector;
    const auto* begin = pointers.begin;
    const auto* end = pointers.end;
    for (const auto& glyph : skin.numberGlyphs)
    {
        const auto index = static_cast<unsigned>((static_cast<int>(fileSlot) + 1) / glyph.divisor % glyph.modulus +
            glyph.resourceOffset + (mode ? 0 : skin.numberThemeStride));
        Slot number;
        if (index < Count(begin, end)) number = CopyViaTemporary(At(begin, index));
        RefLifetime lifetime{number};
        auto* control = number.controlAt08;
        if (number.baseAt00)
            Blit(Resolve<GraphicsSurfaceNativeBody>(target), Resolve<GraphicsSurfaceNativeBody>(number),
                glyph.position.x, glyph.position.y, nullptr, services);
        lifetime.FinishCached(control);
    }
}
void DrawSlotDecoration(SaveLoadPageSecondNativeBodyACh& page, const Slot& target,
    std::int32_t mode, const SaveLoadSlotResources& skin, SaveLoadPageSlotNativeServices& services)
{
    auto marker = Copy(At(Parent(page).newMarkerAt150.begin, std::uint32_t(mode ^ 1)));
    RefLifetime lifetime{marker};
    Blit(Resolve<GraphicsSurfaceNativeBody>(target), Resolve<GraphicsSurfaceNativeBody>(marker),
        skin.newOrigin.x, skin.newOrigin.y, nullptr, services);
    lifetime.FinishClearing();
}
void DrawPreview(SaveLoadPageSecondNativeBodyACh& page, const Slot& target, const Slot& factory,
    std::uint32_t slot, std::int32_t mode, SaveLoadPageSlotNativeServices& services)
{
    auto* raw = services.memory.AllocateBody(0x24);
    GraphicsSurfaceNativeBody* surface = nullptr;
    if (raw)
    {
        try { surface = services.source.ConstructPreviewSurfaceAt40C2E0(raw); }
        catch (...) { [&]() noexcept { services.memory.DeleteBody(raw, 0x24); }(); throw; }
    }
    Slot preview;
    // 451500控制块失败不补回收24h正文；保护只在返回成功后生效。
    ConstructGraphicsSurfaceReferenceAt451500(preview, surface, 1);
    RefLifetime previewLifetime{preview};
    YtlReferenceControlRuntime* cachedControl;
    bool loaded;
    {
        TextCell filename;
        ConstructSaveFileFactoryPreviewNameNativeAt47AFA0(filename.value,
            *Required(Resolve<SaveFileDeleteFactoryNativeBody0Ch>(factory)), Signed(slot + 1u), services.source);
        TextLifetime filenameLifetime{filename.value};
        const auto* suffix = filename.value.GetString();
        auto* env = Required(services.source.CaptureEnvironmentAt4180C0());
        auto* paths = Required(Resolve<std::byte>(env->pathsAt48));
        auto& directory = *reinterpret_cast<RuntimeCString*>(paths + 0x10);
        {
            RuntimeCStringConcatenation path(directory, suffix, services.source.Strings());
            const auto previewBase = static_cast<const volatile Slot&>(preview).baseAt00;
            cachedControl = static_cast<const volatile Slot&>(preview).controlAt08;
            GraphicsSurfaceNativeBody* receiver = nullptr;
            if (previewBase)
            {
                const auto stride = cachedControl ? *reinterpret_cast<const volatile std::uint32_t*>(
                    reinterpret_cast<const std::byte*>(cachedControl) + 0x0C) : 0u;
                receiver = reinterpret_cast<GraphicsSurfaceNativeBody*>(
                    previewBase + static_cast<const volatile Slot&>(preview).biasAt04 * stride);
            }
            loaded = std::uint8_t(services.source.LoadPreviewAt511D90(
                receiver, path.GetString(), nullptr)) != 0;
        }
        filenameLifetime.Finish();
    }
    if (!loaded)
    {
        // 50A790：按mode选0/1，不按vector长度兜底；465870借用temporary。
        auto fallback = Copy(At(Parent(page).noPreviewAt144.begin, std::uint32_t(mode ^ 1)));
        RefLifetime fallbackLifetime{fallback};
        CopyAssignGraphicsSurfaceReferenceAt465870(preview, fallback);
        fallbackLifetime.FinishClearing();
        cachedControl = preview.controlAt08;
    }
    auto* image = Required(CachedSurface(preview, cachedControl));
    const auto& policy=CurrentSaveLoadResourceProfile().page.slots.preview;
    const SIZE* size = nullptr;
    const volatile auto& dimensions = *image;
    if (policy.kind==SaveLoadPreviewKind::ShrinkIfOversized &&
        (dimensions.widthAt08 > policy.extent.cx || dimensions.heightAt0C > policy.extent.cy)) size = &policy.extent;
    // 52B03B重新解址同一个Ref；尺寸判定后服务尚未捕获。
    image = CachedSurface(preview, cachedControl);
    if(policy.kind==SaveLoadPreviewKind::SourceCrop)
    {
        const RECT crop{0,0,policy.extent.cx,policy.extent.cy};
        const auto destination=Required(Resolve<GraphicsSurfaceNativeBody>(target))->handleAt04;
        if(destination&&image->handleAt04)
            services.drawing.SurfaceBlitRaw(destination,image->handleAt04,
                policy.origin.x,policy.origin.y,nullptr,&crop,nullptr,0);
    }
    else Blit(Resolve<GraphicsSurfaceNativeBody>(target), image, policy.origin.x, policy.origin.y, size, services);
    previewLifetime.FinishClearing();
}
}

void Blit(GraphicsSurfaceNativeBody* target, GraphicsSurfaceNativeBody* source,
    std::int32_t x, std::int32_t y, const SIZE* size, SaveLoadPageSlotNativeServices& services)
{
    const auto destination = static_cast<const volatile GraphicsSurfaceNativeBody*>(Required(target))->handleAt04;
    if (!destination) return;
    const auto image = static_cast<const volatile GraphicsSurfaceNativeBody*>(Required(source))->handleAt04;
    if (!image) return;
    (void)services.drawing.SurfaceBlitRaw(destination, image, x, y, size, nullptr, nullptr, 0);
}
void DrawDateDigit(SaveLoadPageSecondNativeBodyACh& page, const Slot& target,
    std::int32_t digit, std::int32_t mode, unsigned position, SaveLoadPageSlotNativeServices& services)
{
    // 50B380读取当前parent；有符号digit先DWORD回绕，再作无符号范围比较。
    const auto& vector = Parent(page).dateDigitsAt12C;
    const volatile auto& pointers = vector;
    const auto* begin = pointers.begin;
    const auto* end = pointers.end;
    const auto& profile=CurrentSaveLoadResourceProfile().page.slots;
    const auto index = std::uint32_t(digit) + (mode ? 0u : static_cast<unsigned>(profile.dateThemeStride));
    Slot glyph;
    if (index < Count(begin, end))
    {
        const volatile auto& raw = At(begin, index);
        auto* control = raw.controlAt08;
        const auto base = raw.baseAt00;
        const auto bias = raw.biasAt04;
        if (control) ++control->referenceCountAt04;
        glyph.baseAt00 = base; glyph.biasAt04 = bias; glyph.controlAt08 = control;
        if (control) ++control->referenceCountAt04;
        ReleaseCached(control);
    }
    RefLifetime lifetime{glyph};
    auto* image = Resolve<GraphicsSurfaceNativeBody>(glyph);
    auto* destination = Resolve<GraphicsSurfaceNativeBody>(target);
    const auto point=profile.dateGlyphs[position].position;
    Blit(destination, image, point.x, point.y, nullptr, services);
    lifetime.FinishClearing();
}
}

namespace KaGuYa::Start::Runtime
{
void RenderSaveLoadPageSlotNativeAt52A470(SaveLoadPageSecondNativeBodyACh& page,
    std::int32_t pageNumber, std::uint32_t slot, YtlReferenceSlotRuntime ownedPage,
    YtlReferenceSlotRuntime ownedFactory, std::int32_t isNew, SaveLoadPageSlotNativeServices& services)
{
    using namespace SaveLoadPageSlotNativeDetail;
    GraphicsSurfaceNativeScope scope(services.surfaces);
    // 按值实参先取得释放资格，配置拒绝也必须先page后factory退役。
    RefLifetime factoryLifetime{ownedFactory};
    RefLifetime pageLifetime{ownedPage};
    (void)pageNumber;
    const auto& profile=CurrentSaveLoadResourceProfile();
    const auto& skin=profile.page.slots;
    bool contentFirst = false, permanentDecoration = false;
    switch (skin.layerOrder)
    {
    case SaveLoadSlotLayerOrder::NumberDateContentLabelNew: break;
    case SaveLoadSlotLayerOrder::ContentMarkerNumberDate:
        contentFirst = permanentDecoration = true; break;
    case SaveLoadSlotLayerOrder::ContentNumberDate:
        contentFirst = true; break;
    default: throw std::logic_error("save slot layer order is not supported");
    }
    auto fileSlot=slot;
    const auto pageId=page.common.vptrAt00->idAt0C(&page.common);
    if(profile.catalog.rollingNewestFirst&&(pageId==4||pageId==5))
    {
        const auto* catalog=static_cast<SaveFileStateNativeArchive*>(
            ResolveEnvironmentArchiveAt417990(*services.source.CaptureEnvironmentAt4180C0(),6));
        fileSlot=static_cast<unsigned>(SaveFileRollingSlot(*catalog,pageId==5,static_cast<int>(slot)));
    }
    const volatile auto& raw = At(page.surfacesAt8C.begin, slot);
    auto* targetControl = raw.controlAt08;
    const auto targetBase = raw.baseAt00;
    const auto targetBias = raw.biasAt04;
    Slot target;
    target.baseAt00 = targetBase; target.biasAt04 = targetBias; target.controlAt08 = targetControl;
    if (targetControl) ++targetControl->referenceCountAt04;
    RefLifetime targetLifetime{target};
    if (targetBase)
    {
        const auto mode = page.common.vptrAt00->idAt0C(&page.common) == 2 ? 1 : 0;
        auto* destination = Required(Resolve<GraphicsSurfaceNativeBody>(target));
        const volatile auto& clear = *destination;
        const auto handle = clear.handleAt04;
        if (handle)
        {
            const auto color = clear.clearColorAt20;
            services.drawing.SurfaceClearRaw(handle, color, nullptr);
        }
        if (!contentFirst) DrawSlotNumber(page, target, fileSlot, mode, skin, services);
        bool available = Required(Resolve<SaveFileNativePage>(ownedPage))->availabilityAt00 != 0;
        if (available) available = Item(ownedPage, fileSlot)->timeAt00.baseAt00 != 0;
        if (available)
        {
            if (contentFirst)
            {
                DrawPreview(page, target, ownedFactory, fileSlot, mode, services);
                // 01..06原星图是常驻主题装饰；07..11没有，不能套现代isNew门。
                const auto& markers = Parent(page).newMarkerAt150;
                if (permanentDecoration && Count(markers.begin, markers.end))
                    DrawSlotDecoration(page, target, mode, skin, services);
                DrawSlotNumber(page, target, fileSlot, mode, skin, services);
            }
            std::array<std::optional<int>,7> values;
            unsigned position=0;
            for(const auto& glyph:skin.dateGlyphs)
            {
                auto& value=values[static_cast<unsigned>(glyph.value)];
                if(!value)
                {
                    if(glyph.value==SaveLoadGlyphValue::Literal)value=0;
                    else if(glyph.value==SaveLoadGlyphValue::Year)
                        value=SaveLoadTimestampYearNativeAt47E120(Time(ownedPage,fileSlot),services.source);
                    else value=DatePart(Time(ownedPage,fileSlot),
                        static_cast<unsigned>(glyph.value)-static_cast<unsigned>(SaveLoadGlyphValue::Month),services.source);
                }
                DrawDateDigit(page,target,*value/glyph.divisor%glyph.modulus+glyph.resourceOffset,mode,position++,services);
            }
            if (!contentFirst) DrawPreview(page, target, ownedFactory, fileSlot, mode, services);
            if(skin.labelKind==SaveLoadLabelKind::PerSlot)
            {
            TextCell shortened;
            TruncateSaveLoadLabelNativeAt4DA8E0(&shortened.value,
                Item(ownedPage, fileSlot)->labelAt0C.GetString(), skin.labelStyle.maxCharacters, skin.labelStyle.maxLines, services.source);
            TextLifetime shortenedLifetime{shortened.value};
            TextCell description;
            auto* cloned = CloneRuntimeCStringDataAt414540(
                reinterpret_cast<ATL::CStringData*>(const_cast<char*>(shortened.value.GetString())) - 1);
            const auto* chars = reinterpret_cast<const char*>(cloned + 1);
            static_assert(sizeof(description.value) == sizeof(chars));
            std::memcpy(&description.value, &chars, sizeof(chars));
            TextLifetime descriptionLifetime{description.value};
            shortenedLifetime.Finish();
            // 52B11B先固定bias，再复制按值Ref；label helper正常消费不清参数。
            auto argument = Copy(target);
            DrawSaveLoadPageSlotLabelNativeAt528D90(Parent(page), argument, description.value, mode, services);
            descriptionLifetime.Finish();
            }
            if (!contentFirst && isNew && skin.newMarker[mode?0:1])
                DrawSlotDecoration(page, target, mode, skin, services);
        }
        else
        {
            // 52B1E5先bias，52B1E9后base，52B1F2再control；不是统一copy读取顺序。
            const volatile auto& rawNoData = At(Parent(page).noDataAt138.begin, std::uint32_t(mode ^ 1));
            const auto bias = rawNoData.biasAt04;
            const auto base = rawNoData.baseAt00;
            auto* control = rawNoData.controlAt08;
            Slot noData;
            noData.baseAt00 = base; noData.biasAt04 = bias; noData.controlAt08 = control;
            if (control) ++control->referenceCountAt04;
            RefLifetime noDataLifetime{noData};
            auto* noDataControl = noData.controlAt08;
            auto* image = Resolve<GraphicsSurfaceNativeBody>(noData);
            Blit(Resolve<GraphicsSurfaceNativeBody>(target), image, skin.noDataOrigin.x, skin.noDataOrigin.y, nullptr, services);
            noDataLifetime.FinishCached(noDataControl);
            if (contentFirst)
            {
                const auto& markers = Parent(page).newMarkerAt150;
                if (permanentDecoration && Count(markers.begin, markers.end))
                    DrawSlotDecoration(page, target, mode, skin, services);
                DrawSlotNumber(page, target, fileSlot, mode, skin, services);
            }
        }
        Slot child;
        page.common.vptrAt00->findChildAt14(&page.common, &child, Signed(slot + 41u));
        RefLifetime childLifetime{child};
        if (child.baseAt00)
        {
            auto* button = Required(Resolve<GuiControlNativeBody50h>(child));
            button->vptrAt00->writeStateAt44(button, available ? 1 : 0);
        }
        childLifetime.FinishClearing();
    }
    targetLifetime.FinishCached(targetControl);
    pageLifetime.FinishClearing();
    factoryLifetime.FinishUncleared();
}
}
