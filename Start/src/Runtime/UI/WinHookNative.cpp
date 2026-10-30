#include <Runtime/WinHookNative.h>
#include <exception>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
thread_local WinHookNativeServices* active=nullptr;
WinHookNativeServices& Services(){if(!active)throw std::logic_error("WinHook native scope is absent");return *active;}
class Win32 final:public IWinHookNativePlatform
{
public:
    void Initialize(CRITICAL_SECTION* p) override{::InitializeCriticalSection(p);}
    void Delete(CRITICAL_SECTION* p) override{::DeleteCriticalSection(p);}
    void Enter(CRITICAL_SECTION* p) override{::EnterCriticalSection(p);}
    void Leave(CRITICAL_SECTION* p) override{::LeaveCriticalSection(p);}
    DWORD CurrentThread() override{return ::GetCurrentThreadId();}
};
void __fastcall Enter(CriticalSectionNativeBody24h* p,void*)
{Services().platform.Enter(&p->sectionAt04);const auto thread=Services().platform.CurrentThread();++p->depthAt20;p->threadAt1C=thread;}
void __fastcall Leave(CriticalSectionNativeBody24h* p,void*)
{if(--p->depthAt20==0)p->threadAt1C=0xFFFFFFFFu;Services().platform.Leave(&p->sectionAt04);}
std::uint8_t __fastcall Held(CriticalSectionNativeBody24h* p,void*){return p->threadAt1C==Services().platform.CurrentThread();}
void __fastcall Pure(CriticalSectionNativeBody24h*,void*){static_cast<void>(_purecall());}
std::uint8_t __fastcall PureHeld(CriticalSectionNativeBody24h*,void*){return static_cast<std::uint8_t>(_purecall());}
CriticalSectionNativeBody24h* __fastcall DeleteBaseCritical(CriticalSectionNativeBody24h* p,void*,std::uint32_t flags)
{p->vptrAt00=&WinHookRetiredCriticalSectionNativeTable;if(flags&1)Services().memory.DeleteBody(p,4);return p;}
CriticalSectionNativeBody24h* __fastcall DeleteCritical(CriticalSectionNativeBody24h* p,void*,std::uint32_t flags)
{Services().platform.Delete(&p->sectionAt04);p->vptrAt00=&WinHookRetiredCriticalSectionNativeTable;if(flags&1)Services().memory.DeleteBody(p,0x24);return p;}
class Locked final
{
public:
    explicit Locked(CriticalSectionNativeBody24h& p):body_(p){body_.vptrAt00->enterAt00(&body_);active_=true;}
    ~Locked() noexcept{if(active_)Finish();}
    void Finish() noexcept{active_=false;try{body_.vptrAt00->leaveAt04(&body_);}catch(...){std::terminate();}}
private:
    CriticalSectionNativeBody24h& body_;bool active_=false;
};
WinHookNodeNativeBody0Ch* NewNode(WinHookNodeNativeBody0Ch* next,WinHookNodeNativeBody0Ch* previous,IEnvironmentRootMemory& memory)
{
    auto* node=static_cast<WinHookNodeNativeBody0Ch*>(memory.AllocateBody(0x0C));
    node->nextAt00=next?next:node;node->previousAt04=next?previous:node;return node;
}
void __fastcall Add(WinHookNativeBody30h* p,void*,void* item){AddWinHookNativeAt43F510(*p,item,Services());}
void __fastcall Remove(WinHookNativeBody30h* p,void*,void* item){RemoveWinHookNativeAt43F640(*p,item,Services());}
void __fastcall Clear(WinHookNativeBody30h* p,void*){ClearWinHookNativeAt43F5C0(*p,Services());}
std::int32_t __fastcall Dispatch(WinHookNativeBody30h* p,void*,HWND w,UINT m,WPARAM a,LPARAM l,LRESULT* r,std::uint32_t id)
{return DispatchWinHookNativeAt43F6E0(*p,w,m,a,l,r,id,Services());}
// 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
// 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const WinHookNativeVtable methods{reinterpret_cast<decltype(WinHookNativeVtable::addAt00)>(&Add),
    reinterpret_cast<decltype(WinHookNativeVtable::removeAt04)>(&Remove),
    reinterpret_cast<decltype(WinHookNativeVtable::clearAt08)>(&Clear),
    reinterpret_cast<decltype(WinHookNativeVtable::dispatchAt0C)>(&Dispatch)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
}
// 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
// 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const CriticalSectionNativeVtable WinHookRetiredCriticalSectionNativeTable{
    reinterpret_cast<decltype(CriticalSectionNativeVtable::enterAt00)>(&Pure),
    reinterpret_cast<decltype(CriticalSectionNativeVtable::leaveAt04)>(&Pure),
    reinterpret_cast<decltype(CriticalSectionNativeVtable::heldAt08)>(&PureHeld),
    reinterpret_cast<decltype(CriticalSectionNativeVtable::deleteAt0C)>(&DeleteBaseCritical)};
const CriticalSectionNativeVtable WinHookCriticalSectionNativeTable{
    reinterpret_cast<decltype(CriticalSectionNativeVtable::enterAt00)>(&Enter),
    reinterpret_cast<decltype(CriticalSectionNativeVtable::leaveAt04)>(&Leave),
    reinterpret_cast<decltype(CriticalSectionNativeVtable::heldAt08)>(&Held),
    reinterpret_cast<decltype(CriticalSectionNativeVtable::deleteAt0C)>(&DeleteCritical)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
IWinHookNativePlatform& DefaultWinHookNativePlatform() noexcept{static Win32 value;return value;}
WinHookNativeScope::WinHookNativeScope(WinHookNativeServices& services) noexcept:previous_(active){active=&services;}
WinHookNativeScope::~WinHookNativeScope() noexcept{active=previous_;}
const WinHookNativeVtable& WinHookNativeMethods() noexcept{return methods;}
const CriticalSectionNativeVtable& WinHookCriticalSectionNativeMethods() noexcept{return WinHookCriticalSectionNativeTable;}
WinHookNativeBody30h* ConstructWinHookNativeAt450E8F(WinHookNativeBody30h& body,WinHookNativeServices& services)
{
    WinHookNativeScope scope(services);body.vptrAt00=&methods;body.sentinelAt04=nullptr;body.countAt08=0;
    body.sentinelAt04=NewNode(nullptr,nullptr,services.memory);
    body.lockAt0C.vptrAt00=&WinHookCriticalSectionNativeTable;services.platform.Initialize(&body.lockAt0C.sectionAt04);
    body.lockAt0C.threadAt1C=0xFFFFFFFFu;body.lockAt0C.depthAt20=0;return &body;
}
WinHookNativeBody30h* NewWinHookNativeAt450E78(WinHookNativeServices& services)
{
    auto* raw=services.memory.AllocateBody(0x30);if(!raw)return nullptr;
    try{return ConstructWinHookNativeAt450E8F(*::new(raw) WinHookNativeBody30h,services);}
    catch(...){[&]() noexcept{services.memory.DeleteBody(raw,0x30);}();throw;}
}
void DestroyWinHookNativeAt5AACF1(WinHookNativeBody30h& body,WinHookNativeServices& services)
{
    WinHookNativeScope scope(services);
    body.lockAt0C.vptrAt00=&WinHookCriticalSectionNativeTable;services.platform.Delete(&body.lockAt0C.sectionAt04);
    body.lockAt0C.vptrAt00=&WinHookRetiredCriticalSectionNativeTable;
    auto* cursor=body.sentinelAt04->nextAt00;
    body.sentinelAt04->nextAt00=body.sentinelAt04;body.sentinelAt04->previousAt04=body.sentinelAt04;body.countAt08=0;
    while(cursor!=body.sentinelAt04){auto* next=cursor->nextAt00;services.memory.DeleteBody(cursor,0x0C);cursor=next;}
    services.memory.DeleteBody(body.sentinelAt04,0x0C);
}
void AddWinHookNativeAt43F510(WinHookNativeBody30h& body,void* item,WinHookNativeServices& services)
{
    WinHookNativeScope scope(services);Locked lock(body.lockAt0C);
    auto* const sentinel=body.sentinelAt04;auto* const previous=sentinel->previousAt04;
    auto* node=NewNode(sentinel,previous,services.memory);node->itemAt08=item;
    // 43F573先得到节点，再以DWORD回绕检查原max_size差值；失败不补删未链接节点。
    if(0x15555554u-body.countAt08<1u)throw std::length_error("list<T> too long");
    ++body.countAt08;sentinel->previousAt04=node;previous->nextAt00=node;lock.Finish();
}
void RemoveWinHookNativeAt43F640(WinHookNativeBody30h& body,void* item,WinHookNativeServices& services)
{
    WinHookNativeScope scope(services);body.lockAt0C.vptrAt00->enterAt00(&body.lockAt0C);
    auto* cursor=body.sentinelAt04->nextAt00;
    while(cursor!=body.sentinelAt04)
    {
        if(cursor->itemAt08==item)
        {
            auto* next=cursor->nextAt00;cursor->previousAt04->nextAt00=next;next->previousAt04=cursor->previousAt04;
            --body.countAt08;services.memory.DeleteBody(cursor,0x0C);cursor=next;
        }
        else cursor=cursor->nextAt00;
    }
    // 43F640无CCriticalLock展开action；正常Leave为terminate边界。
    [&]() noexcept{try{body.lockAt0C.vptrAt00->leaveAt04(&body.lockAt0C);}catch(...){std::terminate();}}();
}
void ClearWinHookNativeAt43F5C0(WinHookNativeBody30h& body,WinHookNativeServices& services)
{
    WinHookNativeScope scope(services);body.lockAt0C.vptrAt00->enterAt00(&body.lockAt0C);
    auto* cursor=body.sentinelAt04->nextAt00;
    body.sentinelAt04->nextAt00=body.sentinelAt04;body.sentinelAt04->previousAt04=body.sentinelAt04;body.countAt08=0;
    while(cursor!=body.sentinelAt04){auto* next=cursor->nextAt00;services.memory.DeleteBody(cursor,0x0C);cursor=next;}
    [&]() noexcept{try{body.lockAt0C.vptrAt00->leaveAt04(&body.lockAt0C);}catch(...){std::terminate();}}();
}
std::int32_t DispatchWinHookNativeAt43F6E0(WinHookNativeBody30h& body,HWND window,UINT message,WPARAM w,LPARAM l,
    LRESULT* result,std::uint32_t id,WinHookNativeServices& services)
{
    WinHookNativeScope scope(services);Locked lock(body.lockAt0C);std::int32_t handled=0;
    auto* cursor=body.sentinelAt04->nextAt00;
    while(cursor!=body.sentinelAt04)
    {
        auto* item=cursor->itemAt08;using Function=std::int32_t(__thiscall*)(void*,HWND,UINT,WPARAM,LPARAM,LRESULT*,std::uint32_t);
        auto* table=*static_cast<const Function* const*>(item);
        handled=table[0](item,window,message,w,l,result,id);
        if(handled)break;
        cursor=cursor->nextAt00;
    }
    lock.Finish();return handled;
}
WinHookRootNativeOwner::WinHookRootNativeOwner(IEnvironmentRootLock& lock,WinHookNativeServices& services) noexcept:lock_(lock),services_(services){}
WinHookRootNativeOwner::~WinHookRootNativeOwner() noexcept{ReleaseAt5AACB0();}
std::int32_t NativeWindowHookMessageSource::DispatchNative(HWND window,UINT message,WPARAM w,LPARAM l,
    LRESULT* result,std::uint32_t id)
{
    // 42EE81在getter解锁后从发布槽读取正文，再按+0C虚槽提交六参数。
    auto* body=owner_.CaptureAt450E20();WinHookNativeScope scope(services_);
    return body->vptrAt00->dispatchAt0C(body,window,message,w,l,result,id);
}
WinHookNativeBody30h* WinHookRootNativeOwner::CaptureAt450E20()
{
    lock_.Enter();struct Unwind{IEnvironmentRootLock& lock;bool active=true;~Unwind() noexcept{if(active)lock.Leave();}} guard{lock_};
    if(!published_)published_=NewWinHookNativeAt450E78(services_);
    guard.active=false;[&]() noexcept{try{lock_.Leave();}catch(...){std::terminate();}}();return published_;
}
void WinHookRootNativeOwner::ReleaseAt5AACB0() noexcept
{
    try
    {
        lock_.Enter();if(auto* body=published_){DestroyWinHookNativeAt5AACF1(*body,services_);services_.memory.DeleteBody(body,0x30);published_=nullptr;}
        lock_.Leave();
    }
    catch(...){std::terminate();}
}
}
