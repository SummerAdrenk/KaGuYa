#include "SaveLoadDialogNative.Internal.h"
#include <Runtime/SaveLoadDialogNativeData.h>
#include <Runtime/SaveLoadDialogNativeScope.h>
#include <Runtime/SaveLoadResourceProfile.h>
#include <new>
#include <bit>

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace SaveLoadDialogNativeDetail;
struct RawUnwind
{void* raw;std::size_t size;bool armed=true;~RawUnwind() noexcept{if(armed)::operator delete(raw,size);}};
GraphicsSurfaceNativeBody* NewSurface(SaveLoadDialogNativeConstructionServices& services)
{
    auto* raw=::operator new(0x24);RawUnwind cleanup{raw,0x24};
    auto* created=raw?::new(raw)GraphicsSurfaceNativeBody(services.surfaces,services.strings):nullptr;
    cleanup.armed=false;return created;
}
void Initialize(Body& b,const SaveLoadDialogNativeVtable37& table)
{
    b.dialogsAt5C={};b.common.vptrAt00=&table.common;
    b.boundsAt78={};b.capturedAt88={};b.canvasAt94={};b.blackAtA0={};
    auto& op=b.operationAtAC;
    op.destination=0;op.previous=0;op.source=nullptr;op.mask=0;op.y=0;op.x=0;
    op.clip=nullptr;op.opcode=-1;op.targetKind=0;op.current=0;op.target=255;op.range=0;
    b.eventsAtEC={};b.pageDigitsAtF8={};b.pageBoundsAt104={};b.pagePositionsAt114={};
    b.slotDigitsAt120={};b.dateDigitsAt12C={};b.noDataAt138={};b.noPreviewAt144={};b.newMarkerAt150={};
    b.deleteConfirmationAt168={};b.slotConfirmationAt174={};
}
void LoadSequence(GuiControlSurfaceReferences& output,std::span<const char* const> names,
    RECT* bounds,SaveLoadDialogNativeConstructionServices& services)
{
    for(std::size_t index=0;index!=names.size();++index)
    {
        Slot local{};RefUnwind lifetime{local};
        auto* fixedDraw=services.source.CaptureDrawAfter413BE0();
        const auto* name=names[index];
        if(name)AdoptGraphicsSurfaceAt407A80(local,NewSurface(services));
        auto* captured=local.controlAt08;
        auto* surface=ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(local);
        if(name&&!services.source.LoadIntoAt4800A0(fixedDraw,surface,name))throw 1;
        if(bounds&&name)
        {
            // 4EBD26重新取base，仍以先前ESI control算bias；不使用装载前固定的surface。
            const Slot selected{local.baseAt00,local.biasAt04,captured};
            auto* current=ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(selected);
            const RECT area{0,0,current->widthAt08,current->heightAt0C};
            ::UnionRect(bounds,bounds,&area);
        }
        const bool slow=output.end==output.capacity;
        if(slow)
        {
            // 4D83D0与4540A0都用43EA10退旧区，原增长/对齐/拷贝发布协议同一实现。
            (void)GrowGuiControlSurfaceReferencesAt4540A0(output,output.end,local);
            captured=local.controlAt08;
        }
        else
        {
            const Slot selected{local.baseAt00,local.biasAt04,captured};
            AppendGuiControlSurfaceReference(output,selected);
        }
        lifetime.Finish(captured);
    }
}
LONG Sub(LONG a,LONG b) noexcept{return std::bit_cast<LONG>(std::uint32_t(a)-std::uint32_t(b));}
template<std::uint32_t Size>
class DialogControl final:public EnvironmentObjectReferenceControl
{
public:
    explicit DialogControl(GuiDialogNativeBody5Ch* body) noexcept:EnvironmentObjectReferenceControl(body,UninitializedFieldsTag{}){}
    void DestroyPayloadAtVslot00() override
    {if(auto* body=static_cast<GuiDialogNativeBody5Ch*>(payloadAt14))body->vptrAt00->deletingDestructorAt04(body,1);}
    void DeleteControlAtVslot04(std::uint32_t flags) override{RetireEnvironmentObjectControl(this,flags);}
};
static_assert(sizeof(DialogControl<0xE4>)==0x18&&sizeof(DialogControl<0xA8>)==0x18);
template<std::uint32_t Size>
void CreateConfirmation(Slot& output,SaveLoadDialogNativeConstructionServices& services)
{
    auto* raw=::operator new(Size);GuiDialogNativeBody5Ch* created=nullptr;
    {
        RawUnwind cleanup{raw,Size};
        if(raw)
        {
            if constexpr(Size==0xE4)created=services.source.ConstructSlotConfirmationAt4EACA0(raw);
            else created=services.source.ConstructDeleteConfirmationAt4EC8A0(raw);
        }
        cleanup.armed=false;
    }
    ReleaseYtlReference(output);
    if(!created)return;
    if(output.controlAt08)output.controlAt08->DeleteControlAtVslot04(1);
    output.biasAt04=0;output.baseAt00=reinterpret_cast<std::uintptr_t>(created);
    auto* storage=::operator new(0x18);
    auto* control=storage?::new(storage)DialogControl<Size>(created):nullptr;output.controlAt08=control;
    *reinterpret_cast<volatile std::uint32_t*>(reinterpret_cast<std::uintptr_t>(control)+4u)=1;
    output.controlAt08->owningAt08=1;
    static_cast<EnvironmentObjectReferenceControl*>(output.controlAt08)->strideAt0C=Size;
    static_cast<EnvironmentObjectReferenceControl*>(output.controlAt08)->countAt10=1;
}
}
SaveLoadDialogNativeBody184h* ConstructSaveLoadDialogNativeAt4EB950(void* storage,std::uint32_t modeArgument,
    std::uint32_t,SaveLoadDialogNativeConstructionServices& services)
{
    SaveLoadDialogNativeConstructionScope scope(services);
    auto* body=::new(storage)Body;
    ConstructGuiDialogAt4E0BE0(&body->common,nullptr,-1);
    // 30h包的所有字段均由原ctor写入；先建立C++对象生命期，不触及周围原未写字节。
    ::new(static_cast<void*>(&body->operationAtAC))TransitOperation;
    Initialize(*body,services.dispatch);
    struct Unwind
    {Body& body;bool armed=true;~Unwind() noexcept{if(armed)DestroyCompletedMembers(body);}} cleanup{*body};
    auto* managerRaw=::operator new(0x0C);
    auto* manager=managerRaw?::new(managerRaw)GuiClickedEventReferences{}:nullptr;
    AdoptGuiClickedEventManagerAt4D7B90(body->eventsAtEC,manager);
    body->pageModeAtE4=modeArgument?0:services.source.ReadInitialPageFromEnvironmentAt4EBC35();
    RECT combined{};
    const auto& profile=CurrentSaveLoadResourceProfile();
    LoadSequence(body->pageDigitsAtF8,profile.page.digits,&combined,services);
    body->pageBoundsAt104={};
    const auto positions=profile.page.digitPositions;
    for(const auto& point:positions)
    {RECT area=combined;::OffsetRect(&area,point.x,point.y);::UnionRect(&body->pageBoundsAt104,&body->pageBoundsAt104,&area);}
    body->pagePositionsAt114.end=body->pagePositionsAt114.begin;
    // 4EBE88缓存首个top，6轮持续使用；原矩形已固定，不再按坐标重算包围框。
    const auto top=body->pageBoundsAt104.top,left=body->pageBoundsAt104.left;
    for(const auto& point:positions)
    {
        const POINT relative{Sub(point.x,left),Sub(point.y,top)};
        auto& out=body->pagePositionsAt114;
        if(out.end==out.capacity)(void)GrowSaveLoadPagePositionsNativeAt4D8D30(out,out.end,relative);
        else{out.end->x=relative.x;out.end->y=relative.y;++out.end;}
    }
    LoadSequence(body->slotDigitsAt120,profile.page.slots.numbers,nullptr,services);
    LoadSequence(body->dateDigitsAt12C,profile.page.slots.dateDigits,nullptr,services);
    LoadSequence(body->noDataAt138,profile.page.slots.noData,nullptr,services);
    LoadSequence(body->noPreviewAt144,profile.page.slots.noPreview,nullptr,services);
    LoadSequence(body->newMarkerAt150,profile.page.slots.newMarker,nullptr,services);
    AdoptGraphicsSurfaceAt407A80(body->canvasAt94,NewSurface(services));
    const auto height=services.source.CaptureEnvironmentAt4180C0()->heightAt1C;
    const auto width=services.source.CaptureEnvironmentAt4180C0()->widthAt18;
    if(!ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(body->canvasAt94)->CreateAt415010(
        services.surfaces,std::bit_cast<std::int32_t>(width),std::bit_cast<std::int32_t>(height),0,1))throw 1;
    auto* fixedDraw=services.source.CaptureDrawAfter413BE0();
    if(!services.source.LoadSurfaceAt480000(fixedDraw,&body->blackAtA0,"bg_black.bgd"))throw 1;
    const auto boundsHeight=services.source.CaptureEnvironmentAt4180C0()->heightAt1C;
    const auto boundsWidth=services.source.CaptureEnvironmentAt4180C0()->widthAt18;
    body->boundsAt78={0,0,std::bit_cast<LONG>(boundsWidth),std::bit_cast<LONG>(boundsHeight)};
    AdoptGuiInputAt536EA0(body->common.inputAt18,NewGameGuiInputAt4EAAED());
    CreateConfirmation<0xE4>(body->slotConfirmationAt174,services);
    CreateConfirmation<0xA8>(body->deleteConfirmationAt168,services);
    cleanup.armed=false;return body;
}
}
