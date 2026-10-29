#include "SettingDialogNative.Internal.h"
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace SettingDialogNativeDetail;
thread_local SettingDialogNativeServices* active = nullptr;
Common* __fastcall Delete(Common* p, void*, std::uint32_t flags) noexcept
{ DeleteSettingDialogNativeAt4FE220(&Dialog(p), flags); return p; }
class DialogControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit DialogControl(Common* p) noexcept : EnvironmentObjectReferenceControl(p, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        auto* child = static_cast<Common*>(Read(payloadAt14));
        if (child) Read(child->vptrAt00)->deletingDestructorAt04(child, 1);
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override { RetireEnvironmentObjectControl(this, flags); }
};
static_assert(sizeof(DialogControl) == 0x18);
}
const GuiDialogIntoNativeVtable37& SettingDialogNativeVtable() noexcept
{
    static const auto table = [] {
        using namespace SettingDialogNativeDetail;
        auto t = GuiDialogIntoBaseVtable();
#define BIND(field, entry) t.common.field = reinterpret_cast<decltype(t.common.field)>(&entry)
        BIND(deletingDestructorAt04, Delete);
        BIND(beginAt44, Begin);
        BIND(pollAt48, Poll);
        BIND(endAt4C, End);
        BIND(processInputAt54, ProcessInput);
        BIND(flushDirtyAt58, FlushDirty);
#undef BIND
        return t;
    }();
    return table;
}
namespace SettingDialogNativeDetail
{
void ConstructOwnedDialogReference(Slot& out, Common* child, std::uint32_t stride, IEnvironmentRootMemory& memory)
{
    out = {};
    if (!child) return;
    out.baseAt00 = reinterpret_cast<std::uintptr_t>(child);
    auto* raw = memory.AllocateBody(0x18);
    auto* control = raw ? ::new(raw) DialogControl(child) : nullptr;
    out.controlAt08 = control;
    *reinterpret_cast<volatile std::uint32_t*>(reinterpret_cast<std::uintptr_t>(control) + 4u) = 1;
    Read(out.controlAt08)->owningAt08 = 1;
    static_cast<EnvironmentObjectReferenceControl*>(Read(out.controlAt08))->strideAt0C = stride;
    static_cast<EnvironmentObjectReferenceControl*>(Read(out.controlAt08))->countAt10 = 1;
}
}
void DestroySettingDialogNativeAt4F9720(SettingDialogNativeBodyACh* b) noexcept
{
    // 原两state都terminate：84 vector -> 80 CString -> 7C CString -> 68 Ref -> Into退化。
    b->common.vptrAt00 = &SettingDialogNativeVtable().common;
    DestroyRuntimeCStringVectorAt43A700(b->snapshotNamesAt84, RequireSettingDialogNativeServices().memory);
    DestroyRuntimeCStringAt40CD40(&b->secondNameAt80);
    DestroyRuntimeCStringAt40CD40(&b->firstNameAt7C);
    ReleaseYtlReference(b->backgroundAt68);
    DestroyGuiDialogIntoNativeAt4F7B80(&b->common);
}
SettingDialogNativeBodyACh* DeleteSettingDialogNativeAt4FE220(SettingDialogNativeBodyACh* b, std::uint32_t flags) noexcept
{
    DestroySettingDialogNativeAt4F9720(b);
    if (flags & 1u) ::operator delete(b, std::size_t{0xAC});
    return b;
}
SettingDialogNativeScope::SettingDialogNativeScope(SettingDialogNativeServices& s) noexcept : SettingDialogNativeScope(&s) {}
SettingDialogNativeScope::SettingDialogNativeScope(SettingDialogNativeServices* s) noexcept
    : previous_(active), system_(s ? &s->system : nullptr) { active = s; }
SettingDialogNativeScope::~SettingDialogNativeScope() noexcept { active = previous_; }
SettingDialogNativeServices& RequireSettingDialogNativeServices()
{
    if (!active) throw std::logic_error("SettingDialog native services are not bound");
    return *active;
}
}
