#include "PropertyPageTextNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
using namespace PropertyPageTextDetail;
namespace
{
using Dialog=DialogEndpointNativeBody20h;using Window=EditWindowEndpointNativeBody24h;
BOOL __fastcall TextMessage(Dialog* p,void*,HWND w,UINT m,WPARAM a,LPARAM b,LRESULT* r,DWORD map)
{return ProcessPropertyPageTextNativeAt430850(*reinterpret_cast<PropertyPageTextNativeBody68h*>(p),w,m,a,b,r,map,RequirePropertyPageTextNativeServices());}
BOOL __fastcall MessMessage(Dialog* p,void*,HWND w,UINT m,WPARAM a,LPARAM b,LRESULT* r,DWORD map)
{return ProcessPropertyPageMessWinNativeAt42FB20(*reinterpret_cast<PropertyPageMessWinNativeBody9Ch*>(p),w,m,a,b,r,map,RequirePropertyPageTextNativeServices());}
BOOL __fastcall ImageMessage(Window* p,void*,HWND w,UINT m,WPARAM a,LPARAM b,LRESULT* r,DWORD map)
{return ProcessPropertyPageImageStaticNativeAt42E700(*reinterpret_cast<PropertyPageImageStaticNativeBody48h*>(p),w,m,a,b,r,map,RequirePropertyPageTextNativeServices());}
Dialog* __fastcall TextDelete(Dialog* p,void*,DWORD flags)
{
    auto& s=RequirePropertyPageTextNativeServices();DestroyPropertyPageTextNativeAt411F10(*reinterpret_cast<PropertyPageTextNativeBody68h*>(p),s);
    if(flags&1)s.endpoints.memory.DeleteBody(p,0x68);return p;
}
Dialog* __fastcall MessDelete(Dialog* p,void*,DWORD flags)
{
    auto& s=RequirePropertyPageTextNativeServices();DestroyPropertyPageMessWinNativeAt411D60(*reinterpret_cast<PropertyPageMessWinNativeBody9Ch*>(p),s);
    if(flags&1)s.endpoints.memory.DeleteBody(p,0x9C);return p;
}
Window* __fastcall ImageDelete(Window* p,void*,DWORD flags)
{
    auto& s=RequirePropertyPageTextNativeServices();DestroyPropertyPageImageStaticNativeAt40E990(*reinterpret_cast<PropertyPageImageStaticNativeBody48h*>(p),s);
    if(flags&1)s.endpoints.memory.DeleteBody(p,0x48);return p;
}
INT_PTR CALLBACK PageProcedure(HWND self,UINT m,WPARAM w,LPARAM l)
{
    // 415710与415810同一24h MSG/结果/NCDESTROY协议；共同核不在本worker重复。
    return DispatchDialogEndpointNativeAt415810(*reinterpret_cast<Dialog*>(self),m,w,l,RequireDialogEndpointNativeServices().api);
}
LRESULT CALLBACK ImageProcedure(HWND self,UINT m,WPARAM w,LPARAM l)
{return DispatchEditWindowNativeMessageAt439F60(*reinterpret_cast<Window*>(self),m,w,l);}
DLGPROC __fastcall PageGetProcedure(Dialog*,void*){return &PageProcedure;}
WNDPROC __fastcall ImageGetProcedure(Window*,void*){return &ImageProcedure;}
// 原424DB0/424DF0是真空叶，不增加DestroyWindow或对象自删。
void __fastcall PageFinal(Dialog*,void*,HWND){}
void __fastcall ImageFinal(Window*,void*,HWND){}
const DialogEndpointNativeVtable4 textTable{
    reinterpret_cast<decltype(DialogEndpointNativeVtable4::processAt00)>(&TextMessage),
    reinterpret_cast<decltype(DialogEndpointNativeVtable4::deleteAt04)>(&TextDelete),
    reinterpret_cast<decltype(DialogEndpointNativeVtable4::procedureAt08)>(&PageGetProcedure),
    reinterpret_cast<decltype(DialogEndpointNativeVtable4::finalMessageAt0C)>(&PageFinal)};
const DialogEndpointNativeVtable4 messTable{
    reinterpret_cast<decltype(DialogEndpointNativeVtable4::processAt00)>(&MessMessage),
    reinterpret_cast<decltype(DialogEndpointNativeVtable4::deleteAt04)>(&MessDelete),
    reinterpret_cast<decltype(DialogEndpointNativeVtable4::procedureAt08)>(&PageGetProcedure),
    reinterpret_cast<decltype(DialogEndpointNativeVtable4::finalMessageAt0C)>(&PageFinal)};
const EditWindowEndpointNativeVtable4 imageTable{
    reinterpret_cast<decltype(EditWindowEndpointNativeVtable4::processAt00)>(&ImageMessage),
    reinterpret_cast<decltype(EditWindowEndpointNativeVtable4::deleteAt04)>(&ImageDelete),
    reinterpret_cast<decltype(EditWindowEndpointNativeVtable4::procedureAt08)>(&ImageGetProcedure),
    reinterpret_cast<decltype(EditWindowEndpointNativeVtable4::finalMessageAt0C)>(&ImageFinal)};
UINT CALLBACK MessCreation(HWND w,UINT m,PropertyPageNativeHeader34h* p)
{return PropertyPageCreationNativeAt430F20(w,m,p);} // 430DE0与430F20的消息2/412960合同相同。
}
const DialogEndpointNativeVtable4& PropertyPageTextNativeMethods()noexcept{return textTable;}
const DialogEndpointNativeVtable4& PropertyPageMessWinNativeMethods()noexcept{return messTable;}
const EditWindowEndpointNativeVtable4& PropertyPageImageStaticNativeMethods()noexcept{return imageTable;}
void InitializePropertyPageTextNativeAt40B91C(PropertyPageTextNativeBody68h& b,WindowClassModuleNativeBody38h& module)noexcept
{InitializePropertyPageNativeAt40B91C(b.pageAt00,textTable,0x66,&PropertyPageCreationNativeAt430F20,module);}
void InitializePropertyPageMessWinNativeAt40B9C0(PropertyPageMessWinNativeBody9Ch& b,WindowClassModuleNativeBody38h& module)noexcept
{
    InitializePropertyPageNativeAt40B91C(b.pageAt00,messTable,0x68,&MessCreation,module);
    auto& e=b.imageAt54.endpointAt00;e.windowAt04=nullptr;e.thunkAt14=nullptr;
    e.currentMessageAt18=nullptr;e.destroyPendingAt1C=0;e.previousAt20=&::DefWindowProcA;e.vptrAt00=&imageTable;
    b.imageAt54.backgroundAt24={};b.imageAt54.colorSurfaceAt30={};b.imageAt54.dibAt3C={};
}
void DestroyPropertyPageTextNativeAt411F10(PropertyPageTextNativeBody68h& b,PropertyPageTextNativeServices& s)
{ReleaseDialogEndpointThunkNativeAt40E730(b.pageAt00.endpointAt00,s.endpoints);}
void DestroyPropertyPageImageStaticNativeAt40E990(PropertyPageImageStaticNativeBody48h& b,PropertyPageTextNativeServices& s)noexcept
{
    const PropertyPageTextNativeScope scope(s);
    ReleaseImageReference(b.dibAt3C);ReleaseImageReference(b.colorSurfaceAt30);ReleaseImageReference(b.backgroundAt24);
    if(auto* thunk=Read(b.endpointAt00.thunkAt14))FreeWindowThunkNativeAt53F190(thunk,s.endpoints.thunks);
}
void DestroyPropertyPageMessWinNativeAt411D60(PropertyPageMessWinNativeBody9Ch& b,PropertyPageTextNativeServices& s)
{DestroyPropertyPageImageStaticNativeAt40E990(b.imageAt54,s);ReleaseDialogEndpointThunkNativeAt40E730(b.pageAt00.endpointAt00,s.endpoints);}
}
