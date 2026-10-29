#include <Runtime/GuiNavigationNative.h>

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
using Pair = GuiDialogNavigationMapping;
using Slot = YtlReferenceSlotRuntime;
template<class T> std::int32_t Distance(const T* begin, const T* end) noexcept
{
    const auto bytes = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(end) -
        reinterpret_cast<std::uintptr_t>(begin));
    return std::bit_cast<std::int32_t>(bytes) / static_cast<std::int32_t>(sizeof(T));
}
template<class T> T* Allocate(std::uint32_t count)
{
    const auto bytes = count * static_cast<std::uint32_t>(sizeof(T));
    if (bytes < 0x1000u) return bytes ? static_cast<T*>(::operator new(bytes)) : nullptr;
    const auto requested = bytes > 0xFFFFFFFFu - 0x23u ? 0xFFFFFFFFu : bytes + 0x23u;
    void* raw = ::operator new(requested);
    if (!raw) _invalid_parameter_noinfo_noreturn();
    const auto aligned = (reinterpret_cast<std::uintptr_t>(raw) + 0x23u) & ~std::uintptr_t{31};
    std::memcpy(reinterpret_cast<void*>(aligned - 4), &raw, sizeof(raw));
    return reinterpret_cast<T*>(aligned);
}
template<class T> void Free(T* begin, T* capacity)
{
    auto bytes = static_cast<std::uint32_t>(Distance(begin, capacity)) * sizeof(T);
    void* raw = begin;
    if (bytes >= 0x1000u)
    {
        std::memcpy(&raw, reinterpret_cast<const void*>(reinterpret_cast<std::uintptr_t>(begin) - 4), sizeof(raw));
        if (reinterpret_cast<std::uintptr_t>(begin) - reinterpret_cast<std::uintptr_t>(raw) - 4 > 0x1Fu)
            _invalid_parameter_noinfo_noreturn();
        bytes += 0x23u;
    }
    ::operator delete(raw, bytes);
}
std::uint32_t Growth(std::uint32_t size, std::uint32_t capacity, std::uint32_t maximum)
{
    if (size == maximum) throw std::length_error("vector<T> too long");
    const auto required = size + 1;
    return capacity > maximum - capacity / 2 ? required : (std::max)(required, capacity + capacity / 2);
}
std::int32_t __fastcall Next(GuiNavigationNativeBody* self, void*, std::int32_t value)
{
    for (auto* pair = self->pairsAt08.begin; pair != self->pairsAt08.end; ++pair)
        if (pair->sourceEventValue == value) return pair->targetEventValue;
    return -1;
}
const GuiNavigationNativeVtable methods{
    reinterpret_cast<decltype(GuiNavigationNativeVtable::nextAt00)>(&Next)};

class NavigationControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit NavigationControl(GuiNavigationNativeBody* body) noexcept
        : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        auto* body = static_cast<GuiNavigationNativeBody*>(payloadAt14);
        if (!body) return;
        // 53D6F0 不读正文vptr；先按 capacity 释放 pair，成功后才清指针。
        if (body->pairsAt08.begin)
        {
            Free(body->pairsAt08.begin, body->pairsAt08.capacity);
            body->pairsAt08 = {};
        }
        ::operator delete(body, std::size_t{0x14});
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
        { RetireEnvironmentObjectControl(this, flags); }
};
static_assert(sizeof(NavigationControl) == 0x18);

void Copy(Slot& to, const Slot& from) noexcept
{
    to.baseAt00 = from.baseAt00;
    to.biasAt04 = from.biasAt04;
    to.controlAt08 = from.controlAt08;
    if (to.controlAt08) ++to.controlAt08->referenceCountAt04;
}
void ReleaseRange(Slot* begin, Slot* end) noexcept
{
    // 4D80B0 的state0是terminate，不能仅因EHFlags=1允许释放异常外传。
    for (; begin != end; ++begin) ReleaseYtlReference(*begin);
}
struct Temporary final
{
    Slot& slot;
    ~Temporary() noexcept { ReleaseYtlReference(slot); }
};
}

GuiNavigationNativeBody& RequireGuiNavigation(const Slot& reference)
{
    auto* body = ResolveEnvironmentObjectReference<GuiNavigationNativeBody>(reference);
    if (!body) throw std::logic_error("GuiOperationKey dereferenced a null body");
    return *body;
}
std::int32_t GuiNavigationPairCount(const GuiNavigationNativeBody& body) noexcept
    { return Distance(body.pairsAt08.begin, body.pairsAt08.end); }
std::int32_t GuiNavigationReferenceCount(const GuiNavigationReferences& refs) noexcept
    { return Distance(refs.begin, refs.end); }

void AppendGuiNavigationPairAt40A880(GuiNavigationNativeBody& body, const Pair& pair)
{
    auto& storage = body.pairsAt08;
    auto* position = storage.end;
    if (position != storage.capacity)
    {
        *position = pair;
        ++storage.end;
        return;
    }
    const auto size = static_cast<std::uint32_t>(Distance(storage.begin, position));
    const auto capacity = Growth(size, static_cast<std::uint32_t>(Distance(storage.begin, storage.capacity)), 0x1FFFFFFFu);
    auto* next = Allocate<Pair>(capacity);
    next[size] = pair; // 先读取插入源，分配回调之后才读取旧范围。
    auto* output = next;
    auto* oldEnd = storage.end;
    for (auto* old = storage.begin; old != position; ++old) *output++ = *old;
    if (position != oldEnd)
    {
        output = next + size + 1;
        for (auto* old = position; old != oldEnd; ++old) *output++ = *old;
    }
    if (storage.begin) Free(storage.begin, storage.capacity);
    storage.begin = next;
    storage.end = next + size + 1;
    storage.capacity = next + capacity;
}

Slot* CreateGuiNavigationAt504CE0(Slot& output, std::int32_t input, const Pair* pairs)
{
    auto* body = static_cast<GuiNavigationNativeBody*>(::operator new(0x14));
    if (body)
    {
        body->vptrAt00 = &methods;
        body->pairsAt08 = {};
        // inputAt04 在control完成后才写，保留构造回调可见的未初始化区。
    }
    output = {};
    if (body)
    {
        output.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
        void* memory = ::operator new(0x18);
        auto* control = memory ? ::new (memory) NavigationControl(body) : nullptr;
        output.controlAt08 = control;
        if (!control) _invalid_parameter_noinfo_noreturn();
        control->referenceCountAt04 = 1;
        output.controlAt08->owningAt08 = 1;
        static_cast<EnvironmentObjectReferenceControl*>(output.controlAt08)->strideAt0C = 0x14;
        static_cast<EnvironmentObjectReferenceControl*>(output.controlAt08)->countAt10 = 1;
    }
    // 504E00后才具备EH清理资格：上面的control失败不会回收部分发布正文。
    try
    {
        RequireGuiNavigation(output).inputAt04 = input;
        for (auto* source = pairs; source->sourceEventValue >= 0; ++source)
        {
            const Pair value = *source;
            AppendGuiNavigationPairAt40A880(RequireGuiNavigation(output), value);
        }
    }
    catch (...)
    {
        Temporary cleanup{output};
        throw;
    }
    return &output;
}

void AppendGuiNavigationReferenceAt4D9D60(GuiNavigationReferences& refs, const Slot& source)
{
    auto* position = refs.end;
    if (position != refs.capacity)
    {
        Copy(*position, source);
        ++refs.end;
        return;
    }
    const auto size = static_cast<std::uint32_t>(Distance(refs.begin, position));
    const auto capacity = Growth(size, static_cast<std::uint32_t>(Distance(refs.begin, refs.capacity)), 0x15555555u);
    auto* next = Allocate<Slot>(capacity);
    auto* inserted = next + size;
    auto* cleanupBegin = inserted;
    try
    {
        Copy(*inserted, source);
        auto* destination = next;
        if (position == refs.end)
        {
            for (auto* old = refs.begin; old != refs.end; ++old) Copy(*destination++, *old);
        }
        else
        {
            for (auto* old = refs.begin; old != position; ++old) Copy(*destination++, *old);
            cleanupBegin = next;
            destination = inserted + 1;
            for (auto* old = position; old != refs.end; ++old) Copy(*destination++, *old);
        }
    }
    catch (...)
    {
        ReleaseRange(cleanupBegin, inserted + 1);
        Free(next, next + capacity);
        throw;
    }
    // 4D9E87先结束保护区；旧区释放后才由53B5D0发布新存储。
    if (refs.begin)
    {
        ReleaseRange(refs.begin, refs.end);
        Free(refs.begin, refs.capacity);
    }
    refs.begin = next;
    refs.end = next + size + 1;
    refs.capacity = next + capacity;
}
void ClearGuiNavigationReferencesAt4D80B0(GuiNavigationReferences& refs) noexcept
{
    ReleaseRange(refs.begin, refs.end);
    refs.end = refs.begin;
}
void DestroyGuiNavigationReferencesAt4F6E70(GuiNavigationReferences& refs) noexcept
{
    if (!refs.begin) return;
    ReleaseRange(refs.begin, refs.end);
    Free(refs.begin, refs.capacity);
    refs = {};
}
void CreateAndAppendGuiNavigation(GuiNavigationReferences& refs, std::int32_t input, const Pair* pairs)
{
    Slot temporary;
    CreateGuiNavigationAt504CE0(temporary, input, pairs);
    Temporary cleanup{temporary};
    AppendGuiNavigationReferenceAt4D9D60(refs, temporary);
}
}
