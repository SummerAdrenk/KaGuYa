#pragma once
#include <Runtime/ParamsDemoStorage.h>
#include <Runtime/TransitRuntime.h>

namespace KaGuYa::Start::Runtime
{
struct DrawManagerNativeBody2Ch;
struct DemoSpriteNativeBodyBCh;
struct SpriteNativeBodyA0h;
class IGraphicsSurfaceApi;
class IGraphicsWindowSurfaceApi;
class IGraphicsSurfaceTransitionApi;
class IGraphicsScreenCalls;

// 49B6F0：首DWORD是wait，不是虚表；服务/分配器只在正文外借用。
struct DemoSequenceNativeBodyA4h final
{
    std::int32_t waitAt00;
    std::int32_t stepAt04;
    union { YtlReferenceSlotRuntime canvasAt08; };
    union { YtlReferenceSlotRuntime maskAt14; };
    union { YtlReferenceSlotRuntime previousAt20; };
    union { YtlReferenceSlotRuntime alphaAt2C; };
    union { EnvironmentVectorStorage<YtlReferenceSlotRuntime> spritesAt38; };
    union { YtlReferenceSlotRuntime dirtyAt44; };
    RECT boundsAt50;
    union { TransitOperation operationAt60; };
    std::uint8_t readyAt90;
    std::uint8_t unwrittenAt91[3];
    std::uint32_t indexAt94;
    union { YtlReferenceSlotRuntime dataAt98; };
    DemoSequenceNativeBodyA4h() noexcept {}
    ~DemoSequenceNativeBodyA4h() noexcept {}
};
static_assert(sizeof(DemoSequenceNativeBodyA4h)==0xA4);
static_assert(offsetof(DemoSequenceNativeBodyA4h,canvasAt08)==0x08);
static_assert(offsetof(DemoSequenceNativeBodyA4h,spritesAt38)==0x38);
static_assert(offsetof(DemoSequenceNativeBodyA4h,dirtyAt44)==0x44);
static_assert(offsetof(DemoSequenceNativeBodyA4h,boundsAt50)==0x50);
static_assert(offsetof(DemoSequenceNativeBodyA4h,operationAt60)==0x60);
static_assert(offsetof(DemoSequenceNativeBodyA4h,readyAt90)==0x90);
static_assert(offsetof(DemoSequenceNativeBodyA4h,indexAt94)==0x94);
static_assert(offsetof(DemoSequenceNativeBodyA4h,dataAt98)==0x98);

class IDemoSequenceNativeSource
{
public:
    virtual ~IDemoSequenceNativeSource() = default;
    virtual EnvironmentRootStorage* CaptureEnvironmentAt4180C0() = 0;
    virtual DrawManagerNativeBody2Ch* CaptureDrawAt413BE0() = 0;
    virtual bool LoadMaskAt480000(void*,YtlReferenceSlotRuntime&,const char*) = 0;
    virtual bool LoadSpriteAt4814F0(void*,SpriteNativeBodyA0h*,const char*,std::uint8_t) = 0;
    virtual void ExecuteSoundAt4BF692(const ParamsDemoCmdSound&) = 0;
};
struct DemoSequenceNativeServices final
{
    IDemoSequenceNativeSource& source;
    IGraphicsSurfaceApi& surfaces;
    IGraphicsWindowSurfaceApi& drawing;
    IGraphicsSurfaceTransitionApi& surfaceTransitions;
    IGraphicsTransitApi& transit;
    IGraphicsScreenCalls& screen;
    IEnvironmentRootMemory& memory;
    IRuntimeStringManagerSource& strings;
};
DemoSequenceNativeBodyA4h* ConstructDemoSequenceNativeAt49B6F0(
    DemoSequenceNativeBodyA4h&,DemoSequenceNativeServices&);
DemoSequenceNativeBodyA4h* NewDemoSequenceNativeAt49B624(DemoSequenceNativeServices&);
void DestroyDemoSequenceNativeAt4A3CA0(DemoSequenceNativeBodyA4h&,DemoSequenceNativeServices&) noexcept;
void DeleteDemoSequenceNativeAt4C5FB0(DemoSequenceNativeBodyA4h*,DemoSequenceNativeServices&) noexcept;
void BindDemoSequenceNativeAt4C2220(DemoSequenceNativeBodyA4h&,const YtlReferenceSlotRuntime&);
std::int32_t ExecuteDemoCommandNativeAt4BF5A0(DemoSequenceNativeBodyA4h&,DemoSequenceNativeServices&);
std::int32_t RunDemoSequenceNativeAt4BDAE0(DemoSequenceNativeBodyA4h&,std::uint32_t,DemoSequenceNativeServices&);
bool AdvanceDemoTransitionNativeAt4C38A0(DemoSequenceNativeBodyA4h&,DemoSequenceNativeServices&);
void ComposeDemoNativeAt4A7F00(DemoSequenceNativeBodyA4h&,GraphicsSurfaceNativeBody*,DemoSequenceNativeServices&);
void UpdateDemoSpritesNativeAt4A7690(DemoSequenceNativeBodyA4h&,DemoSequenceNativeServices&);
void RedrawDemoNativeAt4C3D50(DemoSequenceNativeBodyA4h&,DemoSequenceNativeServices&);
}
