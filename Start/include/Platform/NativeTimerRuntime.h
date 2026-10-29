#pragma once

#include <Windows.h>
#include <mmsystem.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace KaGuYa::Start::Runtime { class IEnvironmentRootMemory; }

namespace KaGuYa::Start::Platform
{
class ITimerPlatformApi;
struct NativeTimerBody10h;
struct NativeFpsTimerBody140h;
struct NativeTimeGetTimeBody14h;
struct NativeTimerCreatorBody0Ch;

// 5B1D58：CTimer六槽。普通正文析构不经虚槽；+14仅为deleting destructor。
struct NativeTimerVtable final
{
    void (__thiscall* resetAt00)(NativeTimerBody10h*);
    std::uint32_t (__thiscall* elapsedAt04)(const NativeTimerBody10h*);
    void (__thiscall* setElapsedAt08)(NativeTimerBody10h*, std::uint32_t);
    void (__thiscall* pauseAt0C)(NativeTimerBody10h*);
    void (__thiscall* resumeAt10)(NativeTimerBody10h*);
    NativeTimerBody10h* (__thiscall* deleteAt14)(NativeTimerBody10h*, std::uint32_t);
};

struct NativeTimerBody10h final
{
    const NativeTimerVtable* vptrAt00;
    std::uint32_t startTickAt04;
    std::uint32_t pausedTickAt08;
    std::int32_t pauseDepthAt0C;
};

// 5B1CF0：+1C只返回AL原byte，不能归一化为bool。
struct NativeFpsTimerVtable final
{
    void (__thiscall* setFramesPerSecondAt00)(NativeFpsTimerBody140h*, std::uint32_t);
    std::uint32_t (__thiscall* framesPerSecondAt04)(const NativeFpsTimerBody140h*);
    std::uint32_t (__thiscall* measuredFramesPerSecondAt08)(const NativeFpsTimerBody140h*);
    std::uint32_t (__thiscall* percentageAt0C)(const NativeFpsTimerBody140h*);
    std::uint32_t (__thiscall* previousLateCountAt10)(const NativeFpsTimerBody140h*);
    void (__thiscall* resetTimingAnchorAt14)(NativeFpsTimerBody140h*);
    void (__thiscall* waitAt18)(NativeFpsTimerBody140h*);
    std::uint8_t (__thiscall* severeLateAt1C)(const NativeFpsTimerBody140h*);
    void (__thiscall* clearAccumulatedAt20)(NativeFpsTimerBody140h*);
    std::uint32_t (__thiscall* accumulatedAt24)(const NativeFpsTimerBody140h*);
    void (__thiscall* resetFrameCountAt28)(NativeFpsTimerBody140h*);
    std::uint32_t (__thiscall* frameCountAt2C)(const NativeFpsTimerBody140h*);
    NativeFpsTimerBody140h* (__thiscall* deleteAt30)(NativeFpsTimerBody140h*, std::uint32_t);
};

// 413D11/43F780：根+6C内嵌正文。禁止{}值初始化替代逐步原构造。
struct NativeFpsTimerBody140h final
{
    const NativeFpsTimerVtable* vptrAt00;
    std::uint32_t framesPerSecondAt04;
    std::uint32_t fixedPeriodAt08;
    std::uint32_t scheduledBaseAt0C;
    std::uint32_t fractionAt10;
    std::uint32_t accumulatedAt14;
    std::uint32_t rawAt18;
    std::array<std::uint32_t, 32> timestampsAt1C;
    std::array<std::uint32_t, 32> processingTimesAt9C;
    std::uint32_t historyIndexAt11C;
    std::uint32_t frameCountAt120;
    std::uint8_t severeLateAt124;
    std::array<std::byte, 3> untouchedAt125;
    std::uint32_t previousLateCountAt128;
    std::uint32_t currentLateCountAt12C;
    NativeTimerBody10h timerAt130;
};

struct NativeTimeGetTimeVtable final
{
    NativeTimeGetTimeBody14h* (__thiscall* deleteAt00)(NativeTimeGetTimeBody14h*, std::uint32_t);
};

// 43FB80：+05..0B不写，TIMECAPS直接交API写入，不经预清临时副本。
struct NativeTimeGetTimeBody14h final
{
    const NativeTimeGetTimeVtable* vptrAt00;
    std::uint8_t periodEnabledAt04;
    std::array<std::byte, 7> untouchedAt05;
    TIMECAPS capabilitiesAt0C;
};

struct NativeTimerCreatorVtable final
{
    NativeTimeGetTimeBody14h* (__thiscall* createAt00)(NativeTimerCreatorBody0Ch*);
    NativeTimerCreatorBody0Ch* (__thiscall* deleteAt04)(NativeTimerCreatorBody0Ch*, std::uint32_t);
};

// 原6012C8/CC/D0的唯一共享状态，不是YTL的18h引用控制块。
struct NativeTimerCreatorBody0Ch final
{
    const NativeTimerCreatorVtable* vptrAt00;
    NativeTimeGetTimeBody14h* sourceAt04;
    std::uint32_t countAt08;
};

struct NativeTimerDependencies final
{
    ITimerPlatformApi& api;
    Runtime::IEnvironmentRootMemory& memory;
    NativeTimerCreatorBody0Ch& creator;
};

// API、分配器、creator均在原正文外；嵌套scope退出恢复，不销毁共享源。
class NativeTimerScope final
{
public:
    explicit NativeTimerScope(NativeTimerDependencies& dependencies) noexcept;
    ~NativeTimerScope() noexcept;
    NativeTimerScope(const NativeTimerScope&) = delete;
    NativeTimerScope& operator=(const NativeTimerScope&) = delete;
private:
    NativeTimerDependencies* previous_;
};

[[nodiscard]] const NativeTimerVtable& NativeTimerMethods() noexcept;
[[nodiscard]] const NativeTimerVtable& NativeTimerBaseMethods() noexcept;
[[nodiscard]] const NativeFpsTimerVtable& NativeFpsTimerMethods() noexcept;
[[nodiscard]] const NativeFpsTimerVtable& NativeFpsTimerBaseMethods() noexcept;
[[nodiscard]] const NativeTimeGetTimeVtable& NativeTimeGetTimeMethods() noexcept;
[[nodiscard]] const NativeTimerCreatorVtable& NativeTimerCreatorMethods() noexcept;

// 宿主为所有CTimer/CFixTimer等共用此creator；独立测试可显式创建隔离实例。
[[nodiscard]] NativeTimerCreatorBody0Ch MakeNativeTimerCreator() noexcept;
[[nodiscard]] NativeTimerCreatorBody0Ch& ProcessNativeTimerCreator() noexcept;

NativeTimerBody10h* ConstructNativeTimerAt43FD20(NativeTimerBody10h& body);
void DestroyNativeTimerAt43FDF0(NativeTimerBody10h& body);
NativeTimerBody10h* DeleteNativeTimerAt43FEE0(NativeTimerBody10h& body, std::uint32_t flags);
NativeFpsTimerBody140h* ConstructNativeFpsTimerAt43F780(NativeFpsTimerBody140h& body);
void DestroyNativeFpsTimerAt43F820(NativeFpsTimerBody140h& body);
NativeFpsTimerBody140h* DeleteNativeFpsTimerAt43F850(NativeFpsTimerBody140h& body, std::uint32_t flags);
void WaitNativeFpsTimerAt43FA60(NativeFpsTimerBody140h& body);
NativeTimeGetTimeBody14h* CreateNativeTimeGetTimeAt43FB80();
NativeTimeGetTimeBody14h* DeleteNativeTimeGetTimeAt43FEA0(NativeTimeGetTimeBody14h& body, std::uint32_t flags);
NativeTimerCreatorBody0Ch* DeleteNativeTimerCreatorAt43FBF0(NativeTimerCreatorBody0Ch& body, std::uint32_t flags);

// 普通析构叶不额外包catch；构造展开guard及调用者自身的终止边界分别负责。
// root普通析构调用43F820，不free内嵌140h；4834F0经当前vptrAt00->waitAt18调用。
static_assert(sizeof(NativeTimerBody10h) == 0x10 && alignof(NativeTimerBody10h) == 4);
static_assert(offsetof(NativeTimerBody10h, startTickAt04) == 4);
static_assert(offsetof(NativeTimerBody10h, pausedTickAt08) == 8);
static_assert(offsetof(NativeTimerBody10h, pauseDepthAt0C) == 0x0C);
static_assert(sizeof(NativeTimerVtable) == 0x18);
static_assert(offsetof(NativeTimerVtable, deleteAt14) == 0x14);
static_assert(sizeof(NativeFpsTimerBody140h) == 0x140 && alignof(NativeFpsTimerBody140h) == 4);
static_assert(offsetof(NativeFpsTimerBody140h, timestampsAt1C) == 0x1C);
static_assert(offsetof(NativeFpsTimerBody140h, processingTimesAt9C) == 0x9C);
static_assert(offsetof(NativeFpsTimerBody140h, historyIndexAt11C) == 0x11C);
static_assert(offsetof(NativeFpsTimerBody140h, frameCountAt120) == 0x120);
static_assert(offsetof(NativeFpsTimerBody140h, severeLateAt124) == 0x124);
static_assert(offsetof(NativeFpsTimerBody140h, previousLateCountAt128) == 0x128);
static_assert(offsetof(NativeFpsTimerBody140h, currentLateCountAt12C) == 0x12C);
static_assert(offsetof(NativeFpsTimerBody140h, timerAt130) == 0x130);
static_assert(sizeof(NativeFpsTimerVtable) == 0x34);
static_assert(offsetof(NativeFpsTimerVtable, waitAt18) == 0x18);
static_assert(offsetof(NativeFpsTimerVtable, deleteAt30) == 0x30);
static_assert(sizeof(NativeTimeGetTimeBody14h) == 0x14 && alignof(NativeTimeGetTimeBody14h) == 4);
static_assert(offsetof(NativeTimeGetTimeBody14h, capabilitiesAt0C) == 0x0C);
static_assert(sizeof(NativeTimeGetTimeVtable) == 4);
static_assert(sizeof(NativeTimerCreatorBody0Ch) == 0x0C);
static_assert(offsetof(NativeTimerCreatorBody0Ch, sourceAt04) == 4);
static_assert(offsetof(NativeTimerCreatorBody0Ch, countAt08) == 8);
static_assert(sizeof(NativeTimerCreatorVtable) == 8);
static_assert(std::is_trivially_destructible_v<NativeTimerBody10h>);
static_assert(std::is_trivially_destructible_v<NativeFpsTimerBody140h>);
static_assert(std::is_trivially_destructible_v<NativeTimeGetTimeBody14h>);
static_assert(std::is_trivially_destructible_v<NativeTimerCreatorBody0Ch>);
}
