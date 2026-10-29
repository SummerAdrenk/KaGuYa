#include "LikabilitySceneNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
void UpdateLikabilitySceneNativeAt4B6280(LikabilitySceneNativeBody20h& body,LikabilitySceneNativeServices& services)
{
    using namespace LikabilitySceneNativeDetail;LikabilitySceneNativeScope scope(services);auto& source=services.source;
    switch(Read(body.phaseAt04))
    {
    case 0:
    {
        auto* mode=Mode(source,0x4B62ED);Field<unsigned>(mode,4)=Read(body.savedModeAt08);
        auto* gui=Resolve<GuiDialogNativeBody5Ch>(body.dialogAt10);Read(gui->vptrAt00)->beginAt44(gui);
        Field<unsigned>(Mode(source,0x4B6329),4)=0;body.phaseAt04=1;
        // 原4B6343直接落入Poll，不在Begin/Env回调之后重分派phase。
        [[fallthrough]];
    }
    case 1:
    {
        auto* gui=Resolve<GuiDialogNativeBody5Ch>(body.dialogAt10);
        if(!Read(gui->vptrAt00)->pollAt48(gui))body.phaseAt04=2;return;
    }
    case 2:
    {
        source.AdvanceScenesAt4C1760(Scenes(source,0x4B62A2));
        auto* mode=Mode(source,0x4B62D5);Field<unsigned>(mode,4)=Read(body.savedModeAt08);return;
    }
    default:return; // 原域外不推进，不添加恢复或成功分支。
    }
}
}
