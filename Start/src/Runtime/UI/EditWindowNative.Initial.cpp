#include <Runtime/EditWindowNativeCreation.h>
#include <bit>
#include <cstring>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Record=Platform::PendingWindowCreateRecord;
template<class T>T Read(const T& value) noexcept{return *static_cast<const volatile T*>(&value);}
void Store(void* object,std::size_t offset,const void* value,std::size_t count) noexcept
{std::memcpy(static_cast<std::byte*>(object)+offset,value,count);}
}
void EditWindowNativeCreationRuntime::PushCreationAt46FC82(EditWindowEndpointNativeBody24h& endpoint)
{
    // 不调用带DMI的Record ctor提前写零；原顺序先object，再线程，再锁内next/head。
    auto* record=&endpoint.creationAt08;void* object=&endpoint;
    Store(record,0,&object,4);
    const auto thread=dependencies_.windows.GetCurrentThreadId();Store(record,4,&thread,4);
    dependencies_.classes.EnterCriticalSection(&dependencies_.winModule.lockAt04);
    auto* next=static_cast<Record*>(Read(dependencies_.winModule.creationHeadAt1C));Store(record,8,&next,4);
    dependencies_.winModule.creationHeadAt1C=record;
    dependencies_.classes.LeaveCriticalSection(&dependencies_.winModule.lockAt04);
}
EditWindowEndpointNativeBody24h* EditWindowNativeCreationRuntime::PopForCurrentThreadAt416B00()
{
    auto& module=dependencies_.winModule;auto& api=dependencies_.classes;
    api.EnterCriticalSection(&module.lockAt04);
    auto* cursor=static_cast<Record*>(Read(module.creationHeadAt1C));
    EditWindowEndpointNativeBody24h* object=nullptr;
    if(cursor)
    {
        const auto thread=dependencies_.windows.GetCurrentThreadId();Record* previous=nullptr;
        for(;;)
        {
            auto* next=Read(cursor->next);
            if(Read(cursor->threadId)==thread)
            {
                if(previous)previous->next=next;else module.creationHeadAt1C=next;
                object=static_cast<EditWindowEndpointNativeBody24h*>(Read(cursor->object));break;
            }
            previous=cursor;cursor=next;if(!cursor)break;
        }
    }
    api.LeaveCriticalSection(&module.lockAt04);return object;
}
LRESULT EditWindowNativeCreationRuntime::InitialAt48E090(HWND window,UINT message,WPARAM w,LPARAM l)
{
    auto* endpoint=PopForCurrentThreadAt416B00();if(!endpoint)return 0;
    const auto* table=Read(endpoint->vptrAt00); // 原48E0A9在写HWND之前固定vptr。
    endpoint->windowAt04=window;
    const auto procedure=table->procedureAt08(endpoint);
    auto* thunk=Read(endpoint->thunkAt14);
    if(!thunk){thunk=AllocateWindowThunkNativeAt53F0E4();endpoint->thunkAt14=thunk;}
    if(thunk)InitializeWindowThunkNativeAt53F1E6(thunk,reinterpret_cast<std::uintptr_t>(procedure),endpoint);
    // 原48E0D5在Init返回后重新读slot；失败不换DefWindowProc、不产生窗口伪成功。
    const auto code=WindowThunkNativeCodeAt53F149(Read(endpoint->thunkAt14));
    auto& api=RequireEditWindowNativeMapServices().api;
    static_cast<void>(api.SetWindowLongA(window,GWL_WNDPROC,std::bit_cast<LONG>(reinterpret_cast<DWORD>(code))));
    return code(window,message,w,l);
}
}
