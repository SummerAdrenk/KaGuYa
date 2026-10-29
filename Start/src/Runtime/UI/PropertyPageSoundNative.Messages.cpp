#include "PropertyPageSoundNative.Internal.h"

namespace KaGuYa::Start::Runtime::PropertySoundDetail
{
namespace
{
LRESULT ToggleTreeCheckbox(Page& p,HTREEITEM item,Services& s)
{
    if(!item)return 0;
    const auto state=(TreeState(p,item,s)>>12)&15;
    if(state<1||state>4)return 0;
    if(state<=2)
    {
        TreeState(p,item,(state^3u)<<12,s);
        if(CategorizedVoiceUi())SynchronizeCategorizedTreeClick(p,item,s);
    }
    RefreshVoice(p,s);RefreshTree(p,s);return 1;
}
void TreeDrawText(Page& p,HTREEITEM treeItem,RuntimeCString& text,Services& s)
{
    TVITEMA item{};item.mask=1;item.hItem=treeItem;
    for(DWORD n=256;;n+=n)
    {
        if(n>0x7FFFFFFFu)throw ATL::CAtlException(E_INVALIDARG);
        auto* head=reinterpret_cast<ATL::CStringData*>(const_cast<char*>(text.GetString()))-1;
        if(((1-head->nRefs)|(head->nAllocLength-static_cast<int>(n)))<0)PrepareRuntimeCStringWriteAt42D590(text,n);
        auto* chars=const_cast<char*>(text.GetString());head=reinterpret_cast<ATL::CStringData*>(chars)-1;
        if(n>static_cast<DWORD>(head->nAllocLength))throw ATL::CAtlException(E_INVALIDARG);
        head->nDataLength=static_cast<int>(n);chars[n]=0;item.pszText=chars;item.cchTextMax=static_cast<int>(n);
        if(!Send(Read(p.treeAt9C),0x110C,0,reinterpret_cast<LPARAM>(&item),s)||lstrlenA(item.pszText)<static_cast<int>(n)-1)break;
    }
    auto* chars=const_cast<char*>(text.GetString());auto* head=reinterpret_cast<ATL::CStringData*>(chars)-1;
    const auto length=strnlen_s(chars,head->nAllocLength);if(length>static_cast<unsigned>(head->nAllocLength))throw ATL::CAtlException(E_INVALIDARG);
    head->nDataLength=static_cast<int>(length);chars[length]=0;
}
LRESULT DrawItem(Page& p,NMCUSTOMDRAW* draw,Services& s)
{
    // NMLVCUSTOMDRAW/NMTVCUSTOMDRAW的30h/34h颜色字段同位；不访问新版SDK追加字段。
    auto* colors=reinterpret_cast<DWORD*>(reinterpret_cast<BYTE*>(draw)+0x30);
    if(draw->hdr.idFrom==1800)
    {
        if(!s.api.IsWindowEnabled(draw->hdr.hwndFrom)) {colors[0]=s.api.GetSysColor(17);return 2;}
        const auto state=Send(Read(p.listAt78.windowAt04),0x102C,draw->dwItemSpec,0xF000,s);
        if((static_cast<DWORD>(state)&0xFFFFF000u)!=0x1000)return 0;
        if(Send(Read(p.listAt78.windowAt04),0x102C,draw->dwItemSpec,2,s))
        {
            const auto focused=s.api.GetFocus()==Read(p.listAt78.windowAt04);
            colors[0]=s.api.GetSysColor(focused?14:17);colors[1]=s.api.GetSysColor(focused?17:15);
        }
        else colors[0]=s.api.GetSysColor(17);
        draw->uItemState&=~1u;return 2;
    }
    if(draw->hdr.idFrom!=2000)return 0;
    const auto selected=Next(p,9,nullptr,s);
    RuntimeCStringResult text([&](RuntimeCString* out){InitializeEmptyRuntimeCString(*out,s.strings);});
    TreeDrawText(p,reinterpret_cast<HTREEITEM>(draw->dwItemSpec),text.Value(),s);
    const auto state=TreeState(p,reinterpret_cast<HTREEITEM>(draw->dwItemSpec),s)&0xF000;
    if(state>0x2000)
    {
        if(selected==reinterpret_cast<HTREEITEM>(draw->dwItemSpec))
        {
            const auto focused=s.api.GetFocus()==Read(p.treeAt9C);
            colors[0]=s.api.GetSysColor(focused?14:17);colors[1]=s.api.GetSysColor(focused?17:15);
        }
        else colors[0]=s.api.GetSysColor(17);
    }
    else if(selected!=reinterpret_cast<HTREEITEM>(draw->dwItemSpec))
    {
        colors[0]=static_cast<DWORD>(Send(Read(p.treeAt9C),0x1120,0,0,s));
        colors[1]=static_cast<DWORD>(Send(Read(p.treeAt9C),0x111F,0,0,s));
        if(colors[0]==colors[1])colors[1]=s.api.GetSysColor(5);
    }
    else if(s.api.GetFocus()==Read(p.treeAt9C))
    {colors[0]=s.api.GetSysColor(14);colors[1]=s.api.GetSysColor(13);}
    else {colors[0]=static_cast<DWORD>(Send(Read(p.treeAt9C),0x1120,0,0,s));colors[1]=s.api.GetSysColor(15);}
    return 2;
}
}
LRESULT CustomDraw(Page& p,NMCUSTOMDRAW* n,BOOL& handled,Services& s)
{
    auto* context=static_cast<EditWindowMessageNativeContext24h*>(Read(p.pageAt00.endpointAt00.currentMessageAt18));
    context->handledAt20=TRUE;LRESULT result=0;
    switch(n->dwDrawStage)
    {
    case 1:result=(n->hdr.idFrom==1800||n->hdr.idFrom==2000)?32:0;break;
    case 0x10001:result=DrawItem(p,n,s);break;
    case 2:case 3:case 4:case 0x10002:case 0x10003:case 0x10004:case 0x30001:break;
    default:context->handledAt20=FALSE;break;
    }
    handled=Read(context->handledAt20);return result;
}
}
namespace KaGuYa::Start::Runtime
{
BOOL ProcessPropertyPageSoundNativeAt42FF30(PropertyPageSoundNativeBodyA8h& p,HWND,UINT message,WPARAM w,LPARAM l,
    LRESULT* result,DWORD map,PropertyPageSoundNativeServices& s)
{
    using namespace PropertySoundDetail;PropertyPageSoundNativeScope scope(s);if(map)return FALSE;
    if(message==WM_INITDIALOG) {*result=InitializePropertyPageSoundControlsNativeAt425690(p,s);return TRUE;}
    if(message==WM_HSCROLL||message==WM_VSCROLL) {*result=Scroll(p,reinterpret_cast<HWND>(l),s);return TRUE;}
    if(message==WM_COMMAND)
    {
        const auto id=LOWORD(w),notification=HIWORD(w);
        if(id==1200||id==1201||id==1202||id==1203||id==1205)
        {*result=Command(p,id,reinterpret_cast<HWND>(l),s);return TRUE;}
        if(notification==0x300) {*result=Command(p,0x300,reinterpret_cast<HWND>(l),s);return TRUE;}
        if(id==1100||id==1101) {*result=Command(p,id,reinterpret_cast<HWND>(l),s);return TRUE;}
    }
    auto* n=reinterpret_cast<NMHDR*>(l);
    if(message==WM_NOTIFY)
    {
        if(n->idFrom==1800&&static_cast<int>(n->code)==-101)
        {
            auto* change=reinterpret_cast<NMLISTVIEW*>(n);const auto state=change->uNewState;
            if(state&2)
            {
                auto* body=Sound(s);s.source.StopAt4383B0(body,Read(body->remainingEffectsBeginAt20));
                const auto value=TypeNumber(p,change->iItem,s);Send(Item(p,1504,s),0x405,1,std::bit_cast<int>(100u-static_cast<DWORD>(value)),s);
            }
            else if((state&0x1000)&&change->iItem==Send(Read(p.listAt78.windowAt04),0x100C,0xFFFFFFFFu,2,s))
            {auto* body=Sound(s);s.source.StopAt4383B0(body,Read(body->remainingEffectsBeginAt20));}
            RefreshEffects(p,s);RefreshTree(p,s);*result=0;return TRUE;
        }
        if(n->idFrom==2000)
        {
            if(n->code==TVN_KEYDOWN&&reinterpret_cast<NMTVKEYDOWN*>(n)->wVKey==VK_SPACE)
            {
                // 五态图包含禁用项，空格由树subclass交给与鼠标相同的二态切换，再消费默认操作。
                *result=ToggleTreeCheckbox(p,Next(p,TVGN_CARET,nullptr,s),s);return TRUE;
            }
            if(static_cast<int>(n->code)==-405)
            {
                auto* change=reinterpret_cast<NMTREEVIEWA*>(n);
                if(change->action!=1) {*result=0;return TRUE;}
                *result=Next(p,0,nullptr,s)==change->itemNew.hItem?1:0;return TRUE;
            }
            if(static_cast<int>(n->code)==-402) {*result=NotifyTree(p,reinterpret_cast<NMTREEVIEWA*>(n),s);return TRUE;}
            if(static_cast<int>(n->code)==-2)
            {
                const auto position=s.api.GetMessagePos();TVHITTESTINFO hit{};
                hit.pt={static_cast<SHORT>(LOWORD(position)),static_cast<SHORT>(HIWORD(position))};
                s.api.MapWindowPoints(nullptr,Read(p.treeAt9C),&hit.pt,1);Send(Read(p.treeAt9C),0x1111,0,reinterpret_cast<LPARAM>(&hit),s);
                if(!(hit.flags&0x40)) {*result=0;return TRUE;}
                *result=ToggleTreeCheckbox(p,hit.hItem,s);return TRUE;
            }
        }
        if(static_cast<int>(n->code)==-12)
        {BOOL handled=TRUE;*result=CustomDraw(p,reinterpret_cast<NMCUSTOMDRAW*>(n),handled,s);if(handled)return TRUE;}
    }
    else if(message==0x464) {RefreshTree(p,s);*result=0;return TRUE;}
    if(message!=WM_NOTIFY)return FALSE;
    const auto window=Read(p.pageAt00.endpointAt00.windowAt04);
    if(n->hwndFrom!=window&&n->hwndFrom!=s.api.GetParent(window)) {*result=1;return FALSE;}
    switch(static_cast<int>(n->code))
    {
    case -213:case -212:case -209:case -208:case -207:case -206:case -205:case -203:case -201:case -200:*result=0;return TRUE;
    case -202:*result=ApplyPropertyPageSoundNativeAt4216F0(p,s)?0:2;return TRUE;
    default:*result=0;return FALSE;
    }
}
}
