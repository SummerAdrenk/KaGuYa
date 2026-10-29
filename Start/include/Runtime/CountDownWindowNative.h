#pragma once
#include <Runtime/LayerWindowNativeBase.h>
#include <Runtime/LayerListNative.h>
#include <Runtime/DrawLayerReferenceArray.h>
#include <Runtime/DrawManagerResourceRuntime.h>
#include <Runtime/CountDownWindowResources.h>
#include <Runtime/CountDownWindowFiveFrameProfile.h>
#include <Runtime/ScriptNamedValueNative.h>

namespace KaGuYa::Start::Runtime
{
class GlobalParamsArchive;
class RootRuntimeOwner;
class IScrVmCommandErrorSink;
struct CountDownWindowNativeResourceProfile;
// 五帧仍用此44h共享body：numbersAt30存五帧、numberAt3C存索引。
// 不可照搬旧10号+34h索引；这里+34h是numbersAt30.end，会破坏容器。
struct CountDownWindowNativeBody44h final
{
    LayerWindowNativeBody24h window;
    union { DrawLayerReferenceArray suffixesAt24; };
    union { DrawLayerReferenceArray numbersAt30; };
    std::int32_t numberAt3C;
    std::int32_t suffixAt40;
    CountDownWindowNativeBody44h() noexcept {}
};
static_assert(sizeof(CountDownWindowNativeBody44h) == 0x44);
static_assert(offsetof(CountDownWindowNativeBody44h, suffixesAt24) == 0x24);
static_assert(offsetof(CountDownWindowNativeBody44h, numbersAt30) == 0x30);
static_assert(offsetof(CountDownWindowNativeBody44h, numberAt3C) == 0x3C);
static_assert(offsetof(CountDownWindowNativeBody44h, suffixAt40) == 0x40);
static_assert(std::is_trivially_destructible_v<CountDownWindowNativeBody44h>);

// GlobalParams各WORD读点独立取源；请求显示另保留先root、后slot0、最后CParams的顺序。
class ICountDownWindowNativeStateSource
{
public:
    virtual ~ICountDownWindowNativeStateSource() = default;
    virtual GlobalParamsArchive* CaptureGlobalInlineAt4180C0() = 0;
    virtual GlobalParamsArchive* CaptureGlobalVia417990() = 0;
    virtual std::int32_t ReadSuffixAfterRootAt486F8C() = 0;
};
class EnvironmentCountDownWindowNativeStateSource final : public ICountDownWindowNativeStateSource
{
public:
    EnvironmentCountDownWindowNativeStateSource(EnvironmentRootOwner&, RootRuntimeOwner&,
        ScriptNamedValueNativeTables, IScrVmCommandErrorSink&,
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource()) noexcept;
    GlobalParamsArchive* CaptureGlobalInlineAt4180C0() override;
    GlobalParamsArchive* CaptureGlobalVia417990() override;
    std::int32_t ReadSuffixAfterRootAt486F8C() override;
private:
    EnvironmentRootOwner& environment_;
    RootRuntimeOwner& root_;
    ScriptNamedValueNativeTables tables_;
    IScrVmCommandErrorSink& errors_;
    IRuntimeStringManagerSource& strings_;
};
struct CountDownWindowNativeServices final
{
    ISpriteNativeResourceSource& resources;
    ICountDownWindowNativeStateSource& state;
    const CountDownWindowResourceProfile& profile;
    IGraphicsSurfaceApi& surfaces;
    IGraphicsWindowSurfaceApi& drawing;
    IRuntimeStringManagerSource& strings;
    IEnvironmentRootMemory& memory;
    // 无五帧策略时，空指针明确保留旧宿主现代profile，不作失败换路。
    const CountDownWindowNativeResourceProfile* nativeProfile = nullptr;
    // 显式五帧策略，与nativeProfile互斥；构造后策略由派生虚表固定，不占body未写字节。
    const CountDownWindowFiveFrameResourceProfile* fiveFrameProfile = nullptr;
};
class CountDownWindowNativeScope final
{
public:
    explicit CountDownWindowNativeScope(CountDownWindowNativeServices&) noexcept;
    ~CountDownWindowNativeScope() noexcept;
    CountDownWindowNativeScope(const CountDownWindowNativeScope&) = delete;
    CountDownWindowNativeScope& operator=(const CountDownWindowNativeScope&) = delete;
private:
    CountDownWindowNativeServices* previous_;
    GraphicsSurfaceNativeScope surfaces_;
    LayerListNativeScope layers_;
};
const LayerItemNativeFrameVtable& CountDownWindowNativeMethods() noexcept;
const LayerItemNativeFrameVtable& FiveFrameCountDownWindowNativeMethods() noexcept;
CountDownWindowNativeBody44h* ConstructCountDownWindowNativeAt458210(CountDownWindowNativeBody44h&,
    LayerListNativeBody18h*, CountDownWindowNativeServices&);
CountDownWindowNativeBody44h* DeleteCountDownWindowNativeAt468770(CountDownWindowNativeBody44h&,
    std::uint32_t flags) noexcept;
RECT* CountDownBoundsNativeAt479CE0(CountDownWindowNativeBody44h&, RECT*, CountDownWindowNativeServices&);
YtlReferenceSlotRuntime* CountDownNumberNativeAt47BE60(CountDownWindowNativeBody44h&,
    YtlReferenceSlotRuntime& freshOutput, std::int32_t mode, CountDownWindowNativeServices&);
YtlReferenceSlotRuntime* CountDownSuffixNativeAt47C7B0(CountDownWindowNativeBody44h&,
    YtlReferenceSlotRuntime& freshOutput, std::int32_t mode, CountDownWindowNativeServices&);
std::uint8_t RequestCountDownWindowNativeAt486F70(CountDownWindowNativeBody44h&,
    std::uint8_t requested, CountDownWindowNativeServices&);
void AdvanceCountDownWindowNativeAt46B0D0(CountDownWindowNativeBody44h&, CountDownWindowNativeServices&);
void DrawCountDownWindowNativeAt4827A0(CountDownWindowNativeBody44h&, const RECT*, CountDownWindowNativeServices&);
void CompositeCountDownWindowNativeAt481830(CountDownWindowNativeBody44h&, GraphicsSurfaceNativeBody*,
    const RECT*, CountDownWindowNativeServices&);
void ResetCountDownWindowNativeAt47EC20(CountDownWindowNativeBody44h&) noexcept;
void ClearCountDownNumberNativeAt46DC00(CountDownWindowNativeBody44h&) noexcept;
}
