#include "SaveLoadPageSlotNative.Internal.h"
#include <Runtime/SaveLoadResourceProfile.h>
#include <Runtime/SaveFileCatalog.h>
#include <Runtime/EnvironmentArchiveRegistration.h>

namespace KaGuYa::Start::Runtime
{
void DrawSaveLoadPageSlotLabelNativeAt528D90(SaveLoadDialogNativeBody184h& parent,
    YtlReferenceSlotRuntime surface, const RuntimeCString& label,
    std::int32_t saveMode, SaveLoadPageSlotNativeServices& services)
{
    using namespace SaveLoadPageSlotNativeDetail;
    (void)parent; // 原ECX在入口后不读取。
    GraphicsSurfaceNativeScope scope(services.surfaces);
    RefLifetime lifetime{surface};
    if (label.GetLength() != 0)
    {
        // 5F55D4/5F55A0/5F5590：catch(...)从40C6E0之前开始，空串检查在外。
        try
        {
            GraphicsTextNativeFontParameters font;
            const auto& style=CurrentSaveLoadResourceProfile().page.slots.labelStyle;
            InitializeGraphicsTextFontNativeAt40C6E0(font);
            font.fontAt00.lfHeight = style.height;
            font.fontAt00.lfWeight = style.weight;
            ::lstrcpynA(font.fontAt00.lfFaceName,style.face,32);
            font.lineSpacingAt3D = style.lineSpacing;
            font.renderModeAt3E = style.renderMode;
            font.textColorAt40 = style.colors[saveMode?0:1];
            font.shadowColorAt44 = 0xFFFFFFFFu;
            font.shadowXAt4C = 0;
            font.shadowYAt50 = 0;
            font.characterSpacingAt3C = style.characterSpacing;
            if (!services.source.CreateTextFontAt4388B0(&font)) throw 1;
            if (!services.source.InitializeTextAt4389D0(label.GetString(), style.origin.x, style.origin.y)) throw 1;
            const auto handle = Required(Resolve<GraphicsSurfaceNativeBody>(surface))->handleAt04;
            if (!services.source.BlitAllTextAt53A980(handle, nullptr)) throw 1;
        }
        catch (...) {}
    }
    lifetime.FinishUncleared();
}
void RefreshSaveLoadPageHoverNative(SaveLoadPageSecondNativeBodyACh& page,
    SaveLoadPageSlotNativeServices& services,bool force)
{
    using namespace SaveLoadPageSlotNativeDetail;
    const auto& profile=CurrentSaveLoadResourceProfile();
    if(profile.page.slots.labelKind!=SaveLoadLabelKind::SharedHover)return;
    int slot=-1;
    if(const auto* active=page.common.activeAt4C)
    {
        const auto event=active->eventAt04;
        if(event>=25&&event<25+profile.catalog.slotsPerPage)slot=event-25;
        else if(event>=41&&event<41+profile.catalog.slotsPerPage)slot=event-41;
    }
    if(!force&&slot==page.hoveredSlotIndex)return;
    auto* surface=Required(Resolve<GraphicsSurfaceNativeBody>(page.hoverSurface));
    if(surface->handleAt04)services.drawing.SurfaceClearRaw(surface->handleAt04,0,nullptr);
    if(slot>=0)
    {
        auto* catalog=static_cast<SaveFileStateNativeArchive*>(
            ResolveEnvironmentArchiveAt417990(*services.source.CaptureEnvironmentAt4180C0(),6));
        const auto id=page.common.vptrAt00->idAt0C(&page.common);
        const auto* selected=id==4?&catalog->QuickPage():id==5?&catalog->AutoPage():&catalog->RegularPages().at(page.pageAt7C);
        const auto physical=(id==4||id==5)?SaveFileRollingSlot(*catalog,id==5,slot):slot;
        const auto& item=selected->Items().at(physical);
        if(item.Available())
        {
            TextCell shortened;
            const auto& style=profile.page.slots.labelStyle;
            TruncateSaveLoadLabelNativeAt4DA8E0(&shortened.value,item.labelAt0C.GetString(),style.maxCharacters,style.maxLines,services.source);
            TextLifetime lifetime{shortened.value};
            DrawSaveLoadPageSlotLabelNativeAt528D90(Parent(page),Copy(page.hoverSurface),shortened.value,id==2,services);
        }
    }
    page.hoveredSlotIndex=slot;
    page.common.vptrAt00->invalidateAt64(&page.common,profile.page.slots.hoverBounds);
}
}
