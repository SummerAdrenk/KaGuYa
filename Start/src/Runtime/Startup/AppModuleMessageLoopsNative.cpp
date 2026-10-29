#include <Runtime/AppModuleMessageLoopsNative.h>
#include <bit>

namespace KaGuYa::Start::Runtime
{
namespace
{
template<class T> T Read(const T& value) noexcept
{
    return *static_cast<const volatile T*>(&value);
}
std::int32_t Signed(std::uint32_t value) noexcept
{
    return std::bit_cast<std::int32_t>(value);
}
CRITICAL_SECTION* CaptureLock(AppModuleNativeBody3Ch*& publication) noexcept
{
    return reinterpret_cast<CRITICAL_SECTION*>(reinterpret_cast<std::uintptr_t>(Read(publication))+0x10u);
}
template<class T> T* Slot(T* base, std::uint32_t index) noexcept
{
    // 用DWORD地址运算保留原乘4/加法回绕及零地址分支，不对空指针做C++数组运算。
    return reinterpret_cast<T*>(reinterpret_cast<std::uintptr_t>(base)+index*4u);
}
class RemovalUnwind final
{
public:
    IAppModuleNativePlatformApi& platform;
    CRITICAL_SECTION* lock;
    bool active = true;
    ~RemovalUnwind() noexcept
    {
        // 593720 -> 40ED50：只解捕获的锁，不撤销已搬移/发布的数组。
        if(active) platform.LeaveCriticalSection(lock);
    }
};
}

AppModuleMessageLoopsNativeServices& DefaultAppModuleMessageLoopsNativeServices() noexcept
{
    static AppModuleMessageLoopsNativeServices services{
        {DefaultEnvironmentRootMemory(),DefaultAppModuleNativePlatformApi()},
        DefaultWindowClassNativeApi(),DefaultAtlBaseModuleNativeDiagnostics()};
    return services;
}

void RegisterAppModuleMessageLoopNativeAt434310(AppModuleNativeBody3Ch& fixedGlobal,
    AppModuleNativeBody3Ch*& publication, void* loop, AppModuleMessageLoopsNativeServices& services)
{
    auto& platform = services.module.platform;
    auto* lock = CaptureLock(publication);
    platform.EnterCriticalSection(lock);
    const auto thread = platform.GetCurrentThreadId();
    auto* table = Read(fixedGlobal.loopsAt34);
    const auto firstCount = Read(table->countAt08)+1u;
    auto* threads = static_cast<DWORD*>(services.allocation.Recalloc(Read(table->threadsAt00),firstCount,4));
    if(threads)
    {
        table->threadsAt00 = threads;
        // 434333先发布；434335重读count，不能缓存首次Count或延迟两数组一起提交。
        const auto secondCount = Read(table->countAt08)+1u;
        auto* loops = static_cast<void**>(services.allocation.Recalloc(Read(table->loopsAt04),secondCount,4));
        if(loops)
        {
            const auto index = Read(table->countAt08);
            table->loopsAt04 = loops;
            auto* threadSlot = Slot(Read(table->threadsAt00),index);
            if(threadSlot)
            {
                *threadSlot = thread;
                loops = Read(table->loopsAt04);
            }
            if(auto* loopSlot = Slot(loops,index)) *loopSlot = loop;
            table->countAt08 = Read(table->countAt08)+1u;
        }
    }
    // 4342A0 state 0是CIdleLoop清理，不是锁guard；异常不能补Leave或删除loop。
    platform.LeaveCriticalSection(lock);
}

void* FindAppModuleMessageLoopNativeAt417F50(AppModuleNativeBody3Ch& receiver,
    AppModuleNativeBody3Ch*& publication, DWORD thread, AppModuleMessageLoopsNativeServices& services)
{
    auto& platform = services.module.platform;
    auto* lock = CaptureLock(publication);
    platform.EnterCriticalSection(lock);
    auto* table = Read(receiver.loopsAt34);
    const auto count = Read(table->countAt08);
    void* result = nullptr;
    if(Signed(count)>0)
    {
        auto* cursor = Read(table->threadsAt00);
        std::uint32_t index = 0;
        while(Signed(index)<Signed(count))
        {
            if(Read(*cursor)==thread)
            {
                if(index==0xFFFFFFFFu) break;
                if(Signed(index)<0 || Signed(index)>=Signed(count))
                {
                    platform.RaiseException(EXCEPTION_ARRAY_BOUNDS_EXCEEDED,1);
                    __assume(0); // 原noncontinuable异常返回后的流不属于有效执行路径。
                }
                result = Read(*Slot(Read(table->loopsAt04),index));
                break;
            }
            ++index;
            cursor = Slot(cursor,1);
        }
    }
    // 原417F50没有EH guard；result在Leave前读取，不能在Leave回调后再查。
    platform.LeaveCriticalSection(lock);
    return result;
}

BOOL RemoveAppModuleMessageLoopNativeAt432800(AppModuleNativeBody3Ch& receiver,
    AppModuleNativeBody3Ch*& publication, AppModuleMessageLoopsNativeServices& services)
{
    auto& platform = services.module.platform;
    auto* lock = CaptureLock(publication);
    platform.EnterCriticalSection(lock);
    // 432844才写已持锁标志；不假定43283E内部抛出时原未初始化栈byte的值。
    RemovalUnwind unwind{platform,lock};
    const auto thread = platform.GetCurrentThreadId();
    auto* table = Read(receiver.loopsAt34);
    auto count = Read(table->countAt08);
    BOOL result = FALSE;
    if(Signed(count)>0)
    {
        auto* cursor = Read(table->threadsAt00);
        std::uint32_t index = 0;
        while(Signed(index)<Signed(count))
        {
            if(Read(*cursor)==thread)
            {
                if(index==0xFFFFFFFFu || Signed(index)<0 || Signed(index)>=Signed(count)) break;
                if(index!=count-1u)
                {
                    const auto threadBytes = (count-index)*4u;
                    auto* threadSlot = Slot(Read(table->threadsAt00),index);
                    services.diagnostics.MoveBytesAt43D2D0(threadSlot,threadBytes,Slot(threadSlot,1),threadBytes-4u);
                    // 4328B6/4328B9：第二次搬移使用回调后的count/base，record本身仍固定。
                    count = Read(table->countAt08);
                    const auto loopBytes = (count-index)*4u;
                    auto* loopSlot = Slot(Read(table->loopsAt04),index);
                    services.diagnostics.MoveBytesAt43D2D0(loopSlot,loopBytes,Slot(loopSlot,1),loopBytes-4u);
                    count = Read(table->countAt08);
                }
                auto* threads = static_cast<DWORD*>(services.allocation.Recalloc(Read(table->threadsAt00),count-1u,4));
                count = Read(table->countAt08);
                if(threads || count==1u) table->threadsAt00 = threads;
                auto* loops = static_cast<void**>(services.allocation.Recalloc(Read(table->loopsAt04),count-1u,4));
                count = Read(table->countAt08);
                if(loops || count==1u) table->loopsAt04 = loops;
                table->countAt08 = count-1u;
                result = TRUE;
                break;
            }
            ++index;
            cursor = Slot(cursor,1);
        }
    }
    // 正常Leave抛出时原flag仍为1，须有一次unwind Leave；不能提前撤清理资格。
    platform.LeaveCriticalSection(lock);
    unwind.active = false;
    return result;
}
}
