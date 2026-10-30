#include "SettingPage03Native.Internal.h"
#include <exception>

namespace KaGuYa::Start::Runtime
{
using namespace SettingPage03NativeDetail;
namespace
{
thread_local SettingPage03NativeServices* active = nullptr;
[[noreturn]] void __fastcall Pure(Common*, void*) { _purecall(); std::terminate(); }
Common* __fastcall DeleteBase(Common* p, void*, std::uint32_t flags) noexcept
{ DestroyBlockBase(p); if (flags & 1u) ::operator delete(p, std::size_t{0x80}); return p; }
template<unsigned Kind> Common* __fastcall DeleteBlock(Common* p, void*, std::uint32_t flags) noexcept
{
    p->vptrAt00 = &SettingPage03BlockNativeVtable(Kind).common;
    // 4FD2F0/3A0/450先提交；构造EH的4F8940绝不能走此业务尾。
    struct Base final { Common* p; ~Base() noexcept { DestroyBlockBase(p); } } base{p};
    if constexpr (Kind == 4) Field<std::int32_t>(p, 0x5C) = 1;
    else Table(p).storeAt94(p);
    // base须在sized delete之前执行。
    base.p = nullptr;
    DestroyBlockBase(p);
    if (flags & 1u) ::operator delete(p, std::size_t{Kind == 4 ? 0x84 : 0x80});
    return p;
}
Common* __fastcall DeleteContainer(Common* p, void*, std::uint32_t flags) noexcept
{ DestroyGuiDialogIntoNativeAt4F7B80(p); if (flags & 1u) ::operator delete(p, std::size_t{0x7C}); return p; }
Common* __fastcall DeleteRadio(Common* p, void*, std::uint32_t flags) noexcept
{ DestroyBlockBase(p); if (flags & 1u) ::operator delete(p, std::size_t{0x80}); return p; }
Common* __fastcall DeleteParent(Common* p, void*, std::uint32_t flags) noexcept
{ DeleteSettingPage03NativeAt4FE380(reinterpret_cast<Parent*>(p), flags); return p; }
Common* __fastcall DeleteParentBase(Common* p, void*, std::uint32_t flags) noexcept
{ DestroySettingPage03NativeAt4F9840(reinterpret_cast<Parent*>(p)); if (flags & 1u) ::operator delete(p, std::size_t{0x84}); return p; }
template<unsigned Kind> class DialogControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit DialogControl(Common* p) noexcept : EnvironmentObjectReferenceControl(p, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    { if (auto* p = static_cast<Common*>(Read(payloadAt14))) Read(p->vptrAt00)->deletingDestructorAt04(p, 1); }
    void DeleteControlAtVslot04(std::uint32_t flags) override { RetireEnvironmentObjectControl(this, flags); }
};
static_assert(sizeof(DialogControl<0>) == 0x18);
template<unsigned Kind> void Adopt(Slot& out, Common* body, std::uint32_t stride)
{
    out = {};
    if (!body) return;
    out.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    auto* raw = RequireSettingPage03NativeServices().memory.AllocateBody(0x18);
    auto* c = raw ? ::new(raw) DialogControl<Kind>(body) : nullptr;
    out.controlAt08 = c;
    // 原new18失败尚无本地清理资格；不补删body、不将null当成功Ref。
    *reinterpret_cast<volatile std::uint32_t*>(reinterpret_cast<std::uintptr_t>(c) + 4u) = 1;
    c->owningAt08 = 1; c->strideAt0C = stride; c->countAt10 = 1;
}
__declspec(naked) void __fastcall RadioNoop(Common*, void*) { __asm { _emit 0xC3 } }
std::int32_t __fastcall RadioPoll(Common* p, void*)
{
    Read(p->vptrAt00)->processInputAt54(p);
    const auto id = PollManager(Field<Slot>(p, 0x74));
    if (id >= 0) { WriteRadio(p, id); Table(p).storeAt94(p); }
    return 1;
}
std::int32_t __fastcall RadioEnd(Common*, void*) { return 1; }
}
namespace SettingPage03NativeDetail
{
void ConstructDialogRef(Slot& out, Common* p, std::uint32_t kind, std::uint32_t stride)
{
    switch (kind)
    {
    case 0: Adopt<0>(out, p, stride); break;
    case 1: Adopt<1>(out, p, stride); break;
    case 2: Adopt<2>(out, p, stride); break;
    case 3: Adopt<3>(out, p, stride); break;
    case 4: Adopt<4>(out, p, stride); break;
    case 5: Adopt<5>(out, p, stride); break;
    default: throw std::invalid_argument("Page03 Ref kind");
    }
}
const RetiredBlockVtable38& BlockBaseTable() noexcept
{
    static const auto t = [] {
        RetiredBlockVtable38 v{GuiDialogBaseVtable(), {}, {}, {}};
        // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
        // 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
        // noexcept 实现仅向可抛槽类型放宽，不改变原异常处理。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
#define BIND(field, entry) v.field = reinterpret_cast<decltype(v.field)>(&entry)
        BIND(common.deletingDestructorAt04, DeleteBase);
        BIND(common.beginAt44, BeginBlock);
        // Pure在_purecall返回后仍必定terminate；不存在需要交付给int槽的正常返回值。
        v.common.pollAt48 = reinterpret_cast<decltype(v.common.pollAt48)>(&Pure);
        BIND(common.endAt4C, RadioEnd);
        BIND(boundsAt8C, BlockBounds); BIND(loadAt90, Pure); BIND(storeAt94, Pure);
#undef BIND
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
        return v;
    }(); return t;
}
void DestroyBlockBase(Common* p) noexcept
{
    if (!p) return;
    p->vptrAt00 = &BlockBaseTable().common;
    ReleaseYtlReference(Field<Slot>(p, 0x74));
    DestroyGuiDialogAt4F7AC0(p);
}
const SettingPage03NativeVtable39& ParentBaseTable() noexcept
{
    static const auto t = [] {
        SettingPage03NativeVtable39 v{GuiDialogIntoBaseVtable(), {}, {}};
        // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
        // 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
        // noexcept 实现仅向可抛槽类型放宽，不改变原异常处理。
        // 按值栈参数 YtlReferenceSlotRuntime 为 12 字节；无聚合返回或新增隐藏参数。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
        v.common.common.deletingDestructorAt04 = reinterpret_cast<decltype(v.common.common.deletingDestructorAt04)>(&DeleteParentBase);
        v.common.common.drawAt60 = reinterpret_cast<decltype(v.common.common.drawAt60)>(&ParentDraw);
        v.common.common.compositeAt5C = reinterpret_cast<decltype(v.common.common.compositeAt5C)>(&ParentComposite);
        v.reloadAt94 = reinterpret_cast<decltype(v.reloadAt94)>(&Pure);
        v.boundsAt98 = reinterpret_cast<decltype(v.boundsAt98)>(&ParentBounds);
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
        return v;
    }(); return t;
}
const GuiDialogNativeVtable35& RadioTable() noexcept
{
    // 5BF0C0的FixRadioButtons只有38槽；不能借本页42槽的pure扩展。
    struct RadioVtable final
    { GuiDialogNativeVtable35 common; decltype(SettingPage03BlockNativeVtable42::boundsAt8C) bounds;
      decltype(SettingPage03BlockNativeVtable42::loadAt90) load, store; };
    static_assert(sizeof(RadioVtable) == 0x98);
    static const auto t = [] {
        RadioVtable v{GuiDialogBaseVtable(), {}, {}, {}};
        // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
        // 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
        // noexcept 实现仅向可抛槽类型放宽，不改变原异常处理。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
        v.common.deletingDestructorAt04 = reinterpret_cast<decltype(v.common.deletingDestructorAt04)>(&DeleteRadio);
        v.common.beginAt44 = reinterpret_cast<decltype(v.common.beginAt44)>(&BeginBlock);
        v.common.pollAt48 = reinterpret_cast<decltype(v.common.pollAt48)>(&RadioPoll);
        v.common.endAt4C = reinterpret_cast<decltype(v.common.endAt4C)>(&RadioEnd);
        v.bounds = reinterpret_cast<decltype(v.bounds)>(&BlockBounds);
        v.load = v.store = reinterpret_cast<decltype(v.load)>(&RadioNoop);
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
        return v;
    }(); return t.common;
}
}
const SettingPage03BlockNativeVtable42& SettingPage03BlockNativeVtable(std::uint32_t kind)
{
    static const auto tables = [] {
        std::array<SettingPage03BlockNativeVtable42, 4> a;
        const auto loads = std::array{&Load00, &Load01, &Load02, &Load04};
        const auto stores = std::array{&Store00, &Store01, &Store02, &Store04};
        const auto deletes = std::array{&DeleteBlock<0>, &DeleteBlock<1>, &DeleteBlock<2>, &DeleteBlock<4>};
        for (std::size_t i = 0; i != a.size(); ++i)
        {
            auto& v = a[i]; const auto& base = BlockBaseTable();
            v.common = base.common; v.boundsAt8C = base.boundsAt8C;
            // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
            // 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
            // noexcept 实现仅向可抛槽类型放宽，不改变原异常处理。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
            v.common.endAt4C = reinterpret_cast<decltype(v.common.endAt4C)>(&EndBlock);
            v.checkedAt98 = reinterpret_cast<decltype(v.checkedAt98)>(&Checked);
            v.valueAt9C = reinterpret_cast<decltype(v.valueAt9C)>(&Value);
            v.setCheckedAtA0 = reinterpret_cast<decltype(v.setCheckedAtA0)>(&SetChecked);
            v.previewForSliderAtA4 = reinterpret_cast<decltype(v.previewForSliderAtA4)>(&PreviewForSlider);
            v.common.deletingDestructorAt04 = reinterpret_cast<decltype(v.common.deletingDestructorAt04)>(deletes[i]);
            v.common.pollAt48 = reinterpret_cast<decltype(v.common.pollAt48)>(i == 3 ? &Poll04 : &PollBlock);
            v.loadAt90 = reinterpret_cast<decltype(v.loadAt90)>(loads[i]);
            v.storeAt94 = reinterpret_cast<decltype(v.storeAt94)>(stores[i]);
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
        }
        return a;
    }();
    if (kind > 4 || kind == 3) throw std::invalid_argument("Page03 block table");
    return tables[kind == 4 ? 3 : kind];
}
const SettingPage03Block03NativeVtable41& SettingPage03Block03NativeVtable() noexcept
{
    static const auto t = [] {
        SettingPage03Block03NativeVtable41 v{GuiDialogIntoBaseVtable(), {}, {}, {}, {}};
        // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
        // 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
        // noexcept 实现仅向可抛槽类型放宽，不改变原异常处理。
        // 按值栈参数 YtlReferenceSlotRuntime 为 12 字节；无聚合返回或新增隐藏参数。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
#define BIND(field, entry) v.field = reinterpret_cast<decltype(v.field)>(&entry)
        BIND(common.common.deletingDestructorAt04, DeleteContainer);
        BIND(common.common.beginAt44, Begin03); BIND(common.common.endAt4C, End03);
        BIND(common.common.drawAt60, Draw03); BIND(common.common.compositeAt5C, Composite03);
        BIND(boundsAt94, Bounds03); BIND(loadAt98, Load03); BIND(storeAt9C, Store03); BIND(setGateAtA0, Gate03);
#undef BIND
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
        return v;
    }(); return t;
}
const SettingPage03NativeVtable39& SettingPage03NativeVtable() noexcept
{
    static const auto t = [] {
        auto v = ParentBaseTable();
        // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
        // 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
        // noexcept 实现仅向可抛槽类型放宽，不改变原异常处理。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
        v.common.common.deletingDestructorAt04 = reinterpret_cast<decltype(v.common.common.deletingDestructorAt04)>(&DeleteParent);
        v.common.common.beginAt44 = reinterpret_cast<decltype(v.common.common.beginAt44)>(&ParentBegin);
        v.common.common.pollAt48 = reinterpret_cast<decltype(v.common.common.pollAt48)>(&ParentPoll);
        v.reloadAt94 = reinterpret_cast<decltype(v.reloadAt94)>(&ParentReload);
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
        return v;
    }(); return t;
}
void DestroySettingPage03NativeAt4F9840(Parent* p) noexcept
{
    p->common.vptrAt00 = &ParentBaseTable().common.common;
    ReleaseYtlReference(p->footerAt78);
    DestroyGuiDialogIntoNativeAt4F7B80(&p->common);
}
Parent* DeleteSettingPage03NativeAt4FE380(Parent* p, std::uint32_t flags) noexcept
{
    p->common.vptrAt00 = &SettingPage03NativeVtable().common.common;
    DestroySettingPage03NativeAt4F9840(p);
    if (flags & 1u) ::operator delete(p, std::size_t{0x8C});
    return p;
}
SettingPage03NativeScope::SettingPage03NativeScope(SettingPage03NativeServices& s) noexcept
    : previous_(active), surfaces_(s.surfaces), graphics_(s.graphics), input_(s.input), screen_(s.screen) { active = &s; }
SettingPage03NativeScope::~SettingPage03NativeScope() noexcept { active = previous_; }
SettingPage03NativeServices& RequireSettingPage03NativeServices()
{ if (!active) throw std::logic_error("SettingPage03 native services are not bound"); return *active; }
}
