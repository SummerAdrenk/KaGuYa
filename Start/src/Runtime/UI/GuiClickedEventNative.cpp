#include <Runtime/GuiClickedEventNative.h>

#include <algorithm>
#include <bit>
#include <cstdlib>
#include <cstring>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Body = GuiClickedEventNativeBody;
using Table = GuiClickedEventNativeVtable;
using Slot = YtlReferenceSlotRuntime;
constexpr std::uint32_t MaximumReferences = 0x15555555u;

std::int32_t __fastcall Invoke(Body* body, void*) { body->clickedFlagAt04 = 1; return 0; }
void __fastcall Clear(Body* body, void*) { body->clickedFlagAt04 = 0; }
Body* __fastcall Delete(Body* body, void*, std::uint32_t flags);
// 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
// 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const Table methods{
    reinterpret_cast<decltype(Table::invokeAt00)>(&Invoke),
    reinterpret_cast<decltype(Table::clearAt04)>(&Clear),
    reinterpret_cast<decltype(Table::deletingDestructorAt08)>(&Delete)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
Body* __fastcall Delete(Body* body, void*, std::uint32_t flags)
{
    body->vptrAt00 = &methods; // 4FC49A；flags=0也恢复本类表，两个DWORD不改。
    if (flags & 1u) ::operator delete(body, std::size_t{0x0C});
    return body;
}
class ClickedEventControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit ClickedEventControl(Body* body) noexcept
        : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        auto* body = static_cast<Body*>(payloadAt14);
        if (body) body->vptrAt00->deletingDestructorAt08(body, 1);
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
        { RetireEnvironmentObjectControl(this, flags); }
};
static_assert(sizeof(ClickedEventControl) == 0x18);

ClickedEventControl* AllocateControl(Body* body, IEnvironmentRootMemory& memory)
{
    void* raw = memory.AllocateBody(0x18);
    return raw ? ::new(raw) ClickedEventControl(body) : nullptr;
}
std::int32_t Distance(const Slot* begin, const Slot* end) noexcept
{
    const auto bytes = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(end) -
        reinterpret_cast<std::uintptr_t>(begin));
    return std::bit_cast<std::int32_t>(bytes) / 12;
}
Slot* AllocateVector(std::uint32_t capacity, IEnvironmentRootMemory& memory)
{
    const auto bytes = capacity <= MaximumReferences ? capacity * 12u : 0xFFFFFFFFu;
    if (bytes < 0x1000u) return bytes ? static_cast<Slot*>(memory.AllocateBody(bytes)) : nullptr;
    const auto request = bytes > 0xFFFFFFFFu - 0x23u ? 0xFFFFFFFFu : bytes + 0x23u;
    void* raw = memory.AllocateBody(request);
    if (!raw) _invalid_parameter_noinfo_noreturn();
    const auto aligned = (reinterpret_cast<std::uintptr_t>(raw) + 0x23u) & ~std::uintptr_t{31};
    std::memcpy(reinterpret_cast<void*>(aligned - 4u), &raw, sizeof(raw));
    return reinterpret_cast<Slot*>(aligned);
}
void DeleteVector(Slot* begin, std::uint32_t capacity, IEnvironmentRootMemory& memory)
{
    auto bytes = capacity * 12u;
    void* raw = begin;
    if (bytes >= 0x1000u)
    {
        const auto address = reinterpret_cast<std::uintptr_t>(begin);
        std::memcpy(&raw, reinterpret_cast<const void*>(address - 4u), sizeof(raw));
        bytes += 0x23u;
        if (address - reinterpret_cast<std::uintptr_t>(raw) - 4u > 0x1Fu)
            _invalid_parameter_noinfo_noreturn();
    }
    memory.DeleteVector(raw, bytes);
}
void CopyRange(const Slot* begin, const Slot* end, Slot* output) noexcept
{
    // 53BF60/53BB70是复制并增持，不移动；各自末尾release的是空范围。
    for (; begin != end; ++begin, ++output) CopyGuiClickedEventReference(*output, *begin);
}
}

const GuiClickedEventNativeVtable& GuiClickedEventVtable() noexcept { return methods; }
Body* NewGuiClickedEventAt4EA7B3(const std::int32_t& source, IEnvironmentRootMemory& memory)
{
    void* raw = memory.AllocateBody(0x0C);
    if (!raw) return nullptr;
    const auto value = source; // 4EA7C6/4EA7C9：分配之后才读已选控件的event字段。
    return ::new(raw) Body{&methods, 0, value};
}
Slot* ConstructGuiClickedEventReferenceAt4D7780(Slot& output, Body* body,
    std::uint8_t owning, IEnvironmentRootMemory& memory)
{
    output = {};
    if (body)
    {
        output.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
        auto* control = AllocateControl(body, memory);
        output.controlAt08 = control;
        if (!control) _invalid_parameter_noinfo_noreturn();
        control->referenceCountAt04 = 1;
        output.controlAt08->owningAt08 = owning;
        static_cast<EnvironmentObjectReferenceControl*>(output.controlAt08)->strideAt0C = 0x0C;
        static_cast<EnvironmentObjectReferenceControl*>(output.controlAt08)->countAt10 = 1;
    }
    return &output;
}
Slot* CreatePopupClickedEventAt4EA7B3(Slot& output, const std::int32_t& source,
    IEnvironmentRootMemory& memory)
{
    auto* body = NewGuiClickedEventAt4EA7B3(source, memory);
    output = {}; // 4EA7E4..4EA7EA：body失败之前不能提前清out。
    if (body)
    {
        output.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
        auto* control = AllocateControl(body, memory);
        if (!control) _invalid_parameter_noinfo_noreturn();
        control->referenceCountAt04 = 1;
        control->owningAt08 = 1;
        control->strideAt0C = 0x0C;
        output.controlAt08 = control; // 4EA82E在refs/owning/stride之后，count之前。
        control->countAt10 = 1;
    }
    return &output;
}
Slot* CreateToolClickedEventAt4E2F5A(Slot& output, const Slot& child, IEnvironmentRootMemory& memory)
{
    void* raw = memory.AllocateBody(0x0C);
    Body* body = nullptr;
    if (raw)
    {
        const auto* selected = ResolveEnvironmentObjectReference<std::byte>(child);
        std::int32_t id;
        std::memcpy(&id, selected + 4, sizeof(id));
        body = ::new(raw) Body{&methods, 0, id};
    }
    output = {};
    if (body)
    {
        output.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
        auto* control = AllocateControl(body, memory);
        control->referenceCountAt04 = 1;
        control->owningAt08 = 1;
        control->strideAt0C = 0x0C;
        output.controlAt08 = control;
        control->countAt10 = 1;
    }
    return &output;
}
Body* ResolveGuiClickedEvent(const Slot& reference) noexcept
    { return ResolveEnvironmentObjectReference<Body>(reference); }
Body& RequireGuiClickedEvent(const Slot& reference)
{
    auto* body = ResolveGuiClickedEvent(reference);
    if (!body) throw std::logic_error("ClickedEvent dereferenced a null body");
    return *body;
}
void CopyGuiClickedEventReference(Slot& output, const Slot& source) noexcept
{
    output.baseAt00 = source.baseAt00;
    output.biasAt04 = source.biasAt04;
    output.controlAt08 = source.controlAt08;
    if (output.controlAt08) ++output.controlAt08->referenceCountAt04;
}
void ReleaseGuiClickedEventReferenceAt4F64D0(Slot& reference) noexcept
    { ReleaseYtlReference(reference); }
void ReleasePopupClickedEventTemporaryAt4EA9BC(Slot& temporary,
    YtlReferenceControlRuntime* capturedControl) noexcept
{
    if (!capturedControl) return;
    if (--capturedControl->referenceCountAt04 == 0)
    {
        if (capturedControl->owningAt08) capturedControl->DestroyPayloadAtVslot00();
        capturedControl->DeleteControlAtVslot04(1);
    }
    temporary.controlAt08 = nullptr;
    temporary.baseAt00 = 0;
}
std::int32_t GuiClickedEventReferenceCount(const GuiClickedEventReferences& references) noexcept
    { return Distance(references.begin, references.end); }
std::int32_t GuiClickedEventReferenceCapacity(const GuiClickedEventReferences& references) noexcept
    { return Distance(references.begin, references.capacity); }

Slot* GrowGuiClickedEventReferencesAt4D9620(GuiClickedEventReferences& refs, Slot* position,
    const Slot& source, IEnvironmentRootMemory& memory)
{
    const auto index = static_cast<std::uint32_t>(Distance(refs.begin, position));
    const auto size = static_cast<std::uint32_t>(Distance(refs.begin, refs.end));
    if (size == MaximumReferences) throw std::length_error("vector<T> too long");
    const auto required = size + 1;
    const auto capacity = static_cast<std::uint32_t>(Distance(refs.begin, refs.capacity));
    const auto grown = capacity > MaximumReferences - capacity / 2 ? required : capacity + capacity / 2;
    const auto nextCapacity = (std::max)(required, grown);
    auto* next = AllocateVector(nextCapacity, memory);
    auto* inserted = next + index;
    auto* cleanupBegin = inserted;
    try
    {
        CopyGuiClickedEventReference(*inserted, source);
        // 4D9709：分配之后重读end，不能把入口时的append判断跨回调缓存。
        if (position == refs.end) CopyRange(refs.begin, refs.end, next);
        else
        {
            CopyRange(refs.begin, position, next);
            cleanupBegin = next;
            CopyRange(position, refs.end, inserted + 1);
        }
    }
    catch (...)
    {
        ReleaseGuiClickedEventRangeAt4D7F90(cleanupBegin, inserted + 1);
        DeleteVector(next, nextCapacity, memory);
        throw;
    }
    // 4D9747先关闭构造保护；53B540旧区释放/存储删除失败不回收未发布的next。
    if (refs.begin)
    {
        ReleaseGuiClickedEventRangeAt4D7F90(refs.begin, refs.end);
        DeleteVector(refs.begin, static_cast<std::uint32_t>(Distance(refs.begin, refs.capacity)), memory);
    }
    refs.begin = next;
    refs.end = next + required;
    refs.capacity = next + nextCapacity;
    return refs.begin + index;
}
void AppendGuiClickedEventReference(GuiClickedEventReferences& refs,
    const Slot& source, IEnvironmentRootMemory& memory)
{
    auto* position = refs.end;
    if (position != refs.capacity)
    {
        CopyGuiClickedEventReference(*position, source);
        ++refs.end;
        return;
    }
    static_cast<void>(GrowGuiClickedEventReferencesAt4D9620(refs, position, source, memory));
}
void ClearGuiClickedEventFlagsAt532200(GuiClickedEventReferences& refs)
{
    for (auto* position = refs.begin; position != refs.end; ++position)
    {
        auto& body = RequireGuiClickedEvent(*position);
        body.vptrAt00->clearAt04(&body);
    }
}
void AppendPopupClickedEventReferenceAt4EA97F(GuiClickedEventReferences& refs,
    const Slot& source, YtlReferenceControlRuntime*& capturedControl, IEnvironmentRootMemory& memory)
{
    auto* position = refs.end;
    if (position != refs.capacity)
    {
        position->baseAt00 = source.baseAt00;
        position->biasAt04 = 0;
        position->controlAt08 = capturedControl;
        if (capturedControl) ++capturedControl->referenceCountAt04;
        ++refs.end;
        return;
    }
    static_cast<void>(GrowGuiClickedEventReferencesAt4D9620(refs, position, source, memory));
    capturedControl = source.controlAt08; // 4EA9B9，仅增长路回读。
}
void ReleaseGuiClickedEventRangeAt4D7F90(Slot* begin, Slot* end) noexcept
{
    for (; begin != end; ++begin) ReleaseYtlReference(*begin);
}
void DestroyGuiClickedEventReferencesAt4F6D10(GuiClickedEventReferences& refs,
    IEnvironmentRootMemory& memory) noexcept
{
    if (!refs.begin) return;
    ReleaseGuiClickedEventRangeAt4D7F90(refs.begin, refs.end);
    DeleteVector(refs.begin, static_cast<std::uint32_t>(Distance(refs.begin, refs.capacity)), memory);
    refs = {};
}
}
