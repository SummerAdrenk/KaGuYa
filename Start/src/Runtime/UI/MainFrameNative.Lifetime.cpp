#include <Runtime/MainFrameNative.h>
#include <Runtime/EnvironmentRootRuntime.h>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace{template<class T>T Read(const T& value)noexcept{return *static_cast<const volatile T*>(&value);}}
void InitializeMainFrameBaseNativeAt434379(MainFrameNativeBody90h& body)noexcept
{
    ::new(&body.windowAt00)EditWindowEndpointNativeBody24h;
    auto& window=body.windowAt00;
    window.windowAt04=nullptr;window.thunkAt14=nullptr;window.currentMessageAt18=nullptr;
    window.destroyPendingAt1C=0;window.previousAt20=&::DefWindowProcA;
    for(auto& word:body.opaqueAt24)word=0;body.acceleratorAt30=nullptr;
}
void InitializeMainFrameTailNativeAt4343C6(MainFrameNativeBody90h& body,MainFrameNativeConstructorTables tables)noexcept
{
    body.windowAt00.vptrAt00=&tables.window;body.filterVptrAt34=tables.filter;body.idleVptrAt38=tables.idle;
    body.menuVptrAt54=tables.menu;body.menuWordAt5C=0;
    ::new(&body.popupAt60)EditWindowEndpointNativeBody24h;
    auto& popup=body.popupAt60;
    popup.windowAt04=nullptr;popup.thunkAt14=nullptr;popup.currentMessageAt18=nullptr;
    popup.destroyPendingAt1C=0;popup.previousAt20=&::DefWindowProcA;popup.vptrAt00=&tables.popup;
}
void ReleaseMainFrameBaseNativeAt40CC70(MainFrameNativeBody90h& body,WindowThunkNativeServices& thunks)
{if(auto* thunk=Read(body.windowAt00.thunkAt14))FreeWindowThunkNativeAt53F190(thunk,thunks);}
void ReleaseMainFrameNativeAt40EA80(MainFrameNativeBody90h& body,IUpdateUiNativeMemory& memory,WindowThunkNativeServices& thunks)
{
    if(auto* thunk=Read(body.popupAt60.thunkAt14))FreeWindowThunkNativeAt53F190(thunk,thunks);
    ReleaseUpdateUiNativeAt40EFB0(body.updateAt3C,memory);
    ReleaseMainFrameBaseNativeAt40CC70(body,thunks);
}
MainFrameNativeBody90h* DeleteMainFrameNativeAt411CC0(MainFrameNativeBody90h* body,DWORD flags,
    IUpdateUiNativeMemory& ui,WindowThunkNativeServices& thunks,IEnvironmentRootMemory& memory)
{
    ReleaseMainFrameNativeAt40EA80(*body,ui,thunks);
    if(flags&1u)memory.DeleteBody(body,0x90);return body;
}
}
