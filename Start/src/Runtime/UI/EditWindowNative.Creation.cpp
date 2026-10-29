#include <Runtime/EditWindowNativeCreation.h>
#include <bit>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
thread_local EditWindowNativeCreationRuntime* activeCreation=nullptr;
struct CreationScope final
{
    EditWindowNativeCreationRuntime* previous;
    explicit CreationScope(EditWindowNativeCreationRuntime& current) noexcept:previous(activeCreation){activeCreation=&current;}
    ~CreationScope() noexcept{activeCreation=previous;}
};
class Win32Creation final:public IEditWindowNativeCreationApi
{
public:
    DWORD GetCurrentThreadId() override{return ::GetCurrentThreadId();}
    void SetLastError(DWORD code) override{::SetLastError(code);}
    BOOL EnableWindow(HWND window,BOOL enabled) override{return ::EnableWindow(window,enabled);}
    HWND CreateWindowExA(DWORD ex,LPCSTR cls,LPCSTR title,DWORD style,int x,int y,int w,int h,HWND parent,HMENU menu,HINSTANCE instance,void* data) override
    {return ::CreateWindowExA(ex,cls,title,style,x,y,w,h,parent,menu,instance,data);}
};
EditWindowNativeCreationRuntime& RequireCreation()
{if(!activeCreation)throw std::logic_error("Edit Initial callback requires its creation module");return *activeCreation;}
}
IEditWindowNativeCreationApi& DefaultEditWindowNativeCreationApi() noexcept{static Win32Creation api;return api;}
LRESULT CALLBACK InitialEditWindowNativeAt48E090(HWND window,UINT message,WPARAM w,LPARAM l)
{return RequireCreation().InitialAt48E090(window,message,w,l);}
LRESULT CALLBACK InitialEditControlNativeAt48E020(HWND window,UINT message,WPARAM w,LPARAM l)
{return RequireCreation().InitialAt48E090(window,message,w,l);}
void EditWindowNativeCreationRuntime::CreateEndpoint(EditWindowEndpointNativeBody24h& endpoint,
    WindowClassNativeInfo68h& info,DWORD style,HWND parent)
{
    CreationScope callbacks(*this);
    const auto atom=RegisterWindowClassNativeAt451DA0(&dependencies_.module,&dependencies_.winModule,&info,
        &endpoint.previousAt20,dependencies_.classes);
    auto* thunk=endpoint.thunkAt14;
    if(!thunk){thunk=AllocateWindowThunkNativeAt53F0E4();endpoint.thunkAt14=thunk;}
    if(!thunk){dependencies_.windows.SetLastError(0xE);return;}
    InitializeWindowThunkNativeAt53F1E6(thunk,0,nullptr);
    if(!atom)return; // 原顺序是准备thunk之后才判AX，不丢失败前缀。
    PushCreationAt46FC82(endpoint);
    const auto& r=dependencies_.creationRectAt601054;
    const auto y=*static_cast<const volatile LONG*>(&r.top);
    const auto bottom=*static_cast<const volatile LONG*>(&r.bottom);
    const auto x=*static_cast<const volatile LONG*>(&r.left);
    const auto right=*static_cast<const volatile LONG*>(&r.right);
    const auto height=std::bit_cast<std::int32_t>(static_cast<DWORD>(bottom)-static_cast<DWORD>(y));
    const auto width=std::bit_cast<std::int32_t>(static_cast<DWORD>(right)-static_cast<DWORD>(x));
    static_cast<void>(dependencies_.windows.CreateWindowExA(0,reinterpret_cast<LPCSTR>(std::uintptr_t(atom)),nullptr,style,
        x,y,width,height,parent,reinterpret_cast<HMENU>(&endpoint),dependencies_.instanceAt61A69C,nullptr));
}
void EditWindowNativeCreationRuntime::CreateControlAt485628(EditControlNativeBody28h& control,HWND parent,DWORD style)
{
    PrepareEditControlClassNativeAt485631(dependencies_.controlClass);
    CreateEndpoint(control.endpointAt00,dependencies_.controlClass,style,parent);
}
}
