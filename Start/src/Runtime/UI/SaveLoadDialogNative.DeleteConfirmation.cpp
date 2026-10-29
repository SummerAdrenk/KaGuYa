#include "SaveLoadDialogNative.Internal.h"
#include <Runtime/SaveLoadDialogNativeCommands.h>
#include <Runtime/SaveLoadMessageBoxNative.h>
#include <Runtime/SystemInformationDisplayNative.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/SaveFileCatalog.h>
#include <Runtime/SaveLoadResourceProfile.h>
#include <bit>

namespace KaGuYa::Start::Runtime
{
namespace
{
std::int32_t Increment(std::int32_t value) noexcept{return std::bit_cast<std::int32_t>(std::uint32_t(value)+1u);}
}
std::int32_t ConfirmSaveLoadDeleteNativeAt51B8F0(SaveLoadDialogNativeBody184h& body,
    ISaveLoadDialogNativeDeleteConfirmationSource& source,SaveLoadDialogNativeDeleteExecution execute,void* services)
{
    auto* env=source.CaptureEnvironmentAt4180C0();
    const auto bytes=std::uint32_t(reinterpret_cast<std::uintptr_t>(env->archivesAt30.end)-reinterpret_cast<std::uintptr_t>(env->archivesAt30.begin));
    const auto count=std::uint32_t(std::bit_cast<std::int32_t>(bytes)/12);
    auto* save=count>1?ResolveEnvironmentObjectReference<void>(env->archivesAt30.begin[1]):nullptr;
    auto flag=*reinterpret_cast<const std::uint8_t*>(reinterpret_cast<std::uintptr_t>(save)+0x80u);
    if(body.slotIndexAt164>=0)
    {
        env=source.CaptureEnvironmentAt4180C0();save=ResolveEnvironmentArchiveAt417990(*env,1);
        flag=*reinterpret_cast<const std::uint8_t*>(reinterpret_cast<std::uintptr_t>(save)+0x7Fu);
    }
    if(!flag){execute(body,services);return 0;}
    auto* box=ResolveEnvironmentObjectReference<SaveLoadMessageBoxNativeBodyA8h>(body.deleteConfirmationAt168);
    if(body.pageModeAtE4==2)box->modeAt80=2;
    else if(body.pageModeAtE4==3)box->modeAt80=3;
    else
    {
        box->modeAt80=body.slotIndexAt164<0;
        struct Temporary
        {
            union{YtlReferenceSlotRuntime slot;};bool armed=false;
            Temporary() noexcept{}
            ~Temporary() noexcept{if(armed)ReleaseYtlReference(slot);}
            void Finish() noexcept{armed=false;SaveLoadDialogNativeDetail::ReleaseWithoutClearing(slot);}
        } page;
        reinterpret_cast<const SaveLoadDialogNativeVtable37*>(body.common.vptrAt00)->findDialogAt90(
            &body.common,&page.slot,2+(body.pageModeAtE4!=0));page.armed=true;
        auto* selected=ResolveEnvironmentObjectReference<GuiDialogNativeBody5Ch>(page.slot);
        const auto index=body.slotIndexAt164<=-2 ? -2-body.slotIndexAt164 :
            *reinterpret_cast<const std::int32_t*>(reinterpret_cast<std::uintptr_t>(selected)+0x7Cu);
        const auto* catalog=static_cast<SaveFileStateNativeArchive*>(
            ResolveEnvironmentArchiveAt417990(*source.CaptureEnvironmentAt4180C0(),6));
        const auto physical=SaveFilePhysicalPageNumber(*catalog,index);
        // 两次Prepare都另解当前Ref；page/slot用32-bit inc，不能改为宽整数加法。
        if(CurrentSaveLoadResourceProfile().deletion.numbered)
        {
            if(body.slotIndexAt164<0)
                PrepareSaveLoadMessageBoxPageNativeAt502950(&ResolveEnvironmentObjectReference<SaveLoadMessageBoxNativeBodyA8h>(body.deleteConfirmationAt168)->common,physical);
            else
                PrepareSaveLoadMessageBoxPageAndSlotNativeAt502B10(&ResolveEnvironmentObjectReference<SaveLoadMessageBoxNativeBodyA8h>(body.deleteConfirmationAt168)->common,physical,Increment(body.slotIndexAt164));
        }
        page.Finish();
    }
    (void)source.CaptureInformationAfter413BE0();
    if(RunNativeGuiModalReferenceAt5071D0(&body.deleteConfirmationAt168,source.ModalServices())==6)execute(body,services);
    return 0;
}
}
