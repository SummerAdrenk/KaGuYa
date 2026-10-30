#include "LogDialogNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace LogDialogNativeDetail;
Common* __fastcall DeleteDC(Common* p,void*,std::uint32_t f) noexcept
{DeleteLogDialogNativeAt4FCA50(reinterpret_cast<DC*>(p),f);return p;}
Common* __fastcall DeleteText(Common* p,void*,std::uint32_t f) noexcept
{DeleteLogTextDialogNativeAt4FC920(reinterpret_cast<Text*>(p),f);return p;}
Common* __fastcall DeleteScroll(Common* p,void*,std::uint32_t f) noexcept
{DeleteLogScrollDialogNativeAt4FE140(reinterpret_cast<Scroll*>(p),f);return p;}
Row* __fastcall DeleteRow(Row* p,void*,std::uint32_t f) noexcept{return DeleteLogTextNativeAt4FE870(p,f);}
int __fastcall EndLeaf(Common*,void*){return 1;}
RECT* __fastcall TextBounds(Common* p,void*,RECT* out){*out=reinterpret_cast<Text*>(p)->boundsAt84;return out;}
RECT* __fastcall ScrollBounds(Common* p,void*,RECT* out){*out=reinterpret_cast<Scroll*>(p)->boundsAt5C;return out;}
}
const GuiDialogNativeVtable35& LogDialogNativeVtable() noexcept
{
    static const auto table=[]{
        auto t=GuiDialogBaseVtable();
        // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
        // 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
        // noexcept 实现仅向可抛槽类型放宽，不改变原异常处理。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
#define SLOT(f,x) t.f=reinterpret_cast<decltype(t.f)>(&x)
        SLOT(deletingDestructorAt04,DeleteDC);SLOT(beginAt44,BeginDC);SLOT(pollAt48,PollDC);SLOT(endAt4C,EndDC);
        SLOT(processInputAt54,InputDC);SLOT(flushDirtyAt58,FlushDC);
#undef SLOT
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
        return t;
    }();return table;
}
const LogDialogNativeVtable36& LogTextDialogNativeVtable() noexcept
{
    static const auto table=[]{
        LogDialogNativeVtable36 t{GuiDialogBaseVtable(), {}};
        // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
        // 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
        // noexcept 实现仅向可抛槽类型放宽，不改变原异常处理。
        // 按值栈参数 YtlReferenceSlotRuntime 为 12 字节；无聚合返回或新增隐藏参数。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
#define SLOT(f,x) t.common.f=reinterpret_cast<decltype(t.common.f)>(&x)
        SLOT(deletingDestructorAt04,DeleteText);SLOT(beginAt44,BeginText);SLOT(pollAt48,PollText);SLOT(endAt4C,EndLeaf);
        SLOT(compositeAt5C,CompositeText);SLOT(drawAt60,DrawText);
#undef SLOT
        t.boundsAt8C=reinterpret_cast<decltype(t.boundsAt8C)>(&TextBounds);return t;
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
    }();return table;
}
const LogDialogNativeVtable36& LogScrollDialogNativeVtable() noexcept
{
    static const auto table=[]{
        LogDialogNativeVtable36 t{GuiDialogBaseVtable(), {}};
        // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
        // 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
        // noexcept 实现仅向可抛槽类型放宽，不改变原异常处理。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
#define SLOT(f,x) t.common.f=reinterpret_cast<decltype(t.common.f)>(&x)
        SLOT(deletingDestructorAt04,DeleteScroll);SLOT(upperInputAt34,UpperScroll);SLOT(beginAt44,BeginScroll);
        SLOT(pollAt48,PollScroll);SLOT(endAt4C,EndLeaf);SLOT(processInputAt54,InputScroll);
#undef SLOT
        t.boundsAt8C=reinterpret_cast<decltype(t.boundsAt8C)>(&ScrollBounds);return t;
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
    }();return table;
}
const LogTextNativeVtable1& LogTextNativeVtable() noexcept
{
    // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
    // 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
    // noexcept 实现仅向可抛槽类型放宽，不改变原异常处理。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
    static const LogTextNativeVtable1 t{reinterpret_cast<decltype(t.deleteAt00)>(&DeleteRow)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
    return t;
}
}
