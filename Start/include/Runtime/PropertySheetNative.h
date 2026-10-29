#pragma once
#include <Runtime/PropertyPageNative.h>

namespace KaGuYa::Start::Runtime
{
struct PropertySheetNativeHeader34h final
{
    DWORD sizeAt00,flagsAt04;
    HWND parentAt08;
    HINSTANCE instanceAt0C;
    HICON iconAt10;
    LPCSTR captionAt14;
    UINT pagesAt18,startAt1C;
    HPROPSHEETPAGE* handlesAt20;
    PFNPROPSHEETCALLBACK callbackAt24;
    HBITMAP watermarkAt28;
    HPALETTE paletteAt2C;
    HBITMAP headerAt30;
};
struct PropertySheetNativeBody64h final
{
    union{EditWindowEndpointNativeBody24h endpointAt00;};
    PropertySheetNativeHeader34h headerAt24;
    HPROPSHEETPAGE* pagesAt58;
    std::int32_t countAt5C,capacityAt60;
    PropertySheetNativeBody64h()noexcept{}
};
// 六页存储保持原偏移；各页54h以后的字段由自己的原生类型/生命周期管理。
struct GamePropertyNativeBody33Ch final
{
    union{PropertySheetNativeBody64h sheetAt00;};
    alignas(4) BYTE pagesAt64[0x2D0];
    std::int32_t lastPageAt334;
    BYTE centeredAt338,unwrittenAt339[3];
    GamePropertyNativeBody33Ch()noexcept{}
};
static_assert(sizeof(PropertySheetNativeHeader34h)==0x34&&sizeof(PropertySheetNativeBody64h)==0x64);
static_assert(offsetof(PropertySheetNativeHeader34h,handlesAt20)==0x20&&offsetof(PropertySheetNativeHeader34h,callbackAt24)==0x24);
static_assert(offsetof(PropertySheetNativeBody64h,pagesAt58)==0x58&&offsetof(PropertySheetNativeBody64h,countAt5C)==0x5C);
static_assert(sizeof(GamePropertyNativeBody33Ch)==0x33C&&offsetof(GamePropertyNativeBody33Ch,lastPageAt334)==0x334);
static_assert(offsetof(GamePropertyNativeBody33Ch,centeredAt338)==0x338);
class IPropertySheetNativeApi
{
public:
    virtual ~IPropertySheetNativeApi()=default;
    virtual HPROPSHEETPAGE CreatePropertySheetPageA(const PropertyPageNativeHeader34h*)=0;
    virtual BOOL DestroyPropertySheetPage(HPROPSHEETPAGE)=0;
    virtual LRESULT SendMessageA(HWND,UINT,WPARAM,LPARAM)=0;
    virtual LRESULT CallWindowProcA(WNDPROC,HWND,UINT,WPARAM,LPARAM)=0;
    virtual LONG GetWindowLongA(HWND,int)=0;
    virtual LONG SetWindowLongA(HWND,int,LONG)=0;
    virtual BOOL DestroyWindow(HWND)=0;
    virtual void* Recalloc(void*,std::size_t,std::size_t)=0;
    virtual void Free(void*)=0;
    virtual void RaiseException(DWORD,DWORD)=0;
    virtual HWND GetActiveWindow()=0;
    virtual INT_PTR PropertySheetA(const PropertySheetNativeHeader34h*)=0;
    virtual void SetLastError(DWORD)=0;
};
IPropertySheetNativeApi& DefaultPropertySheetNativeApi()noexcept;
struct PropertySheetNativeServices final
{
    IPropertySheetNativeApi& api;
    DialogEndpointNativeServices& endpoint;
};
class PropertySheetNativeScope final
{
public:
    explicit PropertySheetNativeScope(PropertySheetNativeServices&)noexcept;
    ~PropertySheetNativeScope()noexcept;
    PropertySheetNativeScope(const PropertySheetNativeScope&)=delete;
    PropertySheetNativeScope& operator=(const PropertySheetNativeScope&)=delete;
private:
    PropertySheetNativeServices* previous_;
    DialogEndpointNativeScope dialog_;
};
PropertySheetNativeServices& RequirePropertySheetNativeServices();
void InitializePropertySheetBaseNativeAt40B88D(PropertySheetNativeBody64h&,const EditWindowEndpointNativeVtable4&,
    WindowClassModuleNativeBody38h&)noexcept;
LRESULT AddPropertySheetPageNativeAt4129C0(PropertySheetNativeBody64h&,const PropertyPageNativeHeader34h*,PropertySheetNativeServices&);
void ReleasePropertySheetBaseNativeAt40CCB0(PropertySheetNativeBody64h&,PropertySheetNativeServices&);
int CALLBACK PropertySheetInitializedNativeAt430F60(HWND,UINT,LPARAM);
LRESULT CALLBACK BoundPropertySheetNativeAt439E40(HWND,UINT,WPARAM,LPARAM);
LRESULT DispatchPropertySheetNativeAt439E40(EditWindowEndpointNativeBody24h&,UINT,WPARAM,LPARAM,PropertySheetNativeServices&);
BOOL ProcessGamePropertyNativeAt42E590(GamePropertyNativeBody33Ch&,HWND,UINT,WPARAM,LPARAM,LRESULT*,DWORD,PropertySheetNativeServices&);
// 4295F6..4296EB仅原生显示子段；IME/Root标志、整33Ch及其页生命期在外层428E30。
void RunPropertySheetModalNativeAt4295F6(PropertySheetNativeBody64h&,PropertySheetNativeServices&);
}
