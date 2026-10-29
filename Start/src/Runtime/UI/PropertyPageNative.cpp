#include <Runtime/PropertyPageNative.h>
#include <cstring>

namespace KaGuYa::Start::Runtime
{
void InitializePropertyPageNativeAt40B91C(PropertyPageNativeBody54h& page,const DialogEndpointNativeVtable4& table,
    WORD resource,PropertyPageNativeCallback callback,WindowClassModuleNativeBody38h& module)noexcept
{
    page.endpointAt00.windowAt04=nullptr;page.endpointAt00.thunkAt14=nullptr;
    page.endpointAt00.currentMessageAt18=nullptr;page.endpointAt00.destroyPendingAt1C=0;
    // 原先清+28..+50，再写size/flags和各指针；+20/+24只在实际赋值处写。
    std::memset(reinterpret_cast<BYTE*>(&page)+0x28,0,0x2C);
    page.headerAt20.sizeAt00=0x34;page.headerAt20.flagsAt04=0x80;
    page.headerAt20.instanceAt08=*static_cast<const volatile HINSTANCE*>(&module.resourceInstanceAt08);
    page.headerAt20.resourceAt0C=MAKEINTRESOURCEA(resource);
    page.headerAt20.procedureAt18=&InitialPropertyPageNativeAt438260;
    page.headerAt20.callbackAt20=callback;page.headerAt20.objectAt1C=reinterpret_cast<LPARAM>(&page);
    page.endpointAt00.vptrAt00=&table;
}
void PushPropertyPageCreationNativeAt412960(Platform::PendingWindowCreateRecord* record,void* object,DialogEndpointNativeServices& s)
{
    if(!record||!object)
    {
        ::RaiseException(0xC0000005u,1,0,nullptr);
        return;
    }
    std::memcpy(record,&object,4);const auto thread=s.api.GetCurrentThreadId();
    std::memcpy(reinterpret_cast<BYTE*>(record)+4,&thread,4);
    s.api.EnterCriticalSection(&s.winModule.lockAt04);
    auto* next=static_cast<Platform::PendingWindowCreateRecord*>(*static_cast<void* const volatile*>(&s.winModule.creationHeadAt1C));
    std::memcpy(reinterpret_cast<BYTE*>(record)+8,&next,4);s.winModule.creationHeadAt1C=record;
    s.api.LeaveCriticalSection(&s.winModule.lockAt04);
}
UINT CALLBACK PropertyPageCreationNativeAt430F20(HWND,UINT message,PropertyPageNativeHeader34h* header)
{
    if(message!=2)return 0;
    auto* object=reinterpret_cast<void*>(*static_cast<const volatile LPARAM*>(&header->objectAt1C));
    auto* record=reinterpret_cast<Platform::PendingWindowCreateRecord*>(reinterpret_cast<DWORD>(object)+8u);
    PushPropertyPageCreationNativeAt412960(record,object,RequireDialogEndpointNativeServices());return 1;
}
INT_PTR CALLBACK InitialPropertyPageNativeAt438260(HWND w,UINT m,WPARAM p,LPARAM l)
{
    // 438260/4382D0全指令协议同义（地址及rel32不同）；原20h Dialog启动核唯一实现。
    return InitialDialogEndpointNativeAt4382D0(w,m,p,l);
}
}
