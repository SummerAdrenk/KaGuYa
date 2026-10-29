#pragma once

#include <Runtime/GraffitiPageNative.h>
#include <Runtime/GuiControlFactoryNative.h>
#include <Runtime/GraphicsSurfaceReference.h>
#include <Runtime/CountResourceProfile.h>

#include <array>

namespace KaGuYa::Start::Runtime
{
// 4E13A0/4F4BC0的共同前缀。Ref的默认构造不能提前写原字节。
struct GraffitiCountDialogNativeBody90h final
{
    union { GuiDialogNativeBody5Ch dialog; };
    RECT rectangleAt5C;
    union { YtlReferenceSlotRuntime canvasAt6C; };
    GuiControlSurfaceReferences backgroundsAt78;
    GuiControlSurfaceReferences digitsAt84;
    GraffitiCountDialogNativeBody90h() noexcept {}
};
struct GraffitiLikableCountDialogNativeBodyB4h final
{
    union { GraffitiCountDialogNativeBody90h common; };
    std::int32_t categoryAt90;
    std::int32_t initialAt94;
    std::int32_t targetAt98;
    std::int32_t incrementAt9C;
    std::int32_t lifecycleAtA0;
    std::int32_t animationAtA4;
    std::int32_t resumeAtA8;
    std::int32_t delayAtAC;
    std::int32_t skipAtB0;
    GraffitiLikableCountDialogNativeBodyB4h() noexcept {}
};
struct GraffitiWeightCountDialogNativeBodyB8h final
{
    union { GraffitiCountDialogNativeBody90h common; };
    std::int32_t initialAt90;
    std::int32_t targetAt94;
    std::int32_t decrementAt98;
    std::int32_t trainingAt9C;
    std::int32_t lifecycleAtA0;
    std::int32_t animationAtA4;
    std::int32_t resumeAtA8;
    std::int32_t skipResumeAtAC;
    std::int32_t delayAtB0;
    std::int32_t skipAtB4;
    GraffitiWeightCountDialogNativeBodyB8h() noexcept {}
};
static_assert(sizeof(GraffitiCountDialogNativeBody90h) == 0x90);
static_assert(offsetof(GraffitiCountDialogNativeBody90h, dialog) == 0);
static_assert(offsetof(GraffitiCountDialogNativeBody90h, rectangleAt5C) == 0x5C);
static_assert(offsetof(GraffitiCountDialogNativeBody90h, canvasAt6C) == 0x6C);
static_assert(offsetof(GraffitiCountDialogNativeBody90h, backgroundsAt78) == 0x78);
static_assert(offsetof(GraffitiCountDialogNativeBody90h, digitsAt84) == 0x84);
static_assert(sizeof(GraffitiLikableCountDialogNativeBodyB4h) == 0xB4);
static_assert(sizeof(GraffitiWeightCountDialogNativeBodyB8h) == 0xB8);
static_assert(alignof(GraffitiLikableCountDialogNativeBodyB4h) == 4);
static_assert(alignof(GraffitiWeightCountDialogNativeBodyB8h) == 4);
static_assert(offsetof(GraffitiLikableCountDialogNativeBodyB4h, categoryAt90) == 0x90);
static_assert(offsetof(GraffitiLikableCountDialogNativeBodyB4h, initialAt94) == 0x94);
static_assert(offsetof(GraffitiLikableCountDialogNativeBodyB4h, targetAt98) == 0x98);
static_assert(offsetof(GraffitiLikableCountDialogNativeBodyB4h, incrementAt9C) == 0x9C);
static_assert(offsetof(GraffitiLikableCountDialogNativeBodyB4h, lifecycleAtA0) == 0xA0);
static_assert(offsetof(GraffitiLikableCountDialogNativeBodyB4h, animationAtA4) == 0xA4);
static_assert(offsetof(GraffitiLikableCountDialogNativeBodyB4h, resumeAtA8) == 0xA8);
static_assert(offsetof(GraffitiLikableCountDialogNativeBodyB4h, delayAtAC) == 0xAC);
static_assert(offsetof(GraffitiLikableCountDialogNativeBodyB4h, skipAtB0) == 0xB0);
static_assert(offsetof(GraffitiWeightCountDialogNativeBodyB8h, decrementAt98) == 0x98);
static_assert(offsetof(GraffitiWeightCountDialogNativeBodyB8h, initialAt90) == 0x90);
static_assert(offsetof(GraffitiWeightCountDialogNativeBodyB8h, targetAt94) == 0x94);
static_assert(offsetof(GraffitiWeightCountDialogNativeBodyB8h, trainingAt9C) == 0x9C);
static_assert(offsetof(GraffitiWeightCountDialogNativeBodyB8h, lifecycleAtA0) == 0xA0);
static_assert(offsetof(GraffitiWeightCountDialogNativeBodyB8h, animationAtA4) == 0xA4);
static_assert(offsetof(GraffitiWeightCountDialogNativeBodyB8h, resumeAtA8) == 0xA8);
static_assert(offsetof(GraffitiWeightCountDialogNativeBodyB8h, skipResumeAtAC) == 0xAC);
static_assert(offsetof(GraffitiWeightCountDialogNativeBodyB8h, delayAtB0) == 0xB0);
static_assert(offsetof(GraffitiWeightCountDialogNativeBodyB8h, skipAtB4) == 0xB4);
static_assert(std::is_standard_layout_v<GraffitiLikableCountDialogNativeBodyB4h>);
static_assert(std::is_standard_layout_v<GraffitiWeightCountDialogNativeBodyB8h>);
static_assert(std::is_trivially_destructible_v<GraffitiLikableCountDialogNativeBodyB4h>);
static_assert(std::is_trivially_destructible_v<GraffitiWeightCountDialogNativeBodyB8h>);

// 固定本次借用的CSoundManager接收者；回调不得重新Capture另一个根。
// 4BCD70最后参数为TypeAllowed门控，不能误标为loop/noAutoPlay。
struct GraffitiCountDialogSoundBorrow final
{
    void* object;
    void* service;
    bool (*playAt4BCD70)(void*,void*, const char*, std::int32_t, std::int32_t, std::uint8_t);
    std::uint32_t (*remainingBeginAt20)(void*,void*);
    bool (*isPlayingAt419370)(void*,void*, std::uint32_t);
};
class IGraffitiCountDialogNativeSource
{
public:
    virtual ~IGraffitiCountDialogNativeSource() = default;
    virtual EnvironmentRootStorage* CaptureEnvironmentAt4180C0() = 0;
    virtual GraffitiPageNativeParamsReceiver CaptureParamsRootAfter413BE0() = 0;
    // 以下实际生产root叶尚未提供实现；必须显式接通，不允许返回伪成功。
    virtual const char* ReadCueName(std::uint32_t globalAddress) = 0;
    virtual GraffitiCountDialogSoundBorrow CaptureSoundAfter413BE0() = 0;
};
struct GraffitiCountDialogNativeServices final
{
    IGraffitiCountDialogNativeSource& source;
    IGuiControlResourceLoadServices& loader;
    IGraphicsSurfaceApi& surfaces;
    IGuiControlGraphicsSource& graphics;
    IGuiInputNativeSource& input;
    IGuiDialogNativeScreenSource& screen;
    IScrVmCommandErrorSink& errors;
    IRuntimeStringManagerSource& strings;
    ScriptNamedValueNativeTables names;
    // 显式绑定本游戏；空指针是装配错误，不回退到18。
    const CountResourceProfile* resources = nullptr;
};
// 宿主必须覆盖Run/Delete/最后一次Ref与vector释放，服务长期存活且不归正文拥有。
class GraffitiCountDialogNativeScope final
{
public:
    explicit GraffitiCountDialogNativeScope(GraffitiCountDialogNativeServices&) noexcept;
    ~GraffitiCountDialogNativeScope() noexcept;
    GraffitiCountDialogNativeScope(const GraffitiCountDialogNativeScope&) = delete;
    GraffitiCountDialogNativeScope& operator=(const GraffitiCountDialogNativeScope&) = delete;
private:
    GraffitiCountDialogNativeServices* previous_;
    GraphicsSurfaceNativeScope surfaces_;
    GuiControlGraphicsScope graphics_;
    GuiInputNativeScope input_;
    GuiDialogNativeScope screen_;
};
GraffitiCountDialogNativeServices& RequireGraffitiCountDialogNativeServices();
const GuiDialogNativeVtable35& GraffitiCountDialogBaseVtable() noexcept;
const GuiDialogNativeVtable35& GraffitiLikableCountDialogVtable() noexcept;
const GuiDialogNativeVtable35& GraffitiWeightCountDialogVtable() noexcept;
// 可审计的原PE地址表；不是可执行地址或RTTI拼接的额外槽。
const std::array<std::uint32_t, 35>& GraffitiLikableCountDialogOriginalSlots() noexcept;
const std::array<std::uint32_t, 35>& GraffitiWeightCountDialogOriginalSlots() noexcept;

// 纯构造入口不分配/释放caller的B4/B8块；调用方已建立Scope。
GraffitiLikableCountDialogNativeBodyB4h* ConstructGraffitiLikableAt4E13A0(
    GraffitiLikableCountDialogNativeBodyB4h*);
GraffitiWeightCountDialogNativeBodyB8h* ConstructGraffitiWeightAt4F4BC0(
    GraffitiWeightCountDialogNativeBodyB8h*);
void DestroyGraffitiCountDialogBaseAt4F7A40(GraffitiCountDialogNativeBody90h&) noexcept;
GuiDialogNativeBody5Ch* DeleteGraffitiLikableAt4FC870(
    GraffitiLikableCountDialogNativeBodyB4h*, std::uint32_t) noexcept;
GuiDialogNativeBody5Ch* DeleteGraffitiWeightAt4FE7C0(
    GraffitiWeightCountDialogNativeBodyB8h*, std::uint32_t) noexcept;
void RefreshGraffitiLikableAt528770(GraffitiLikableCountDialogNativeBodyB4h&, std::int32_t);
void RefreshGraffitiWeightAt528A80(GraffitiWeightCountDialogNativeBodyB8h&, std::int32_t);
void SetGraffitiWeightWidgetsAt5398B0(GraffitiWeightCountDialogNativeBodyB8h&, std::int32_t);

// 唯一生产构造适配，49DCA0继续负责raw/EH/Ref发布；不建立第二份Dialog状态。
class GraffitiCountDialogNativeConstructionSource final : public IGraffitiPageNativeDialogSource
{
public:
    explicit GraffitiCountDialogNativeConstructionSource(GraffitiCountDialogNativeServices& services) noexcept
        : services_(services) {}
    GuiDialogNativeBody5Ch* ConstructLikableAt4E13A0(void* rawB4h) override;
    GuiDialogNativeBody5Ch* ConstructWeightAt4F4BC0(void* rawB8h) override;
    GraffitiCountDialogNativeServices* CountDialogServices() const noexcept override{return &services_;}
private:
    GraffitiCountDialogNativeServices& services_;
};
}
