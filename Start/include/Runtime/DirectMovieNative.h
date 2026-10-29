#pragma once

#include <Runtime/DirectMovieRuntime.h>
#include <Runtime/WinHookNative.h>
#include <cstddef>
#include <optional>
#include <unknwn.h>

namespace KaGuYa::Start::Runtime
{
struct DirectMovieNativeBody38h;
struct DirectMovieNativeVtable11 final
{
    std::int32_t (__thiscall* messageAt00)(DirectMovieNativeBody38h*,HWND,UINT,WPARAM,LPARAM,LRESULT*,std::uint32_t);
    HRESULT (__thiscall* acquireAt04)(DirectMovieNativeBody38h*);
    void (__thiscall* releaseAt08)(DirectMovieNativeBody38h*);
    void (__thiscall* loadAt0C)(DirectMovieNativeBody38h*,const char*,std::uint8_t);
    void (__thiscall* runAt10)(DirectMovieNativeBody38h*);
    void (__thiscall* pauseAt14)(DirectMovieNativeBody38h*);
    void (__thiscall* rewindAt18)(DirectMovieNativeBody38h*);
    void (__thiscall* finishAt1C)(DirectMovieNativeBody38h*);
    void (__thiscall* pulseAt20)(DirectMovieNativeBody38h*);
    void (__thiscall* resizeAt24)(DirectMovieNativeBody38h*);
    DirectMovieNativeBody38h* (__thiscall* deleteAt28)(DirectMovieNativeBody38h*,std::uint32_t);
};

// 4A0BFD..4A0C54：接口由正文持有；两个 padding 区域不参与构造写入。
struct DirectMovieNativeBody38h final
{
    const DirectMovieNativeVtable11* vptrAt00;
    void* graphAt04;
    void* controlAt08;
    void* seekingAt0C;
    void* positionAt10;
    void* audioAt14;
    void* videoAt18;
    void* eventAt1C;
    void* windowAt20;
    std::uint32_t neutralAt24;
    std::uint8_t loopAt28;
    std::uint8_t unwrittenAt29[3];
    std::uint32_t stateAt2C;
    long volumeAt30;
    std::uint8_t mutedAt34;
    std::uint8_t unwrittenAt35[3];
};
static_assert(sizeof(void*)==4&&sizeof(DirectMovieNativeBody38h)==0x38);
static_assert(sizeof(DirectMovieNativeVtable11)==0x2C);
static_assert(offsetof(DirectMovieNativeBody38h,graphAt04)==4);
static_assert(offsetof(DirectMovieNativeBody38h,eventAt1C)==0x1C);
static_assert(offsetof(DirectMovieNativeBody38h,loopAt28)==0x28);
static_assert(offsetof(DirectMovieNativeBody38h,stateAt2C)==0x2C);
static_assert(offsetof(DirectMovieNativeBody38h,mutedAt34)==0x34);

class IDirectMovieNativeSource
{
public:
    virtual ~IDirectMovieNativeSource()=default;
    virtual HRESULT CreateGraphAt4AAD7A(REFCLSID,IUnknown*,DWORD,REFIID,void**)=0;
    virtual int ConvertAcpAt4BC74C(const char*,wchar_t*,int)=0;
    virtual HWND CaptureWindowAt4180C0(std::uint32_t site)=0;
    virtual DirectMoviePosition PositionAt4AFFF0()=0;
    virtual void ErrorAt420C50(const char* cp932,std::uint32_t captureSite)=0;
    virtual WinHookNativeBody30h* CaptureHooksAt450E20(std::uint32_t site)=0;
};
struct DirectMovieNativeServices final
{
    IDirectMovieNativeSource& source;
    WinHookNativeServices& hooks;
};
class DirectMovieNativeScope final
{
public:
    explicit DirectMovieNativeScope(DirectMovieNativeServices&)noexcept;
    explicit DirectMovieNativeScope(DirectMovieNativeServices*)noexcept;
    ~DirectMovieNativeScope()noexcept;
    DirectMovieNativeScope(const DirectMovieNativeScope&)=delete;
    DirectMovieNativeScope& operator=(const DirectMovieNativeScope&)=delete;
private:
    DirectMovieNativeServices* previous_;
    std::optional<WinHookNativeScope> hooks_;
};

// 系统叶不另持 COM 接口或播放状态；host/registry 的寿命须覆盖正文及最后 Ref。
class DirectMovieNativeSource final:public IDirectMovieNativeSource
{
public:
    DirectMovieNativeSource(IDirectMovieHost& host,WinHookRootNativeOwner& hooks)noexcept:host_(host),hooks_(hooks){}
    HRESULT CreateGraphAt4AAD7A(REFCLSID,IUnknown*,DWORD,REFIID,void**)override;
    int ConvertAcpAt4BC74C(const char*,wchar_t*,int)override;
    HWND CaptureWindowAt4180C0(std::uint32_t)override;
    DirectMoviePosition PositionAt4AFFF0()override;
    void ErrorAt420C50(const char*,std::uint32_t)override;
    WinHookNativeBody30h* CaptureHooksAt450E20(std::uint32_t)override;
private:
    IDirectMovieHost& host_;
    WinHookRootNativeOwner& hooks_;
};

DirectMovieNativeServices& RequireDirectMovieNativeServices();
const DirectMovieNativeVtable11& DirectMovieNativeMethods()noexcept;
DirectMovieNativeBody38h* ConstructDirectMovieNativeAt4A0BFD(DirectMovieNativeBody38h&,DirectMovieNativeServices&);
void RetireDirectMovieNativeAt4A4D70(DirectMovieNativeBody38h&)noexcept;
void DestroyDirectMovieNativeAt4A3EA0(DirectMovieNativeBody38h&,DirectMovieNativeServices&)noexcept;
DirectMovieNativeBody38h* DeleteDirectMovieNativeAt4A62A0(DirectMovieNativeBody38h*,std::uint32_t)noexcept;
HRESULT AcquireDirectMovieNativeAt4AAD60(DirectMovieNativeBody38h&,DirectMovieNativeServices&);
void ReleaseDirectMovieNativeAt4C08C0(DirectMovieNativeBody38h&);
void LoadDirectMovieNativeAt4BC720(DirectMovieNativeBody38h&,const char*,std::uint8_t,DirectMovieNativeServices&);
void RunDirectMovieNativeAt4BCC50(DirectMovieNativeBody38h&);
void PauseDirectMovieNativeAt4BC910(DirectMovieNativeBody38h&);
void RewindDirectMovieNativeAt4C34A0(DirectMovieNativeBody38h&);
void FinishDirectMovieNativeAt4AC940(DirectMovieNativeBody38h&);
void PulseDirectMovieNativeAt4B5010(DirectMovieNativeBody38h&);
void ResizeDirectMovieNativeAt4BC220(DirectMovieNativeBody38h&,DirectMovieNativeServices&);
std::int32_t MessageDirectMovieNativeAt4BF3C0(DirectMovieNativeBody38h&,HWND,UINT,WPARAM,LPARAM,LRESULT*,std::uint32_t);
}
