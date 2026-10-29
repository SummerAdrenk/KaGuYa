#include "DirectMovieNative.Internal.h"
#include <dshow.h>
#include <exception>
#include <new>
#include <stdexcept>

#pragma comment(lib,"Ole32.lib")
#pragma comment(lib,"Strmiids.lib")

extern "C" int __cdecl _purecall();
namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace DirectMovieNativeDetail;
thread_local DirectMovieNativeServices* active=nullptr;
std::int32_t __fastcall Message(Body* b,void*,HWND h,UINT m,WPARAM w,LPARAM l,LRESULT* r,unsigned a)
{return MessageDirectMovieNativeAt4BF3C0(*b,h,m,w,l,r,a);}
HRESULT __fastcall Acquire(Body* b,void*){return AcquireDirectMovieNativeAt4AAD60(*b,RequireDirectMovieNativeServices());}
void __fastcall Release(Body* b,void*){ReleaseDirectMovieNativeAt4C08C0(*b);}
void __fastcall Load(Body* b,void*,const char* p,std::uint8_t loop){LoadDirectMovieNativeAt4BC720(*b,p,loop,RequireDirectMovieNativeServices());}
void __fastcall Run(Body* b,void*){RunDirectMovieNativeAt4BCC50(*b);}
void __fastcall Pause(Body* b,void*){PauseDirectMovieNativeAt4BC910(*b);}
void __fastcall Rewind(Body* b,void*){RewindDirectMovieNativeAt4C34A0(*b);}
void __fastcall Finish(Body* b,void*){FinishDirectMovieNativeAt4AC940(*b);}
void __fastcall Pulse(Body* b,void*){PulseDirectMovieNativeAt4B5010(*b);}
void __fastcall Resize(Body* b,void*){ResizeDirectMovieNativeAt4BC220(*b,RequireDirectMovieNativeServices());}
Body* __fastcall Delete(Body* b,void*,unsigned flags)noexcept{return DeleteDirectMovieNativeAt4A62A0(b,flags);}
const DirectMovieNativeVtable11 methods{
    reinterpret_cast<decltype(methods.messageAt00)>(&Message),reinterpret_cast<decltype(methods.acquireAt04)>(&Acquire),
    reinterpret_cast<decltype(methods.releaseAt08)>(&Release),reinterpret_cast<decltype(methods.loadAt0C)>(&Load),
    reinterpret_cast<decltype(methods.runAt10)>(&Run),reinterpret_cast<decltype(methods.pauseAt14)>(&Pause),
    reinterpret_cast<decltype(methods.rewindAt18)>(&Rewind),reinterpret_cast<decltype(methods.finishAt1C)>(&Finish),
    reinterpret_cast<decltype(methods.pulseAt20)>(&Pulse),reinterpret_cast<decltype(methods.resizeAt24)>(&Resize),
    reinterpret_cast<decltype(methods.deleteAt28)>(&Delete)};
Body* __fastcall DeleteBase(Body* b,void*,unsigned flags)noexcept;
// 5B48A0：前十槽为 _purecall，末槽4A72D0按4字节基类尺寸删除。
const std::uintptr_t retired[]{
    reinterpret_cast<std::uintptr_t>(&_purecall),reinterpret_cast<std::uintptr_t>(&_purecall),
    reinterpret_cast<std::uintptr_t>(&_purecall),reinterpret_cast<std::uintptr_t>(&_purecall),
    reinterpret_cast<std::uintptr_t>(&_purecall),reinterpret_cast<std::uintptr_t>(&_purecall),
    reinterpret_cast<std::uintptr_t>(&_purecall),reinterpret_cast<std::uintptr_t>(&_purecall),
    reinterpret_cast<std::uintptr_t>(&_purecall),reinterpret_cast<std::uintptr_t>(&_purecall),
    reinterpret_cast<std::uintptr_t>(&DeleteBase)};
Body* __fastcall DeleteBase(Body* b,void*,unsigned flags)noexcept
{RetireDirectMovieNativeAt4A4D70(*b);if(flags&1u)::operator delete(b,std::size_t{4});return b;}

// 4AAD60 的八个原 CP932 消息，不能以 ASCII 接口名代替整条错误文本。
#define MOVIE_INTERFACE_ERROR(name) name "\x83\x49\x83\x75\x83\x57\x83\x46\x83\x4E\x83\x67\x82\xCC\x8D\xEC\x90\xAC\x82\xC9\x8E\xB8\x94\x73 : DirectShow"
constexpr const char* acquireErrors[]={
    MOVIE_INTERFACE_ERROR("IGraphBuilder"),MOVIE_INTERFACE_ERROR("IMediaControl"),
    MOVIE_INTERFACE_ERROR("IMediaSeeking"),MOVIE_INTERFACE_ERROR("IMediaPosition"),
    MOVIE_INTERFACE_ERROR("IMediaEventEx"),MOVIE_INTERFACE_ERROR("IVideoWindow"),
    MOVIE_INTERFACE_ERROR("IBasicVideo"),MOVIE_INTERFACE_ERROR("IBasicAudio")};
#undef MOVIE_INTERFACE_ERROR
}
DirectMovieNativeScope::DirectMovieNativeScope(DirectMovieNativeServices& s)noexcept:DirectMovieNativeScope(&s){}
DirectMovieNativeScope::DirectMovieNativeScope(DirectMovieNativeServices* s)noexcept:previous_(active)
{active=s;if(s)hooks_.emplace(s->hooks);}
DirectMovieNativeScope::~DirectMovieNativeScope()noexcept{active=previous_;}
DirectMovieNativeServices& RequireDirectMovieNativeServices()
{if(!active)throw std::logic_error("DirectMovie native services are required");return *active;}
const DirectMovieNativeVtable11& DirectMovieNativeMethods()noexcept{return methods;}
void RetireDirectMovieNativeAt4A4D70(Body& b)noexcept
{b.vptrAt00=reinterpret_cast<const DirectMovieNativeVtable11*>(retired);}
namespace DirectMovieNativeDetail
{
[[noreturn]] void Error(DirectMovieNativeServices& s,int code,const char* text,unsigned site)
{s.source.ErrorAt420C50(text,site);throw code;}
}
HRESULT AcquireDirectMovieNativeAt4AAD60(Body& b,DirectMovieNativeServices& s)
{
    auto result=s.source.CreateGraphAt4AAD7A(CLSID_FilterGraph,nullptr,1,IID_IGraphBuilder,&b.graphAt04);
    if(result<0)Error(s,1,acquireErrors[0],0x4AAE3F);
    const GUID* ids[]={&IID_IMediaControl,&IID_IMediaSeeking,&IID_IMediaPosition,&IID_IMediaEventEx,&IID_IVideoWindow,&IID_IBasicVideo,&IID_IBasicAudio};
    void** slots[]={&b.controlAt08,&b.seekingAt0C,&b.positionAt10,&b.eventAt1C,&b.windowAt20,&b.videoAt18,&b.audioAt14};
    constexpr unsigned errorSites[]={0x4AAE69,0x4AAE93,0x4AAEBD,0x4AAEE7,0x4AAF11,0x4AAF3B,0x4AAF65};
    for(unsigned i=0;i<7;++i)
    {
        // 每次 QueryInterface 都重读 graph，允许回调替换正文内的接口槽。
        result=Com<HRESULT,REFIID,void**>(Read(b.graphAt04),0,*ids[i],slots[i]);
        if(result<0)Error(s,int(i+2),acquireErrors[i+1],errorSites[i]);
    }
    return result;
}
DirectMovieNativeBody38h* ConstructDirectMovieNativeAt4A0BFD(Body& b,DirectMovieNativeServices& s)
{
    DirectMovieNativeScope scope(s);
    b.vptrAt00=&methods;b.graphAt04=nullptr;b.controlAt08=nullptr;b.seekingAt0C=nullptr;b.positionAt10=nullptr;
    b.audioAt14=nullptr;b.videoAt18=nullptr;b.eventAt1C=nullptr;b.windowAt20=nullptr;b.neutralAt24=0;
    b.loopAt28=0;b.stateAt2C=0;b.volumeAt30=0;b.mutedAt34=0;
    try
    {
        AcquireDirectMovieNativeAt4AAD60(b,s);
        auto* hooks=s.source.CaptureHooksAt450E20(0x4A0C5D);
        Read(hooks->vptrAt00)->addAt00(hooks,&b);
    }
    catch(...)
    {
        // 59CBE7 -> 4A4D70 仅退虚表，不执行完整析构或注销/释放 COM。
        RetireDirectMovieNativeAt4A4D70(b);throw;
    }
    return &b;
}
void ReleaseDirectMovieNativeAt4C08C0(Body& b)
{
    if(auto* window=Read(b.windowAt20))Com<HRESULT,long>(window,0x4C,0);
    if(auto* event=Read(b.eventAt1C))Com<HRESULT,OAHWND,long,LONG_PTR>(event,0x34,0,0,0);
    void** slots[]={&b.audioAt14,&b.videoAt18,&b.windowAt20,&b.eventAt1C,&b.positionAt10,&b.seekingAt0C,&b.controlAt08,&b.graphAt04};
    for(auto** slot:slots)
    {
        if(auto* current=Read(*slot)){Com<ULONG>(current,8);*slot=nullptr;}
    }
}
void DestroyDirectMovieNativeAt4A3EA0(Body& b,DirectMovieNativeServices& s)noexcept
{
    DirectMovieNativeScope scope(s);b.vptrAt00=&methods;
    ReleaseDirectMovieNativeAt4C08C0(b);
    auto* hooks=s.source.CaptureHooksAt450E20(0x4A3F8D);
    Read(hooks->vptrAt00)->removeAt04(hooks,&b);
    RetireDirectMovieNativeAt4A4D70(b);
}
Body* DeleteDirectMovieNativeAt4A62A0(Body* b,unsigned flags)noexcept
{
    auto& s=RequireDirectMovieNativeServices();b->vptrAt00=&methods;
    ReleaseDirectMovieNativeAt4C08C0(*b);
    auto* hooks=s.source.CaptureHooksAt450E20(0x4A6394);
    Read(hooks->vptrAt00)->removeAt04(hooks,b);
    RetireDirectMovieNativeAt4A4D70(*b);
    if(flags&1u)::operator delete(b,std::size_t{0x38});return b;
}
HRESULT DirectMovieNativeSource::CreateGraphAt4AAD7A(REFCLSID clsid,IUnknown* outer,DWORD context,REFIID iid,void** out)
{return ::CoCreateInstance(clsid,outer,context,iid,out);}
int DirectMovieNativeSource::ConvertAcpAt4BC74C(const char* p,wchar_t* out,int count)
{return ::MultiByteToWideChar(CP_ACP,0,p,-1,out,count);}
HWND DirectMovieNativeSource::CaptureWindowAt4180C0(unsigned){return host_.MainWindow();}
DirectMoviePosition DirectMovieNativeSource::PositionAt4AFFF0(){return host_.CurrentVideoPosition();}
void DirectMovieNativeSource::ErrorAt420C50(const char* text,unsigned){host_.ShowDirectShowError(text);}
WinHookNativeBody30h* DirectMovieNativeSource::CaptureHooksAt450E20(unsigned){return hooks_.CaptureAt450E20();}
}
