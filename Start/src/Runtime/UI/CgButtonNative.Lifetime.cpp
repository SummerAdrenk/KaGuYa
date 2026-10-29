#include "CgButtonNative.Internal.h"
#include <stdexcept>
#include <cstdlib>

namespace KaGuYa::Start::Runtime
{
namespace { thread_local CgButtonNativeServices* current = nullptr; }
CgButtonNativeScope::CgButtonNativeScope(CgButtonNativeServices& s) noexcept
    : previous_(current), surfaces_(s.surfaces), buttons_(s.mode.buttons), mode_(s.mode),
      publicButtons_(s.publicButtons), selector_(s.selector) { current = &s; }
CgButtonNativeScope::~CgButtonNativeScope() noexcept { current = previous_; }
CgButtonNativeServices& RequireCgButtonNativeServices()
{
    if (!current) throw std::logic_error("CCGdispBtn native call has no bound services");
    return *current;
}
namespace CgButtonNativeDetail
{
void Assign(Slot& out, const Slot* source)
{
    if (&out == source) return;
    ReleaseYtlReference(out);
    out = Copy(*source);
}
void DestroyDerived(Body& b) noexcept
{
    DestroyCgScButtonNativeAt4A4C60(&b);
}
}
void DestroyCgScButtonNativeAt4A4C60(void* receiver)noexcept
{
    CgButtonNativeDetail::Fields84 b(receiver);
    // 4A4C60/5EE754：normal清理异常终止，不继续释放其它成员。
    // 原PE 4A4C87立即数=5B49D0；中间15槽只有删除入口4A72A0不同。
    b.common.vptrAt00 = reinterpret_cast<const ModeButtonNativeVtable13*>(&ICgScButtonNativeVtable());
    ReleaseYtlReference(b.selectorAt78);
    ReleaseYtlReference(b.pageButtonsAt5C);
    DestroyGuiControlSurfaceReferencesAt43EAA0(b.pagesAt50);
    ReleaseYtlReference(b.common.publicGuiAt44);
    DestroyButtonBaseNativeAt4A4BB0(b.common);
}
namespace CgButtonNativeDetail
{
namespace
{
class ButtonControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit ButtonControl(Button* p) noexcept : EnvironmentObjectReferenceControl(p, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    { auto* p = static_cast<Button*>(payloadAt14); if (p) Read(p->vptrAt00)->deleteAt00(p, 1); }
    void DeleteControlAtVslot04(unsigned f) override { RetireEnvironmentObjectControl(this, f); }
};
class ArrayControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit ArrayControl(Button* p) noexcept : EnvironmentObjectReferenceControl(p, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override;
    void DeleteControlAtVslot04(unsigned f) override { RetireEnvironmentObjectControl(this, f); }
};
void ArrayControl::DestroyPayloadAtVslot00()
{
    auto* p = static_cast<Button*>(payloadAt14);
    if (!p) return;
    auto* cookie = reinterpret_cast<unsigned*>(p) - 1;
    // 原491DA7与公共ModeSelect数组control同叶：非零cookie取当时正文删除槽(flags=3)。
    if (Read(*cookie)) Read(p->vptrAt00)->deleteAt00(p, 3);
    else std::free(cookie);
}
class CgControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit CgControl(Body* p) noexcept : EnvironmentObjectReferenceControl(p, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    { auto* p = static_cast<Body*>(payloadAt14); if (p) Methods(*p)->deleteAt00(p, 1); }
    void DeleteControlAtVslot04(unsigned f) override { RetireEnvironmentObjectControl(this, f); }
};
static_assert(sizeof(ButtonControl) == 0x18 && sizeof(ArrayControl) == 0x18 && sizeof(CgControl) == 0x18);
template<class C, class T> void Publish(Slot& out, T* p, unsigned stride, unsigned count)
{
    out.biasAt04 = 0;
    out.baseAt00 = reinterpret_cast<std::uintptr_t>(p);
    auto* raw = RequireCgButtonNativeServices().mode.memory.AllocateBody(0x18);
    auto* c = raw ? ::new(raw) C(p) : nullptr;
    out.controlAt08 = c;
    // 原498A40/49A8B8空control写+04；不转换成可捕获bad_alloc或成功空槽。
    *reinterpret_cast<volatile unsigned*>(reinterpret_cast<std::uintptr_t>(c) + 4u) = 1;
    out.controlAt08->owningAt08 = 1;
    static_cast<EnvironmentObjectReferenceControl*>(out.controlAt08)->strideAt0C = stride;
    static_cast<EnvironmentObjectReferenceControl*>(out.controlAt08)->countAt10 = count;
}
}
void AdoptButton(Slot& out, Button* p)
{ out = {}; if (p) Publish<ButtonControl>(out, p, 0x6C, 1); }
void AdoptPageArray(Slot& out, Button* p, int count)
{
    ReleaseYtlReference(out);
    if (count <= 0) return;
    if (out.controlAt08) out.controlAt08->DeleteControlAtVslot04(1);
    Publish<ArrayControl>(out, p, 0x6C, unsigned(count));
}
void ConstructReference(Slot& fresh, int category, CgButtonNativeServices& s)
{
    CgButtonNativeScope scope(s);
    auto* raw = s.mode.memory.AllocateBody(0x84);
    Body* p = nullptr;
    try { if (raw) p = ConstructCgButtonNativeAt499DC0(*::new(raw) Body, category, s); }
    catch (...)
    {
        struct RawUnwind final
        { void* raw; IEnvironmentRootMemory& memory; ~RawUnwind() noexcept { memory.DeleteBody(raw, 0x84); } } unwind{raw,s.mode.memory};
        throw;
    }
    fresh = {};
    if (p) Publish<CgControl>(fresh, p, 0x84, 1);
}
}
void DestroyCgButtonNativeAt4A3C10(CgButtonNativeBody84h& b) noexcept
{
    b.common.vptrAt00 = reinterpret_cast<const ModeButtonNativeVtable13*>(&CgButtonNativeVtable());
    CgButtonNativeDetail::DestroyDerived(b);
}
CgButtonNativeBody84h* DeleteCgButtonNativeAt4A6170(CgButtonNativeBody84h* b, unsigned flags) noexcept
{
    DestroyCgButtonNativeAt4A3C10(*b);
    if (flags & 1u) ::operator delete(b, std::size_t{0x84});
    return b;
}
void ConstructCgButtonReferenceNative(YtlReferenceSlotRuntime& fresh, int category, CgButtonNativeServices& s)
{ CgButtonNativeDetail::ConstructReference(fresh, category, s); }
}
