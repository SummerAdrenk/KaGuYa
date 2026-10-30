#include <Runtime/GuiDirtyRectNative.h>

#include <algorithm>
#include <bit>
#include <cstdlib>
#include <exception>
#include <limits>
#include <new>
#include <optional>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Body = GuiDirtyRectNativeBody80h;
using Table = GuiDirtyRectNativeVtable;

std::int32_t Area(const RECT& rect) noexcept
{
    const auto width = static_cast<std::uint32_t>(rect.right) - static_cast<std::uint32_t>(rect.left);
    const auto height = static_cast<std::uint32_t>(rect.bottom) - static_cast<std::uint32_t>(rect.top);
    return std::bit_cast<std::int32_t>(width * height);
}

RECT Bounds(const RECT& a, const RECT& b) noexcept
{
    return {(std::min)(a.left, b.left), (std::min)(a.top, b.top),
        (std::max)(a.right, b.right), (std::max)(a.bottom, b.bottom)};
}

std::int32_t Cost(std::int32_t area, std::int32_t first, std::int32_t second) noexcept
{
    return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(area) -
        static_cast<std::uint32_t>(second) - static_cast<std::uint32_t>(first));
}

LONG AddDword(LONG a, LONG b) noexcept
{
    return std::bit_cast<LONG>(static_cast<std::uint32_t>(a) + static_cast<std::uint32_t>(b));
}

void CheckCount(std::uint32_t count)
{
    if (count > 5) throw std::out_of_range("dirty80 count is outside its native storage");
}

void __fastcall Clear(Body* self, void*) { self->ClearAt4418C0(); }
void __fastcall Coalesce(Body* self, void*) { self->CoalesceAt442060(); }
void __fastcall AddRect(Body* self, void*, const RECT* rect)
{
    const RECT values = *rect;
    self->vptrAt00->addLTRBAt0C(self, values.left, values.top, values.right, values.bottom);
}
void __fastcall AddLTRB(Body* self, void*, LONG left, LONG top, LONG right, LONG bottom)
    { self->AddAt441680(left, top, right, bottom); }
void __fastcall AddXYWH10(Body* self, void*, LONG x, LONG y, LONG width, LONG height)
    { self->vptrAt00->addLTRBAt0C(self, x, y, AddDword(x, width), AddDword(y, height)); }
void __fastcall AddXYWH14(Body* self, void*, LONG x, LONG y, LONG width, LONG height)
    { self->vptrAt00->addLTRBAt0C(self, x, y, AddDword(x, width), AddDword(y, height)); }
std::uint32_t __fastcall Count(Body* self, void*) { return self->countAt7C; }
RECT* __fastcall Rects(Body* self, void*) { return self->rectsAt04; }

// 退役虚槽仍触发 CRT 纯虚调用；各桥接保留该槽的栈参数和返回类型。
template<class Result, class... Args>
[[noreturn]] Result __fastcall PureSlot(Body*, void*, Args...)
{
    _purecall();
    std::terminate();
}
Body* __fastcall DeleteRetired(Body*, void*, std::uint32_t);
Body* __fastcall DeleteBody(Body*, void*, std::uint32_t);

// 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
// 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const Table retired{
    reinterpret_cast<decltype(Table::clearAt00)>(&PureSlot<void>),
    reinterpret_cast<decltype(Table::coalesceAt04)>(&PureSlot<void>),
    reinterpret_cast<decltype(Table::addRectAt08)>(&PureSlot<void, const RECT*>),
    reinterpret_cast<decltype(Table::addLTRBAt0C)>(&PureSlot<void, LONG, LONG, LONG, LONG>),
    reinterpret_cast<decltype(Table::addXYWHAt10)>(&PureSlot<void, LONG, LONG, LONG, LONG>),
    reinterpret_cast<decltype(Table::addXYWHAt14)>(&PureSlot<void, LONG, LONG, LONG, LONG>),
    reinterpret_cast<decltype(Table::countAt18)>(&PureSlot<std::uint32_t>),
    reinterpret_cast<decltype(Table::rectsAt1C)>(&PureSlot<RECT*>),
    reinterpret_cast<decltype(Table::deletingDestructorAt20)>(&DeleteRetired)};
const Table methods{
    reinterpret_cast<decltype(Table::clearAt00)>(&Clear),
    reinterpret_cast<decltype(Table::coalesceAt04)>(&Coalesce),
    reinterpret_cast<decltype(Table::addRectAt08)>(&AddRect),
    reinterpret_cast<decltype(Table::addLTRBAt0C)>(&AddLTRB),
    reinterpret_cast<decltype(Table::addXYWHAt10)>(&AddXYWH10),
    reinterpret_cast<decltype(Table::addXYWHAt14)>(&AddXYWH14),
    reinterpret_cast<decltype(Table::countAt18)>(&Count),
    reinterpret_cast<decltype(Table::rectsAt1C)>(&Rects),
    reinterpret_cast<decltype(Table::deletingDestructorAt20)>(&DeleteBody)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif

Body* __fastcall DeleteBody(Body* self, void*, std::uint32_t flags)
{
    self->vptrAt00 = &retired;
    if (flags & 1u) ::operator delete(self, std::size_t{0x80});
    return self;
}
Body* __fastcall DeleteRetired(Body* self, void*, std::uint32_t flags)
{
    self->vptrAt00 = &retired;
    if (flags & 1u) ::operator delete(self, std::size_t{4});
    return self;
}

class DirtyControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit DirtyControl(Body* body) noexcept
        : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        auto* body = static_cast<Body*>(payloadAt14);
        if (body) body->vptrAt00->deletingDestructorAt20(body, 1);
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
        { RetireEnvironmentObjectControl(this, flags); }
};
static_assert(sizeof(DirtyControl) == 0x18);
}

GuiDirtyRectNativeBody80h::GuiDirtyRectNativeBody80h() noexcept
    : vptrAt00(&methods), countAt7C(0) {}

void GuiDirtyRectNativeBody80h::ClearAt4418C0() noexcept { countAt7C = 0; }

void GuiDirtyRectNativeBody80h::AddAt441680(LONG left, LONG top, LONG right, LONG bottom)
{
    const auto count = countAt7C;
    CheckCount(count);
    auto& scratch = rectsAt04[count];
    ::SetRect(&scratch, left, top, right, bottom);
    if (::IsRectEmpty(&scratch)) return;
    areasAt64[count] = Area(scratch);
    if (count != 5) { ++countAt7C; return; }

    struct Candidate { RECT rect; std::int32_t area; std::uint32_t first, second; };
    std::optional<Candidate> best;
    auto minimum = (std::numeric_limits<std::int32_t>::max)();
    for (std::uint32_t first = 0; first < 5; ++first)
        for (std::uint32_t second = first + 1; second < 6; ++second)
        {
            const auto rect = Bounds(rectsAt04[first], rectsAt04[second]);
            const auto area = Area(rect);
            const auto cost = Cost(area, areasAt64[first], areasAt64[second]);
            if (cost < minimum)
            {
                minimum = cost;
                best = Candidate{rect, area, first, second};
            }
        }
    // 原代码此边界使用未写入的栈候选；拒绝而不猜测 pair，保留已写 scratch/area。
    if (!best) throw std::domain_error("dirty80 merge candidate was never initialized by 441680");
    areasAt64[best->first] = best->area;
    ::SetRect(&rectsAt04[best->first], best->rect.left, best->rect.top, best->rect.right, best->rect.bottom);
    if (best->second != 5)
    {
        areasAt64[best->second] = areasAt64[5];
        rectsAt04[best->second] = rectsAt04[5];
    }
}

void GuiDirtyRectNativeBody80h::CoalesceAt442060()
{
    auto count = countAt7C;
    CheckCount(count);
    for (std::uint32_t first = 0; first + 1 < count; ++first)
        for (auto second = first + 1; second < count;)
        {
            const auto rect = Bounds(rectsAt04[first], rectsAt04[second]);
            const auto area = Area(rect);
            if (Cost(area, areasAt64[first], areasAt64[second]) > 0) { ++second; continue; }
            areasAt64[first] = area;
            ::SetRect(&rectsAt04[first], rect.left, rect.top, rect.right, rect.bottom);
            --count;
            if (second != count)
            {
                areasAt64[second] = areasAt64[count];
                rectsAt04[second] = rectsAt04[count];
            }
        }
    // 44218F/442197：循环内只改局部 count，最后才写正文。
    countAt7C = count;
}

void CreateGuiDirtyRectReferenceAt4E0D42(YtlReferenceSlotRuntime& slot, IEnvironmentRootMemory& memory)
{
    void* storage = memory.AllocateBody(0x80);
    auto* body = storage ? ::new(storage) Body : nullptr;
    ReleaseYtlReference(slot);
    if (!body) return;
    if (slot.controlAt08) slot.controlAt08->DeleteControlAtVslot04(1);
    slot.biasAt04 = 0;
    slot.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    storage = memory.AllocateBody(0x18);
    auto* control = storage ? ::new(storage) DirtyControl(body) : nullptr;
    slot.controlAt08 = control;
    // 原空 control 分支写 null+4；显式终止，不伪造成功或回收已发布正文。
    if (!control) _invalid_parameter_noinfo_noreturn();
    control->referenceCountAt04 = 1;
    slot.controlAt08->owningAt08 = 1;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->strideAt0C = 0x80;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->countAt10 = 1;
}

void ConstructGuiDirtyRectReferenceAt45B227(YtlReferenceSlotRuntime& slot, IEnvironmentRootMemory& memory)
{
    void* storage = memory.AllocateBody(0x80);
    auto* body = storage ? ::new(storage) Body : nullptr;
    slot = {};
    if (!body) return;
    slot.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    storage = memory.AllocateBody(0x18);
    auto* control = storage ? ::new(storage) DirtyControl(body) : nullptr;
    slot.controlAt08 = control;
    // 与4E0D42一致的已登记安全边界：原null control写入，不能变成成功返回。
    if (!control) _invalid_parameter_noinfo_noreturn();
    control->referenceCountAt04 = 1;
    slot.controlAt08->owningAt08 = 1;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->strideAt0C = 0x80;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->countAt10 = 1;
}

GuiDirtyRectNativeBody80h* ResolveGuiDirtyRectReference(const YtlReferenceSlotRuntime& slot) noexcept
    { return ResolveEnvironmentObjectReference<GuiDirtyRectNativeBody80h>(slot); }

void AdoptGuiDirtyRectReferenceAt498860(YtlReferenceSlotRuntime& slot,Body* body,IEnvironmentRootMemory& memory)
{
    ReleaseYtlReference(slot);
    if (!body) return;
    if (slot.controlAt08) slot.controlAt08->DeleteControlAtVslot04(1);
    slot.biasAt04=0; slot.baseAt00=reinterpret_cast<std::uintptr_t>(body);
    auto* raw=memory.AllocateBody(0x18);
    auto* control=raw?::new(raw) DirtyControl(body):nullptr;
    slot.controlAt08=control;
    *reinterpret_cast<volatile std::uint32_t*>(reinterpret_cast<std::uintptr_t>(control)+4u)=1;
    slot.controlAt08->owningAt08=1;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->strideAt0C=0x80;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->countAt10=1;
}

void ReleaseGuiDirtyRectReferenceAt460500(YtlReferenceSlotRuntime& slot) noexcept
    { ReleaseYtlReference(slot); }
}
