#include "CgSceneNative.Internal.h"
#include <Runtime/CgOperationResourceProfile.h>

namespace KaGuYa::Start::Runtime::CgSceneNativeDetail
{
namespace
{
template<class F>F Virtual(void* p,unsigned offset) noexcept
{return Read(Field<F>(Read(Field<void*>(p,0)),offset));}
bool RightPressed(ICgSceneNativeSource& source,unsigned site)
{
    auto* mouse=Resolve<MouseManagerNativeBody3Ch>(source.CaptureInputAt418210(site)->mouseAt00);
    return !Read(mouse->previousAt30[1])&&Read(mouse->pressedAt10[1])&&!Read(mouse->suppressionAt28);
}
void Mode(ICgSceneNativeSource& source,unsigned site,unsigned value){Field<unsigned>(Registry(source,2,site),4)=value;}
void Cue(CgSceneNativeServices& services)
{
    auto& source=services.source;
    auto* parameters=Resolve<void>(Field<Slot>(Registry(source,1,0x4B7956),0x1C));
    const auto* name=services.buttons.source.CueNameAt(0x61A56C);
    source.EnsureRootAt413BE0(0x4B7976);
    auto* table=Read(Field<void*>(parameters,0));
    auto* root=source.RootAt6198C4(0x4B797F);
    const auto volume=Read(Field<const int*(__thiscall*)(void*)>(table,0x24));
    const auto value=Read(*volume(parameters));
    auto* sound=Resolve<void>(source.RootSoundSlotAt24(root));source.PlayCueAt4BCD70(sound,name,0,value,1);
}
}
void PollDisplay(Body& body,CgSceneNativeServices& services)
{
    auto& source=services.source;const auto command=PollCgOperationWindowNativeAt4BD590(*Operation(body),services.operation);
    switch(command)
    {
    case 4:Mode(source,0x4B793A,0);body.phaseAt04=14;Cue(services);return;
    case 1:Mode(source,0x4B79A4,0);BeginCgDisplayWindowNativeAt4AA860(*Display(body),services.display);return;
    case 5:
        Mode(source,0x4B79D0,5);
        if(!HitCgOperationWindowNativeAt4C2690(*Operation(body),services.operation))EndCgDisplayWindowDragNativeAt4C13A0(*Display(body),0,services.display);
        return;
    case 6:
    {
        const auto selected=Read(Resolve<CgOperationDialogNativeBodyA8h>(Operation(body)->dialogAt24)->selectionAt7C);
        auto* end=Read(body.selectionAt18.pairsAt14.end);auto* begin=Read(body.selectionAt18.pairsAt14.begin);
        const auto count=std::bit_cast<int>(reinterpret_cast<std::uintptr_t>(end)-reinterpret_cast<std::uintptr_t>(begin))>>3;
        for(int i=0;i<count;i=Add(i,1))
        {
            if(Read(Advance(begin,i)->ordinalAt00)!=selected)continue;
            if(Read(body.selectionAt18.selectedPairAt10)!=i)
            {
                body.selectionAt18.selectedPairAt10=i;Mode(source,0x4B7A71,0);
                BeginCgDisplayWindowNativeAt4AA860(*Display(body),services.display);body.phaseAt04=9;
            }
            return;
        }
        return;
    }
    case 3:
        if(CurrentCgOperationResourceProfile().hideRequiresNotDragging&&Read(Display(body)->draggingAt54))
        {UpdateCgDisplayWindowNativeAt4BD050(*Display(body),services.display);return;}
        SetCgOperationWindowVisibilityNativeAt4C22B0(*Operation(body),0);body.phaseAt04=13;Mode(source,0x4B7AE3,0);break;
    default:break;
    }
    if(!Read(Display(body)->draggingAt54)&&HitCgOperationWindowNativeAt4C2690(*Operation(body),services.operation))return;
    UpdateCgDisplayWindowNativeAt4BD050(*Display(body),services.display);
}
void PollHidden(Body& body,CgSceneNativeServices& services)
{
    auto& source=services.source;UpdateCgDisplayWindowNativeAt4BD050(*Display(body),services.display);
    if(CurrentCgOperationResourceProfile().hideRequiresNotDragging&&Read(Display(body)->draggingAt54))return;
    auto* keyboard=Resolve<KeyboardManagerNativeBody438h>(source.CaptureInputAt418210(0x4B7BF0)->keyboardAt18);
    if(!ReadKeyboardPressedAt47F4E0(*keyboard,0)&&!RightPressed(source,0x4B7C07))return;
    auto* operation=Operation(body);RECT bounds;
    const auto* methods=reinterpret_cast<const LayerItemNativeFrameVtable*>(Read(operation->window.layer.vptrAt00));
    operation->window.layer.enabledAt0C=1;methods->boundsAt0C(&operation->window.layer,&bounds);
    auto* parent=Read(operation->window.layer.parentAt10);Read(parent->vptrAt00)->addDirtyAt20(parent,&bounds);
    Mode(source,0x4B7C48,5);body.phaseAt04=12;
}
}
