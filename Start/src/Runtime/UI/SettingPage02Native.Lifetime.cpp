#include "SettingPage02Native.Internal.h"
#include <exception>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace SettingPage02NativeDetail;
thread_local SettingPage02NativeServices* active = nullptr;
__declspec(naked) void __fastcall RadioNoop(Common*, void*) { __asm { _emit 0xC3 } }
[[noreturn]] void __fastcall Pure(Common*, void*) { _purecall(); std::terminate(); }
Common* __fastcall DeleteChoiceBase(Common* p, void*, std::uint32_t flags) noexcept
{ DestroyChoice(reinterpret_cast<Choice*>(p)); if (flags & 1u) ::operator delete(p, std::size_t{0x80}); return p; }
Common* __fastcall DeleteRadio(Common* p, void*, std::uint32_t flags) noexcept { return DeleteChoiceBase(p, nullptr, flags); }
Common* __fastcall Delete02B1(Common* p, void*, std::uint32_t flags) noexcept { return DeleteChoiceBase(p, nullptr, flags); }
Common* __fastcall DeleteMode(Common* p, void*, std::uint32_t flags) noexcept
{ DestroyMode(reinterpret_cast<Mode*>(p)); if (flags & 1u) ::operator delete(p, std::size_t{0x7C}); return p; }
Common* __fastcall DeleteContainer(Common* p, void*, std::uint32_t flags) noexcept
{ DestroyGuiDialogIntoNativeAt4F7B80(p); if (flags & 1u) ::operator delete(p, std::size_t{0x7C}); return p; }
Common* __fastcall DeleteParentBase(Common* p, void*, std::uint32_t flags) noexcept
{ DestroySettingPageNativeAt4F9840(reinterpret_cast<Parent*>(p)); if (flags & 1u) ::operator delete(p, std::size_t{0x84}); return p; }
Common* __fastcall Delete02(Common* p, void*, std::uint32_t flags) noexcept
{ DeleteSettingPage02NativeAt4FE350(reinterpret_cast<Parent*>(p), flags); return p; }
class ChildControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit ChildControl(Common* p) noexcept : EnvironmentObjectReferenceControl(p, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    { auto* child = static_cast<Common*>(Read(payloadAt14)); if (child) Read(child->vptrAt00)->deletingDestructorAt04(child, 1); }
    void DeleteControlAtVslot04(std::uint32_t flags) override { RetireEnvironmentObjectControl(this, flags); }
};
static_assert(sizeof(ChildControl) == 0x18);
const SettingPageChoiceNativeVtable38& ChoiceTable(bool base, bool page02)
{
    static const auto retired = [] {
        SettingPageChoiceNativeVtable38 t{GuiDialogBaseVtable(), {}, {}, {}};
        // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
        // 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
        // noexcept 实现仅向可抛槽类型放宽，不改变原异常处理。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
        t.common.deletingDestructorAt04 = reinterpret_cast<decltype(t.common.deletingDestructorAt04)>(&DeleteChoiceBase);
        t.common.beginAt44 = reinterpret_cast<decltype(t.common.beginAt44)>(&BeginChoice);
        // Pure在_purecall返回后仍必定terminate；不存在需要交付给int槽的正常返回值。
        t.common.pollAt48 = reinterpret_cast<decltype(t.common.pollAt48)>(&Pure);
        t.common.endAt4C = reinterpret_cast<decltype(t.common.endAt4C)>(&EndChoice);
        t.boundsAt8C = reinterpret_cast<decltype(t.boundsAt8C)>(&ChoiceBounds);
        t.loadAt90 = reinterpret_cast<decltype(t.loadAt90)>(&Pure);
        t.storeAt94 = reinterpret_cast<decltype(t.storeAt94)>(&Pure);
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
        return t;
    }();
    static const auto radio = [&] {
        auto t = retired;
        // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
        // 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
        // noexcept 实现仅向可抛槽类型放宽，不改变原异常处理。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
        t.common.deletingDestructorAt04 = reinterpret_cast<decltype(t.common.deletingDestructorAt04)>(&DeleteRadio);
        t.common.pollAt48 = reinterpret_cast<decltype(t.common.pollAt48)>(&PollChoice);
        t.loadAt90 = reinterpret_cast<decltype(t.loadAt90)>(&RadioNoop); // 原512670真实ret。
        t.storeAt94 = reinterpret_cast<decltype(t.storeAt94)>(&RadioNoop); // 原515B60真实ret。
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
        return t;
    }();
    static const auto block = [&] {
        auto t = radio;
        // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
        // 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
        // noexcept 实现仅向可抛槽类型放宽，不改变原异常处理。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
        t.common.deletingDestructorAt04 = reinterpret_cast<decltype(t.common.deletingDestructorAt04)>(&Delete02B1);
        t.loadAt90 = reinterpret_cast<decltype(t.loadAt90)>(&Load02B1);
        t.storeAt94 = reinterpret_cast<decltype(t.storeAt94)>(&Store02B1);
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
        return t;
    }();
    return base ? retired : page02 ? block : radio;
}
}
namespace SettingPage02NativeDetail
{
void ConstructDialogRef(Slot& out, Common* body, std::uint32_t stride)
{
    out = {};
    if (!body) return;
    struct FreshBody final
    {
        Common* body;
        ~FreshBody() noexcept { if (body) Read(body->vptrAt00)->deletingDestructorAt04(body, 1); }
    } cleanup{body};
    auto* raw = RequireSettingPage02NativeServices().memory.AllocateBody(0x18);
    if (!raw) throw std::bad_alloc();
    auto* control = ::new(raw) ChildControl(body);
    out.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    out.controlAt08 = control;
    *reinterpret_cast<volatile std::uint32_t*>(reinterpret_cast<std::uintptr_t>(control) + 4u) = 1;
    Read(out.controlAt08)->owningAt08 = 1;
    static_cast<EnvironmentObjectReferenceControl*>(Read(out.controlAt08))->strideAt0C = stride;
    static_cast<EnvironmentObjectReferenceControl*>(Read(out.controlAt08))->countAt10 = 1;
    cleanup.body = nullptr;
}
void AssignRef(Slot& target, const Slot& source)
{
    if (&target == &source) return;
    ReleaseYtlReference(target);
    CopyGuiClickedEventReference(target, source);
}
const SettingPageChoiceNativeVtable38& ChoiceBaseTable() noexcept { return ChoiceTable(true, false); }
void DestroyChoice(Choice* p) noexcept
{
    p->common.vptrAt00 = &ChoiceBaseTable().common;
    ReleaseYtlReference(p->managerAt74);
    DestroyGuiDialogAt4F7AC0(&p->common);
}
void DestroyMode(Mode* p) noexcept
{
    p->common.vptrAt00 = &SettingPage02Block02NativeVtable();
    ReleaseYtlReference(p->managerAt6C);
    DestroyGuiDialogAt4F7AC0(&p->common);
}
}
const SettingPageChoiceNativeVtable38& SettingFixRadioButtonsNativeVtable() noexcept { return ChoiceTable(false, false); }
const SettingPageChoiceNativeVtable38& SettingPage02Block01NativeVtable() noexcept { return ChoiceTable(false, true); }
const GuiDialogNativeVtable35& SettingPage02Block02NativeVtable() noexcept
{
    static const auto t = [] {
        auto v = GuiDialogBaseVtable();
        // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
        // 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
        // noexcept 实现仅向可抛槽类型放宽，不改变原异常处理。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
        v.deletingDestructorAt04 = reinterpret_cast<decltype(v.deletingDestructorAt04)>(&DeleteMode);
        v.beginAt44 = reinterpret_cast<decltype(v.beginAt44)>(&BeginMode);
        v.pollAt48 = reinterpret_cast<decltype(v.pollAt48)>(&PollMode);
        v.endAt4C = reinterpret_cast<decltype(v.endAt4C)>(&EndChoice);
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
        return v;
    }(); return t;
}
const SettingPageRadioContainerNativeVtable40& SettingPageRadioContainerNativeVtable(SettingPageRadioContainerKind kind) noexcept
{
    static const auto tables = [] {
        std::array<SettingPageRadioContainerNativeVtable40, 5> result{};
        const auto loads = std::array{&Load01B0, &Load01B1, &Load02B0, &Load02B3, &LoadSceneChoices};
        const auto stores = std::array{&Store01B0, &Store01B1, &Store02B0, &Store02B3, &StoreSceneChoices};
        for (std::size_t i = 0; i != result.size(); ++i)
        {
            auto& t = result[i]; t.common = GuiDialogIntoBaseVtable();
            // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
            // 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
            // noexcept 实现仅向可抛槽类型放宽，不改变原异常处理。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
            t.common.common.deletingDestructorAt04 = reinterpret_cast<decltype(t.common.common.deletingDestructorAt04)>(&DeleteContainer);
            t.common.common.beginAt44 = reinterpret_cast<decltype(t.common.common.beginAt44)>(&BeginContainer);
            t.common.common.endAt4C = reinterpret_cast<decltype(t.common.common.endAt4C)>(&EndContainer);
            if (i == 2) t.common.common.pollAt48 = reinterpret_cast<decltype(t.common.common.pollAt48)>(&Poll02B0);
            t.boundsAt94 = reinterpret_cast<decltype(t.boundsAt94)>(&ContainerBounds);
            t.loadAt98 = reinterpret_cast<decltype(t.loadAt98)>(loads[i]);
            t.storeAt9C = reinterpret_cast<decltype(t.storeAt9C)>(stores[i]);
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
        }
        return result;
    }(); return tables[static_cast<std::uint32_t>(kind)];
}
const SettingPageNativeVtable39& SettingPageNativeBaseVtable() noexcept
{
    static const auto table = [] {
        SettingPageNativeVtable39 t{GuiDialogIntoBaseVtable(), {}, {}};
        // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
        // 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
        // noexcept 实现仅向可抛槽类型放宽，不改变原异常处理。
        // 按值栈参数 YtlReferenceSlotRuntime 为 12 字节；无聚合返回或新增隐藏参数。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
        t.common.common.deletingDestructorAt04 = reinterpret_cast<decltype(t.common.common.deletingDestructorAt04)>(&DeleteParentBase);
        t.common.common.drawAt60 = reinterpret_cast<decltype(t.common.common.drawAt60)>(&ParentDraw);
        t.common.common.compositeAt5C = reinterpret_cast<decltype(t.common.common.compositeAt5C)>(&ParentComposite);
        t.reloadAt94 = reinterpret_cast<decltype(t.reloadAt94)>(&Pure);
        t.boundsAt98 = reinterpret_cast<decltype(t.boundsAt98)>(&ParentBounds);
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
        return t;
    }(); return table;
}
const SettingPageNativeVtable39& SettingPage02NativeVtable() noexcept
{
    static const auto table = [] {
        auto t = SettingPageNativeBaseVtable();
        // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
        // 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
        // noexcept 实现仅向可抛槽类型放宽，不改变原异常处理。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
        t.common.common.deletingDestructorAt04 = reinterpret_cast<decltype(t.common.common.deletingDestructorAt04)>(&Delete02);
        t.reloadAt94 = reinterpret_cast<decltype(t.reloadAt94)>(&Reload02);
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
        return t;
    }(); return table;
}
void DestroySettingPageNativeAt4F9840(Parent* p) noexcept
{
    p->common.vptrAt00 = &SettingPageNativeBaseVtable().common.common;
    ReleaseYtlReference(p->footerAt78);
    DestroyGuiDialogIntoNativeAt4F7B80(&p->common);
}
Parent* DeleteSettingPage02NativeAt4FE350(Parent* p, std::uint32_t flags) noexcept
{
    p->common.vptrAt00 = &SettingPage02NativeVtable().common.common;
    DestroySettingPageNativeAt4F9840(p);
    if (flags & 1u) ::operator delete(p, std::size_t{0x84});
    return p;
}
SettingPage02NativeScope::SettingPage02NativeScope(SettingPage02NativeServices& s) noexcept : SettingPage02NativeScope(&s) {}
SettingPage02NativeScope::SettingPage02NativeScope(SettingPage02NativeServices* s) noexcept
    : previous_(active), surfaces_(s ? &s->surfaces : nullptr), graphics_(s ? &s->graphics : nullptr),
      input_(s ? &s->input : nullptr), screen_(s ? &s->screen : nullptr) { active = s; }
SettingPage02NativeScope::~SettingPage02NativeScope() noexcept { active = previous_; }
SettingPage02NativeServices& RequireSettingPage02NativeServices()
{ if (!active) throw std::logic_error("Setting pages native services are not bound"); return *active; }
}
