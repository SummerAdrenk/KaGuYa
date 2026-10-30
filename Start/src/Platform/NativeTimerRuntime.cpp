#include <Platform/NativeTimerRuntime.h>
#include <Platform/FrameTimer.h>
#include <Runtime/EnvironmentRootRuntime.h>

#include <bit>
#include <cstdlib>
#include <exception>
#include <stdexcept>

namespace KaGuYa::Start::Platform
{
namespace
{
thread_local NativeTimerDependencies* activeDependencies = nullptr;

__declspec(noinline) NativeTimerDependencies& Current()
{
    if (!activeDependencies)
        throw std::logic_error("Native timer requires its external NativeTimerScope");
    return *activeDependencies;
}

std::uint32_t RawTick()
{
    // 原每次先读6012CC+04决定分支；API回调之后不重新选择同一次取时源。
    const bool multimedia = Current().creator.sourceAt04->periodEnabledAt04 != 0;
    return multimedia ? Current().api.MultimediaTick() : Current().api.SystemTick();
}

struct TimerConstructionCleanup final
{
    NativeTimerBody10h& body;
    bool armed = true;
    ~TimerConstructionCleanup() noexcept
    {
        // 594BC0 -> 43FE30仅退基类表，不减已提交的6012D0、不清共享payload。
        if (armed) body.vptrAt00 = &NativeTimerBaseMethods();
    }
};

struct FpsConstructionCleanup final
{
    NativeFpsTimerBody140h& body;
    bool timerComplete = false;
    bool armed = true;
    ~FpsConstructionCleanup() noexcept
    {
        if (!armed) return;
        // 5E3BC0：state1先594B58，再state0的594B50；清理抛出则终止。
        if (timerComplete) DestroyNativeTimerAt43FDF0(body.timerAt130);
        body.vptrAt00 = &NativeFpsTimerBaseMethods();
    }
};

void __fastcall Reset(NativeTimerBody10h* body, void*)
{
    const auto tick = RawTick();
    body->pauseDepthAt0C = 0;
    body->startTickAt04 = tick;
}

std::uint32_t __fastcall Elapsed(const NativeTimerBody10h* body, void*)
{
    // 43FF97为有符号jle；负深度仍取时，减法在API返回后读取start。
    if (body->pauseDepthAt0C > 0) return body->pausedTickAt08 - body->startTickAt04;
    const auto tick = RawTick();
    return tick - body->startTickAt04;
}

void __fastcall SetElapsed(NativeTimerBody10h* body, void*, std::uint32_t elapsed)
{
    const auto tick = body->pauseDepthAt0C > 0 ? body->pausedTickAt08 : RawTick();
    body->startTickAt04 = tick - elapsed;
}

void __fastcall Pause(NativeTimerBody10h* body, void*)
{
    const auto old = std::bit_cast<std::uint32_t>(body->pauseDepthAt0C);
    body->pauseDepthAt0C = std::bit_cast<std::int32_t>(old + 1u);
    if (old == 0) body->pausedTickAt08 = RawTick();
}

void __fastcall Resume(NativeTimerBody10h* body, void*)
{
    const auto depth = std::bit_cast<std::uint32_t>(body->pauseDepthAt0C) - 1u;
    body->pauseDepthAt0C = std::bit_cast<std::int32_t>(depth);
    if (depth != 0) return;
    const auto tick = RawTick();
    // 44008A/8D：API返回后才读paused/start，不能跨回调冻结。
    body->startTickAt04 += tick - body->pausedTickAt08;
}

NativeTimerBody10h* __fastcall DeleteTimer(NativeTimerBody10h* body, void*, std::uint32_t flags)
{ return DeleteNativeTimerAt43FEE0(*body, flags); }

NativeTimerBody10h* __fastcall DeleteTimerBase(NativeTimerBody10h* body, void*, std::uint32_t flags)
{
    body->vptrAt00 = &NativeTimerBaseMethods();
    if ((flags & 1u) != 0) Current().memory.DeleteBody(body, 4); // 43FF30
    return body;
}

void __fastcall ResetAnchor(NativeFpsTimerBody140h* body, void*)
{
    auto& timer = body->timerAt130;
    body->scheduledBaseAt0C = timer.vptrAt00->elapsedAt04(&timer);
    body->severeLateAt124 = 0;
    body->previousLateCountAt128 = 0;
    body->currentLateCountAt12C = 0;
}

void __fastcall ClearAccumulated(NativeFpsTimerBody140h* body, void*) { body->accumulatedAt14 = 0; }
void __fastcall ResetFrameCount(NativeFpsTimerBody140h* body, void*) { body->frameCountAt120 = 0; }

void __fastcall SetFps(NativeFpsTimerBody140h* body, void*, std::uint32_t fps)
{
    body->vptrAt00->resetTimingAnchorAt14(body);
    body->vptrAt00->clearAccumulatedAt20(body); // 43FA0B重新取表。
    body->frameCountAt120 = 0;
    body->rawAt18 = 0;
    body->historyIndexAt11C = 0;
    body->framesPerSecondAt04 = fps;
    if (fps != 0)
    {
        body->fractionAt10 = 0;
        body->fixedPeriodAt08 = 0x03E80000u / fps;
    }
}

std::uint32_t __fastcall Fps(const NativeFpsTimerBody140h* body, void*) { return body->framesPerSecondAt04; }
std::uint32_t __fastcall PreviousLate(const NativeFpsTimerBody140h* body, void*) { return body->previousLateCountAt128; }
std::uint32_t __fastcall Accumulated(const NativeFpsTimerBody140h* body, void*) { return body->accumulatedAt14; }
std::uint32_t __fastcall FrameCount(const NativeFpsTimerBody140h* body, void*) { return body->frameCountAt120; }
std::uint8_t __fastcall SevereLate(const NativeFpsTimerBody140h* body, void*) { return body->severeLateAt124; }

std::uint32_t __fastcall MeasuredFps(const NativeFpsTimerBody140h* body, void*)
{
    const auto index = body->historyIndexAt11C;
    if (index < 16) return 0;
    // 43F951/55前16..31区间取15个间隔；之后固定32项环取31个间隔。
    const auto elapsed = index < 32
        ? body->timestampsAt1C[index - 1] - body->timestampsAt1C[index - 16]
        : body->timestampsAt1C[(index - 1) & 31u] - body->timestampsAt1C[index & 31u];
    if (elapsed == 0) return 0;
    return ((elapsed >> 1) + (index < 32 ? 15000u : 31000u)) / elapsed;
}

std::uint32_t __fastcall Percentage(const NativeFpsTimerBody140h* body, void*)
{
    if (body->historyIndexAt11C < 16) return 0;
    std::uint32_t sum = 0;
    // 43F8D3..8EE固定累加processing的前16项，并非按historyIndex旋转。
    for (std::size_t index = 0; index < 16; ++index) sum += body->processingTimesAt9C[index];
    const std::uint32_t product = sum * body->framesPerSecondAt04;
    return 100u - product / 160u; // 32位imul截断后，CCCCCCCD乘高位再shr7。
}

void __fastcall Wait(NativeFpsTimerBody140h* body, void*) { WaitNativeFpsTimerAt43FA60(*body); }
NativeFpsTimerBody140h* __fastcall DeleteFps(NativeFpsTimerBody140h* body, void*, std::uint32_t flags)
{ return DeleteNativeFpsTimerAt43F850(*body, flags); }
NativeFpsTimerBody140h* __fastcall DeleteFpsBase(NativeFpsTimerBody140h* body, void*, std::uint32_t flags)
{
    body->vptrAt00 = &NativeFpsTimerBaseMethods();
    if ((flags & 1u) != 0) Current().memory.DeleteBody(body, 4); // 43F890
    return body;
}

[[noreturn]] void PureCall()
{
    _purecall();
    std::terminate();
}
void __fastcall PureTimerVoid(NativeTimerBody10h*, void*) { PureCall(); }
void __fastcall PureTimerSet(NativeTimerBody10h*, void*, std::uint32_t) { PureCall(); }
std::uint32_t __fastcall PureTimerRead(const NativeTimerBody10h*, void*) { PureCall(); }
void __fastcall PureFpsVoid(NativeFpsTimerBody140h*, void*) { PureCall(); }
void __fastcall PureFpsSet(NativeFpsTimerBody140h*, void*, std::uint32_t) { PureCall(); }
std::uint32_t __fastcall PureFpsRead(const NativeFpsTimerBody140h*, void*) { PureCall(); }
std::uint8_t __fastcall PureFpsByte(const NativeFpsTimerBody140h*, void*) { PureCall(); }

NativeTimeGetTimeBody14h* __fastcall CreateSource(NativeTimerCreatorBody0Ch*, void*)
{ return CreateNativeTimeGetTimeAt43FB80(); }
NativeTimerCreatorBody0Ch* __fastcall DeleteCreator(NativeTimerCreatorBody0Ch* body, void*, std::uint32_t flags)
{ return DeleteNativeTimerCreatorAt43FBF0(*body, flags); }
NativeTimeGetTimeBody14h* __fastcall DeleteSource(NativeTimeGetTimeBody14h* body, void*, std::uint32_t flags)
{ return DeleteNativeTimeGetTimeAt43FEA0(*body, flags); }

// 保留原生x86虚槽：ECX接收者、未读取的EDX占位及栈参数按对应槽布局桥接。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const NativeTimerVtable timerMethods{
    reinterpret_cast<decltype(NativeTimerVtable::resetAt00)>(&Reset),
    reinterpret_cast<decltype(NativeTimerVtable::elapsedAt04)>(&Elapsed),
    reinterpret_cast<decltype(NativeTimerVtable::setElapsedAt08)>(&SetElapsed),
    reinterpret_cast<decltype(NativeTimerVtable::pauseAt0C)>(&Pause),
    reinterpret_cast<decltype(NativeTimerVtable::resumeAt10)>(&Resume),
    reinterpret_cast<decltype(NativeTimerVtable::deleteAt14)>(&DeleteTimer)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
// 保留原生x86虚槽：ECX接收者、未读取的EDX占位及栈参数按对应槽布局桥接。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const NativeTimerVtable timerBaseMethods{
    reinterpret_cast<decltype(NativeTimerVtable::resetAt00)>(&PureTimerVoid),
    reinterpret_cast<decltype(NativeTimerVtable::elapsedAt04)>(&PureTimerRead),
    reinterpret_cast<decltype(NativeTimerVtable::setElapsedAt08)>(&PureTimerSet),
    reinterpret_cast<decltype(NativeTimerVtable::pauseAt0C)>(&PureTimerVoid),
    reinterpret_cast<decltype(NativeTimerVtable::resumeAt10)>(&PureTimerVoid),
    reinterpret_cast<decltype(NativeTimerVtable::deleteAt14)>(&DeleteTimerBase)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
// 保留原生x86虚槽：ECX接收者、未读取的EDX占位及栈参数按对应槽布局桥接。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const NativeFpsTimerVtable fpsMethods{
    reinterpret_cast<decltype(NativeFpsTimerVtable::setFramesPerSecondAt00)>(&SetFps),
    reinterpret_cast<decltype(NativeFpsTimerVtable::framesPerSecondAt04)>(&Fps),
    reinterpret_cast<decltype(NativeFpsTimerVtable::measuredFramesPerSecondAt08)>(&MeasuredFps),
    reinterpret_cast<decltype(NativeFpsTimerVtable::percentageAt0C)>(&Percentage),
    reinterpret_cast<decltype(NativeFpsTimerVtable::previousLateCountAt10)>(&PreviousLate),
    reinterpret_cast<decltype(NativeFpsTimerVtable::resetTimingAnchorAt14)>(&ResetAnchor),
    reinterpret_cast<decltype(NativeFpsTimerVtable::waitAt18)>(&Wait),
    reinterpret_cast<decltype(NativeFpsTimerVtable::severeLateAt1C)>(&SevereLate),
    reinterpret_cast<decltype(NativeFpsTimerVtable::clearAccumulatedAt20)>(&ClearAccumulated),
    reinterpret_cast<decltype(NativeFpsTimerVtable::accumulatedAt24)>(&Accumulated),
    reinterpret_cast<decltype(NativeFpsTimerVtable::resetFrameCountAt28)>(&ResetFrameCount),
    reinterpret_cast<decltype(NativeFpsTimerVtable::frameCountAt2C)>(&FrameCount),
    reinterpret_cast<decltype(NativeFpsTimerVtable::deleteAt30)>(&DeleteFps)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
// 保留原生x86虚槽：ECX接收者、未读取的EDX占位及栈参数按对应槽布局桥接。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const NativeFpsTimerVtable fpsBaseMethods{
    reinterpret_cast<decltype(NativeFpsTimerVtable::setFramesPerSecondAt00)>(&PureFpsSet),
    reinterpret_cast<decltype(NativeFpsTimerVtable::framesPerSecondAt04)>(&PureFpsRead),
    reinterpret_cast<decltype(NativeFpsTimerVtable::measuredFramesPerSecondAt08)>(&PureFpsRead),
    reinterpret_cast<decltype(NativeFpsTimerVtable::percentageAt0C)>(&PureFpsRead),
    reinterpret_cast<decltype(NativeFpsTimerVtable::previousLateCountAt10)>(&PureFpsRead),
    reinterpret_cast<decltype(NativeFpsTimerVtable::resetTimingAnchorAt14)>(&PureFpsVoid),
    reinterpret_cast<decltype(NativeFpsTimerVtable::waitAt18)>(&PureFpsVoid),
    reinterpret_cast<decltype(NativeFpsTimerVtable::severeLateAt1C)>(&PureFpsByte),
    reinterpret_cast<decltype(NativeFpsTimerVtable::clearAccumulatedAt20)>(&PureFpsVoid),
    reinterpret_cast<decltype(NativeFpsTimerVtable::accumulatedAt24)>(&PureFpsRead),
    reinterpret_cast<decltype(NativeFpsTimerVtable::resetFrameCountAt28)>(&PureFpsVoid),
    reinterpret_cast<decltype(NativeFpsTimerVtable::frameCountAt2C)>(&PureFpsRead),
    reinterpret_cast<decltype(NativeFpsTimerVtable::deleteAt30)>(&DeleteFpsBase)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
// 保留原生x86虚槽：ECX接收者、未读取的EDX占位及栈参数按对应槽布局桥接。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const NativeTimeGetTimeVtable sourceMethods{
    reinterpret_cast<decltype(NativeTimeGetTimeVtable::deleteAt00)>(&DeleteSource)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
// 保留原生x86虚槽：ECX接收者、未读取的EDX占位及栈参数按对应槽布局桥接。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const NativeTimerCreatorVtable creatorMethods{
    reinterpret_cast<decltype(NativeTimerCreatorVtable::createAt00)>(&CreateSource),
    reinterpret_cast<decltype(NativeTimerCreatorVtable::deleteAt04)>(&DeleteCreator)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
}

__declspec(noinline) NativeTimerScope::NativeTimerScope(NativeTimerDependencies& dependencies) noexcept
    : previous_(activeDependencies) { activeDependencies = &dependencies; }
__declspec(noinline) NativeTimerScope::~NativeTimerScope() noexcept { activeDependencies = previous_; }

const NativeTimerVtable& NativeTimerMethods() noexcept { return timerMethods; }
const NativeTimerVtable& NativeTimerBaseMethods() noexcept { return timerBaseMethods; }
const NativeFpsTimerVtable& NativeFpsTimerMethods() noexcept { return fpsMethods; }
const NativeFpsTimerVtable& NativeFpsTimerBaseMethods() noexcept { return fpsBaseMethods; }
const NativeTimeGetTimeVtable& NativeTimeGetTimeMethods() noexcept { return sourceMethods; }
const NativeTimerCreatorVtable& NativeTimerCreatorMethods() noexcept { return creatorMethods; }
NativeTimerCreatorBody0Ch MakeNativeTimerCreator() noexcept { return {&creatorMethods, nullptr, 0}; }
NativeTimerCreatorBody0Ch& ProcessNativeTimerCreator() noexcept
{
    // 原6012C8为进程共享对象；显式43FBF0收尾由宿主安排，scope不代替其寿命。
    static NativeTimerCreatorBody0Ch creator{&creatorMethods, nullptr, 0};
    return creator;
}

NativeTimerBody10h* ConstructNativeTimerAt43FD20(NativeTimerBody10h& body)
{
    body.vptrAt00 = &timerMethods;
    TimerConstructionCleanup cleanup{body};
    auto& creator = Current().creator;
    const auto old = creator.countAt08;
    creator.countAt08 = old + 1u;
    if (old == 0) creator.sourceAt04 = creator.vptrAt00->createAt00(&creator);
    body.startTickAt04 = RawTick();
    body.pauseDepthAt0C = 0;
    cleanup.armed = false;
    return &body;
}

void DestroyNativeTimerAt43FDF0(NativeTimerBody10h& body)
{
    body.vptrAt00 = &timerMethods;
    auto& creator = Current().creator;
    --creator.countAt08;
    if (creator.countAt08 == 0)
    {
        auto* const source = creator.sourceAt04;
        if (source)
        {
            source->vptrAt00->deleteAt00(source, 1);
            creator.sourceAt04 = nullptr; // 43FE12在回调返回后清当前全局源。
        }
    }
    body.vptrAt00 = &timerBaseMethods;
}

NativeTimerBody10h* DeleteNativeTimerAt43FEE0(NativeTimerBody10h& body, std::uint32_t flags)
{
    DestroyNativeTimerAt43FDF0(body);
    if ((flags & 1u) != 0) Current().memory.DeleteBody(&body, 0x10);
    return &body;
}

NativeFpsTimerBody140h* ConstructNativeFpsTimerAt43F780(NativeFpsTimerBody140h& body)
{
    body.vptrAt00 = &fpsMethods;
    FpsConstructionCleanup cleanup{body};
    ConstructNativeTimerAt43FD20(body.timerAt130);
    cleanup.timerComplete = true;
    body.vptrAt00->resetTimingAnchorAt14(&body);
    body.vptrAt00->clearAccumulatedAt20(&body);
    body.frameCountAt120 = 0;
    body.rawAt18 = 0;
    body.historyIndexAt11C = 0;
    body.framesPerSecondAt04 = 60;
    body.fixedPeriodAt08 = 0x0010AAAAu;
    body.fractionAt10 = 0;
    cleanup.armed = false;
    return &body;
}

void DestroyNativeFpsTimerAt43F820(NativeFpsTimerBody140h& body)
{
    body.vptrAt00 = &fpsMethods;
    DestroyNativeTimerAt43FDF0(body.timerAt130);
    body.vptrAt00 = &fpsBaseMethods;
}

NativeFpsTimerBody140h* DeleteNativeFpsTimerAt43F850(NativeFpsTimerBody140h& body, std::uint32_t flags)
{
    DestroyNativeFpsTimerAt43F820(body);
    if ((flags & 1u) != 0) Current().memory.DeleteBody(&body, 0x140);
    return &body;
}

void WaitNativeFpsTimerAt43FA60(NativeFpsTimerBody140h& body)
{
    auto& timer = body.timerAt130;
    const auto now = timer.vptrAt00->elapsedAt04(&timer);
    const auto fps = body.framesPerSecondAt04;
    if (fps != 0 && body.frameCountAt120 % fps == 0)
    {
        body.previousLateCountAt128 = body.currentLateCountAt12C;
        body.currentLateCountAt12C = 0;
    }
    body.timestampsAt1C[body.historyIndexAt11C & 31u] = now;
    auto index = body.historyIndexAt11C + 1u;
    body.historyIndexAt11C = index;
    if (index == 64) body.historyIndexAt11C = index = 32;
    ++body.frameCountAt120;
    if (body.framesPerSecondAt04 == 0)
    {
        body.processingTimesAt9C[index & 31u] = 0;
        return;
    }
    const auto fraction = (body.fractionAt10 & 0xFFFFu) + body.fixedPeriodAt08;
    const auto elapsed = now - body.scheduledBaseAt0C;
    body.fractionAt10 = fraction;
    const auto budget = fraction >> 16;
    if (elapsed < budget)
    {
        body.accumulatedAt14 += elapsed;
        body.processingTimesAt9C[index & 31u] = elapsed;
        body.severeLateAt124 = 0;
        Current().api.SleepMilliseconds(budget - elapsed);
        body.scheduledBaseAt0C += budget; // 43FB6E晚读锚点，保留Sleep回调的修改。
        return;
    }
    const auto late = elapsed - budget;
    const auto correction = late < 250 ? late : 0u;
    body.severeLateAt124 = static_cast<std::uint8_t>(correction >= 3u * budget);
    if (body.severeLateAt124 != 0) ++body.currentLateCountAt12C;
    body.scheduledBaseAt0C = now - correction;
    body.processingTimesAt9C[index & 31u] = 0;
}

NativeTimeGetTimeBody14h* CreateNativeTimeGetTimeAt43FB80()
{
    auto* body = static_cast<NativeTimeGetTimeBody14h*>(Current().memory.AllocateBody(0x14));
    if (!body) return nullptr;
    body->vptrAt00 = &sourceMethods;
    // 原43FB80无分配后EH回收；API异常不凭空回滚原分配。
    const auto result = Current().api.ReadCapabilities(body->capabilitiesAt0C);
    if (result == TIMERR_NOERROR && body->capabilitiesAt0C.wPeriodMin < 10)
    {
        const auto period = body->capabilitiesAt0C.wPeriodMin;
        body->periodEnabledAt04 = 1;
        Current().api.BeginPeriod(period);
    }
    else
    {
        body->capabilitiesAt0C.wPeriodMin = 1;
        body->periodEnabledAt04 = 0;
    }
    return body;
}

NativeTimeGetTimeBody14h* DeleteNativeTimeGetTimeAt43FEA0(NativeTimeGetTimeBody14h& body, std::uint32_t flags)
{
    const bool enabled = body.periodEnabledAt04 != 0;
    body.vptrAt00 = &sourceMethods;
    if (enabled) Current().api.EndPeriod(body.capabilitiesAt0C.wPeriodMin);
    if ((flags & 1u) != 0) Current().memory.DeleteBody(&body, 0x14);
    return &body;
}

NativeTimerCreatorBody0Ch* DeleteNativeTimerCreatorAt43FBF0(NativeTimerCreatorBody0Ch& body, std::uint32_t flags)
{
    auto* const source = body.sourceAt04; // 43FBF6先取payload，再安装creator原表。
    body.vptrAt00 = &creatorMethods;
    if (source)
    {
        source->vptrAt00->deleteAt00(source, 1);
        body.sourceAt04 = nullptr;
    }
    if ((flags & 1u) != 0) Current().memory.DeleteBody(&body, 0x0C);
    return &body;
}
}
