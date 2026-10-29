#include <Runtime/PropertySheetNative.h>
#include <atlbase.h>
#include <bit>
#include <cstdlib>
#include <cstring>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
thread_local PropertySheetNativeServices* current;
template<class T>T Read(const T& value)noexcept{return *static_cast<const volatile T*>(&value);}
class Api final:public IPropertySheetNativeApi
{
public:
    HPROPSHEETPAGE CreatePropertySheetPageA(const PropertyPageNativeHeader34h* p)override
    {return ::CreatePropertySheetPageA(reinterpret_cast<const PROPSHEETPAGEA*>(p));}
    BOOL DestroyPropertySheetPage(HPROPSHEETPAGE p)override{return ::DestroyPropertySheetPage(p);}
    LRESULT SendMessageA(HWND w,UINT m,WPARAM p,LPARAM l)override{return ::SendMessageA(w,m,p,l);}
    LRESULT CallWindowProcA(WNDPROC p,HWND w,UINT m,WPARAM a,LPARAM l)override{return ::CallWindowProcA(p,w,m,a,l);}
    LONG GetWindowLongA(HWND w,int i)override{return ::GetWindowLongA(w,i);}
    LONG SetWindowLongA(HWND w,int i,LONG v)override{return ::SetWindowLongA(w,i,v);}
    BOOL DestroyWindow(HWND w)override{return ::DestroyWindow(w);}
    void* Recalloc(void* p,std::size_t n,std::size_t size)override{return ::_recalloc(p,n,size);}
    void Free(void* p)override{::free(p);}
    void RaiseException(DWORD code,DWORD flags)override{::RaiseException(code,flags,0,nullptr);}
    HWND GetActiveWindow()override{return ::GetActiveWindow();}
    INT_PTR PropertySheetA(const PropertySheetNativeHeader34h* p)override{return ::PropertySheetA(reinterpret_cast<const PROPSHEETHEADERA*>(p));}
    void SetLastError(DWORD e)override{::SetLastError(e);}
};
}
IPropertySheetNativeApi& DefaultPropertySheetNativeApi()noexcept{static Api api;return api;}
PropertySheetNativeScope::PropertySheetNativeScope(PropertySheetNativeServices& s)noexcept:previous_(current),dialog_(s.endpoint){current=&s;}
PropertySheetNativeScope::~PropertySheetNativeScope()noexcept{current=previous_;}
PropertySheetNativeServices& RequirePropertySheetNativeServices()
{if(!current)throw std::logic_error("native PropertySheet requires explicit page/thunk services");return *current;}
void InitializePropertySheetBaseNativeAt40B88D(PropertySheetNativeBody64h& body,const EditWindowEndpointNativeVtable4& table,
    WindowClassModuleNativeBody38h& module)noexcept
{
    body.endpointAt00.windowAt04=nullptr;body.endpointAt00.thunkAt14=nullptr;
    body.endpointAt00.currentMessageAt18=nullptr;body.endpointAt00.destroyPendingAt1C=0;body.endpointAt00.previousAt20=&::DefWindowProcA;
    body.pagesAt58=nullptr;body.countAt5C=0;body.capacityAt60=0;
    std::memset(&body.headerAt24,0,0x34);body.headerAt24.sizeAt00=0x34;body.headerAt24.flagsAt04=0x100;
    body.headerAt24.instanceAt0C=Read(module.resourceInstanceAt08);body.headerAt24.handlesAt20=nullptr;
    body.headerAt24.pagesAt18=0;body.headerAt24.captionAt14=nullptr;body.headerAt24.startAt1C=0;body.headerAt24.parentAt08=nullptr;
    body.headerAt24.callbackAt24=&PropertySheetInitializedNativeAt430F60;body.endpointAt00.vptrAt00=&table;
}
LRESULT AddPropertySheetPageNativeAt4129C0(PropertySheetNativeBody64h& body,const PropertyPageNativeHeader34h* page,PropertySheetNativeServices& s)
{
    auto& api=s.api;const auto created=api.CreatePropertySheetPageA(page);if(!created)return 0;
    const auto window=Read(body.endpointAt00.windowAt04);
    if(window)
    {
        const auto result=api.SendMessageA(window,0x467,0,reinterpret_cast<LPARAM>(created));
        if(!result)static_cast<void>(api.DestroyPropertySheetPage(created));return result;
    }
    auto count=Read(body.countAt5C);const auto capacity=Read(body.capacityAt60);
    if(count==capacity)
    {
        auto* storage=Read(body.pagesAt58);DWORD aliasWitness;
        const auto local=reinterpret_cast<DWORD>(&aliasWitness),begin=reinterpret_cast<DWORD>(storage);
        if(local>=begin&&local<begin+static_cast<DWORD>(capacity)*4u)throw ATL::CAtlException(E_FAIL);
        const DWORD grown=capacity?static_cast<DWORD>(count)*2u:1u;
        if(std::bit_cast<std::int32_t>(grown)<0||grown>0x1FFFFFFFu)
        {static_cast<void>(api.DestroyPropertySheetPage(created));return 0;}
        auto* next=static_cast<HPROPSHEETPAGE*>(api.Recalloc(storage,grown,4));
        if(!next){static_cast<void>(api.DestroyPropertySheetPage(created));return 0;}
        count=Read(body.countAt5C);body.capacityAt60=static_cast<std::int32_t>(grown);body.pagesAt58=next;
    }
    auto* slot=reinterpret_cast<HPROPSHEETPAGE*>(reinterpret_cast<DWORD>(Read(body.pagesAt58))+static_cast<DWORD>(count)*4u);
    if(slot){*slot=created;count=Read(body.countAt5C);}
    body.countAt5C=std::bit_cast<std::int32_t>(static_cast<DWORD>(count)+1u);return 1;
}
void ReleasePropertySheetBaseNativeAt40CCB0(PropertySheetNativeBody64h& body,PropertySheetNativeServices& s)
{
    auto& api=s.api;
    if(Read(body.countAt5C)>0)
        for(DWORD bits=0;std::bit_cast<std::int32_t>(bits)<Read(body.countAt5C);++bits)
        {
            const auto index=std::bit_cast<std::int32_t>(bits);
            if(index<0||index>=Read(body.countAt5C))api.RaiseException(0xC000008Cu,1);
            auto* pages=Read(body.pagesAt58);static_cast<void>(api.DestroyPropertySheetPage(pages[index]));
        }
    if(auto* pages=Read(body.pagesAt58)){api.Free(pages);body.pagesAt58=nullptr;}
    body.countAt5C=0;body.capacityAt60=0;
    if(auto* thunk=Read(body.endpointAt00.thunkAt14))FreeWindowThunkNativeAt53F190(thunk,s.endpoint.thunks);
}
int CALLBACK PropertySheetInitializedNativeAt430F60(HWND window,UINT message,LPARAM)
{
    if(message!=1)return 0;
    auto& s=RequirePropertySheetNativeServices();
    auto* body=static_cast<PropertySheetNativeBody64h*>(PopWindowCreationNativeAt416B00(&s.endpoint.winModule,s.endpoint.api));
    const auto* table=Read(body->endpointAt00.vptrAt00);
    const auto procedure=table->procedureAt08(&body->endpointAt00);auto* thunk=Read(body->endpointAt00.thunkAt14);
    if(!thunk){thunk=AllocateWindowThunkNativeAt53F0E4(s.endpoint.thunks);body->endpointAt00.thunkAt14=thunk;}
    if(thunk)
    {
        InitializeWindowThunkNativeAt53F1E6(thunk,reinterpret_cast<DWORD>(procedure),body,s.endpoint.thunks);
        const auto code=WindowThunkNativeCodeAt53F149(Read(body->endpointAt00.thunkAt14),s.endpoint.thunks);
        const auto previous=s.api.SetWindowLongA(window,GWL_WNDPROC,std::bit_cast<LONG>(reinterpret_cast<DWORD>(code)));
        if(previous){body->endpointAt00.previousAt20=reinterpret_cast<WNDPROC>(static_cast<DWORD>(previous));body->endpointAt00.windowAt04=window;}
    }
    body->headerAt24.pagesAt18=0;body->headerAt24.handlesAt20=nullptr;
    if(auto* pages=Read(body->pagesAt58)){s.api.Free(pages);body->pagesAt58=nullptr;}
    body->countAt5C=0;body->capacityAt60=0;return 0;
}
void RunPropertySheetModalNativeAt4295F6(PropertySheetNativeBody64h& body,PropertySheetNativeServices& s)
{
    PropertySheetNativeScope scope(s);const auto active=s.api.GetActiveWindow();const auto parent=Read(body.headerAt24.parentAt08);
    body.headerAt24.flagsAt04=Read(body.headerAt24.flagsAt04)&~DWORD{0x400};
    body.headerAt24.handlesAt20=Read(body.pagesAt58);body.headerAt24.pagesAt18=static_cast<UINT>(Read(body.countAt5C));
    body.headerAt24.parentAt08=parent?parent:active;
    auto* thunk=Read(body.endpointAt00.thunkAt14);
    if(!thunk){thunk=AllocateWindowThunkNativeAt53F0E4(s.endpoint.thunks);body.endpointAt00.thunkAt14=thunk;}
    if(!thunk){s.api.SetLastError(0x0E);return;}
    InitializeWindowThunkNativeAt53F1E6(thunk,0,nullptr,s.endpoint.thunks);
    // 24h普通端点与20hDialog拥有相同原创建记录偏移，但不使用彼此的窗口分派。
    PushPropertyPageCreationNativeAt412960(&body.endpointAt00.creationAt08,&body,s.endpoint);
    static_cast<void>(s.api.PropertySheetA(&body.headerAt24));
    auto* pages=Read(body.pagesAt58);body.headerAt24.pagesAt18=0;body.headerAt24.handlesAt20=nullptr;
    if(pages){s.api.Free(pages);body.pagesAt58=nullptr;}body.countAt5C=0;body.capacityAt60=0;
}
}
