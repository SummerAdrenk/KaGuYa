#include "ReplayButtonNative.Internal.h"
#include <Runtime/ICgScButtonNative.h>
#include <algorithm>
#include <cstdlib>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace { thread_local ReplayButtonNativeServices* currentReplay = nullptr; }
ReplayButtonNativeScope::ReplayButtonNativeScope(ReplayButtonNativeServices& s) noexcept
    : previous_(currentReplay), commonServices_{s.mode,s.environmentLock,s.source,s.pictures,
        s.surfaces,s.copy,s.strings,s.publicButtons,s.selector}, commonScope_(commonServices_)
{ currentReplay = &s; }
ReplayButtonNativeScope::~ReplayButtonNativeScope() noexcept { currentReplay = previous_; }
ReplayButtonNativeServices& RequireReplayButtonNativeServices()
{
    if (!currentReplay) throw std::logic_error("CModeReplayBtn native call has no bound services");
    return *currentReplay;
}
namespace ReplayButtonNativeDetail
{
void ReleaseCaptured(Slot& slot, YtlReferenceControlRuntime* control, bool clearSlot)
{
    if (!control) return;
    if (--control->referenceCountAt04 == 0)
    {
        if (Read(control->owningAt08)) control->DestroyPayloadAtVslot00();
        control->DeleteControlAtVslot04(1);
    }
    if (clearSlot) { slot.controlAt08 = nullptr; slot.baseAt00 = 0; }
}
Slot CopyCaptured(const Slot& slot, YtlReferenceControlRuntime* control) noexcept
{
    Slot result{Read(slot.baseAt00),Read(slot.biasAt04),control};
    if (control) ++control->referenceCountAt04;
    return result;
}
void Assign(Slot& out, const Slot* source)
{
    if (&out == source) return;
    ReleaseYtlReference(out);
    out = CopyCaptured(*source, Read(source->controlAt08));
}
void* Root(unsigned ensureSite, unsigned readSite)
{
    auto& source = RequireReplayButtonNativeServices().source;
    source.EnsureRootAt413BE0(ensureSite);
    return source.RootAt6198C4(readSite);
}
void* Draw(unsigned ensureSite, unsigned readSite)
{
    auto* root = Root(ensureSite, readSite);
    return Resolve<void>(RequireReplayButtonNativeServices().source.RootDrawSlotAt18(root));
}
void Blit(Surface* target, Surface* source, int x, int y)
{
    // 4425C0先读目标handle，再读源；零handle不触发Graphics叶。
    const auto destination = Read(target->handleAt04);
    if (!destination) return;
    const auto handle = Read(source->handleAt04);
    if (!handle) return;
    static_cast<void>(RequireReplayButtonNativeServices().mode.buttons.graphics.SurfaceBlitRaw(
        destination,handle,x,y,nullptr,nullptr,nullptr,0));
}
namespace
{
Slot* Allocate(unsigned capacity)
{
    const auto bytes = capacity > 0x15555555u ? 0xFFFFFFFFu : capacity * 12u;
    if (bytes < 0x1000u) return bytes ? static_cast<Slot*>(::operator new(std::size_t{bytes})) : nullptr;
    const auto extra = bytes + 0x23u;
    auto* raw = ::operator new(std::size_t{extra <= bytes ? 0xFFFFFFFFu : extra});
    if (!raw) _invalid_parameter_noinfo_noreturn();
    const auto aligned = (reinterpret_cast<std::uintptr_t>(raw) + 0x23u) & ~std::uintptr_t{31};
    reinterpret_cast<void**>(aligned)[-1] = raw;
    return reinterpret_cast<Slot*>(aligned);
}
void FreeBytes(Slot* first, unsigned bytes)
{
    void* raw = first;
    if (bytes >= 0x1000u)
    {
        raw = reinterpret_cast<void**>(first)[-1];
        bytes += 0x23u;
        if (reinterpret_cast<std::uintptr_t>(first) - reinterpret_cast<std::uintptr_t>(raw) - 4u > 0x1Fu)
            _invalid_parameter_noinfo_noreturn();
    }
    ::operator delete(raw,std::size_t{bytes});
}
Slot* CopyRange(Slot* first, Slot* last, Slot* out)
{
    for (; first != last; first = Advance(first,1), out = Advance(out,1))
        *out = CopyCaptured(*first,Read(first->controlAt08));
    // 4C5680正常尾保留空范围调用；该叶自身的展开为terminate边界。
    [&]() noexcept { ReleaseModeButtonReferenceRangeNativeAt498C10(out,out); }();
    return out;
}
class ButtonControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit ButtonControl(Button* p) noexcept : EnvironmentObjectReferenceControl(p,UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    { auto* p = static_cast<Button*>(payloadAt14); if (p) Read(p->vptrAt00)->deleteAt00(p,1); }
    void DeleteControlAtVslot04(unsigned flags) override { RetireEnvironmentObjectControl(this,flags); }
};
class ArrayControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit ArrayControl(Button* p) noexcept : EnvironmentObjectReferenceControl(p,UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        auto* p = static_cast<Button*>(payloadAt14);
        if (!p) return;
        auto* cookie = reinterpret_cast<unsigned*>(p) - 1;
        if (Read(*cookie)) Read(p->vptrAt00)->deleteAt00(p,3);
        else std::free(cookie); // 原491DA7共用数组control叶的零cookie分支。
    }
    void DeleteControlAtVslot04(unsigned flags) override { RetireEnvironmentObjectControl(this,flags); }
};
class ReplayControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit ReplayControl(Body* p) noexcept : EnvironmentObjectReferenceControl(p,UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    { auto* p = static_cast<Body*>(payloadAt14); if (p) Methods(*p)->deleteAt00(p,1); }
    void DeleteControlAtVslot04(unsigned flags) override { RetireEnvironmentObjectControl(this,flags); }
};
static_assert(sizeof(ButtonControl) == 0x18 && sizeof(ArrayControl) == 0x18 && sizeof(ReplayControl) == 0x18);
template<class C, class T> void Publish(Slot& out, T* p, unsigned stride, unsigned count)
{
    out.biasAt04 = 0;
    out.baseAt00 = reinterpret_cast<std::uintptr_t>(p);
    auto* raw = RequireReplayButtonNativeServices().mode.memory.AllocateBody(0x18);
    auto* c = raw ? ::new(raw) C(p) : nullptr;
    out.controlAt08 = c;
    // 原49EED8/498ABF直接写control+04；不得把空control改为成功空Ref或可捕获bad_alloc。
    *reinterpret_cast<volatile unsigned*>(reinterpret_cast<std::uintptr_t>(c) + 4u) = 1;
    out.controlAt08->owningAt08 = 1;
    static_cast<EnvironmentObjectReferenceControl*>(out.controlAt08)->strideAt0C = stride;
    static_cast<EnvironmentObjectReferenceControl*>(out.controlAt08)->countAt10 = count;
}
}
void AppendChild(ModeButtonReferenceVector& v, const Slot& source)
{
    auto* position = Read(v.end);
    if (position != Read(v.capacity))
    { *position = CopyCaptured(source,Read(source.controlAt08)); v.end = Advance(position,1); return; }
    // 4992A0：分配后重读end，允许重入使原尾插转入4C5680两段复制分支。
    const auto offset = unsigned(Distance(Read(v.begin),position));
    const auto size = unsigned(Count(v));
    if (size == 0x15555555u) throw std::length_error("vector<T> too long");
    const auto oldCapacity = unsigned(Distance(Read(v.begin),Read(v.capacity)));
    const auto required = size + 1u;
    const auto grown = oldCapacity > 0x15555555u - oldCapacity / 2u ? required : oldCapacity + oldCapacity / 2u;
    const auto capacity = (std::max)(required,grown);
    auto* next = Allocate(capacity);
    auto* inserted = Advance(next,int(offset));
    auto* protectedFirst = inserted;
    auto* protectedEnd = Advance(inserted,1);
    try
    {
        *inserted = CopyCaptured(source,Read(source.controlAt08));
        auto* last = Read(v.end);
        if (position == last)
        {
            auto* output = next;
            for (auto* at = Read(v.begin); at != last; at = Advance(at,1), output = Advance(output,1))
                *output = CopyCaptured(*at,Read(at->controlAt08));
            ReleaseModeButtonReferenceRangeNativeAt498C10(output,output);
        }
        else
        {
            CopyRange(Read(v.begin),position,next);
            protectedFirst = next;
            CopyRange(position,Read(v.end),Advance(inserted,1));
        }
    }
    catch (...)
    {
        // 4994D3 handler直接清构造保护范围，再4C5970按unsigned字节释放，之后重抛。
        ReleaseModeButtonReferenceRangeNativeAt498C10(protectedFirst,protectedEnd);
        FreeBytes(next,capacity * 12u);
        throw;
    }
    // 原state=-1后才退旧vector；释放异常不回滚next、不发布新三指针。
    if (auto* first = Read(v.begin))
    {
        ReleaseModeButtonReferenceRangeNativeAt498C10(first,Read(v.end));
        FreeBytes(Read(v.begin),unsigned(Distance(Read(v.begin),Read(v.capacity))) * 12u);
    }
    v.begin = next; v.end = Advance(next,int(required)); v.capacity = Advance(next,int(capacity));
}
void AdoptButton(Slot& out, Button* p)
{ out = {}; if (p) Publish<ButtonControl>(out,p,0x6C,1); }
void AdoptPageArray(Slot& out, Button* p, int count)
{
    ReleaseYtlReference(out);
    if (count <= 0) return;
    if (Read(out.controlAt08)) Read(out.controlAt08)->DeleteControlAtVslot04(1);
    Publish<ArrayControl>(out,p,0x6C,unsigned(count));
}
void ConstructReference(Slot& fresh, int category, ReplayButtonNativeServices& s)
{
    ReplayButtonNativeScope scope(s);
    auto* raw = s.mode.memory.AllocateBody(0x84);
    Body* body = nullptr;
    try { if (raw) body = ConstructReplayButtonNativeAt49F270(*::new(raw) Body,category,s); }
    catch (...)
    {
        struct RawUnwind final
        { void* raw; IEnvironmentRootMemory& memory; ~RawUnwind() noexcept { memory.DeleteBody(raw,0x84); } } unwind{raw,s.mode.memory};
        throw;
    }
    // 49EE9E以后撤销raw资格；control申请失败不补删已正常返回的部分/完整84h正文。
    fresh = {};
    if (body) Publish<ReplayControl>(fresh,body,0x84,1);
}
}
void DestroyReplayButtonNativeAt4A4C60(ReplayButtonNativeBody84h& b) noexcept
{ DestroyCgScButtonNativeAt4A4C60(&b); }
ReplayButtonNativeBody84h* DeleteReplayButtonNativeAt4A6D70(ReplayButtonNativeBody84h* b, unsigned flags) noexcept
{
    b->common.vptrAt00 = reinterpret_cast<const ModeButtonNativeVtable13*>(&ReplayButtonNativeVtable());
    DestroyReplayButtonNativeAt4A4C60(*b);
    if (flags & 1u) ::operator delete(b,std::size_t{0x84});
    return b;
}
void ConstructReplayButtonReferenceNative(YtlReferenceSlotRuntime& fresh, int category, ReplayButtonNativeServices& s)
{ ReplayButtonNativeDetail::ConstructReference(fresh,category,s); }
}
