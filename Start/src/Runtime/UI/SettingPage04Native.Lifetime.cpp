#include "SettingPage04Native.Internal.h"
#include <exception>

namespace KaGuYa::Start::Runtime
{
using namespace SettingPage04NativeDetail;
namespace
{
thread_local SettingPage04NativeServices* active=nullptr;
__declspec(naked) void __fastcall Ret(Common*,void*) {__asm {_emit 0xC3}}
int __fastcall One(Common*,void*) {return 1;}
[[noreturn]] void __fastcall Pure(Common*,void*) {_purecall();std::terminate();}
Common* __fastcall DeleteRadio(Common* p,void*,unsigned flags) noexcept
{DestroyRadio(p);if(flags&1u)::operator delete(p,std::size_t{0x80});return p;}
template<unsigned Kind> Common* __fastcall DeleteBlock(Common* p,void*,unsigned flags) noexcept
{
    // 04六个正常删除槽均不Store；不得借03的析构提交资格。
    DestroyBlock(p,Kind);
    constexpr std::size_t size=Kind==1?0x94:Kind==2||Kind==5?0x88:Kind==3?0x8C:0x7C;
    if(flags&1u)::operator delete(p,size);return p;
}
Common* __fastcall DeleteParent(Common* p,void*,unsigned flags) noexcept
{DeleteSettingPage04NativeAt4FE3B0(reinterpret_cast<Parent*>(p),flags);return p;}
Common* __fastcall DeleteParentBase(Common* p,void*,unsigned flags) noexcept
{DestroySettingPage04NativeAt4F9840(reinterpret_cast<Parent*>(p));if(flags&1u)::operator delete(p,std::size_t{0x84});return p;}
template<unsigned Kind> class DialogRef final:public EnvironmentObjectReferenceControl
{
public:
    explicit DialogRef(Common* p) noexcept:EnvironmentObjectReferenceControl(p,UninitializedFieldsTag{}){}
    void DestroyPayloadAtVslot00() override
    {if(auto* p=static_cast<Common*>(Read(payloadAt14)))Read(p->vptrAt00)->deletingDestructorAt04(p,1);}
    void DeleteControlAtVslot04(unsigned flags) override {RetireEnvironmentObjectControl(this,flags);}
};
static_assert(sizeof(DialogRef<0>)==0x18);
template<unsigned Kind> void Adopt(Slot& out,Common* p,unsigned stride)
{
    out={};if(!p)return;
    out.baseAt00=reinterpret_cast<std::uintptr_t>(p);
    auto* raw=RequireSettingPage04NativeServices().memory.AllocateBody(0x18);
    auto* c=raw?::new(raw) DialogRef<Kind>(p):nullptr;out.controlAt08=c;
    // new18失败之前只有raw body，不补删、不取得Ref资格；原null写入仍故障。
    *reinterpret_cast<volatile unsigned*>(reinterpret_cast<std::uintptr_t>(c)+4u)=1;
    c->owningAt08=1;c->strideAt0C=stride;c->countAt10=1;
}
}
namespace SettingPage04NativeDetail
{
void ConstructDialogRef(Slot& out,Common* p,unsigned kind,unsigned stride)
{
    switch(kind){case 0:Adopt<0>(out,p,stride);break;case 1:Adopt<1>(out,p,stride);break;
    case 2:Adopt<2>(out,p,stride);break;case 3:Adopt<3>(out,p,stride);break;
    case 4:Adopt<4>(out,p,stride);break;case 5:Adopt<5>(out,p,stride);break;
    case 6:Adopt<6>(out,p,stride);break;default:throw std::invalid_argument("Page04 Ref kind");}
}
const SettingPage04RadioNativeVtable38& RadioBaseTable() noexcept
{
    static const auto t=[]{
        SettingPage04RadioNativeVtable38 v{GuiDialogBaseVtable(), {}, {}, {}};
        // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
        // 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
        // noexcept 实现仅向可抛槽类型放宽，不改变原异常处理。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
#define SET(field,fn) v.field=reinterpret_cast<decltype(v.field)>(&fn)
        SET(common.deletingDestructorAt04,DeleteRadio);SET(common.beginAt44,RadioBegin);
        // Pure在_purecall返回后仍必定terminate；不存在需要交付给int槽的正常返回值。
        v.common.pollAt48=reinterpret_cast<decltype(v.common.pollAt48)>(&Pure);
        SET(common.endAt4C,One);SET(boundsAt8C,RadioBounds);
        SET(loadAt90,Pure);SET(storeAt94,Pure);
#undef SET
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
        return v;
    }();return t;
}
const SettingPage04RadioNativeVtable38& RadioTable() noexcept
{
    static const auto t=[]{auto v=RadioBaseTable();
        // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
        // 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
        v.common.pollAt48=reinterpret_cast<decltype(v.common.pollAt48)>(&RadioPoll);
        v.loadAt90=v.storeAt94=reinterpret_cast<decltype(v.loadAt90)>(&Ret);return v;}();return t;
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
}
const SettingPage04RadioNativeVtable38& CheckboxTable() noexcept
{
    static const auto table=[]{auto v=RadioTable();
        // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
        // 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
        v.common.beginAt44=reinterpret_cast<decltype(v.common.beginAt44)>(&CheckboxBegin);
        v.common.pollAt48=reinterpret_cast<decltype(v.common.pollAt48)>(&CheckboxPoll);
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
        return v;}();return table;
}
void DestroyRadio(Common* p) noexcept
{p->vptrAt00=&RadioBaseTable().common;ReleaseYtlReference(Field<Slot>(p,0x74));DestroyGuiDialogAt4F7AC0(p);}
void DestroyBlock(Common* p,unsigned kind) noexcept
{
    if(kind==3)
    {
        p->vptrAt00=&SettingPage04Block03NativeVtable().common;
        DestroyGuiControlSurfaceReferencesAt43EAA0(Field<GuiControlSurfaceReferences>(p,0x80));
        DestroyRadio(p);return;
    }
    if(kind==1)
    {
        p->vptrAt00=&SettingPage04ContainerNativeVtable(1).common.common;
        ReleaseYtlReference(Field<Slot>(p,0x88));ReleaseYtlReference(Field<Slot>(p,0x7C));
    }
    else if(kind==2||kind==5)
    {p->vptrAt00=&SettingPage04ManagedNativeVtable(kind).common.common.common;ReleaseYtlReference(Field<Slot>(p,0x7C));}
    DestroyGuiDialogIntoNativeAt4F7B80(p);
}
const SettingPage04NativeVtable39& ParentBaseTable() noexcept
{
    static const auto t=[]{SettingPage04NativeVtable39 v{GuiDialogIntoBaseVtable(), {}, {}};
        // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
        // 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
        // noexcept 实现仅向可抛槽类型放宽，不改变原异常处理。
        // 按值栈参数 YtlReferenceSlotRuntime 为 12 字节；无聚合返回或新增隐藏参数。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
        v.common.common.deletingDestructorAt04=reinterpret_cast<decltype(v.common.common.deletingDestructorAt04)>(&DeleteParentBase);
        v.common.common.drawAt60=reinterpret_cast<decltype(v.common.common.drawAt60)>(&ParentDraw);
        v.common.common.compositeAt5C=reinterpret_cast<decltype(v.common.common.compositeAt5C)>(&ParentComposite);
        v.reloadAt94=reinterpret_cast<decltype(v.reloadAt94)>(&Pure);
        v.boundsAt98=reinterpret_cast<decltype(v.boundsAt98)>(&ParentBounds);return v;}();return t;
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
}
}
const SettingPage04ContainerNativeVtable40& SettingPage04ContainerNativeVtable(unsigned kind)
{
    static const auto tables=[]{
        std::array<SettingPage04ContainerNativeVtable40,3> a;
        const auto loads=std::array{&Load00,&Load01,&Load04};
        const auto stores=std::array{&Store00,&Store01,&Store04};
        const auto deletes=std::array{&DeleteBlock<0>,&DeleteBlock<1>,&DeleteBlock<4>};
        for(unsigned i=0;i<3;++i)
        {
            auto& v=a[i];v.common=GuiDialogIntoBaseVtable();
            // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
            // 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
            // noexcept 实现仅向可抛槽类型放宽，不改变原异常处理。
            // 按值栈参数 YtlReferenceSlotRuntime 为 12 字节；无聚合返回或新增隐藏参数。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
#define SET(field,fn) v.field=reinterpret_cast<decltype(v.field)>(&fn)
            v.common.common.deletingDestructorAt04=reinterpret_cast<decltype(v.common.common.deletingDestructorAt04)>(deletes[i]);
            SET(common.common.beginAt44,ContainerBegin);SET(common.common.endAt4C,ContainerEnd);SET(boundsAt94,ContainerBounds);
            v.loadAt98=reinterpret_cast<decltype(v.loadAt98)>(loads[i]);v.storeAt9C=reinterpret_cast<decltype(v.storeAt9C)>(stores[i]);
            if(i!=2){SET(common.common.drawAt60,CombinedDraw);SET(common.common.compositeAt5C,CombinedComposite);}
            if(i==1){SET(common.common.beginAt44,Begin01);SET(common.common.pollAt48,Poll01);
                SET(common.common.drawAt60,NameDraw);SET(common.common.compositeAt5C,NameComposite);}
#undef SET
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
        }
        return a;
    }();
    if(kind==0)return tables[0];if(kind==1)return tables[1];if(kind==4)return tables[2];
    throw std::invalid_argument("Page04 container table");
}
const SettingPage04ManagedNativeVtable41& SettingPage04ManagedNativeVtable(unsigned kind)
{
    static const auto tables=[]{
        std::array<SettingPage04ManagedNativeVtable41,2> a;
        for(unsigned i=0;i<2;++i)
        {
            auto& v=a[i];v.common=SettingPage04ContainerNativeVtable(0);
            // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
            // 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
            // noexcept 实现仅向可抛槽类型放宽，不改变原异常处理。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
            v.common.common.common.deletingDestructorAt04=reinterpret_cast<decltype(v.common.common.common.deletingDestructorAt04)>(i?&DeleteBlock<5>:&DeleteBlock<2>);
            v.common.common.common.beginAt44=reinterpret_cast<decltype(v.common.common.common.beginAt44)>(&BeginManaged);
            v.common.common.common.pollAt48=reinterpret_cast<decltype(v.common.common.common.pollAt48)>(i?&Poll05:&Poll02);
            v.common.loadAt98=reinterpret_cast<decltype(v.common.loadAt98)>(i?&Load05:&Load02);
            v.common.storeAt9C=reinterpret_cast<decltype(v.common.storeAt9C)>(i?&Store05:&Store02);
            v.gateAtA0=reinterpret_cast<decltype(v.gateAtA0)>(i?&Gate05:&Gate02);
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
        }
        return a;
    }();if(kind==2)return tables[0];if(kind==5)return tables[1];throw std::invalid_argument("Page04 managed table");
}
const SettingPage04RadioNativeVtable38& SettingPage04Block03NativeVtable() noexcept
{
    static const auto t=[]{auto v=RadioBaseTable();
        // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
        // 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
        // noexcept 实现仅向可抛槽类型放宽，不改变原异常处理。
        // 按值栈参数 YtlReferenceSlotRuntime 为 12 字节；无聚合返回或新增隐藏参数。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
        v.common.deletingDestructorAt04=reinterpret_cast<decltype(v.common.deletingDestructorAt04)>(&DeleteBlock<3>);
        v.common.beginAt44=reinterpret_cast<decltype(v.common.beginAt44)>(&Ret);
        v.common.pollAt48=reinterpret_cast<decltype(v.common.pollAt48)>(&One);
        v.common.drawAt60=reinterpret_cast<decltype(v.common.drawAt60)>(&TextDraw);
        v.common.compositeAt5C=reinterpret_cast<decltype(v.common.compositeAt5C)>(&TextComposite);
        v.loadAt90=v.storeAt94=reinterpret_cast<decltype(v.loadAt90)>(&Ret);return v;}();return t;
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
}
const SettingPage04NativeVtable39& SettingPage04NativeVtable() noexcept
{
    static const auto t=[]{auto v=ParentBaseTable();
        // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
        // 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
        // noexcept 实现仅向可抛槽类型放宽，不改变原异常处理。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
        v.common.common.deletingDestructorAt04=reinterpret_cast<decltype(v.common.common.deletingDestructorAt04)>(&DeleteParent);
        v.common.common.beginAt44=reinterpret_cast<decltype(v.common.common.beginAt44)>(&ParentBegin);
        v.common.common.resultAt50=reinterpret_cast<decltype(v.common.common.resultAt50)>(&ParentResult);
        v.reloadAt94=reinterpret_cast<decltype(v.reloadAt94)>(&ParentReload);return v;}();return t;
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
}
void DestroySettingPage04NativeAt4F9840(Parent* p) noexcept
{p->common.vptrAt00=&ParentBaseTable().common.common;ReleaseYtlReference(p->footerAt78);DestroyGuiDialogIntoNativeAt4F7B80(&p->common);}
Parent* DeleteSettingPage04NativeAt4FE3B0(Parent* p,unsigned flags) noexcept
{p->common.vptrAt00=&SettingPage04NativeVtable().common.common;DestroySettingPage04NativeAt4F9840(p);if(flags&1u)::operator delete(p,std::size_t{0x88});return p;}
SettingPage04NativeScope::SettingPage04NativeScope(SettingPage04NativeServices& s) noexcept
    :previous_(active),surfaces_(s.surfaces),graphics_(s.graphics),input_(s.input),screen_(s.screen){active=&s;}
SettingPage04NativeScope::~SettingPage04NativeScope() noexcept {active=previous_;}
SettingPage04NativeServices& RequireSettingPage04NativeServices()
{if(!active)throw std::logic_error("SettingPage04 native services are not bound");return *active;}
}
