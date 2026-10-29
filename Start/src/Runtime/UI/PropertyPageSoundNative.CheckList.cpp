#include "PropertyPageSoundNative.Internal.h"

namespace KaGuYa::Start::Runtime::PropertySoundDetail
{
void ToggleList(EditWindowEndpointNativeBody24h& list,int index,IPropertyPageSoundNativeApi& api)
{
    ListItem3Ch clicked{};clicked.mask=8;clicked.item=index;clicked.stateMask=2;
    api.SendMessageA(Read(list.windowAt04),0x1005,0,reinterpret_cast<LPARAM>(&clicked));
    if(!(clicked.state&2))return;
    const auto state=static_cast<DWORD>(api.SendMessageA(Read(list.windowAt04),0x102C,index,0xF000))>>12;
    auto selected=api.SendMessageA(Read(list.windowAt04),0x100C,0xFFFFFFFFu,2);
    while(selected!=-1)
    {
        if(selected!=index)
        {
            ListItem3Ch item{};item.state=state==1?0x2000:0x1000;item.stateMask=0xF000;
            api.SendMessageA(Read(list.windowAt04),0x102B,selected,reinterpret_cast<LPARAM>(&item));
        }
        selected=api.SendMessageA(Read(list.windowAt04),0x100C,selected,2);
    }
}
}
namespace KaGuYa::Start::Runtime
{
BOOL ProcessPropertyPageSoundCheckListNativeAt42DAB0(EditWindowEndpointNativeBody24h& list,HWND,UINT message,
    WPARAM w,LPARAM l,LRESULT* result,DWORD map,IPropertyPageSoundNativeApi& api)
{
    using namespace PropertySoundDetail;
    if(map)return FALSE;
    if(message==WM_CREATE)
    {
        const auto answer=api.CallWindowProcA(Read(list.previousAt20),Read(list.windowAt04),message,w,l);
        if(!answer)api.SendMessageA(Read(list.windowAt04),0x1036,0,36);*result=answer;return TRUE;
    }
    if(message==WM_LBUTTONDOWN||message==WM_LBUTTONDBLCLK)
    {
        struct ListHit14h {POINT pt;UINT flags;int iItem,iSubItem;};
        static_assert(sizeof(ListHit14h)==0x14);
        ListHit14h hit{};hit.pt={static_cast<SHORT>(LOWORD(l)),static_cast<SHORT>(HIWORD(l))};
        const auto index=api.SendMessageA(Read(list.windowAt04),0x1012,0,reinterpret_cast<LPARAM>(&hit));
        if(index!=-1&&hit.flags==8&&api.GetKeyState(VK_CONTROL)>=0)ToggleList(list,hit.iItem,api);
        *result=1;return FALSE; // 428980将handled写0，继续原WNDPROC完成被点击项本身。
    }
    if(message==WM_KEYDOWN)
    {
        if(w==VK_SPACE)
        {
            const auto index=api.SendMessageA(Read(list.windowAt04),0x100C,0xFFFFFFFFu,1);
            if(index!=-1&&api.GetKeyState(VK_CONTROL)>=0)ToggleList(list,static_cast<int>(index),api);
        }
        *result=1;return FALSE;
    }
    return FALSE;
}
}
