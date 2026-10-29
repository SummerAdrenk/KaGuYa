#pragma once

#include <Runtime/LayerWindowNativeBase.h>
#include <Runtime/SpriteNative.h>
#include <Runtime/InputNativeObjects.h>

namespace KaGuYa::Start::Runtime
{
class IGraphicsWindowSurfaceApi;

// 499A20 / 5B4A10：正文只保留原字段；服务、数组cookie均不嵌入60h。
struct CgDisplayWindowNativeBody60h final
{
    LayerWindowNativeBody24h window;
    union { YtlReferenceSlotRuntime spritesAt24; };
    std::uint32_t axesAt30;
    std::int32_t stepAt34;
    std::int32_t maximumXAt38;
    std::int32_t maximumYAt3C;
    std::int32_t minimumXAt40;
    std::int32_t minimumYAt44;
    std::int32_t repeatWaitAt48;
    std::int32_t repeatCountdownAt4C;
    std::uint32_t savedSlot2At50;
    std::uint8_t draggingAt54;
    std::uint8_t unwrittenAt55[3];
    POINT lastPointerAt58;
    CgDisplayWindowNativeBody60h() noexcept {}
    ~CgDisplayWindowNativeBody60h() noexcept {}
    CgDisplayWindowNativeBody60h(const CgDisplayWindowNativeBody60h&) = delete;
    CgDisplayWindowNativeBody60h& operator=(const CgDisplayWindowNativeBody60h&) = delete;
};
static_assert(sizeof(CgDisplayWindowNativeBody60h) == 0x60);
static_assert(alignof(CgDisplayWindowNativeBody60h) == 4);
static_assert(std::is_standard_layout_v<CgDisplayWindowNativeBody60h>);
static_assert(offsetof(CgDisplayWindowNativeBody60h, window) == 0);
static_assert(offsetof(CgDisplayWindowNativeBody60h, spritesAt24) == 0x24);
static_assert(offsetof(CgDisplayWindowNativeBody60h, axesAt30) == 0x30);
static_assert(offsetof(CgDisplayWindowNativeBody60h, stepAt34) == 0x34);
static_assert(offsetof(CgDisplayWindowNativeBody60h, maximumXAt38) == 0x38);
static_assert(offsetof(CgDisplayWindowNativeBody60h, maximumYAt3C) == 0x3C);
static_assert(offsetof(CgDisplayWindowNativeBody60h, minimumXAt40) == 0x40);
static_assert(offsetof(CgDisplayWindowNativeBody60h, minimumYAt44) == 0x44);
static_assert(offsetof(CgDisplayWindowNativeBody60h, repeatWaitAt48) == 0x48);
static_assert(offsetof(CgDisplayWindowNativeBody60h, repeatCountdownAt4C) == 0x4C);
static_assert(offsetof(CgDisplayWindowNativeBody60h, savedSlot2At50) == 0x50);
static_assert(offsetof(CgDisplayWindowNativeBody60h, draggingAt54) == 0x54);
static_assert(offsetof(CgDisplayWindowNativeBody60h, unwrittenAt55) == 0x55);
static_assert(offsetof(CgDisplayWindowNativeBody60h, lastPointerAt58) == 0x58);

// 每个418210/4180C0现场重新捕获；不缓存Mouse/Keyboard/registry正文。
// 业务算法不交给virtual回调；这里只借用已有的根与平台/Graphics导出。
struct CgDisplayWindowNativeServices final
{
    InputRootOwner& input;
    InputNativeServices& inputServices;
    EnvironmentRootOwner& environment;
    const Platform::ICursorApi& windowApi;
    IGraphicsWindowSurfaceApi& drawing;
};
class CgDisplayWindowNativeScope final
{
public:
    explicit CgDisplayWindowNativeScope(CgDisplayWindowNativeServices&) noexcept;
    ~CgDisplayWindowNativeScope() noexcept;
    CgDisplayWindowNativeScope(const CgDisplayWindowNativeScope&) = delete;
    CgDisplayWindowNativeScope& operator=(const CgDisplayWindowNativeScope&) = delete;
private:
    CgDisplayWindowNativeServices* previous_;
};

// 七槽复用已核定的LayerItem窗口ABI；draw/composite的EAX不作为基类返回合同。
const LayerItemNativeFrameVtable& CgDisplayWindowNativeMethods() noexcept;
// 原栈参数顺序为width,height,count,parent，ret 10h；SIZE保持前两DWORD。
// 数组阶段catch-all只清bounds并返回；不能以bounds为空推断未注册或无析构资格。
CgDisplayWindowNativeBody60h* ConstructCgDisplayWindowNativeAt499A20(
    CgDisplayWindowNativeBody60h&, SIZE size, std::int32_t count, LayerListNativeBody18h* parent,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
void DestroyCgDisplayWindowNative(CgDisplayWindowNativeBody60h&) noexcept;
CgDisplayWindowNativeBody60h* DeleteCgDisplayWindowNativeAt4A6030(
    CgDisplayWindowNativeBody60h&, std::uint32_t flags) noexcept;

std::uint32_t CgDisplayWindowSpriteCountAt4ACF90(const CgDisplayWindowNativeBody60h&) noexcept;
SpriteNativeBodyA0h* ResolveCgDisplayWindowSpriteNative(
    const CgDisplayWindowNativeBody60h&, std::uint32_t index) noexcept;
std::uint32_t DrawCgDisplayWindowNativeAt4B5300(CgDisplayWindowNativeBody60h&,
    const RECT* clip, IGraphicsWindowSurfaceApi&);
std::uint32_t CompositeCgDisplayWindowNativeAt4B5040(CgDisplayWindowNativeBody60h&,
    GraphicsSurfaceNativeBody* destination, const RECT* clip, IGraphicsWindowSurfaceApi&);
RECT* ReadCgDisplayWindowBoundsNativeAt479DB0(CgDisplayWindowNativeBody60h&, RECT* output) noexcept;
std::uint8_t QueryCgDisplayWindowNativeAt4C1240(std::uint32_t, std::uint32_t) noexcept;
void AdvanceCgDisplayWindowNativeAt4A7470(CgDisplayWindowNativeBody60h&);
void ResetCgDisplayWindowNativeAt4AF1E0(CgDisplayWindowNativeBody60h&) noexcept;
void BeginCgDisplayWindowNativeAt4AA860(CgDisplayWindowNativeBody60h&, CgDisplayWindowNativeServices&);
void UpdateCgDisplayWindowNativeAt4BD050(CgDisplayWindowNativeBody60h&, CgDisplayWindowNativeServices&);
void ConfigureCgDisplayWindowPanNativeAt4C17F0(CgDisplayWindowNativeBody60h&, std::int32_t step);
void EndCgDisplayWindowDragNativeAt4C13A0(CgDisplayWindowNativeBody60h&,
    std::int32_t restoreRegistry, CgDisplayWindowNativeServices&);
}
