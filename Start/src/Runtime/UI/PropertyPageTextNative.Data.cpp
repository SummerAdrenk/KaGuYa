#include "PropertyPageTextNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
using namespace PropertyPageTextDetail;
BOOL ExchangePropertyPageTextNativeAt415FF0(PropertyPageTextNativeBody68h& b,BOOL save,DWORD id,PropertyPageTextNativeServices& s)
{
    auto* page=&b.pageAt00.endpointAt00.windowAt04;
    auto check=[&](int control,BYTE& value)
    {
        if(id!=0xFFFFFFFFu&&id!=DWORD(control))return;
        const WPARAM old=Read(value)!=0;
        const HWND item=s.win32.getDlgItem(Read(*page),control);
        if(save)value=s.win32.sendMessage(item,0xF0,0,0)==1;
        else s.win32.sendMessage(item,0xF1,old,0);
    };
    check(1200,b.shadowAt54);check(1201,b.antialiasAt55);
    if(id==0xFFFFFFFFu||id==1300)
    {
        HWND item=s.win32.getDlgItem(Read(*page),1300);
        if(save)b.colorIndexAt5C=0xFFFFFFFFu;
        DWORD index=0;
        do
        {
            if(s.win32.sendMessage(item,0x87,0,0)&0x40)
            {
                if(save){if(s.win32.sendMessage(item,0xF0,0,0))b.colorIndexAt5C=index;}
                else s.win32.sendMessage(item,0xF1,index==Read(b.colorIndexAt5C),0);
                ++index;
            }
            item=s.win32.getWindow(item,GW_HWNDNEXT);
        }while(item&&!(s.win32.getWindowLong(item,GWL_STYLE)&WS_GROUP));
    }
    check(1202,b.eraseVoiceAt56);check(1203,b.skipUnreadAt57);check(1204,b.continueSkipAt58);
    check(1205,b.skipVoiceAt59);check(1206,b.skipEffectAt5A);return TRUE;
}
BOOL ApplyPropertyPageTextNativeAt422D60(PropertyPageTextNativeBody68h& b,PropertyPageTextNativeServices& s)
{
    ExchangePropertyPageTextNativeAt415FF0(b,TRUE,0xFFFFFFFFu,s);
    auto* saved=InlineArchive(s,1,RefOrder::BiasStride);Field<BYTE>(saved,0x60)=Read(b.shadowAt54)!=0;
    saved=InlineArchive(s,1,RefOrder::BiasStride);Field<BYTE>(saved,0x61)=Read(b.antialiasAt55)!=0;
    auto* page=&b.pageAt00.endpointAt00.windowAt04;
    const DWORD color=TextColor(page,s);Field<DWORD>(&b,0x60+Read(b.colorIndexAt5C)*4u)=color;
    saved=InlineArchive(s,1,RefOrder::BiasStride);Field<DWORD>(saved,0x64)=Read(b.colorsAt60[0]);
    saved=InlineArchive(s,1,RefOrder::BiasStride);Field<DWORD>(saved,0x68)=Read(b.colorsAt60[1]);
    auto flags=[&](BYTE value,DWORD bit,bool inverse)
    {
        auto* target=LookupArchive(s,1);const DWORD old=Read(Field<DWORD>(target,0x38));
        Field<DWORD>(target,0x38)=((value!=0)!=inverse)?(old|bit):(old&~bit);
    };
    flags(Read(b.eraseVoiceAt56),4,false);
    const auto speed=SendItem(page,1503,0x400,0,0,s);
    saved=InlineArchive(s,1);Field<DWORD>(saved,0x34)=DWORD(speed);
    const auto wait=SendItem(page,1504,0x400,0,0,s);
    saved=InlineArchive(s,1);Field<DWORD>(saved,0x3C)=DWORD(wait);
    flags(Read(b.skipUnreadAt57),1,true);flags(Read(b.continueSkipAt58),8,false);
    flags(Read(b.skipVoiceAt59),2,false);flags(Read(b.skipEffectAt5A),0x20,false);return TRUE;
}
BOOL ApplyPropertyPageMessWinNativeAt4212A0(PropertyPageMessWinNativeBody9Ch& b,PropertyPageTextNativeServices& s)
{
    auto* page=&b.pageAt00.endpointAt00.windowAt04;
    const auto checked=SendItem(page,1200,0xF0,0,0,s);auto* saved=LookupArchive(s,1);
    const DWORD old=Read(Field<DWORD>(saved,0x38));Field<DWORD>(saved,0x38)=checked?(old|0x10):(old&~0x10u);
    const DWORD color=TextColor(page,s);saved=InlineArchive(s,1,RefOrder::BiasStride);Field<DWORD>(saved,0x40)=color;
    const auto alpha=SendItem(page,1503,0x400,0,0,s);saved=InlineArchive(s,1);Field<DWORD>(saved,0x44)=DWORD(alpha);
    return TRUE;
}
}
namespace KaGuYa::Start::Runtime::PropertyPageTextDetail
{
void InitText(PropertyPageTextNativeBody68h& b,PropertyPageTextNativeServices& s)
{
    auto* page=&b.pageAt00.endpointAt00.windowAt04;SheetButtons(s.win32.getParent(Read(*page)),s);
    b.shadowAt54=Read(Field<BYTE>(InlineArchive(s,1),0x60))!=0;
    b.antialiasAt55=Read(Field<BYTE>(InlineArchive(s,1),0x61))!=0;b.colorIndexAt5C=0;
    b.colorsAt60[0]=Read(Field<DWORD>(InlineArchive(s,1),0x64));
    const DWORD second=Read(Field<DWORD>(InlineArchive(s,1),0x68));
    const DWORD first=Read(b.colorsAt60[0]);b.colorsAt60[1]=second;LoadTextColor(b,first,TRUE,s);
    b.eraseVoiceAt56=(Read(Field<DWORD>(InlineArchive(s,1),0x38))&4)!=0;
    const HWND speed=s.win32.getDlgItem(Read(*page),1503);
    s.win32.sendMessage(speed,0x407,0,0);s.win32.sendMessage(speed,0x408,0,4);
    const DWORD speedValue=Read(Field<DWORD>(InlineArchive(s,1,RefOrder::BiasStride,EnvOrder::BeginEndBegin),0x34));
    s.win32.sendMessage(speed,0x405,1,std::bit_cast<LONG>(speedValue));
    const HWND wait=s.win32.getDlgItem(Read(*page),1504);
    s.win32.sendMessage(wait,0x407,0,0);s.win32.sendMessage(wait,0x408,0,4);
    const DWORD waitValue=Read(Field<DWORD>(InlineArchive(s,1,RefOrder::BiasStride,EnvOrder::BeginEndBegin),0x3C));
    s.win32.sendMessage(wait,0x405,1,std::bit_cast<LONG>(waitValue));
    b.skipUnreadAt57=(Read(Field<BYTE>(InlineArchive(s,1),0x38))&1)==0;
    b.continueSkipAt58=(Read(Field<DWORD>(InlineArchive(s,1),0x38))&8)!=0;
    b.skipVoiceAt59=(Read(Field<DWORD>(InlineArchive(s,1,RefOrder::BiasStride),0x38))&2)!=0;
    b.skipEffectAt5A=(Read(Field<DWORD>(InlineArchive(s,1),0x38))&0x20)!=0;
    ExchangePropertyPageTextNativeAt415FF0(b,FALSE,0xFFFFFFFFu,s);
}
void InitMess(PropertyPageMessWinNativeBody9Ch& b,PropertyPageTextNativeServices& s)
{
    auto* page=&b.pageAt00.endpointAt00.windowAt04;SheetButtons(s.win32.getParent(Read(*page)),s);
    if(Read(Field<BYTE>(InlineArchive(s,1),0x38))&0x10)s.win32.checkDlgButton(Read(*page),1200,1);
    int components[3];for(DWORD i=0;i<3;++i)components[i]=Read(Field<BYTE>(InlineArchive(s,1),0x40+i));
    for(int i=0;i<3;++i)
    {
        const HWND item=s.win32.getDlgItem(Read(*page),1500+i);
        s.win32.sendMessage(item,0x407,0,0);s.win32.sendMessage(item,0x408,0,255);s.win32.sendMessage(item,0x414,32,0);
        s.win32.sendMessage(item,0x405,1,255-components[i]);SetNumber(page,components[i],1400+i,s);
    }
    const DWORD alpha=Read(Field<DWORD>(InlineArchive(s,1,RefOrder::BiasStride),0x44));
    const HWND item=s.win32.getDlgItem(Read(*page),1503);
    s.win32.sendMessage(item,0x407,0,0);s.win32.sendMessage(item,0x408,0,255);s.win32.sendMessage(item,0x414,16,0);
    s.win32.sendMessage(item,0x405,1,std::bit_cast<LONG>(alpha));
    auto& image=b.imageAt54;InitializePropertyPageImageStaticNativeAt414660(image,s);
    UpdatePropertyPageImageStaticNativeAt438D60(image,MessColor(page,s),s);
    const HWND child=s.win32.getDlgItem(Read(*page),1600);
    const auto procedure=Read(image.endpointAt00.vptrAt00)->procedureAt08(&image.endpointAt00);
    void* thunk=Read(image.endpointAt00.thunkAt14);
    if(!thunk){thunk=AllocateWindowThunkNativeAt53F0E4(s.endpoints.thunks);image.endpointAt00.thunkAt14=thunk;}
    if(thunk)
    {
        InitializeWindowThunkNativeAt53F1E6(thunk,reinterpret_cast<DWORD>(procedure),&image,s.endpoints.thunks);
        const auto code=WindowThunkNativeCodeAt53F149(Read(image.endpointAt00.thunkAt14),s.endpoints.thunks);
        const LONG previous=s.win32.setWindowLong(child,GWL_WNDPROC,std::bit_cast<LONG>(reinterpret_cast<DWORD>(code)));
        if(previous){image.endpointAt00.previousAt20=reinterpret_cast<WNDPROC>(previous);image.endpointAt00.windowAt04=child;}
    }
}
}
