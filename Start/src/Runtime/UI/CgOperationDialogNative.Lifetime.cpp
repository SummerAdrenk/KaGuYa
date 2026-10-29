#include "CgOperationDialogNative.Internal.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace CgOperationNativeDetail
{
namespace
{
thread_local CgOperationDialogNativeServices* services = nullptr;

class DialogControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit DialogControl(Common* body) noexcept
        : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        auto* body = static_cast<Common*>(payloadAt14);
        if (body) body->vptrAt00->deletingDestructorAt04(body, 1);
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    { RetireEnvironmentObjectControl(this, flags); }
};
static_assert(sizeof(DialogControl) == 0x18);

void ConstructDialogReference(Slot& output, Common* body, std::uint32_t stride)
{
    output = {};
    if (!body) return;
    output.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    void* raw = ::operator new(0x18);
    auto* control = raw ? ::new(raw) DialogControl(body) : nullptr;
    // 4DB682/777：局部control先初始化，最后才发布；失败不补删正文。
    control->referenceCountAt04 = 1;
    control->owningAt08 = 1;
    control->strideAt0C = stride;
    output.controlAt08 = control;
    control->countAt10 = 1;
}
std::uint32_t IntDistance(const std::int32_t* begin, const std::int32_t* end) noexcept
{
    const auto bytes = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(end) -
        reinterpret_cast<std::uintptr_t>(begin));
    // 4D8307/30A/323是SAR，不是SHR；后续容量比较才按DWORD无符号解释。
    return static_cast<std::uint32_t>(std::bit_cast<std::int32_t>(bytes) >> 2);
}
void DeleteInts(std::int32_t* begin, std::uint32_t capacity) noexcept
{
    auto bytes = capacity * 4u;
    void* raw = begin;
    if (bytes >= 0x1000u)
    {
        const auto address = reinterpret_cast<std::uintptr_t>(begin);
        std::memcpy(&raw, reinterpret_cast<const void*>(address - 4u), sizeof(raw));
        bytes += 0x23u;
        if (address - reinterpret_cast<std::uintptr_t>(raw) - 4u > 0x1Fu)
            _invalid_parameter_noinfo_noreturn();
    }
    ::operator delete(raw, bytes);
}
std::int32_t* AllocateInts(std::uint32_t capacity)
{
    const auto bytes = capacity <= 0x3FFFFFFFu ? capacity * 4u : 0xFFFFFFFFu;
    if (bytes < 0x1000u) return bytes ? static_cast<std::int32_t*>(::operator new(bytes)) : nullptr;
    const auto request = bytes > 0xFFFFFFFFu - 0x23u ? 0xFFFFFFFFu : bytes + 0x23u;
    void* raw = ::operator new(request);
    if (!raw) _invalid_parameter_noinfo_noreturn();
    const auto aligned = (reinterpret_cast<std::uintptr_t>(raw) + 0x23u) & ~std::uintptr_t{31};
    std::memcpy(reinterpret_cast<void*>(aligned - 4u), &raw, sizeof(raw));
    return reinterpret_cast<std::int32_t*>(aligned);
}
}

CgOperationDialogNativeServices& Services()
{
    if (!services) throw std::logic_error("CGOperation native leaf services are not bound");
    return *services;
}
void RefGuard::Captured(YtlReferenceControlRuntime* control, bool clear) noexcept
{
    active = false;
    if (!control) return;
    if (--control->referenceCountAt04 == 0)
    {
        if (control->owningAt08) control->DestroyPayloadAtVslot00();
        control->DeleteControlAtVslot04(1);
    }
    if (clear) { slot.controlAt08 = nullptr; slot.baseAt00 = 0; }
}
void RefGuard::Reload(bool clear, bool checkAfterPayload) noexcept
{
    active = false;
    auto* control = slot.controlAt08;
    if (!control) return;
    if (--control->referenceCountAt04 == 0)
    {
        if (control->owningAt08) control->DestroyPayloadAtVslot00();
        if (!checkAfterPayload || slot.controlAt08) slot.controlAt08->DeleteControlAtVslot04(1);
    }
    if (clear) { slot.controlAt08 = nullptr; slot.baseAt00 = 0; }
}
void AssignReference(Slot& destination, const Slot& source, bool clearControl)
{
    if (&destination == &source) return;
    // 4DB21E/4DB2A2：旧payload可抛；此处不是terminate态，失败不发布新值。
    auto* control = destination.controlAt08;
    if (control)
    {
        if (--control->referenceCountAt04 == 0)
        {
            if (control->owningAt08) control->DestroyPayloadAtVslot00();
            if (destination.controlAt08) destination.controlAt08->DeleteControlAtVslot04(1);
        }
        if (clearControl) destination.controlAt08 = nullptr;
        destination.baseAt00 = 0;
    }
    CopyGuiControlResourceReference(destination, source);
}
void ConstructButtonsReference(Slot& output, Buttons* body)
{ ConstructDialogReference(output, reinterpret_cast<Common*>(body), 0x94); }
void ConstructPopupReference(Slot& output, Popup* body)
{ ConstructDialogReference(output, reinterpret_cast<Common*>(body), 0x9C); }

// 4D83D0/53BED0/490210/53B4B0与4540A0同一Ref复制、清理、发布协议。
Slot* GrowSurfacesAt4D83D0(Surfaces& refs, Slot* position, const Slot& source)
{ return GrowGuiControlSurfaceReferencesAt4540A0(refs, position, source); }
void AppendSurface(Surfaces& refs, const Slot& source)
{
    auto* position = refs.end;
    if (position == refs.capacity) { GrowSurfacesAt4D83D0(refs, position, source); return; }
    CopyGuiControlResourceReference(*position, source);
    ++refs.end;
}
void AppendAvailability(EnvironmentVectorStorage<std::int32_t>& refs, const std::int32_t& source)
{
    auto* position = refs.end;
    if (position != refs.capacity) { *position = source; ++refs.end; return; }
    const auto index = IntDistance(refs.begin, position);
    const auto size = IntDistance(refs.begin, refs.end);
    if (size == 0x3FFFFFFFu) throw std::length_error("vector<T> too long");
    const auto required = size + 1u;
    const auto capacity = IntDistance(refs.begin, refs.capacity);
    const auto grown = capacity > 0x3FFFFFFFu - capacity / 2u ? required : capacity + capacity / 2u;
    const auto nextCapacity = (std::max)(grown, required);
    auto* next = AllocateInts(nextCapacity);
    // 4D8358..39E：先读别名source；分配回调后重读头，旧position决定尾/中间路径。
    next[index] = source;
    const auto bytes = [](const std::int32_t* first, const std::int32_t* last) noexcept
    {
        return static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(last) -
            reinterpret_cast<std::uintptr_t>(first));
    };
    if (position == refs.end)
        std::memmove(next, refs.begin, bytes(refs.begin, refs.end));
    else
    {
        std::memmove(next, refs.begin, bytes(refs.begin, position));
        std::memmove(next + index + 1u, position, bytes(position, refs.end));
    }
    if (refs.begin) DeleteInts(refs.begin, IntDistance(refs.begin, refs.capacity));
    refs.begin = next;
    refs.end = next + required;
    refs.capacity = next + nextCapacity;
}
void DestroyAvailability(EnvironmentVectorStorage<std::int32_t>& refs) noexcept
{
    if (!refs.begin) return;
    DeleteInts(refs.begin, IntDistance(refs.begin, refs.capacity));
    refs = {};
}
void DestroyButtons(Buttons& body) noexcept
{
    body.common.vptrAt00 = &ButtonsMethods().common;
    // eh vector destructor iterator：逆序销毁两张Surface，再manager，再基类。
    ReleaseYtlReference(body.surfacesAt68[1]);
    ReleaseYtlReference(body.surfacesAt68[0]);
    ReleaseYtlReference(body.eventsAt5C);
    DestroyGuiDialogAt4F7AC0(&body.common);
}
void DestroyPopup(Popup& body) noexcept
{
    body.common.vptrAt00 = &PopupMethods().common;
    DestroyGuiControlSurfaceReferencesAt43EAA0(body.pagesAt88);
    ReleaseYtlReference(body.backgroundAt7C);
    ReleaseYtlReference(body.eventsAt5C);
    DestroyGuiDialogAt4F7AC0(&body.common);
}
}

CgOperationDialogNativeScope::CgOperationDialogNativeScope(CgOperationDialogNativeServices& value) noexcept
    : previous_(CgOperationNativeDetail::services), surfaces_(value.surfaces)
{ CgOperationNativeDetail::services = &value; }
CgOperationDialogNativeScope::~CgOperationDialogNativeScope() noexcept
{ CgOperationNativeDetail::services = previous_; }
void DestroyCgOperationDialogNative(CgOperationDialogNativeBodyA8h& body) noexcept
{
    body.into.common.vptrAt00 = &CgOperationDialogNativeMethods().common;
    DestroyGuiControlSurfaceReferencesAt43EAA0(body.digitsAt8C);
    CgOperationNativeDetail::DestroyAvailability(body.availabilityAt80);
    DestroyGuiDialogIntoNativeAt4F7B80(&body.into.common);
}
CgOperationDialogNativeBodyA8h* DeleteCgOperationDialogNativeAt4FBEE0(
    CgOperationDialogNativeBodyA8h* body, std::uint32_t flags) noexcept
{
    DestroyCgOperationDialogNative(*body);
    if (flags & 1u) ::operator delete(body, std::size_t{0xA8});
    return body;
}
void AdoptCgOperationDialogNativeAt499CBA(YtlReferenceSlotRuntime& output,
    CgOperationDialogNativeBodyA8h* body)
{
    // ReplaceEnvironmentObjectReference的字段发布顺序与499CBA同序；自定义control执行原+04。
    ReleaseYtlReference(output);
    if (!body) return;
    if (output.controlAt08) output.controlAt08->DeleteControlAtVslot04(1);
    output.biasAt04 = 0;
    output.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    void* raw = ::operator new(0x18);
    auto* control = raw ? ::new(raw) CgOperationNativeDetail::DialogControl(&body->into.common) : nullptr;
    output.controlAt08 = control;
    control->referenceCountAt04 = 1;
    output.controlAt08->owningAt08 = 1;
    static_cast<EnvironmentObjectReferenceControl*>(output.controlAt08)->strideAt0C = 0xA8;
    static_cast<EnvironmentObjectReferenceControl*>(output.controlAt08)->countAt10 = 1;
}
CgOperationDialogNativeBodyA8h* ResolveCgOperationDialogNative(const YtlReferenceSlotRuntime& value) noexcept
{ return ResolveEnvironmentObjectReference<CgOperationDialogNativeBodyA8h>(value); }
std::int32_t ReadCgOperationSelectionNativeAt7C(const CgOperationDialogNativeBodyA8h& body) noexcept
{ return body.selectionAt7C; }
void WriteCgOperationSelectionNativeAt7C(CgOperationDialogNativeBodyA8h& body, std::int32_t value) noexcept
{ body.selectionAt7C = value; }
std::int32_t ResultCgOperationDialogNativeAt50AB30(const CgOperationDialogNativeBodyA8h& body) noexcept
{ return body.resultAt78; }
}
