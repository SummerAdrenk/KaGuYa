#pragma once
#include <Runtime/GuiControlFactoryNative.h>
#include <Runtime/GuiDialogNative.h>
#include <Runtime/InputRootNative.h>
#include <Runtime/ScriptNamedValueNative.h>
#include <span>

namespace KaGuYa::Start::Runtime
{
struct ScriptParamsNativeBody;
class IScrVmCommandErrorSink;
class LikabilityProfileState;
class ILikabilityProfileHost;

// 原4E0E30：common是真实首成员，不把94h整对象冒充另一种派生GUI。
struct LikabilityDialogNativeBody94h final
{
    union { GuiDialogNativeBody5Ch common; };
    RECT boundsAt5C;
    union { YtlReferenceSlotRuntime clickedManagerAt6C; };
    union { YtlReferenceSlotRuntime backgroundAt78; };
    union { YtlReferenceSlotRuntime canvasAt84; };
    std::int32_t phaseAt90;
    // 现代非持久化扩展，旧94h前缀不变；分页/动画仅由本正文持有，不复制旧对象布局。
    LikabilityProfileState* profileState = nullptr;
    // +90原ctor未写，RECT晚写；C++存储构造不能整块清零或提前默认构造Ref。
    LikabilityDialogNativeBody94h() noexcept {}
    LikabilityDialogNativeBody94h(const LikabilityDialogNativeBody94h&) = delete;
    LikabilityDialogNativeBody94h& operator=(const LikabilityDialogNativeBody94h&) = delete;
};
static_assert(sizeof(LikabilityDialogNativeBody94h) == 0x98);
static_assert(alignof(LikabilityDialogNativeBody94h) == 4);
static_assert(std::is_standard_layout_v<LikabilityDialogNativeBody94h>);
static_assert(offsetof(LikabilityDialogNativeBody94h, common) == 0x00);
static_assert(offsetof(LikabilityDialogNativeBody94h, boundsAt5C) == 0x5C);
static_assert(offsetof(LikabilityDialogNativeBody94h, clickedManagerAt6C) == 0x6C);
static_assert(offsetof(LikabilityDialogNativeBody94h, backgroundAt78) == 0x78);
static_assert(offsetof(LikabilityDialogNativeBody94h, canvasAt84) == 0x84);
static_assert(offsetof(LikabilityDialogNativeBody94h, phaseAt90) == 0x90);
static_assert(offsetof(LikabilityDialogNativeBody94h, profileState) == 0x94);

// 5C072C只有35槽；+8C的5DB790是下一类RTTI，不能扩成业务槽。
using LikabilityDialogNativeVtable35 = GuiDialogNativeVtable35;

// 只抽原调用叶。Root opaque accessor不Ensure、不另捕获、不增加持有。
// 命名查询/值读取、数字合成、GUI构造、Begin/Poll/Input业务均留在本模块。
class ILikabilityDialogNativeSource
{
public:
    virtual ~ILikabilityDialogNativeSource() = default;
    virtual EnvironmentRootStorage* CaptureEnvironmentAt4180C0(std::uint32_t callSite) = 0;
    // 528F20原内联60h惰建及解锁后重读，必须与services.environmentLock同源。
    virtual EnvironmentRootStorage*& EnvironmentPublicationAt6198C8(std::uint32_t readOrWriteSite) = 0;
    virtual void EnsureRootAt413BE0(std::uint32_t callSite) = 0;
    virtual void* RootAt6198C4(std::uint32_t readSite) = 0;
    virtual YtlReferenceSlotRuntime& RootDrawSlotAt18(void* capturedRoot) = 0;
    // 498020之后才在已捕获Root上调用，不提前构造/缓存另一代CParams。
    virtual ScriptParamsNativeBody* ParamsAt47BDB0(void* capturedRoot, std::uint32_t callSite) = 0;
    // 原4ACDD0的fresh返回槽；成功返回之后才有调用方临时Ref的展开资格。
    virtual YtlReferenceSlotRuntime* CaptureBackgroundAt4ACDD0(void* nativeDraw,
        YtlReferenceSlotRuntime* freshOutput, std::uint32_t callSite) = 0;
    // 518F30两个内联418210，标识原Enter站点；必须借同一真实Input owner。
    virtual InputRootNativeBody24h* CaptureInputAt418210(std::uint32_t callSite) = 0;
    // 原52624D捕获Graphics及SurfaceDraw叶；caller已固定非零handle/原dirty项地址。
    virtual void DrawBackgroundAt52624D(std::int32_t handle, const RECT* clip) = 0;
};
struct LikabilityDialogNativeServices final
{
    ILikabilityDialogNativeSource& source;
    IGuiControlResourceLoadServices& loader;
    IGraphicsSurfaceApi& surfaces;
    IGuiControlGraphicsSource& graphics;
    IGuiInputNativeSource& input;
    IGuiDialogNativeScreenSource& screen;
    IEnvironmentRootLock& environmentLock;
    IEnvironmentRootMemory& memory;
    IScrVmCommandErrorSink& errors;
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource();
    ScriptNamedValueNativeTables names = GameScriptNamedValueTables();
    ILikabilityProfileHost* profileHost = nullptr;
};

// Main先建scope再持GUI Ref；正文及现代扩展、子控件/Surface/事件的最后释放均在同源scope内。
class LikabilityDialogNativeScope final
{
public:
    explicit LikabilityDialogNativeScope(LikabilityDialogNativeServices&) noexcept;
    ~LikabilityDialogNativeScope() noexcept;
    LikabilityDialogNativeScope(const LikabilityDialogNativeScope&) = delete;
    LikabilityDialogNativeScope& operator=(const LikabilityDialogNativeScope&) = delete;
private:
    LikabilityDialogNativeServices* previous_;
    GraphicsSurfaceNativeScope surfaces_;
    GuiControlGraphicsScope graphics_;
    GuiInputNativeScope input_;
    GuiDialogNativeScope screen_;
};
LikabilityDialogNativeServices& RequireLikabilityDialogNativeServices();
const LikabilityDialogNativeVtable35& LikabilityDialogNativeVtable() noexcept;
LikabilityDialogNativeBody94h* ConstructLikabilityDialogNativeAt4E0E30(LikabilityDialogNativeBody94h&);
// Main Scene20按sizeof(Body)分配；保留历史函数名，返回真实common，不另建拥有者。
GuiDialogNativeBody5Ch* ConstructLikabilityDialogNativeAt4E0E30(void* rawBody, LikabilityDialogNativeServices&);
LikabilityDialogNativeBody94h* ConstructLikabilityDialogNativeAt4E0E30(
    LikabilityDialogNativeBody94h&, LikabilityDialogNativeServices&);
void DestroyLikabilityDialogNative(LikabilityDialogNativeBody94h&) noexcept;
LikabilityDialogNativeBody94h* DeleteLikabilityDialogNativeAt4FC750(
    LikabilityDialogNativeBody94h*, std::uint32_t flags) noexcept;
void BeginLikabilityDialogNativeAt50C950(LikabilityDialogNativeBody94h&);
std::int32_t PollLikabilityDialogNativeAt52D4A0(LikabilityDialogNativeBody94h&);
void ProcessLikabilityDialogInputNativeAt518F30(LikabilityDialogNativeBody94h&);
void FlushLikabilityDialogDirtyNativeAt5261A0(LikabilityDialogNativeBody94h&);
void RefreshLikabilityDialogNativeAt528F20(LikabilityDialogNativeBody94h&);

std::span<const std::uint32_t,35> LikabilityDialogNativeOriginalSlotsSample18() noexcept;
const GuiButtonDescriptorNative2Ch& LikabilityDialogNativeButtonSample18() noexcept;
std::span<const char* const,2> LikabilityDialogNativeBackgroundNamesSample18() noexcept;
std::span<const char* const,11> LikabilityDialogNativeDigitNamesSample18() noexcept;
}
