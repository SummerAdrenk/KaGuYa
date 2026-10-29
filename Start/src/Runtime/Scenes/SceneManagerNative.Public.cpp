#include "SceneManagerNative.Internal.h"
#include <new>

namespace KaGuYa::Start::Runtime
{
using namespace SceneManagerNativeDetail;
void ConfigureSceneManagerNativeAt4703D0(SceneManagerNativeBody48h& body,std::int32_t mode,SceneManagerNativeServices& services)
{
    SceneManagerNativeScope scope(services);
    auto* button=NewModeSelectButtonNativeAt4703F9(mode,services.mode);
    Replace(body.modeAt20,button,OwnedBody::Mode,0x10,services);
    button=Resolve<ModeSelectButtonNativeBody10h>(body.modeAt20);button->vptrAt00->resetAt04(button);
    if(mode==2){ReleaseYtlReference(body.dialogAt2C);return;}
    if(!Read(body.dialogAt2C.baseAt00))
    {
        auto* raw=services.memory.AllocateBody(0x8C);
        OptionCharSelectDialogNativeBody8Ch* dialog=nullptr;
        if(raw)
        {
            struct Unwind final
            {void* raw;IEnvironmentRootMemory& memory;bool armed=true;~Unwind() noexcept{if(armed)memory.DeleteBody(raw,0x8C);}} unwind{raw,services.memory};
            dialog=ConstructOptionCharSelectDialogAt4E4A20(::new(raw) OptionCharSelectDialogNativeBody8Ch);
            unwind.armed=false;
        }
        Replace(body.dialogAt2C,dialog,OwnedBody::Dialog,0x8C,services);
    }
    auto* dialog=Resolve<GuiDialogNativeBody5Ch>(body.dialogAt2C);dialog->vptrAt00->beginAt44(dialog);
}
void ClearSceneManagerPublicNativeAt46DB90(SceneManagerNativeBody48h& body)
{ReleaseYtlReference(body.modeAt20);ReleaseYtlReference(body.dialogAt2C);}
YtlReferenceSlotRuntime* BorrowModeButtonsNativeAt47B7B0(SceneManagerNativeBody48h& body) noexcept
{
    const auto base=Read(body.modeAt20.baseAt00);
    if(!base)return reinterpret_cast<YtlReferenceSlotRuntime*>(4);
    auto* control=static_cast<EnvironmentObjectReferenceControl*>(Read(body.modeAt20.controlAt08));
    const auto stride=control?Read(control->strideAt0C):0u;
    return reinterpret_cast<YtlReferenceSlotRuntime*>(base+Read(body.modeAt20.biasAt04)*stride+4u);
}
YtlReferenceSlotRuntime* BorrowOptionCharDialogNativeAt478A30(SceneManagerNativeBody48h& body) noexcept
{
    // 原478A30返回Manager+2C的借用Ref地址，而不是增持或返回现代selector。
    return &body.dialogAt2C;
}
}
