#include "SaveLoadMessageBoxNative.Internal.h"
#include <Runtime/GraphicsSurfaceCaptureRuntime.h>
#include <Runtime/SpriteNativeStorage.h>
#include <Runtime/SaveLoadResourceProfile.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace SaveLoadMessageBoxNativeDetail;
using Surface = GraphicsSurfaceNativeBody;
using Group = GuiControlResourceNativeBody38h;

void SelectSurfaceAt50B1F0(Slot& output, Group* group)
{
    output = {};
    LocalReference cleanup{output};
    const auto index = Read(group->slotIndicesAt00[0]);
    if (index >= 0) CopyAssignGraphicsSurfaceReferenceAt465870(output, group->surfacesAt1C.begin[index]);
    cleanup.active = false;
}
void Restore(Slot& destination, Slot& cache, SaveLoadMessageBoxNativeServices& s)
{
    if (!Read(cache.baseAt00))
    {
        FreshReference copied;
        CloneGraphicsSurfaceAt444470(copied.slot, *Resolve<Surface>(destination), 1,
            s.surfaces, s.copy, s.strings);
        LocalReference lifetime{copied.slot}; // Prepare state3，Clone返回后才有资格。
        CopyAssignGraphicsSurfaceReferenceAt465870(cache, copied.slot);
        lifetime.Current(); // 原40D2F0为noexcept叶；无Create失败的现代异常兜底。
    }
    auto* selected = Resolve<Surface>(destination);
    const auto handle = Read(selected->handleAt04);
    if (handle)
    {
        const auto color = Read(selected->clearColorAt20);
        s.graphics.CompositionAt4182F0().ClearSurface(handle, color, nullptr);
    }
    // 原先取cache源handle，再重解destination，443A00不检查handle/Initialized。
    const auto source = Read(Resolve<Surface>(cache)->handleAt04);
    const auto target = Read(Resolve<Surface>(destination)->handleAt04);
    s.copy.SurfaceCopyBlitRaw(target, source, 0, 0, nullptr, nullptr, nullptr, 0);
}
void Prepare(Common* p, std::int32_t page, std::int32_t slot, bool both)
{
    auto& b = Dialog(p);
    if (Read(b.modeAt80) != (both ? 0 : 1)) return; // 门控前不取任何Source/DLL叶。
    FreshReference child;
    p->vptrAt00->findChildAt14(p, &child.slot, both ? 18 : 19);
    LocalReference childLifetime{child.slot}; // state0。
    if (!Read(child.slot.baseAt00)) throw 1;
    FreshReference group;
    auto* selected = Resolve<GuiControlNativeBody50h>(child.slot);
    CopyGuiControlResourceReference(group.slot, *Read(selected->resourcesAt28.begin));
    LocalReference groupLifetime{group.slot}; // state1；空vector仍保留原非法访问。
    if (!Read(group.slot.baseAt00)) throw 2;
    FreshReference surface;
    SelectSurfaceAt50B1F0(surface.slot, Resolve<Group>(group.slot));
    LocalReference surfaceLifetime{surface.slot}; // state2。
    if (!Read(surface.slot.baseAt00)) throw 3;
    auto& s = RequireSaveLoadMessageBoxNativeServices();
    Restore(surface.slot, both ? b.pageAndSlotBaseAt84 : b.pageOnlyBaseAt90, s);
    DrawSaveLoadMessageBoxNumberNativeAt529FD0(p, &surface.slot, page, 1);
    if (both) DrawSaveLoadMessageBoxNumberNativeAt529FD0(p, &surface.slot, slot, 0);
    surfaceLifetime.Current();
    groupLifetime.Current();
    childLifetime.Current();
}
void BlitDigit(Common* p, Slot& destination, std::uintptr_t destinationBase,
    std::int32_t digit, const POINT& position, LONG originX, LONG originY)
{
    // 原不限制digit范围；负数/超范围沿原DWORD指针算术，不变成成功或clamp。
    auto* source = Indexed<Surface>(Dialog(p).digitsAt9C, digit);
    auto* target = ResolveWithBase<Surface>(destination, destinationBase);
    const auto x = Sub(position.x, originX), y = Sub(position.y, originY);
    const auto targetHandle = Read(target->handleAt04);
    if (!targetHandle) return;
    const auto sourceHandle = Read(source->handleAt04);
    if (!sourceHandle) return;
    (void)RequireSaveLoadMessageBoxNativeServices().graphics.CompositionAt4182F0().BlitSurface(
        targetHandle, sourceHandle, x, y, nullptr, nullptr, nullptr, 0);
}
}
void PrepareSaveLoadMessageBoxPageNativeAt502950(Common* p, std::int32_t page)
{ Prepare(p, page, 0, false); }
void PrepareSaveLoadMessageBoxPageAndSlotNativeAt502B10(Common* p, std::int32_t page, std::int32_t slot)
{ Prepare(p, page, slot, true); }
void DrawSaveLoadMessageBoxNumberNativeAt529FD0(Common* p, Slot* destination,
    std::int32_t number, std::uint32_t pageFlag)
{
    auto& b = Dialog(p);
    if (!Read(b.digitsAt9C.baseAt00))
    {
        auto& s = RequireSaveLoadMessageBoxNativeServices();
        // 52A007..52A0FB与444FE0同一cookie/24h数组/18h Ref发布协议，直接复用。
        // 只保护完整Surface元素构造；Ref control失败和后续Load失败不回滚已发布前缀。
        ReplaceSpriteSurfaceArrayAt444FE0(b.digitsAt9C, 10, s.surfaces, s.strings);
        const auto names = CurrentSaveLoadResourceProfile().deletion.digits;
        if(names.size()!=10)throw std::logic_error("delete confirmation requires ten digit resources");
        for (std::int32_t i = 0; i != 10; ++i)
        {
            auto* selected = Indexed<Surface>(b.digitsAt9C, i); // Ensure之前先固定receiver。
            auto* receiver = s.loader.CaptureLoadReceiverAt413BE0();
            if (!s.loader.LoadIntoAt4800A0(receiver, *selected, names[i])) throw 1;
        }
    }
    auto base = Read(destination->baseAt00);
    auto* selected = ResolveWithBase<Surface>(*destination, base);
    const auto x = Read(selected->originXAt10), y = Read(selected->originYAt14);
    const auto positions = CurrentSaveLoadResourceProfile().deletion.digitPositions;
    if(positions.size()!=5&&positions.size()!=6)throw std::logic_error("delete confirmation digit layout is incomplete");
    const auto pageDigits=positions.size()-2;
    const auto count=std::uint8_t(pageFlag)?pageDigits:2u;
    const auto start=std::uint8_t(pageFlag)?0u:pageDigits;
    int divisor=1;
    for(std::size_t i=1;i<count;++i)divisor*=10;
    for(std::size_t i=0;i<count;++i)
    {
        const auto digit=number/divisor;
        if(digit<0||digit>9)throw std::out_of_range("delete confirmation number exceeds layout");
        BlitDigit(p,*destination,base,digit,positions[start+i],x,y);
        number%=divisor;divisor/=10;
        base = Read(destination->baseAt00);
    }
}
}
