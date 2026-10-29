#include "SaveLoadDialogNative.Internal.h"
#include <Runtime/SaveLoadDialogNativeWorkflow.h>
#include <Runtime/GuiDialogIntoNative.h>
#include <cstring>

namespace KaGuYa::Start::Runtime
{
void BeginSaveLoadDialogNativeAt50E8B0(SaveLoadDialogNativeBody184h& body,ISaveLoadDialogNativeBeginSource& source,
    ISaveLoadDialogNativePageFactory& factory)
{
    struct Temporary
    {
        union{YtlReferenceSlotRuntime slot;};bool armed=false;
        Temporary() noexcept{}
        ~Temporary() noexcept{if(armed)ReleaseYtlReference(slot);}
        void Finish() noexcept{armed=false;SaveLoadDialogNativeDetail::ReleaseWithoutClearing(slot);}
    } background;
    auto* returned=source.CaptureBackgroundAfter413BE0(&background.slot);
    background.armed=true; // 50E916成功返回Capture才有EH资格。
    CopyAssignGraphicsSurfaceReferenceAt465870(body.capturedAt88,*returned);
    background.Finish();
    ClearGuiClickedEventFlagsAt532200(*ResolveEnvironmentObjectReference<GuiClickedEventReferences>(body.eventsAtEC));
    auto& common=body.common;
    common.vptrAt00->writeEnabledAt24(&common,1);
    common.vptrAt00->resetInputAt3C(&common);
    GuiDialogBaseVtable().resetChildrenAt68(&common); // 原直接522E90，不经派生重新选槽。
    InitializeSaveLoadDialogPageNativeAt5330A0(body,body.pageModeAtE4,factory);
    OpenSaveLoadDialogChildrenNativeAt532560(body);
    body.phaseAt6C=0;body.resultAt68=0;
    // 原50EA19被IDA误命名为threadpool；原PE E8目标418210，实际读取同一input.mouse Ref。
    auto* input=source.CaptureInputAt418210();
    auto* mouse=ResolveEnvironmentObjectReference<MouseManagerNativeBody3Ch>(input->mouseAt00);
    constexpr std::uint16_t zero=0;
    auto* bytes=reinterpret_cast<std::byte*>(mouse);
    std::memcpy(bytes+0x11,&zero,2);mouse->pressedAt10[0]=0;
    std::memcpy(bytes+0x31,&zero,2);mouse->previousAt30[0]=0;
    std::memcpy(bytes+0x14,&zero,2);mouse->latchedAt13[0]=0;
    mouse->secondaryAt2C=0;mouse->suppressionAt28=0;
    mouse->logicalAt18.y=0;mouse->logicalAt18.x=0;
    mouse->movedAt33=0;mouse->wheelAt34=0;
}
}
