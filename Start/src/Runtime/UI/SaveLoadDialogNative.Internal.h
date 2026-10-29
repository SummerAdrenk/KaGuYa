#pragma once
#include <Runtime/SaveLoadDialogNative.h>
#include <Runtime/GraphicsSurfaceReference.h>

namespace KaGuYa::Start::Runtime::SaveLoadDialogNativeDetail
{
using Body=SaveLoadDialogNativeBody184h;
using Slot=YtlReferenceSlotRuntime;
void DestroyCompletedMembers(Body&) noexcept;
void ReleaseWithoutClearing(Slot&);
struct RefUnwind final
{
    Slot& slot;
    bool armed=true;
    ~RefUnwind() noexcept{if(armed)ReleaseYtlReference(slot);}
    void Finish(YtlReferenceControlRuntime* captured) noexcept
    {
        armed=false;
        if(!captured)return;
        if(--captured->referenceCountAt04==0)
        {if(captured->owningAt08)captured->DestroyPayloadAtVslot00();captured->DeleteControlAtVslot04(1);}
        slot.controlAt08=nullptr;slot.baseAt00=0;
    }
};
}
