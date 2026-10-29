#include "PropertyPageSoundNative.Internal.h"
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/ReadScriptSoundWaitSource.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
thread_local PropertyPageSoundNativeServices* current;
class Api final : public IPropertyPageSoundNativeApi
{
public:
    HWND GetDlgItem(HWND w,int id) override {return ::GetDlgItem(w,id);}
    HWND GetParent(HWND w) override {return ::GetParent(w);}
    LRESULT SendMessageA(HWND w,UINT m,WPARAM p,LPARAM l) override {return ::SendMessageA(w,m,p,l);}
    BOOL EnableWindow(HWND w,BOOL b) override {return ::EnableWindow(w,b);}
    LONG GetWindowLongA(HWND w,int n) override {return ::GetWindowLongA(w,n);}
    LONG SetWindowLongA(HWND w,int n,LONG l) override {return ::SetWindowLongA(w,n,l);}
    LRESULT CallWindowProcA(WNDPROC f,HWND w,UINT m,WPARAM p,LPARAM l) override {return ::CallWindowProcA(f,w,m,p,l);}
    int GetWindowTextLengthA(HWND w) override {return ::GetWindowTextLengthA(w);}
    int GetWindowTextA(HWND w,char* p,int n) override {return ::GetWindowTextA(w,p,n);}
    BOOL SetWindowTextA(HWND w,const char* p) override {return ::SetWindowTextA(w,p);}
    DWORD GetMessagePos() override {return ::GetMessagePos();}
    int MapWindowPoints(HWND a,HWND b,POINT* p,UINT n) override {return ::MapWindowPoints(a,b,p,n);}
    SHORT GetKeyState(int n) override {return ::GetKeyState(n);}
    HWND GetFocus() override {return ::GetFocus();}
    BOOL IsWindowEnabled(HWND w) override {return ::IsWindowEnabled(w);}
    DWORD GetSysColor(int n) override {return ::GetSysColor(n);}
    BOOL GetWindowRect(HWND w,RECT* p) override {return ::GetWindowRect(w,p);}
    int GetSystemMetrics(int n) override {return ::GetSystemMetrics(n);}
    HIMAGELIST ImageList_LoadImageA(HINSTANCE h,LPCSTR p,int x,int g,COLORREF c,UINT t,UINT f) override
    {return ::ImageList_LoadImageA(h,p,x,g,c,t,f);}
};
}
IPropertyPageSoundNativeApi& DefaultPropertyPageSoundNativeApi() noexcept {static Api api;return api;}
PropertyPageSoundNativeScope::PropertyPageSoundNativeScope(PropertyPageSoundNativeServices& s) noexcept
    :previous_(current) {current=&s;}
PropertyPageSoundNativeScope::~PropertyPageSoundNativeScope() noexcept {current=previous_;}
PropertyPageSoundNativeServices& RequirePropertyPageSoundNativeServices()
{
    if(!current)throw std::logic_error("CPageSound requires explicit Root/Env/Sound/Win32 services");
    return *current;
}
RootPropertyPageSoundNativeSource::RootPropertyPageSoundNativeSource(RootRuntimeOwner& r,EnvironmentRootOwner& e,
    SoundManagerNativeRuntime& s,ISoundManagerNativeOpen& o) noexcept :root_(r),environment_(e),sounds_(s),open_(o) {}
EnvironmentRootStorage* RootPropertyPageSoundNativeSource::EnvironmentAt4180C0() {return environment_.CaptureAt4180C0();}
EnvironmentRootStorage* RootPropertyPageSoundNativeSource::EnvironmentInlineAt6198C8() {return environment_.CaptureAt4180C0();}
void RootPropertyPageSoundNativeSource::EnsureRootAt413BE0() {root_.EnsureCreatedAt413BE0();}
void* RootPropertyPageSoundNativeSource::RootAt6198C4() {return root_.ReadPublishedAt6198C4();}
YtlReferenceSlotRuntime& RootPropertyPageSoundNativeSource::SoundAt24(void* p)
{
    if (!p) return *reinterpret_cast<YtlReferenceSlotRuntime*>(0x24u);
    return static_cast<RootRuntimePayload*>(p)->SoundManagerSlotAt24();
}
void RootPropertyPageSoundNativeSource::StopAt4383B0(SoundManagerNativeBody50h* p,DWORD slot)
{static_cast<void>(sounds_.StopAbsoluteCapturedAt4383B0(p,slot));}
void RootPropertyPageSoundNativeSource::ClearAt4384D0(SoundManagerNativeBody50h* p)
{static_cast<void>(sounds_.ClearVoicesCapturedAt4384D0(p));}
bool RootPropertyPageSoundNativeSource::PlayingAt419370(SoundManagerNativeBody50h* p,DWORD slot)
{return sounds_.Wait(p).IsPlayingAt419370(slot);}
void RootPropertyPageSoundNativeSource::SetVolumeAt438170(int handle,int maximum,int value)
{sounds_.Library().Calls().SetVolume(handle,maximum,value);}
void RootPropertyPageSoundNativeSource::OpenAt42BC90(SoundManagerNativeBody50h* p,DWORD slot,const char* name,int value,int loop,BYTE noAuto)
{static_cast<void>(open_.OpenAt42BC90(*p,slot,name,value,loop,noAuto));}
}

namespace KaGuYa::Start::Runtime::PropertySoundDetail
{
void* Resolve(const YtlReferenceSlotRuntime& ref,bool biasFirst,bool reloadBase) noexcept
{
    const auto base=Read(ref.baseAt00);if(!base)return nullptr;
    auto* control=Read(ref.controlAt08);DWORD stride=0,bias;
    if(biasFirst) {bias=Read(ref.biasAt04);if(control)stride=At<DWORD>(control,0xC);}
    else {if(control)stride=At<DWORD>(control,0xC);bias=Read(ref.biasAt04);}
    return reinterpret_cast<void*>((reloadBase?Read(ref.baseAt00):base)+bias*stride);
}
namespace
{
void* Archive(EnvironmentRootStorage* root,DWORD index,bool getter,bool beginFirst,bool biasFirst=false,bool reloadBegin=false)
{
    auto& slots=root->archivesAt30;YtlReferenceSlotRuntime* begin;YtlReferenceSlotRuntime* end;
    if(getter||beginFirst) {begin=Read(slots.begin);end=Read(slots.end);}
    else {end=Read(slots.end);begin=Read(slots.begin);}
    if(index>=Count12(end,begin))return nullptr;
    if(reloadBegin)begin=Read(slots.begin);
    return Resolve(begin[index],getter||biasFirst);
}
}
SaveDataEnvironmentArchive* Save(Services& s,bool getter,bool inlineCapture,bool beginFirst,bool biasFirst,bool reloadBegin)
{
    auto* root=inlineCapture?s.source.EnvironmentInlineAt6198C8():s.source.EnvironmentAt4180C0();
    return static_cast<SaveDataEnvironmentArchive*>(Archive(root,1,getter,beginFirst,biasFirst,reloadBegin));
}
EnvironmentSoundParamsArchive* Params(Services& s,bool getter,bool inlineCapture,bool beginFirst)
{auto* save=Save(s,getter,inlineCapture,beginFirst);return static_cast<EnvironmentSoundParamsArchive*>(Resolve(save->soundParamsAt1C));}
EnvironmentSoundParamsArchive* RegistrationParams(Services& s,bool biasFirst,bool reloadBegin)
{auto* save=Save(s,false,false,false,biasFirst,reloadBegin);return static_cast<EnvironmentSoundParamsArchive*>(Resolve(save->soundParamsAt1C,false,true));}
UnSaveDataRuntime* UnSave(Services& s)
{return static_cast<UnSaveDataRuntime*>(Archive(s.source.EnvironmentAt4180C0(),2,true,true));}
void* Root(Services& s) {s.source.EnsureRootAt413BE0();return s.source.RootAt6198C4();}
SoundManagerNativeBody50h* Sound(void* root,Services& s)
{return static_cast<SoundManagerNativeBody50h*>(Resolve(s.source.SoundAt24(root)));}
SoundManagerNativeBody50h* Sound(Services& s) {return Sound(Root(s),s);}
SoundManagerNativeBody50h* SoundGetter(void* root,Services& s)
{return static_cast<SoundManagerNativeBody50h*>(Resolve(s.source.SoundAt24(root),false,true));}
SoundManagerNativeBody50h* SoundGetter(Services& s) {return SoundGetter(Root(s),s);}
}
