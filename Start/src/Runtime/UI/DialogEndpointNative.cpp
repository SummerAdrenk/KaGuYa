#include <Runtime/DialogEndpointNative.h>
#include <Runtime/EnvironmentRootRuntime.h>
#include <Runtime/ProcessWindowClassNative.h>
#include <bit>
#include <cstring>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
thread_local DialogEndpointNativeServices* current;
template<class T>T Read(const T& value)noexcept{return *static_cast<const volatile T*>(&value);}
using Record=Platform::PendingWindowCreateRecord;
void Store(void* p,std::size_t offset,const void* value)noexcept{std::memcpy(static_cast<BYTE*>(p)+offset,value,4);}
bool DirectResult(UINT m)noexcept
{return m==0x2E||m==0x2F||m==0x37||m==0x39||m==0x110||(m>=0x132&&m<=0x138);}
class Api final:public IDialogEndpointNativeApi
{
public:
    DWORD GetCurrentThreadId()override{return ::GetCurrentThreadId();}
    void EnterCriticalSection(LPCRITICAL_SECTION p)override{::EnterCriticalSection(p);}
    void LeaveCriticalSection(LPCRITICAL_SECTION p)override{::LeaveCriticalSection(p);}
    LONG SetWindowLongA(HWND w,int i,LONG v)override{return ::SetWindowLongA(w,i,v);}
    LONG GetWindowLongA(HWND w,int i)override{return ::GetWindowLongA(w,i);}
    HWND GetParent(HWND w)override{return ::GetParent(w);}
    HWND GetWindow(HWND w,UINT i)override{return ::GetWindow(w,i);}
    BOOL GetWindowRect(HWND w,RECT* p)override{return ::GetWindowRect(w,p);}
    BOOL GetClientRect(HWND w,RECT* p)override{return ::GetClientRect(w,p);}
    int MapWindowPoints(HWND a,HWND b,POINT* p,UINT n)override{return ::MapWindowPoints(a,b,p,n);}
    HMONITOR MonitorFromWindow(HWND w,DWORD f)override{return ::MonitorFromWindow(w,f);}
    BOOL GetMonitorInfoA(HMONITOR m,MONITORINFO* p)override{return ::GetMonitorInfoA(m,p);}
    BOOL SetWindowPos(HWND w,HWND a,int x,int y,int cx,int cy,UINT f)override{return ::SetWindowPos(w,a,x,y,cx,cy,f);}
};
}
IDialogEndpointNativeApi& DefaultDialogEndpointNativeApi()noexcept{static Api api;return api;}
DialogEndpointNativeServices DefaultDialogEndpointNativeServices()noexcept
{return {ProcessWindowClassWinModuleNative(),DefaultDialogEndpointNativeApi(),ProcessWindowThunkNativeServices(),DefaultEnvironmentRootMemory()};}
DialogEndpointNativeScope::DialogEndpointNativeScope(DialogEndpointNativeServices& s)noexcept:previous_(current){current=&s;}
DialogEndpointNativeScope::~DialogEndpointNativeScope()noexcept{current=previous_;}
DialogEndpointNativeServices& RequireDialogEndpointNativeServices()
{if(!current)throw std::logic_error("native dialog requires explicit module/thunk services");return *current;}
void PushDialogCreationNativeAt42F8B9(DialogEndpointNativeBody20h& body,DialogEndpointNativeServices& s)
{
    auto* record=&body.creationAt08;void* object=&body;Store(record,0,&object);
    const auto thread=s.api.GetCurrentThreadId();Store(record,4,&thread);
    s.api.EnterCriticalSection(&s.winModule.lockAt04);
    auto* next=static_cast<Record*>(Read(s.winModule.creationHeadAt1C));Store(record,8,&next);
    s.winModule.creationHeadAt1C=record;s.api.LeaveCriticalSection(&s.winModule.lockAt04);
}
void* PopWindowCreationNativeAt416B00(WindowClassWinModuleNativeBody2Ch* module,IDialogEndpointNativeApi& api)
{
    if(!module)return nullptr; // 原416B07的合法null receiver分支。
    auto* lock=&module->lockAt04;api.EnterCriticalSection(lock);
    auto* cursor=static_cast<Record*>(Read(module->creationHeadAt1C));void* object=nullptr;
    if(cursor)
    {
        const auto thread=api.GetCurrentThreadId();Record* previous=nullptr;
        for(;;)
        {
            auto* next=Read(cursor->next);
            if(Read(cursor->threadId)==thread)
            {
                if(previous)previous->next=next;else module->creationHeadAt1C=next;
                object=Read(cursor->object);break;
            }
            previous=cursor;cursor=next;if(!cursor)break;
        }
    }
    api.LeaveCriticalSection(lock);return object;
}
INT_PTR CALLBACK InitialDialogEndpointNativeAt4382D0(HWND window,UINT message,WPARAM w,LPARAM l)
{
    auto& s=RequireDialogEndpointNativeServices();
    auto* body=static_cast<DialogEndpointNativeBody20h*>(PopWindowCreationNativeAt416B00(&s.winModule,s.api));
    if(!body)return 0;
    const auto* table=Read(body->vptrAt00);body->windowAt04=window;
    const auto procedure=table->procedureAt08(body);auto* thunk=Read(body->thunkAt14);
    if(!thunk){thunk=AllocateWindowThunkNativeAt53F0E4(s.thunks);body->thunkAt14=thunk;}
    if(thunk)
    {
        InitializeWindowThunkNativeAt53F1E6(thunk,reinterpret_cast<DWORD>(procedure),body,s.thunks);
        thunk=Read(body->thunkAt14);
    }
    const auto code=reinterpret_cast<DLGPROC>(WindowThunkNativeCodeAt53F149(thunk,s.thunks));
    static_cast<void>(s.api.SetWindowLongA(window,DWLP_DLGPROC,std::bit_cast<LONG>(reinterpret_cast<DWORD>(code))));
    return code(window,message,w,l);
}
INT_PTR CALLBACK BoundDialogEndpointNativeAt415810(HWND first,UINT message,WPARAM w,LPARAM l)
{return DispatchDialogEndpointNativeAt415810(*reinterpret_cast<DialogEndpointNativeBody20h*>(first),message,w,l,RequireDialogEndpointNativeServices().api);}
INT_PTR DispatchDialogEndpointNativeAt415810(DialogEndpointNativeBody20h& body,UINT message,WPARAM w,LPARAM l,IDialogEndpointNativeApi& api)
{
    EditWindowMessageNativeContext24h context;
    context.message.hwnd=Read(body.windowAt04);auto* previous=Read(body.currentMessageAt18);
    const auto window=Read(body.windowAt04);context.message.message=message;
    body.currentMessageAt18=&context;const auto* table=Read(body.vptrAt00);
    context.message.lParam=l;context.sizeAt1C=0x24;context.handledAt20=1;context.message.wParam=w;
    context.message.time=0;context.message.pt.y=0;context.message.pt.x=0;LRESULT result=0;
    const auto handled=table->processAt00(&body,window,message,w,l,&result,0);
    body.currentMessageAt18=previous; // 原先恢复再检查结果；异常不补恢复。
    INT_PTR answer=handled;
    if(handled)
    {
        if(DirectResult(message))answer=result;
        else if(!(Read(body.destroyPendingAt1C)&1u))static_cast<void>(api.SetWindowLongA(Read(body.windowAt04),DWLP_MSGRESULT,result));
    }
    else if(message==WM_NCDESTROY)body.destroyPendingAt1C=Read(body.destroyPendingAt1C)|1u;
    const auto flags=Read(body.destroyPendingAt1C);
    if((flags&1u)&&!Read(body.currentMessageAt18))
    {
        const auto finalWindow=Read(body.windowAt04);body.destroyPendingAt1C=flags&~1u;
        const auto* finalTable=Read(body.vptrAt00);body.windowAt04=nullptr;
        finalTable->finalMessageAt0C(&body,finalWindow); // Final可删正文，其后不再读写body。
    }
    return answer;
}
void ReleaseDialogEndpointThunkNativeAt40E730(DialogEndpointNativeBody20h& body,DialogEndpointNativeServices& s)
{if(auto* thunk=Read(body.thunkAt14))FreeWindowThunkNativeAt53F190(thunk,s.thunks);}
}
