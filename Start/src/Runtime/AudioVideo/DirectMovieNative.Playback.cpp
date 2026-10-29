#include "DirectMovieNative.Internal.h"
#include <dshow.h>

namespace KaGuYa::Start::Runtime
{
using namespace DirectMovieNativeDetail;
void LoadDirectMovieNativeAt4BC720(Body& b,const char* path,std::uint8_t loop,DirectMovieNativeServices& s)
{
    wchar_t wide[260];
    // 原版忽略转换返回值，不清零工作区，也没有换编码/路径兜底。
    s.source.ConvertAcpAt4BC74C(path,wide,260);
    if(Com<HRESULT,const wchar_t*,const wchar_t*>(Read(b.graphAt04),0x34,wide,nullptr)<0)
        Error(s,1,"\x83\x74\x83\x40\x83\x43\x83\x8B\x96\xBC\x82\xCC\x90\xDD\x92\xE8\x82\xC9\x8E\xB8\x94\x73 : DirectShow",0x4BC829);
    b.loopAt28=loop;
    const auto window=reinterpret_cast<OAHWND>(s.source.CaptureWindowAt4180C0(0x4BC773));
    if(Com<HRESULT,OAHWND>(Read(b.windowAt20),0x74,window)<0)
        Error(s,2,"\x90\x65\x83\x45\x83\x42\x83\x93\x83\x68\x83\x45\x82\xCC\x90\xDD\x92\xE8\x82\xC9\x8E\xB8\x94\x73 : DirectShow",0x4BC859);
    if(Com<HRESULT,long>(Read(b.windowAt20),0x24,0x44000000L)<0)
        Error(s,3,"\x83\x45\x83\x42\x83\x93\x83\x68\x83\x45\x83\x58\x83\x5E\x83\x43\x83\x8B\x82\xCC\x90\xDD\x92\xE8\x82\xC9\x8E\xB8\x94\x73 : DirectShow",0x4BC889);
    Com<HRESULT,OAHWND>(Read(b.windowAt20),0x7C,window);
    const auto position=s.source.PositionAt4AFFF0();
    if(Com<HRESULT,long,long,long,long>(Read(b.windowAt20),0x9C,position.x,position.y,position.width,position.height)<0)
        Error(s,4,"\x83\x45\x83\x42\x83\x93\x83\x68\x83\x45\x88\xCA\x92\x75\x82\xCC\x90\xDD\x92\xE8\x82\xC9\x8E\xB8\x94\x73 : DirectShow",0x4BC8B7);
    if(Com<HRESULT,OAHWND,long,LONG_PTR>(Read(b.eventAt1C),0x34,window,0x8001L,0)<0)
        Error(s,5,"\x83\x43\x83\x78\x83\x93\x83\x67\x92\xCA\x92\x6D\x82\xF0\x8F\x88\x97\x9D\x82\xB7\x82\xE9\x83\x45\x83\x42\x83\x93\x83\x68\x83\x45\x82\xCC\x93\x6F\x98\x5E\x82\xC9\x8E\xB8\x94\x73 : DirectShow",0x4BC8E7);
    const auto* table=Read(b.vptrAt00);b.stateAt2C=2;table->runAt10(&b);
}
void RunDirectMovieNativeAt4BCC50(Body& b)
{
    if(!Read(b.stateAt2C))return;
    if(auto* audio=Read(b.audioAt14))
    {
        long previous;
        const auto hr=Com<HRESULT,long*>(audio,0x20,&previous);
        if(hr!=E_NOTIMPL&&hr>=0)
        {
            const auto volume=Read(b.mutedAt34)?-10000L:Read(b.volumeAt30);
            Com<HRESULT,long>(Read(b.audioAt14),0x1C,volume);
        }
    }
    Com<HRESULT>(Read(b.controlAt08),0x1C);
    Com<HRESULT,long>(Read(b.windowAt20),0x94,-1);
    b.stateAt2C=1;
}
void PauseDirectMovieNativeAt4BC910(Body& b)
{
    auto* control=Read(b.controlAt08);if(!control)return;
    const auto state=Read(b.stateAt2C);
    if(state==1){if(Com<HRESULT>(control,0x20)>=0)b.stateAt2C=3;}
    else if(state==2||state==3)Read(b.vptrAt00)->runAt10(&b);
}
void RewindDirectMovieNativeAt4C34A0(Body& b)
{
    auto* control=Read(b.controlAt08);if(!control||!Read(b.seekingAt0C))return;
    const auto state=Read(b.stateAt2C);if(state!=3&&state!=1)return;
    Com<HRESULT>(control,0x24);
    auto* seeking=Read(b.seekingAt0C);LONGLONG position=0;b.stateAt2C=2;
    Com<HRESULT,LONGLONG*,DWORD,LONGLONG*,DWORD>(seeking,0x38,&position,1,nullptr,0);
    Com<HRESULT>(Read(b.controlAt08),0x20);
}
void FinishDirectMovieNativeAt4AC940(Body& b)
{
    auto* control=Read(b.controlAt08);
    if(control&&Read(b.stateAt2C)==1&&Com<HRESULT>(control,0x20)>=0)b.stateAt2C=0;
}
void PulseDirectMovieNativeAt4B5010(Body& b)
{
    auto* window=Read(b.windowAt20);if(!window)return;
    // SDK IVideoWindow：+90 为 put_FullScreenMode，Run 的 +94 为 SetWindowForeground。
    Com<HRESULT,long>(window,0x90,-1);Com<HRESULT,long>(Read(b.windowAt20),0x90,0);
}
void ResizeDirectMovieNativeAt4BC220(Body& b,DirectMovieNativeServices& s)
{
    if(!Read(b.windowAt20))return;
    const auto p=s.source.PositionAt4AFFF0();
    Com<HRESULT,long,long,long,long>(Read(b.windowAt20),0x9C,p.x,p.y,p.width,p.height);
}
}
