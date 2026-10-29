#include <Runtime/MainFramePopupPositionNative.h>
#include <Runtime/EnvironmentRootRuntime.h>
#include <Runtime/MainFrameNativeCallbacks.h>
#include <bit>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
thread_local MainFramePopupPositionNativeServices* current;
template<class T>T Read(const void* p,DWORD offset=0)noexcept
{return *reinterpret_cast<const volatile T*>(reinterpret_cast<DWORD>(p)+offset);}
void Write(void* p,DWORD offset,DWORD value)noexcept
{*reinterpret_cast<volatile DWORD*>(reinterpret_cast<DWORD>(p)+offset)=value;}
MainFramePopupPositionNativeServices& Services()
{if(!current)throw std::logic_error("native PopupPosition requires explicit root/thunk services");return *current;}
BOOL __fastcall Process(EditWindowEndpointNativeBody24h* body,void*,HWND w,UINT m,WPARAM p,LPARAM l,LRESULT* out,DWORD map)
{return ProcessMainFramePopupPositionNativeAt430CA0(*body,w,m,p,l,out,map,Services());}
EditWindowEndpointNativeBody24h* __fastcall Delete(EditWindowEndpointNativeBody24h* body,void*,DWORD flags)
{
    auto& s=Services();if(auto* thunk=Read<void*>(body,0x14))FreeWindowThunkNativeAt53F190(thunk,s.thunks);
    if(flags&1u)s.memory.DeleteBody(body,0x2C);return body;
}
WNDPROC __fastcall Procedure(EditWindowEndpointNativeBody24h*,void*);
void __fastcall Final(EditWindowEndpointNativeBody24h*,void*,HWND){} // 原424DF0确为空叶。
LRESULT CALLBACK Bound(HWND first,UINT m,WPARAM w,LPARAM l)
{
    auto& s=Services();
    // 439F60与既有端点核同285字节；此作用域只借用当前API，不复制窗口或Root。
    class Messages final:public IEditWindowNativeMessageApi
    {
    public:
        explicit Messages(IMainFramePopupPositionNativeApi& a):api(a){}
        LRESULT CallWindowProcA(WNDPROC p,HWND h,UINT n,WPARAM v,LPARAM q)override{return api.CallWindowProcA(p,h,n,v,q);}
        LONG GetWindowLongA(HWND h,int n)override{return api.GetWindowLongA(h,n);}
        LONG SetWindowLongA(HWND h,int n,LONG v)override{return api.SetWindowLongA(h,n,v);}
    private:IMainFramePopupPositionNativeApi& api;
    } messages(s.api);
    return DispatchMainFrameNativeMessageAt43A080(*reinterpret_cast<EditWindowEndpointNativeBody24h*>(first),m,w,l,messages);
}
WNDPROC __fastcall Procedure(EditWindowEndpointNativeBody24h*,void*){return &Bound;}
const EditWindowEndpointNativeVtable4 table{
    reinterpret_cast<decltype(EditWindowEndpointNativeVtable4::processAt00)>(&Process),
    reinterpret_cast<decltype(EditWindowEndpointNativeVtable4::deleteAt04)>(&Delete),
    reinterpret_cast<decltype(EditWindowEndpointNativeVtable4::procedureAt08)>(&Procedure),
    reinterpret_cast<decltype(EditWindowEndpointNativeVtable4::finalMessageAt0C)>(&Final)};
class Api final:public IMainFramePopupPositionNativeApi
{
public:
    LRESULT CallWindowProcA(WNDPROC p,HWND w,UINT m,WPARAM a,LPARAM l)override{return ::CallWindowProcA(p,w,m,a,l);}
    LONG GetWindowLongA(HWND w,int i)override{return ::GetWindowLongA(w,i);}
    LONG SetWindowLongA(HWND w,int i,LONG v)override{return ::SetWindowLongA(w,i,v);}
    BOOL ClientToScreen(HWND w,POINT* p)override{return ::ClientToScreen(w,p);}
};
}
IMainFramePopupPositionNativeApi& DefaultMainFramePopupPositionNativeApi()noexcept{static Api api;return api;}
MainFramePopupPositionNativeScope::MainFramePopupPositionNativeScope(MainFramePopupPositionNativeServices& s)noexcept:previous_(current){current=&s;}
MainFramePopupPositionNativeScope::~MainFramePopupPositionNativeScope()noexcept{current=previous_;}
const EditWindowEndpointNativeVtable4& MainFramePopupPositionNativeVtable()noexcept{return table;}
BOOL InstallMainFramePopupPositionNativeAt4387D0(EditWindowEndpointNativeBody24h& body,HWND window,MainFramePopupPositionNativeServices& s)
{
    const auto procedure=body.vptrAt00->procedureAt08(&body);auto* thunk=Read<void*>(&body,0x14);
    if(!thunk){thunk=AllocateWindowThunkNativeAt53F0E4(s.thunks);body.thunkAt14=thunk;if(!thunk)return FALSE;}
    InitializeWindowThunkNativeAt53F1E6(thunk,reinterpret_cast<DWORD>(procedure),&body,s.thunks);
    const auto code=WindowThunkNativeCodeAt53F149(Read<void*>(&body,0x14),s.thunks);
    const auto previous=s.api.SetWindowLongA(window,GWL_WNDPROC,std::bit_cast<LONG>(reinterpret_cast<DWORD>(code)));
    if(!previous)return FALSE;
    body.windowAt04=window;body.previousAt20=reinterpret_cast<WNDPROC>(static_cast<DWORD>(previous));return TRUE;
}
BOOL ProcessMainFramePopupPositionNativeAt430CA0(EditWindowEndpointNativeBody24h& body,HWND,UINT message,WPARAM w,LPARAM l,
    LRESULT* result,DWORD map,MainFramePopupPositionNativeServices& s)
{
    const auto saved=Read<DWORD>(&body,0x24);
    const auto finish=[&](BOOL handled){Write(&body,0x24,saved);return handled;};
    if(map||message!=WM_WINDOWPOSCHANGING)return finish(FALSE);
    const auto phase=Read<DWORD>(&body,0x28);auto* position=reinterpret_cast<WINDOWPOS*>(l);
    if(phase==1)
    {
        s.source.EnsureRootAt413BE0();auto* root=s.source.RootAt6198C4();
        LocateMainFramePopupNativeAt46B640(root,reinterpret_cast<DWORD*>(&position->x),reinterpret_cast<DWORD*>(&position->y),s.source,s.api);
        const auto window=Read<HWND>(&body,4);const auto previous=Read<WNDPROC>(&body,0x20);
        static_cast<void>(s.api.CallWindowProcA(previous,window,WM_WINDOWPOSCHANGING,w,l));
        const auto code=WindowThunkNativeCodeAt53F149(Read<void*>(&body,0x14),s.thunks);
        const auto actual=s.api.GetWindowLongA(Read<HWND>(&body,4),GWL_WNDPROC);
        if(static_cast<DWORD>(actual)==reinterpret_cast<DWORD>(code))
        {
            const auto next=std::bit_cast<LONG>(reinterpret_cast<DWORD>(Read<WNDPROC>(&body,0x20)));
            if(s.api.SetWindowLongA(Read<HWND>(&body,4),GWL_WNDPROC,next))
            {body.previousAt20=&::DefWindowProcA;body.windowAt04=nullptr;}
        }
        s.source.EnsureRootAt413BE0();root=s.source.RootAt6198C4();s.source.PositionAt208(root).indexAt208=-1;
        Write(&body,0x28,Read<DWORD>(&body,0x28)+1u);*result=0;return finish(TRUE);
    }
    if(phase==0)
    {
        s.source.EnsureRootAt413BE0();auto& state=s.source.PositionAt208(s.source.RootAt6198C4());
        DWORD width=0,height=0;
        if(*static_cast<const volatile std::int32_t*>(&state.indexAt208)>=0)
        {height=Read<DWORD>(position,0x14);width=Read<DWORD>(position,0x10);}
        state.widthAt20C=width;state.heightAt210=height;Write(&body,0x28,Read<DWORD>(&body,0x28)+1u);
    }
    const auto window=Read<HWND>(&body,4);const auto previous=Read<WNDPROC>(&body,0x20);
    *result=s.api.CallWindowProcA(previous,window,WM_WINDOWPOSCHANGING,w,l);return finish(TRUE);
}
}
