#include "PropertyPageTextNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
using namespace PropertyPageTextDetail;
namespace
{
// 原14项跳表：-205/-203从430C20进入，ECX=0；不运行Apply或DDX。
template<class Apply>BOOL Notify(const HWND* page,NMHDR* n,LRESULT* result,Apply apply,PropertyPageTextNativeServices& s)
{
    const HWND own=Read(*page);
    if(Read(n->hwndFrom)!=own&&Read(n->hwndFrom)!=s.win32.getParent(own)){*result=1;return FALSE;}
    const DWORD code=Read(n->code);const DWORD index=code+213u;
    if(index>13u){*result=0;return FALSE;}
    if(code==DWORD(-211)||code==DWORD(-210)||code==DWORD(-204)){*result=0;return FALSE;}
    if(code==DWORD(-202)){*result=apply()?0:2;return TRUE;}
    *result=0;return TRUE; // 含PSN_RESET(-203)：没有Cancel回滚或提交。
}
void Vertical(const HWND* page,HWND source,PropertyPageTextNativeServices& s)
{
    const DWORD value=255u-DWORD(s.win32.sendMessage(source,0x400,0,0));
    for(int i=0;i<3;++i)if(source==s.win32.getDlgItem(Read(*page),1500+i))
    {SetNumber(page,std::bit_cast<int>(value),1400+i,s);break;}
}
void DrawTextSwatch(PropertyPageTextNativeBody68h& b,PropertyPageTextNativeServices& s)
{
    auto* page=&b.pageAt00.endpointAt00.windowAt04;const HWND child=s.win32.getDlgItem(Read(*page),1600);
    const HDC dc=s.win32.getDC(child);const HPEN pen=s.win32.createPen(0,0,0);
    const HGDIOBJ oldPen=s.win32.selectObject(dc,pen);
    const DWORD color=TextColor(page,s);const HBRUSH brush=s.win32.createSolidBrush(color);
    const HGDIOBJ oldBrush=s.win32.selectObject(dc,brush);RECT rect{};s.win32.getClientRect(child,&rect);
    s.win32.rectangle(dc,rect.left,rect.top,rect.right,rect.bottom);
    s.win32.selectObject(dc,oldPen);s.win32.selectObject(dc,oldBrush);
    if(brush)s.win32.deleteObject(brush);if(pen)s.win32.deleteObject(pen);
    // 430BC1调用DeleteDC，不是ReleaseDC；WM_DRAWITEM的lParam结构未被使用。
    if(dc)s.win32.deleteDC(dc);
}
}
BOOL ProcessPropertyPageTextNativeAt430850(PropertyPageTextNativeBody68h& b,HWND,UINT m,WPARAM w,LPARAM l,LRESULT* result,DWORD map,PropertyPageTextNativeServices& s)
{
    if(map)return FALSE;auto* page=&b.pageAt00.endpointAt00.windowAt04;
    if(m==WM_INITDIALOG){InitText(b,s);*result=1;return TRUE;}
    if(m==WM_COMMAND)
    {
        const WORD id=LOWORD(w);
        if(HIWORD(w)==EN_CHANGE){EditColor(page,id,reinterpret_cast<HWND>(l),nullptr,s);*result=0;return TRUE;}
        if(id==1100)
        {
            const DWORD index=Read(b.colorIndexAt5C);auto* defaults=LookupArchive(s,0);
            const DWORD color=Read(Field<DWORD>(defaults,index?0x3C:0x38));LoadTextColor(b,color,FALSE,s);*result=0;return TRUE;
        }
        if(id==1300||id==1301)
        {
            const DWORD color=TextColor(page,s);Field<DWORD>(&b,0x60+Read(b.colorIndexAt5C)*4u)=color;
            ExchangePropertyPageTextNativeAt415FF0(b,TRUE,0xFFFFFFFFu,s);
            LoadTextColor(b,Read(Field<DWORD>(&b,0x60+Read(b.colorIndexAt5C)*4u)),FALSE,s);*result=0;return TRUE;
        }
        return FALSE;
    }
    if(m==WM_VSCROLL){Vertical(page,reinterpret_cast<HWND>(l),s);Invalidate(page,s.win32.getDlgItem(Read(*page),1600),s);*result=0;return TRUE;}
    if(m==WM_DRAWITEM){DrawTextSwatch(b,s);*result=0;return TRUE;}
    if(m==WM_NOTIFY)return Notify(page,reinterpret_cast<NMHDR*>(l),result,[&]{return ApplyPropertyPageTextNativeAt422D60(b,s);},s);
    return FALSE;
}
BOOL ProcessPropertyPageMessWinNativeAt42FB20(PropertyPageMessWinNativeBody9Ch& b,HWND,UINT m,WPARAM w,LPARAM l,LRESULT* result,DWORD map,PropertyPageTextNativeServices& s)
{
    if(map)return FALSE;auto* page=&b.pageAt00.endpointAt00.windowAt04;
    if(m==WM_INITDIALOG){InitMess(b,s);*result=1;return TRUE;}
    if(m==WM_COMMAND)
    {
        const WORD id=LOWORD(w);
        if(HIWORD(w)==EN_CHANGE){EditColor(page,id,reinterpret_cast<HWND>(l),&b.imageAt54,s);*result=0;return TRUE;}
        if(id==1100){DefaultMess(b,s);*result=0;return TRUE;}return FALSE;
    }
    if(m==WM_VSCROLL){Vertical(page,reinterpret_cast<HWND>(l),s);RefreshMess(b,s);*result=0;return TRUE;}
    if(m==WM_HSCROLL)
    {
        const HWND source=reinterpret_cast<HWND>(l);const LRESULT position=s.win32.sendMessage(source,0x400,0,0);
        if(source==s.win32.getDlgItem(Read(*page),1503))SendItem(page,1503,0x405,1,position,s);
        RefreshMess(b,s);*result=0;return TRUE;
    }
    if(m==WM_NOTIFY)return Notify(page,reinterpret_cast<NMHDR*>(l),result,[&]{return ApplyPropertyPageMessWinNativeAt4212A0(b,s);},s);
    return FALSE;
}
BOOL ProcessPropertyPageImageStaticNativeAt42E700(PropertyPageImageStaticNativeBody48h& b,HWND,UINT m,WPARAM,LPARAM,LRESULT* result,DWORD map,PropertyPageTextNativeServices& s)
{
    if(map||m!=WM_PAINT)return FALSE;
    BYTE* dib;
    if(ResolvePresent(b.dibAt3C,dib))
    {
        const HDC source=Read(Field<HDC>(dib,8));
        if(source)
        {
            const HWND window=Read(b.endpointAt00.windowAt04);PAINTSTRUCT paint;
            const HDC dc=s.win32.beginPaint(window,&paint);RECT dirty;s.win32.copyRect(&dirty,&paint.rcPaint);
            if(!s.win32.isRectEmpty(&dirty))
            {
                RECT r{};s.win32.getClientRect(Read(b.endpointAt00.windowAt04),&r);
                s.win32.bitBlt(dc,r.left,r.top,std::bit_cast<int>(DWORD(r.right)-DWORD(r.left)),
                    std::bit_cast<int>(DWORD(r.bottom)-DWORD(r.top)),source,0,0,SRCCOPY);
            }
            s.win32.endPaint(window,&paint);
        }
    }
    *result=0;return TRUE; // 空DIB/DC仍handled，但不BeginPaint。
}
}
