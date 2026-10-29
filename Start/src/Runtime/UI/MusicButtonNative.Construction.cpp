#include "MusicButtonNative.Internal.h"
#include <new>

namespace KaGuYa::Start::Runtime::MusicButtonNativeDetail
{
namespace
{
class DialogControl final:public EnvironmentObjectReferenceControl
{
public:
    explicit DialogControl(GuiDialogNativeBody5Ch* p) noexcept:EnvironmentObjectReferenceControl(p,UninitializedFieldsTag{}){}
    void DestroyPayloadAtVslot00() override{auto* p=static_cast<GuiDialogNativeBody5Ch*>(payloadAt14);if(p)Read(p->vptrAt00)->deletingDestructorAt04(p,1);}
    void DeleteControlAtVslot04(unsigned f) override{RetireEnvironmentObjectControl(this,f);}
};
static_assert(sizeof(DialogControl)==0x18);
}
void AdoptDialog(Slot& r,GuiDialogNativeBody5Ch* p)
{
    if(auto* old=Read(r.controlAt08))old->DeleteControlAtVslot04(1);
    r.biasAt04=0;r.baseAt00=reinterpret_cast<std::uintptr_t>(p);
    auto* raw=RequireMusicButtonNativeServices().mode.memory.AllocateBody(0x18);
    auto* c=raw?::new(raw) DialogControl(p):nullptr;r.controlAt08=c;
    *reinterpret_cast<volatile unsigned*>(reinterpret_cast<std::uintptr_t>(c)+4u)=1;
    r.controlAt08->owningAt08=1;
    static_cast<EnvironmentObjectReferenceControl*>(r.controlAt08)->strideAt0C=0x84;
    static_cast<EnvironmentObjectReferenceControl*>(r.controlAt08)->countAt10=1;
}
}
namespace KaGuYa::Start::Runtime
{
MusicButtonNativeBody70h* ConstructMusicButtonNativeAt4A1C70(MusicButtonNativeBody70h& p,MusicButtonNativeServices& s)
{
    using namespace MusicButtonNativeDetail;MusicButtonNativeScope scope(s);
    ::new(&p.common) ModeButtonNativeBody50h;ConstructModeButtonNativeAt4A2E60(p.common,s.mode);p.common.vptrAt00=&MusicButtonNativeVtable();
    p.dialogAt50={};p.reservedAt5C={};
    struct Prefix final
    {
        Body& p;bool active=true;
        ~Prefix() noexcept{if(active){ReleaseYtlReference(p.reservedAt5C);ReleaseYtlReference(p.dialogAt50);DestroyModeButtonNativeAt4A4D90(p.common);}}
    }prefix{p};
    try
    {
        ReleaseYtlReference(p.dialogAt50);
        auto* raw=s.mode.memory.AllocateBody(0x84);
        GuiDialogNativeBody5Ch* dialog=nullptr;
        {
            struct Allocation final{void* raw;IEnvironmentRootMemory& memory;bool active=true;~Allocation() noexcept{if(active)memory.DeleteBody(raw,0x84);}} allocation{raw,s.mode.memory};
            dialog=raw?s.dialogs.ConstructMusicDialogAt4E3900(raw):nullptr;allocation.active=false;
        }
        AdoptDialog(p.dialogAt50,dialog);auto* selected=Dialog(p);Read(selected->vptrAt00)->beginAt44(selected);
        p.common.skippedGuiAt40=2;
        auto* returned=s.source.PublicGuiAt47B7B0(Scenes(s.source,0x4A1DCC));
        if(&p.common.publicGuiAt44!=returned)
        {
            ReleaseYtlReference(p.common.publicGuiAt44);
            p.common.publicGuiAt44={Read(returned->baseAt00),Read(returned->biasAt04),Read(returned->controlAt08)};
            if(p.common.publicGuiAt44.controlAt08)++p.common.publicGuiAt44.controlAt08->referenceCountAt04;
        }
        p.common.itemCountAt3C=static_cast<std::int32_t>(s.dialogs.NativeDialogServices().resources.songs.size());
        p.soundSelectionAt68=-1;p.common.releaseStateAt10=0;
        p.common.selectedAt04=-1;p.common.directionAt08=-1;p.common.confirmLatchAt0C=0;
        const auto height=Read(s.source.CaptureEnvironmentAt4180C0(0x4A1E7F)->heightAt1C);
        const auto width=Read(s.source.CaptureEnvironmentAt4180C0(0x4A1E87)->widthAt18);
        const RECT rect{0,0,std::bit_cast<int>(width),std::bit_cast<int>(height)};
        if(!::IsRectEmpty(&rect)){auto* dirty=Dirty(p);Read(dirty->vptrAt00)->addRectAt08(dirty,&rect);}
        SynchronizeMusicButtonNativeAt4ABB40(p);p.movieAt6C=0;
    }
    catch(...)
    {
        // 原try states3..4→catch4A1EF8；只清继承子项，不补写count/selected/movie或退已发布Dialog。
        ClearModeButtonChildrenNativeAt4A8490(p.common);
    }
    prefix.active=false;return &p;
}
}
