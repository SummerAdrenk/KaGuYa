#include <Runtime/SaveLoadDialogNativeWorkflow.h>
#include <Runtime/GraphicsSurfaceReference.h>

namespace KaGuYa::Start::Runtime
{
std::int32_t PollSaveLoadDialogNativeAt52F7A0(SaveLoadDialogNativeBody184h& body,
    ISaveLoadDialogNativeCatalogSource& catalog,ISaveLoadDialogNativePageFactory& pages,
    ISaveLoadDialogNativeTransitionSource& transition,IGraphicsTransitApi& graphics,
    const SaveLoadDialogNativeCommandEntries& commands)
{
    switch(body.phaseAt6C)
    {
    case 0:body.phaseAt6C=1;body.transitionPhaseAt70=0;[[fallthrough]];
    case 1:case 2:
        AdvanceSaveLoadDialogNativeAt51CA80(body,transition,graphics);return 1;
    case 3:ProcessSaveLoadDialogEventsNativeAt522350(body,catalog,pages);break;
    case 4:body.phaseAt6C=-1;return 0;
    case 5:
        if(!commands.returnTitleAt51E320(body,commands.externalServices))body.phaseAt6C=3;
        break;
    case 6:
        if(!commands.deleteAt51B8F0(body,commands.externalServices))body.phaseAt6C=3;
        break;
    case 7:
        if(!commands.saveAt525480(body,commands.externalServices))body.phaseAt6C=3;
        break;
    case 8:
        if(!commands.loadAt520AB0(body,commands.externalServices))body.phaseAt6C=3;
        else
        {
            CopyAssignGraphicsSurfaceReferenceAt465870(body.capturedAt88,body.blackAtA0);
            body.phaseAt6C=2;body.transitionPhaseAt70=0;
        }
        break;
    default:return 0;
    }
    body.common.vptrAt00->flushDirtyAt58(&body.common);return 1;
}
}
