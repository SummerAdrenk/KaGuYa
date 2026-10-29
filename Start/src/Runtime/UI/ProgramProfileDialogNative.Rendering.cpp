#include "ProgramProfileDialogNative.Internal.h"
#include <Runtime/GraphicsSurfaceRuntime.h>

namespace KaGuYa::Start::Runtime::ProgramProfileDetail
{
void Load(Slot& output, const char* name)
{
    if (!name) throw std::out_of_range("Program image index is outside its original table");
    auto& source = RequireProgramSelectDialogNativeServices().source;
    source.EnsureRootAt413BE0(0);
    auto* root = source.RootAt6198C4(0);
    if (!source.LoadSurfaceAt480000(Resolve(source.RootDrawSlotAt18(root)), &output, name, 0)) throw 1;
}
namespace
{
const char* Image(std::span<const char* const> names, int index)
{
    if (index < 0 || static_cast<std::size_t>(index) >= names.size() || !names[index])
        throw std::out_of_range("Program image selection outside the original domain");
    return names[index];
}
void Blit(Slot& dest, Slot& from, POINT position, const RECT* clip = nullptr)
{
    auto* target = Resolve<Surface>(dest); auto* image = Resolve<Surface>(from);
    if (!target || !image || !target->handleAt04 || !image->handleAt04) throw 1;
    if (!RequireProgramSelectDialogNativeServices().graphics.CompositionAt4182F0().BlitSurface(
        target->handleAt04, image->handleAt04, position.x, position.y, nullptr, nullptr, clip, 0)) throw 1;
}
void Put(Panel& p, const char* name, const POINT* position)
{
    Ref r; Load(r.slot, name);
    const auto* surface = Resolve<Surface>(r.slot);
    Blit(p.base.backgroundAt74, r.slot, position ? *position : POINT{surface->originXAt10, surface->originYAt14});
}
}
void Calendar(Panel& p)
{
    if (p.role != Role::Main) return;
    const auto& cal = p.owner->mode->calendar;
    if (!cal.monthImages.empty()) Put(p, Image(cal.monthImages, Value("month")), nullptr);
    if (!cal.timeImages.empty()) Put(p, Image(cal.timeImages, Value("time")), nullptr);
    if (cal.digits.empty()) return;
    if (cal.monthDigits)
    {
        const int month = Value("month");
        Put(p, Image(cal.digits, month % 10), &cal.positions[0]);
        if (!cal.monthModuloOnly && (!cal.suppressLeadingZero || month / 10))
            Put(p, Image(cal.digits, month / 10), &cal.positions[1]);
    }
    const int date = Value("date");
    Put(p, Image(cal.digits, date % 10), &cal.positions[2]);
    if (!cal.suppressLeadingZero || date / 10) Put(p, Image(cal.digits, date / 10), &cal.positions[3]);
}
void DrawPanel(Panel& p, const RECT* clip)
{
    if (p.role == Role::Confirmation && p.base.backgroundAt74.baseAt00)
    {
        auto* bg = Resolve<Surface>(p.base.backgroundAt74);
        if (bg->handleAt04)
            static_cast<void>(RequireProgramSelectDialogNativeServices().graphics.CompositionAt4182F0().DrawSurface(
                bg->handleAt04, 0, 0, nullptr, nullptr, clip, 0));
    }
    GuiDialogBaseVtable().drawAt60(&p.base.common, clip);
}
void CompositePanel(Panel& p, Slot owned, const RECT* clip)
{
    Ref target; target.slot = owned;
    if (p.role == Role::Confirmation && p.base.backgroundAt74.baseAt00)
        Blit(target.slot, p.base.backgroundAt74, {0, 0}, clip);
    GuiDialogBaseVtable().compositeAt5C(&p.base.common, Copy(target.slot), clip);
}
}
