#include "PropertyPageTextNative.Internal.h"

namespace KaGuYa::Start::Runtime::PropertyPageTextDetail
{
namespace
{
struct Text final
{
    union {RuntimeCString value;};bool armed=false;
    Text()noexcept{}
    ~Text()noexcept{if(armed)DestroyRuntimeCStringAt40CD40(&value);}
    void Empty(PropertyPageTextNativeServices& s){InitializeEmptyRuntimeCString(value,s.strings);armed=true;}
    void Release(const char* cached)
    {armed=false;ReleaseRuntimeCStringHeader(reinterpret_cast<ATL::CStringData*>(const_cast<char*>(cached))-1);}
};
// 40BEB0：h/H后缀优先于0x前缀；非法尾使整值为0，DWORD溢出自然回绕。
int Number(const char* input,PropertyPageTextNativeServices& s)
{
    Text text;ConstructRuntimeCStringInputNativeAt40B530(&text.value,input,s.strings);text.armed=true;
    const char* chars=text.value.GetString();int end=text.value.GetLength(),start=0;DWORD radix=10,sign=1,value=0;
    auto byte=[&](int i)->char
    {
        if(i<0||i>Read((reinterpret_cast<const ATL::CStringData*>(chars)-1)->nDataLength))throw ATL::CAtlException(E_INVALIDARG);
        return chars[i];
    };
    if(end>=2&&(byte(end-1)=='h'||byte(end-1)=='H')){radix=16;--end;}
    else if(end>=3&&byte(0)=='0'&&(byte(1)=='x'||byte(1)=='X')){radix=16;start=2;}
    for(int i=start;i<end;++i)
    {
        const char c=byte(i);DWORD digit;
        if(c>='0'&&c<='9')digit=static_cast<DWORD>(c-'0');
        else if(radix==16&&c>='a'&&c<='f')digit=static_cast<DWORD>(c-'a'+10);
        else if(radix==16&&c>='A'&&c<='F')digit=static_cast<DWORD>(c-'A'+10);
        else if(radix==10&&i==start&&c=='-'){sign=0xFFFFFFFFu;continue;}
        else{sign=0;break;}
        value=value*radix+digit;
    }
    const int answer=std::bit_cast<int>(value*sign);text.Release(chars);return answer;
}
}
LRESULT SendItem(const HWND* page,int id,UINT message,WPARAM w,LPARAM l,PropertyPageTextNativeServices& s)
{const HWND item=s.win32.getDlgItem(Read(*page),id);return s.win32.sendMessage(item,message,w,l);}
void Invalidate(const HWND* page,HWND child,PropertyPageTextNativeServices& s)
{
    RECT r{};s.win32.getWindowRect(child,&r);
    if(s.win32.screenToClient(Read(*page),reinterpret_cast<POINT*>(&r)))
        s.win32.screenToClient(Read(*page),reinterpret_cast<POINT*>(&r.right));
    s.win32.invalidateRect(Read(*page),&r,TRUE);
}
void SetNumber(const HWND* page,int value,int id,PropertyPageTextNativeServices& s)
{
    const HWND item=s.win32.getDlgItem(Read(*page),id);Text text;text.Empty(s);
    FormatRuntimeCStringAt417130(text.value,"%d",value);
    const char* chars=text.value.GetString();s.win32.setWindowText(item,chars);text.Release(chars);
}
DWORD TextColor(const HWND* page,PropertyPageTextNativeServices& s)
{
    const DWORD r=255u-static_cast<DWORD>(SendItem(page,1500,0x400,0,0,s));
    const DWORD g=255u-static_cast<DWORD>(SendItem(page,1501,0x400,0,0,s));
    const DWORD b=255u-static_cast<DWORD>(SendItem(page,1502,0x400,0,0,s));
    return BYTE(r)|(DWORD(BYTE(g))<<8)|(DWORD(BYTE(b))<<16);
}
DWORD MessColor(const HWND* page,PropertyPageTextNativeServices& s)
{
    const DWORD r=255u-static_cast<DWORD>(SendItem(page,1500,0x400,0,0,s));
    const DWORD g=255u-static_cast<DWORD>(SendItem(page,1501,0x400,0,0,s));
    const DWORD b=255u-static_cast<DWORD>(SendItem(page,1502,0x400,0,0,s));
    const DWORD a=255u-static_cast<DWORD>(SendItem(page,1503,0x400,0,0,s));
    // 418D0B..1C没有movzx：保留越范围值的高位，不替换成RGB宏。
    return (r<<16)|(g<<8)|b|(a<<24);
}
void SheetButtons(HWND parent,PropertyPageTextNativeServices& s)
{
    if(Read(s.sheetButtonsAt6197EC))return;
    s.win32.setWindowText(s.win32.getDlgItem(parent,2),"\x95\xC2\x82\xB6\x82\xE9");
    s.win32.setWindowText(s.win32.getDlgItem(parent,1),"\x8D\x58\x90\x56");
    const LONG style=s.win32.getWindowLong(parent,GWL_EXSTYLE);
    const LONG changed=style&~0x400;
    if(style!=changed)s.win32.setWindowLong(parent,GWL_EXSTYLE,changed);
    s.sheetButtonsAt6197EC=1;
}
void LoadTextColor(PropertyPageTextNativeBody68h& body,DWORD color,BOOL ranges,PropertyPageTextNativeServices& s)
{
    auto* page=&body.pageAt00.endpointAt00.windowAt04;
    const int components[]={BYTE(color),BYTE(color>>8),BYTE(color>>16)};
    for(int i=0;i<3;++i)
    {
        const HWND item=s.win32.getDlgItem(Read(*page),1500+i);
        if(ranges)
        {
            s.win32.sendMessage(item,0x407,0,0);s.win32.sendMessage(item,0x408,0,255);
            s.win32.sendMessage(item,0x414,32,0);
        }
        s.win32.sendMessage(item,0x405,1,255-components[i]);SetNumber(page,components[i],1400+i,s);
    }
    Invalidate(page,s.win32.getDlgItem(Read(*page),1600),s);
}
void EditColor(const HWND* page,WORD id,HWND edit,PropertyPageImageStaticNativeBody48h* image,PropertyPageTextNativeServices& s)
{
    Text text;text.Empty(s);const int length=s.win32.getWindowTextLength(edit);
    if(length<0)throw ATL::CAtlException(E_INVALIDARG);
    char* chars=const_cast<char*>(text.value.GetString());auto* header=reinterpret_cast<ATL::CStringData*>(chars)-1;
    if(std::bit_cast<int>((1u-DWORD(header->nRefs))|(DWORD(header->nAllocLength)-DWORD(length)))<0)
    {PrepareRuntimeCStringWriteAt42D590(text.value,length);chars=const_cast<char*>(text.value.GetString());header=reinterpret_cast<ATL::CStringData*>(chars)-1;}
    if(length>header->nAllocLength)throw ATL::CAtlException(E_INVALIDARG);
    header->nDataLength=length;chars[length]=0;s.win32.getWindowText(edit,chars,std::bit_cast<int>(DWORD(length)+1));
    const int value=Number(chars,s);
    // 42489F/424A8F：改edit后仍用原value更新trackbar；不能将局部值夹到0..255。
    if(value<0)SetNumber(page,0,id,s);
    if(value>=256)SetNumber(page,255,id,s);
    for(int i=0;i<3;++i)
        if(edit==s.win32.getDlgItem(Read(*page),1400+i))
        {SendItem(page,1500+i,0x405,1,std::bit_cast<LONG>(255u-DWORD(value)),s);break;}
    if(!image)Invalidate(page,s.win32.getDlgItem(Read(*page),1600),s);
    else if(Read(image->endpointAt00.windowAt04))
    {
        UpdatePropertyPageImageStaticNativeAt438D60(*image,MessColor(page,s),s);
        Invalidate(page,Read(image->endpointAt00.windowAt04),s);
    }
    text.Release(chars);
}
void RefreshMess(PropertyPageMessWinNativeBody9Ch& body,PropertyPageTextNativeServices& s)
{
    auto* page=&body.pageAt00.endpointAt00.windowAt04;
    UpdatePropertyPageImageStaticNativeAt438D60(body.imageAt54,MessColor(page,s),s);
    Invalidate(page,Read(body.imageAt54.endpointAt00.windowAt04),s);
}
void DefaultMess(PropertyPageMessWinNativeBody9Ch& body,PropertyPageTextNativeServices& s)
{
    auto* page=&body.pageAt00.endpointAt00.windowAt04;
    int values[3];for(DWORD i=0;i<3;++i)values[i]=Read(Field<BYTE>(InlineArchive(s,0),0x40+i));
    for(int i=0;i<3;++i){SendItem(page,1500+i,0x405,1,255-values[i],s);SetNumber(page,values[i],1400+i,s);}
    const DWORD alpha=Read(Field<DWORD>(InlineArchive(s,0,RefOrder::BiasStride),0x44));
    SendItem(page,1503,0x405,1,std::bit_cast<LONG>(alpha),s);RefreshMess(body,s);
}
}
