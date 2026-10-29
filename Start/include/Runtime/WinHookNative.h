#pragma once
#include <Runtime/EnvironmentRootRuntime.h>
#include <Platform/WindowHookNativeProtocol.h>

namespace KaGuYa::Start::Runtime
{
struct CriticalSectionNativeBody24h;
struct CriticalSectionNativeVtable final
{
    void(__thiscall* enterAt00)(CriticalSectionNativeBody24h*);
    void(__thiscall* leaveAt04)(CriticalSectionNativeBody24h*);
    std::uint8_t(__thiscall* heldAt08)(CriticalSectionNativeBody24h*);
    CriticalSectionNativeBody24h*(__thiscall* deleteAt0C)(CriticalSectionNativeBody24h*,std::uint32_t);
};
struct CriticalSectionNativeBody24h final
{
    const CriticalSectionNativeVtable* vptrAt00;
    CRITICAL_SECTION sectionAt04;
    std::uint32_t threadAt1C;
    std::uint32_t depthAt20;
};
struct WinHookNativeBody30h;
struct WinHookNativeVtable final
{
    void(__thiscall* addAt00)(WinHookNativeBody30h*,void*);
    void(__thiscall* removeAt04)(WinHookNativeBody30h*,void*);
    void(__thiscall* clearAt08)(WinHookNativeBody30h*);
    std::int32_t(__thiscall* dispatchAt0C)(WinHookNativeBody30h*,HWND,UINT,WPARAM,LPARAM,LRESULT*,std::uint32_t);
};
struct WinHookNodeNativeBody0Ch final
{
    WinHookNodeNativeBody0Ch* nextAt00;
    WinHookNodeNativeBody0Ch* previousAt04;
    void* itemAt08;
};
struct WinHookNativeBody30h final
{
    const WinHookNativeVtable* vptrAt00;
    WinHookNodeNativeBody0Ch* sentinelAt04;
    std::uint32_t countAt08;
    CriticalSectionNativeBody24h lockAt0C;
};
static_assert(sizeof(CriticalSectionNativeBody24h)==0x24&&offsetof(CriticalSectionNativeBody24h,threadAt1C)==0x1C);
static_assert(sizeof(WinHookNativeBody30h)==0x30&&offsetof(WinHookNativeBody30h,lockAt0C)==0x0C);
static_assert(sizeof(WinHookNodeNativeBody0Ch)==0x0C&&sizeof(WinHookNativeVtable)==0x10);

class IWinHookNativePlatform
{
public:
    virtual ~IWinHookNativePlatform()=default;
    virtual void Initialize(CRITICAL_SECTION*)=0;
    virtual void Delete(CRITICAL_SECTION*)=0;
    virtual void Enter(CRITICAL_SECTION*)=0;
    virtual void Leave(CRITICAL_SECTION*)=0;
    virtual DWORD CurrentThread()=0;
};
IWinHookNativePlatform& DefaultWinHookNativePlatform() noexcept;
struct WinHookNativeServices final
{
    IEnvironmentRootMemory& memory;
    IWinHookNativePlatform& platform;
};
class WinHookNativeScope final
{
public:
    explicit WinHookNativeScope(WinHookNativeServices&) noexcept;
    ~WinHookNativeScope() noexcept;
    WinHookNativeScope(const WinHookNativeScope&)=delete;
    WinHookNativeScope& operator=(const WinHookNativeScope&)=delete;
private:
    WinHookNativeServices* previous_;
};
const WinHookNativeVtable& WinHookNativeMethods() noexcept;
// 原5AD814/5AD800共享表，供其他原24h全局锁作静态数据初始化；不是新建一套锁协议。
extern const CriticalSectionNativeVtable WinHookCriticalSectionNativeTable;
extern const CriticalSectionNativeVtable WinHookRetiredCriticalSectionNativeTable;
const CriticalSectionNativeVtable& WinHookCriticalSectionNativeMethods() noexcept;
WinHookNativeBody30h* ConstructWinHookNativeAt450E8F(WinHookNativeBody30h&,WinHookNativeServices&);
WinHookNativeBody30h* NewWinHookNativeAt450E78(WinHookNativeServices&);
void DestroyWinHookNativeAt5AACF1(WinHookNativeBody30h&,WinHookNativeServices&);
void AddWinHookNativeAt43F510(WinHookNativeBody30h&,void*,WinHookNativeServices&);
void RemoveWinHookNativeAt43F640(WinHookNativeBody30h&,void*,WinHookNativeServices&);
void ClearWinHookNativeAt43F5C0(WinHookNativeBody30h&,WinHookNativeServices&);
std::int32_t DispatchWinHookNativeAt43F6E0(WinHookNativeBody30h&,HWND,UINT,WPARAM,LPARAM,LRESULT*,
    std::uint32_t,WinHookNativeServices&);
class WinHookRootNativeOwner final
{
public:
    WinHookRootNativeOwner(IEnvironmentRootLock&,WinHookNativeServices&) noexcept;
    ~WinHookRootNativeOwner() noexcept;
    WinHookNativeBody30h* CaptureAt450E20();
    void ReleaseAt5AACB0() noexcept;
    WinHookNativeBody30h* Published() const noexcept{return published_;}
    WinHookNativeBody30h*& PublicationSlot() noexcept{return published_;}
private:
    IEnvironmentRootLock& lock_;
    WinHookNativeServices& services_;
    WinHookNativeBody30h* published_=nullptr;
};
class NativeWindowHookMessageSource final:public Platform::IWindowHookMessageSource
{
public:
    NativeWindowHookMessageSource(WinHookRootNativeOwner& owner,WinHookNativeServices& services) noexcept
        :owner_(owner),services_(services){}
    std::int32_t DispatchNative(HWND,UINT,WPARAM,LPARAM,LRESULT*,std::uint32_t) override;
private:
    WinHookRootNativeOwner& owner_;
    WinHookNativeServices& services_;
};
}
