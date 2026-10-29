#include <Runtime/EditWindowNativeLifetime.h>
#include <Runtime/YtlReferenceArrayRuntime.h>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
thread_local EditWindowNativeMapServices* activeServices=nullptr;
using Endpoint=EditWindowEndpointNativeBody24h;
template<class T>T Read(const T& value) noexcept{return *static_cast<const volatile T*>(&value);}
LRESULT CALLBACK WindowProcedure(Endpoint* body,UINT message,WPARAM w,LPARAM l)
{return DispatchEditWindowNativeMessageAt439F60(*body,message,w,l,RequireEditWindowNativeMapServices().api);}
LRESULT CALLBACK ControlProcedure(Endpoint* body,UINT message,WPARAM w,LPARAM l)
{return DispatchEditWindowNativeMessageAt439F60(*body,message,w,l,RequireEditWindowNativeMapServices().api);}
BOOL __fastcall WindowMap(Endpoint* p,void*,HWND,UINT message,WPARAM w,LPARAM l,LRESULT* result,DWORD map)
{return ProcessEditWindowNativeAt4855D0(*reinterpret_cast<EditWindowNativeBody98h*>(p),message,w,l,result,map,RequireEditWindowNativeMapServices());}
BOOL __fastcall ControlMap(Endpoint* p,void*,HWND,UINT message,WPARAM w,LPARAM l,LRESULT* result,DWORD map)
{return ProcessEditControlNativeAt485480(*reinterpret_cast<EditControlNativeBody28h*>(p),message,w,l,result,map,RequireEditWindowNativeMapServices().api);}
Endpoint* __fastcall DeleteWindow(Endpoint* p,void*,std::uint32_t flags) noexcept
{DeleteEditWindowNativeAt468810(reinterpret_cast<EditWindowNativeBody98h*>(p),flags);return p;}
Endpoint* __fastcall DeleteControl(Endpoint* p,void*,std::uint32_t flags)
{DeleteEditControlNativeAt4687E0(reinterpret_cast<EditControlNativeBody28h*>(p),flags);return p;}
WNDPROC __fastcall WindowTarget(Endpoint*,void*)
{return reinterpret_cast<WNDPROC>(&WindowProcedure);}
WNDPROC __fastcall ControlTarget(Endpoint*,void*)
{return reinterpret_cast<WNDPROC>(&ControlProcedure);}
// 424DF0/4834E0均为retn4；保留EAX，不添加Root移除或析构调用。
__declspec(naked) void __fastcall Final(Endpoint*,void*,HWND)
{__asm{ret 4}}
const EditWindowEndpointNativeVtable4 windowTable{
    reinterpret_cast<decltype(EditWindowEndpointNativeVtable4::processAt00)>(&WindowMap),
    reinterpret_cast<decltype(EditWindowEndpointNativeVtable4::deleteAt04)>(&DeleteWindow),
    reinterpret_cast<decltype(EditWindowEndpointNativeVtable4::procedureAt08)>(&WindowTarget),
    reinterpret_cast<decltype(EditWindowEndpointNativeVtable4::finalMessageAt0C)>(&Final)};
const EditWindowEndpointNativeVtable4 controlTable{
    reinterpret_cast<decltype(EditWindowEndpointNativeVtable4::processAt00)>(&ControlMap),
    reinterpret_cast<decltype(EditWindowEndpointNativeVtable4::deleteAt04)>(&DeleteControl),
    reinterpret_cast<decltype(EditWindowEndpointNativeVtable4::procedureAt08)>(&ControlTarget),
    reinterpret_cast<decltype(EditWindowEndpointNativeVtable4::finalMessageAt0C)>(&Final)};
}
const EditWindowEndpointNativeVtable4& EditWindowNativeVtable() noexcept{return windowTable;}
const EditWindowEndpointNativeVtable4& EditControlNativeVtable() noexcept{return controlTable;}
EditWindowNativeScope::EditWindowNativeScope(EditWindowNativeMapServices& services,WindowThunkNativeServices& thunks) noexcept
    :EditWindowNativeScope(&services,&thunks){}
EditWindowNativeScope::EditWindowNativeScope(EditWindowNativeMapServices* services,WindowThunkNativeServices* thunks) noexcept
    :previous_(activeServices),thunks_(thunks){activeServices=services;}
EditWindowNativeScope::~EditWindowNativeScope() noexcept{activeServices=previous_;}
EditWindowNativeMapServices& RequireEditWindowNativeMapServices()
{if(!activeServices)throw std::logic_error("Edit native window callbacks require external services");return *activeServices;}
void InitializeEditWindowNativeStorageAt46FA74(EditWindowNativeBody98h& body)
{
    ::new(&body.outerAt00)Endpoint;
    const auto previous=&::DefWindowProcA; // 46FA74同一进口先写outer，随后写Control。
    body.outerAt00.windowAt04=nullptr;body.outerAt00.thunkAt14=nullptr;
    body.outerAt00.currentMessageAt18=nullptr;body.outerAt00.destroyPendingAt1C=0;
    body.outerAt00.previousAt20=previous;body.outerAt00.vptrAt00=&windowTable;
    ::new(&body.controlAt24)EditControlNativeBody28h;
    auto& control=*::new(&body.controlAt24.endpointAt00)Endpoint;
    control.windowAt04=nullptr;control.thunkAt14=nullptr;control.currentMessageAt18=nullptr;
    control.destroyPendingAt1C=0;control.previousAt20=previous;control.vptrAt00=&controlTable;
    body.logicalAt4C={};body.marginsAt5C={};::new(&body.layoutAt6C)YtlReferenceSlotRuntime{};
    body.byteLimitAt7C=0;body.alignmentAt80=0;body.textColorAt84=0;body.backgroundAt88=0xFFFFFF;
    ::new(&body.brushAt8C)YtlReferenceSlotRuntime{};
    body.marginsAt5C={1,2,1,2}; // 原5B38A0的16字节，最后46FB5B发布。
    // creation records、event48和identifier78不在此构造中写，不能预填零或optional。
}
EditControlNativeBody28h* DeleteEditControlNativeAt4687E0(EditControlNativeBody28h* body,std::uint32_t flags)
{
    if(const auto thunk=Read(body->endpointAt00.thunkAt14))FreeWindowThunkNativeAt53F190(thunk);
    if(flags&1u)::operator delete(body,std::size_t{0x28});
    return body;
}
EditWindowNativeBody98h* DeleteEditWindowNativeAt468810(EditWindowNativeBody98h* body,std::uint32_t flags) noexcept
{
    body->outerAt00.vptrAt00=&windowTable;
    if(const auto window=Read(body->outerAt00.windowAt04))
        static_cast<void>(RequireEditWindowNativeMapServices().api.DestroyWindow(window));
    // 原FuncInfo5E6720 EHFlags5：任一步失败终止，不补扫后续字段或重试当前项。
    ReleaseYtlReference(body->brushAt8C);
    ReleaseYtlReference(body->layoutAt6C);
    if(const auto inner=Read(body->controlAt24.endpointAt00.thunkAt14))FreeWindowThunkNativeAt53F190(inner);
    if(const auto outer=Read(body->outerAt00.thunkAt14))FreeWindowThunkNativeAt53F190(outer);
    if(flags&1u)::operator delete(body,std::size_t{0x98});
    return body;
}
}
